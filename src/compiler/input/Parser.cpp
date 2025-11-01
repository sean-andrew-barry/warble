import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Error;
import compiler.ir.Opcode;
import compiler.ir.symbol.Type;
import compiler.ir.Token;
import compiler.text.cursor.String;
import compiler.text.Unicode;
import <cstdint>;
import <algorithm>;
import <limits>;
import <vector>;

namespace {
  constexpr int DigitValue(ir::Token token) {
    switch (token) {
      case ir::Token::Digits0: return 0;
      case ir::Token::Digits1: return 1;
      case ir::Token::Digits2: return 2;
      case ir::Token::Digits3: return 3;
      case ir::Token::Digits4: return 4;
      case ir::Token::Digits5: return 5;
      case ir::Token::Digits6: return 6;
      case ir::Token::Digits7: return 7;
      case ir::Token::Digits8: return 8;
      case ir::Token::Digits9: return 9;
      case ir::Token::DigitsA: return 10;
      case ir::Token::DigitsB: return 11;
      case ir::Token::DigitsC: return 12;
      case ir::Token::DigitsD: return 13;
      case ir::Token::DigitsE: return 14;
      case ir::Token::DigitsF: return 15;
      default: return -1;
    }
  }

  constexpr int CharactersNibble(ir::Token token) {
    switch (token) {
      case ir::Token::Characters0: return 0;
      case ir::Token::Characters1: return 1;
      case ir::Token::Characters2: return 2;
      case ir::Token::Characters3: return 3;
      case ir::Token::Characters4: return 4;
      case ir::Token::Characters5: return 5;
      case ir::Token::Characters6: return 6;
      case ir::Token::Characters7: return 7;
      case ir::Token::Characters8: return 8;
      case ir::Token::Characters9: return 9;
      case ir::Token::CharactersA: return 10;
      case ir::Token::CharactersB: return 11;
      case ir::Token::CharactersC: return 12;
      case ir::Token::CharactersD: return 13;
      case ir::Token::CharactersE: return 14;
      case ir::Token::CharactersF: return 15;
      default: return -1;
    }
  }

  bool ConsumeCharactersRun(compiler::text::cursor::Token& cursor, uint32_t& count) {
    uint32_t value = 0;
    bool saw_any = false;

    while (!cursor.Done()) {
      int nibble = CharactersNibble(cursor.Peek());
      if (nibble < 0) {
        break;
      }

      saw_any = true;
      value = (value << 4) | static_cast<uint32_t>(nibble);
      cursor.Advance();
    }

    if (!saw_any) {
      return false;
    }

    count = value;
    return true;
  }

  constexpr bool IsNewlineToken(ir::Token token) {
    switch (token) {
      case ir::Token::LineFeed:
      case ir::Token::CarriageReturn:
      case ir::Token::CarriageReturnLineFeed:
      case ir::Token::LineSeparator:
      case ir::Token::ParagraphSeparator:
        return true;
      default:
        return false;
    }
  }

  // Stream hex nibble tokens (Digits0..DigitsF) into 32-bit big-endian limbs.
  // Only underscores are skipped; stops at the first non-digit token.
  // Returns false if no digits were seen.
  bool PackHexNibblesToLimbs(compiler::text::cursor::Token& cursor,
                             std::vector<uint32_t>& limbs,
                             ir::Symbols& symbols,
                             ir::Index symbol) {
    // First pass: count hex digits (ignore underscores only)
    compiler::text::cursor::Token look = cursor;
    size_t digit_count = 0;
    while (!look.Done()) {
      ir::Token t = look.Peek();
      if (t == ir::Token::Underscore) { look.Advance(); continue; }
      int dv = DigitValue(t);
      if (dv < 0) break;
      ++digit_count;
      look.Advance();
    }

    if (digit_count == 0) {
      return false;
    }

    const uint32_t bits_per_digit = 4u;
    const uint64_t total_bits = static_cast<uint64_t>(digit_count) * bits_per_digit;
    uint32_t group_bits = static_cast<uint32_t>(total_bits % 32u);
    if (group_bits == 0 && total_bits != 0) group_bits = 32u; // first partial group size

    const uint32_t offset = static_cast<uint32_t>(limbs.size());
    uint32_t current = 0;
    bool emitted_any = false;

    while (!cursor.Done()) {
      ir::Token t = cursor.Peek();
      if (t == ir::Token::Underscore) { cursor.Advance(); continue; }

      int dv = DigitValue(t);
      if (dv < 0) break; // stop at first non-digit (e.g., Dot/Exponent/etc.)
      cursor.Advance();

      current = (current << bits_per_digit) | static_cast<uint32_t>(dv);
      group_bits -= bits_per_digit;
      if (group_bits == 0) {
        if (current != 0 || emitted_any) {
          limbs.push_back(current);
          emitted_any = true;
        }
        current = 0;
        group_bits = 32u;
      }
    }

    // Flush any partial group (only nonzero or if we've already emitted prior limbs)
    if (group_bits != 32u) {
      if (current != 0 || emitted_any) {
        limbs.push_back(current);
        emitted_any = true;
      }
    }

    if (!emitted_any) {
      symbols.Payload(symbol, 0u, 0u);
    } else {
      uint32_t length = static_cast<uint32_t>(limbs.size()) - offset;
      symbols.Payload(symbol, offset, length);
    }

    return true;
  }
  
  // Remove leading zero words (big-endian: front is MSW)
  void TrimMagnitude(std::vector<uint32_t>& value) {
    while (!value.empty() && value.front() == 0u) {
      value.erase(value.begin());
    }
  }

  void MultiplySmall(std::vector<uint32_t>& magnitude, uint32_t factor) {
    if (factor == 0) {
      magnitude.clear();
      return;
    }

    if (factor == 1 || magnitude.empty()) {
      return;
    }

    uint64_t carry = 0;
    for (auto it = magnitude.rbegin(); it != magnitude.rend(); ++it) {
      uint64_t product = static_cast<uint64_t>(*it) * factor + carry;
      *it = static_cast<uint32_t>(product & 0xFFFFFFFFu);
      carry = static_cast<uint32_t>(product >> 32u);
    }

    if (carry != 0) {
      magnitude.insert(magnitude.begin(), static_cast<uint32_t>(carry));
    }

    TrimMagnitude(magnitude);
  }

  void AddMagnitude(std::vector<uint32_t>& magnitude, const std::vector<uint32_t>& addend) {
    if (addend.empty()) {
      return;
    }

    if (magnitude.empty()) {
      magnitude = addend;
      return;
    }

    size_t max_size = std::max(magnitude.size(), addend.size());
    if (magnitude.size() < max_size) {
      magnitude.insert(magnitude.begin(), max_size - magnitude.size(), 0u);
    }

    uint32_t carry = 0;
    for (size_t i = 0; i < max_size; ++i) {
      size_t mag_index = magnitude.size() - 1 - i;
      uint32_t add_value = (i < addend.size()) ? addend[addend.size() - 1 - i] : 0u;

      uint64_t sum = static_cast<uint64_t>(magnitude[mag_index]) + add_value + carry;
      magnitude[mag_index] = static_cast<uint32_t>(sum & 0xFFFFFFFFu);
      carry = static_cast<uint32_t>(sum >> 32u);
    }

    if (carry != 0) {
      magnitude.insert(magnitude.begin(), carry);
    }

    TrimMagnitude(magnitude);
  }

  void StoreMagnitude(ir::Symbols& symbols, ir::Index symbol, std::vector<uint32_t>& storage, std::vector<uint32_t>& magnitude) {
    TrimMagnitude(magnitude);

    uint32_t offset = 0;
    uint32_t length = 0;

    if (!magnitude.empty()) {
      offset = static_cast<uint32_t>(storage.size());
      length = static_cast<uint32_t>(magnitude.size());
      storage.insert(storage.end(), magnitude.begin(), magnitude.end());
    }

    symbols.Payload(symbol, offset, length);
  }

  uint32_t DivideSmall(std::vector<uint32_t>& magnitude, uint32_t divisor) {
    uint64_t remainder = 0;

    for (size_t i = 0; i < magnitude.size(); ++i) {
      uint64_t current = (remainder << 32u) | magnitude[i];
      magnitude[i] = static_cast<uint32_t>(current / divisor);
      remainder = current % divisor;
    }

    TrimMagnitude(magnitude);
    return static_cast<uint32_t>(remainder);
  }

  size_t DecimalDigitCount(const std::vector<uint32_t>& value) {
    if (value.empty()) {
      return 0;
    }

    // Fast path for up to 64-bit values
    if (value.size() <= 2) {
      uint64_t v = 0;
      if (value.size() == 1) {
        v = value[0];
      } else {
        v = (static_cast<uint64_t>(value[0]) << 32u) | value[1];
      }

      static constexpr uint64_t POW10_U64[20] = {
        1ull,
        10ull,
        100ull,
        1000ull,
        10000ull,
        100000ull,
        1000000ull,
        10000000ull,
        100000000ull,
        1000000000ull,
        10000000000ull,
        100000000000ull,
        1000000000000ull,
        10000000000000ull,
        100000000000000ull,
        1000000000000000ull,
        10000000000000000ull,
        100000000000000000ull,
        1000000000000000000ull,
        10000000000000000000ull
      };

      // digits is smallest d with v < 10^d
      size_t d = 0;
      while (d + 1 < std::size(POW10_U64) && v >= POW10_U64[d + 1]) {
        ++d;
      }
      return d + 1; // because POW10_U64[0] == 1 -> 1 digit for v in [1,9]
    }

    // Fallback for big integers: repeated divide by 10
    std::vector<uint32_t> copy = value;
    size_t digits = 0;
    while (!copy.empty()) {
      DivideSmall(copy, 10u);
      ++digits;
    }
    return digits;
  }

  // Multiply a big-endian magnitude by 10^digits efficiently using 32-bit factors.
  void MultiplyPow10Vec(std::vector<uint32_t>& magnitude, size_t digits) {
    if (magnitude.empty() || digits == 0) {
      return;
    }

    static constexpr uint32_t POW10_U32[10] = {
      1u,
      10u,
      100u,
      1000u,
      10000u,
      100000u,
      1000000u,
      10000000u,
      100000000u,
      1000000000u
    };

    while (digits >= 9) {
      MultiplySmall(magnitude, POW10_U32[9]); // 10^9
      digits -= 9;
    }

    if (digits > 0) {
      MultiplySmall(magnitude, POW10_U32[digits]);
    }
  }

  bool MultiplyPow10(uint64_t& value, size_t digits) {
    while (digits > 0) {
      if (value > std::numeric_limits<uint64_t>::max() / 10u) {
        return false;
      }
      value *= 10u;
      --digits;
    }
    return true;
  }

  bool ToUint64(const std::vector<uint32_t>& value, uint64_t& out) {
    if (value.size() > 2) {
      return false;
    }

    uint64_t result = 0;
    if (value.size() == 1) {
      result = value[0];
    } else if (value.size() == 2) {
      result = (static_cast<uint64_t>(value[0]) << 32u) | value[1];
    }
    out = result;
    return true;
  }

  void StoreFloat(ir::Symbols& symbols, ir::Index symbol, std::vector<uint32_t>& storage, const std::vector<uint32_t>& significand, int64_t exponent) {
    std::vector<uint32_t> data = significand;
    TrimMagnitude(data);

    uint32_t offset = static_cast<uint32_t>(storage.size());
    uint32_t length = static_cast<uint32_t>(data.size() + 2u);

    storage.insert(storage.end(), data.cbegin(), data.cend());

    uint64_t exponent_bits = static_cast<uint64_t>(exponent);
    storage.push_back(static_cast<uint32_t>(exponent_bits >> 32u));
    storage.push_back(static_cast<uint32_t>(exponent_bits & 0xFFFFFFFFu));

    symbols.Payload(symbol, offset, length);
  }
} // namespace

namespace compiler::input {
  ir::Index Parser::NextSymbol() {
    if (symbols_index >= mod.Symbols().Count()) {
      return ir::Index{};
    }

    return ir::Index{static_cast<uint32_t>(symbols_index++)};
  }

  void Parser::ConsumeCharactersToken() {
    if (cursor.Peek() != ir::Token::Characters) {
      return;
    }

    if (widths_index < mod.Widths().size()) {
      widths_index += 1;
    }

    Advance();
  }

  void Parser::ConsumeEscapeSequence() {
    while (IsEscapeToken(cursor.Peek())) {
      Advance();

      while (cursor.Peek() == ir::Token::Characters) {
        ConsumeCharactersToken();
      }
    }
  }

  ir::Index Parser::ReportError(ir::Error error, ir::Index parent) {
    return Create(ir::symbol::Type::Error, parent, static_cast<uint64_t>(error));
  }

  int Parser::Precedence(ir::Token token) const {
    switch (token) {
      // Property / member access (highest)
      case ir::Token::MemberAccess: // x.y
      case ir::Token::MemberAccessStatic: // x:y
      case ir::Token::MemberAccessOptional: // x?.y
      case ir::Token::MemberAccessStaticOptional: // x?:y
        return 20;

      // Function calls
      case ir::Token::Call: // x -> y
      case ir::Token::ParameterOpen: // x(){ return y; }
      case ir::Token::CaptureOpen: // x[]{ return y; }
      case ir::Token::TupleOpen: // x(y)
      case ir::Token::ArrayOpen: // x[y]
      case ir::Token::ObjectOpen: // x{y}
      case ir::Token::EnumOpen: // x<y>
      case ir::Token::CharOpen: // x'y'
      case ir::Token::StringOpen: // x"y"
      case ir::Token::TemplateStringOpen: // x`y`
        return 19;

      // Unary prefix operators
      case ir::Token::BitwiseNot: // ~x
      case ir::Token::Increment: // ++x
      case ir::Token::Positive: // +x
      case ir::Token::Decrement: // --x
      case ir::Token::Negative: // -x
      case ir::Token::MutableReference: // *x
      case ir::Token::Reference: // &x
      case ir::Token::Symbol: // $x
      case ir::Token::Counted: // #x
      case ir::Token::Copy: // @x
      case ir::Token::Not: // !x
      case ir::Token::Move: // =x (unary move)
      case ir::Token::Spread: // ...x (prefix spread)
        return 18;

      // Await / yield style (kept distinct if needed)
      case ir::Token::Await: // await x
      case ir::Token::Yield: // yield x (when treated unary)
        return 17;

      case ir::Token::Exponent: // x ** y
        return 16;

      // Multiplicative
      case ir::Token::Multiply: // x * y
      case ir::Token::Divide: // x / y
      case ir::Token::Modulo: // x % y
        return 15;

      // Additive
      case ir::Token::Add: // x + y
      case ir::Token::Subtract: // x - y
        return 14;

      // Shifts
      case ir::Token::BitwiseLeftShift: // x << y
      case ir::Token::BitwiseRightShift: // x >> y
      case ir::Token::BitwiseTripleLeftShift: // x <<< y
      case ir::Token::BitwiseTripleRightShift: // x >>> y
        return 13;

      // Relational
      case ir::Token::Lesser: // x < y
      case ir::Token::LesserOrEqual: // x <= y
      case ir::Token::Greater: // x > y
      case ir::Token::GreaterOrEqual: // x >= y
      case ir::Token::AssertLesserOrEqual: // x <== y
      case ir::Token::AssertGreaterOrEqual: // x >== y
        return 11;

      // Equality
      case ir::Token::Equal: // x == y
      case ir::Token::NotEqual: // x != y
      case ir::Token::AssertEqual: // x === y
      case ir::Token::AssertNotEqual: // x !== y
        return 10;

      // Bitwise logical
      case ir::Token::BitwiseAnd: // x & y
        return 9;
      case ir::Token::BitwiseXor: // x ^ y
        return 8;
      case ir::Token::BitwiseOr: // x | y
        return 7;

      // Logical
      case ir::Token::And: // x && y
      case ir::Token::Wrap: // x !! y (truthy and)
        return 6;
      case ir::Token::Or: // x || y
      case ir::Token::Unwrap: // x ?? y (truthy or / unwrap)
        return 5;

      // Range construction
      case ir::Token::ExclusiveRange: // x..y
      case ir::Token::InclusiveRange: // x...y
        return 4;

      // Assignment family
      case ir::Token::Assign: // =
      case ir::Token::AssignAnd: // &&=
      case ir::Token::AssignOr: // ||=
      case ir::Token::AssignTruthyAnd: // !!=
      case ir::Token::AssignTruthyOr: // ??=
      case ir::Token::AssignOptional: // ?=
      case ir::Token::AssignAdd: // +=
      case ir::Token::AssignSubtract: // -=
      case ir::Token::AssignMultiply: // *=
      case ir::Token::AssignDivide: // /=
      case ir::Token::AssignModulo: // %=
      case ir::Token::AssignExponent: // **=
      case ir::Token::BitwiseAssignAnd: // &=
      case ir::Token::BitwiseAssignXor: // ^=
      case ir::Token::BitwiseAssignOr: // |=
      case ir::Token::BitwiseAssignLeftShift: // <<=
      case ir::Token::BitwiseAssignRightShift: // >>=
      case ir::Token::BitwiseAssignTripleLeftShift: // <<<=
      case ir::Token::BitwiseAssignTripleRightShift: // >>>=
        return 3;

      // Spread already handled above as unary (18). Keep Yield again for safety.
      default:
        return -1; // None / unhandled token has no precedence
    }
  }

  bool Parser::IsRightAssociative(ir::Token token) {
    // TODO: This list is old and outdated
    switch (token) {
      case ir::Token::Exponent: // x ** y
      case ir::Token::Assign: // x = y
      case ir::Token::AssignAnd: // x &&= y
      case ir::Token::AssignOr: // x ||= y
      case ir::Token::AssignTruthyAnd: // x !!= y
      case ir::Token::AssignTruthyOr: // x ??= y
      case ir::Token::AssignOptional: // x ?= y
      case ir::Token::AssignAdd: // x += y
      case ir::Token::AssignSubtract: // x -= y
      case ir::Token::AssignMultiply: // x *= y
      case ir::Token::AssignDivide: // x /= y
      case ir::Token::AssignModulo: // x %= y
      case ir::Token::AssignExponent: // x **= y
      case ir::Token::BitwiseAssignAnd: // x &= y
      case ir::Token::BitwiseAssignXor: // x ^= y
      case ir::Token::BitwiseAssignOr: // x |= y
      case ir::Token::BitwiseAssignLeftShift: // x <<= y
      case ir::Token::BitwiseAssignRightShift: // x >>= y
      case ir::Token::BitwiseAssignTripleLeftShift: // x <<<= y
      case ir::Token::BitwiseAssignTripleRightShift: // x >>>= y
        return true;
      default:
        return false;
    }
  }

  ir::Index Parser::ParsePrimary(ir::Index parent) {
    ir::Token token = cursor.Peek();

    switch (token) {
      case ir::Token::Undefined: return Undefined(parent);
      case ir::Token::Null: return Null(parent);
      case ir::Token::True:
      case ir::Token::False: return Boolean(parent);
      case ir::Token::CharOpen: return Character(parent);
      case ir::Token::Integer:
      case ir::Token::Digit:
      case ir::Token::Hex:
      case ir::Token::Octal:
      case ir::Token::Binary: return Integer(parent);
      case ir::Token::Decimal: return Decimal(parent);
      case ir::Token::StringOpen: return String(parent);
      case ir::Token::TemplateStringOpen: return TemplateString(parent);
      case ir::Token::ArrayOpen: return Array(parent);
      case ir::Token::EnumOpen: return Enum(parent);
      case ir::Token::TupleOpen: return Tuple(parent);
      case ir::Token::ObjectOpen: return Object(parent);
      case ir::Token::Identifier: return Identifier(parent);
      case ir::Token::Error: return Error(parent);
      case ir::Token::ConditionOpen: {
        if (!cursor.Match(ir::Token::ConditionOpen)) {
          return ReportError(ir::Error::ParserExpectedConditionOpen, parent);
        }

        ir::Index inner = Expression(parent);

        if (!cursor.Match(ir::Token::ConditionClose)) {
          ReportError(ir::Error::ParserExpectedConditionClose, parent);
        }

        return inner;
      }
      default:
        return ReportError(ir::Error::ParserExpectedPrimaryExpression, parent);
    }
  }

  ir::Index Parser::ParseUnary(ir::Index parent) {
    ir::Token op = cursor.Peek();
    if (IsUnaryPrefixToken(op)) {
      cursor.Advance();

      WhiteSpace(parent);

      ir::Index operand = ParseUnary(parent);

      ir::Index result = MakeTemporary(parent, operand, operand);

      ir::Opcode opcode = UnaryOpcode(op);
      Instruct(opcode, result, operand);

      SlideDestruct(operand);
      SlideDestruct(result);
      return result;
    }

    return ParsePrimary(parent);
  }

  ir::Index Parser::ParseBinary(ir::Index parent, int min_precedence) {
    ir::Index lhs = ParseUnary(parent);

    while (true) {
      WhiteSpace(parent);

      ir::Token op = cursor.Peek();
      int precedence = Precedence(op);

      if (precedence < min_precedence) {
        break;
      }

      bool right_associative = IsRightAssociative(op);
      cursor.Advance();

      WhiteSpace(parent);

      ir::Index rhs = ParseBinary(parent, precedence + (right_associative ? 0 : 1));

      ir::Index result = MakeTemporary(parent, lhs, rhs);

      ir::Opcode opcode = BinaryOpcode(op);
      Instruct(opcode, result, lhs, rhs);

      SlideDestruct(lhs);
      SlideDestruct(rhs);
      SlideDestruct(result);

      lhs = result;
    }

    return lhs;
  }

  bool Parser::SlideDestruct(ir::Index symbol) {
    if (instructions.empty()) return false;
    // Find the last matching Destruct for this symbol
    int found = -1;
    for (int i = static_cast<int>(instructions.size()) - 1; i >= 0; --i) {
      const auto& ins = instructions[static_cast<size_t>(i)];
      if (ins.Is(ir::Opcode::Destruct) && (ins.LHS() == symbol || ins.RHS() == symbol || ins.Dest() == symbol)) {
        found = i;
        break;
      }
    }
    if (found < 0) return false;

    ir::Instruction moved = instructions[static_cast<size_t>(found)];
    instructions.erase(instructions.begin() + found);
    instructions.push_back(moved);
    return true;
  }

  ir::Index Parser::MakeTemporary(ir::Index parent, ir::Index lhs, ir::Index rhs) {
    ir::Index result = Create(ir::symbol::Type::Auto, parent);

    size_t lhs_row = static_cast<size_t>(lhs.Row());
    if (lhs_row < symbols.Count()) {
      symbols.FirstToken(result, symbols.FirstToken(lhs));
    }

    size_t rhs_row = static_cast<size_t>(rhs.Row());
    if (rhs_row < symbols.Count()) {
      symbols.LastToken(result, symbols.LastToken(rhs));
    } else if (lhs_row < symbols.Count()) {
      symbols.LastToken(result, symbols.LastToken(lhs));
    }

    // Emit construct/destruct for temporaries immediately; they'll be slid as used
    Instruct(ir::Opcode::Construct, result, result);
    Instruct(ir::Opcode::Destruct, result, result);
    return result;
  }

  ir::Opcode Parser::UnaryOpcode(ir::Token token) const {
    switch (token) {
      case ir::Token::Negative: return ir::Opcode::Negate;
      case ir::Token::Positive: return ir::Opcode::Move;
      case ir::Token::Not: return ir::Opcode::Not;
      case ir::Token::BitwiseNot: return ir::Opcode::BitwiseNot;
      case ir::Token::Reference:
      case ir::Token::MutableReference: return ir::Opcode::AddressOf;
      case ir::Token::Symbol: return ir::Opcode::SymbolOf;
      case ir::Token::Copy: return ir::Opcode::CopyOf;
      case ir::Token::Move: return ir::Opcode::Move;
      case ir::Token::Spread: return ir::Opcode::Spread;
      default: return ir::Opcode::NoOperation;
    }
  }

  ir::Opcode Parser::BinaryOpcode(ir::Token token) const {
    switch (token) {
      case ir::Token::Add: return ir::Opcode::Add;
      case ir::Token::Subtract: return ir::Opcode::Subtract;
      case ir::Token::Multiply: return ir::Opcode::Multiply;
      case ir::Token::Divide: return ir::Opcode::Divide;
      case ir::Token::Modulo: return ir::Opcode::Modulo;
      case ir::Token::Exponent: return ir::Opcode::Exponent;
      case ir::Token::BitwiseAnd: return ir::Opcode::BitwiseAnd;
      case ir::Token::BitwiseOr: return ir::Opcode::BitwiseOr;
      case ir::Token::BitwiseXor: return ir::Opcode::BitwiseExclusiveOr;
      case ir::Token::BitwiseLeftShift: return ir::Opcode::BitwiseShiftLeft;
      case ir::Token::BitwiseRightShift: return ir::Opcode::BitwiseShiftRight;
      case ir::Token::And: return ir::Opcode::And;
      case ir::Token::Or: return ir::Opcode::Or;
      case ir::Token::Wrap: return ir::Opcode::CompareTruthy;
      case ir::Token::Unwrap: return ir::Opcode::CompareFalsy;
      case ir::Token::Equal:
      case ir::Token::AssertEqual: return ir::Opcode::CompareEqual;
      case ir::Token::NotEqual:
      case ir::Token::AssertNotEqual: return ir::Opcode::CompareNotEqual;
      case ir::Token::Greater: return ir::Opcode::CompareGreater;
      case ir::Token::GreaterOrEqual:
      case ir::Token::AssertGreaterOrEqual: return ir::Opcode::CompareGreaterOrEqual;
      case ir::Token::Lesser: return ir::Opcode::CompareLess;
      case ir::Token::LesserOrEqual:
      case ir::Token::AssertLesserOrEqual: return ir::Opcode::CompareLessOrEqual;
      default: return ir::Opcode::NoOperation;
    }
  }

  // `Create` is for symbols that don't have children and don't need to be closed later
  ir::Index Parser::Create(ir::symbol::Type type, ir::Index parent) {
    ir::Index index = symbols.Add(type);
    symbols.Parent(index, parent);
    symbols.FirstToken(index, static_cast<uint32_t>(std::distance(tokens.cbegin(), cursor.cbegin())));
    symbols.LastToken(index, static_cast<uint32_t>(std::distance(tokens.cbegin(), cursor.cbegin()) + 1));

    return index;
  }

  ir::Index Parser::Open(ir::symbol::Type type, ir::Index parent) {
    ir::Index index = symbols.Add(type);
    symbols.Parent(index, parent);
    symbols.FirstToken(index, static_cast<uint32_t>(std::distance(tokens.cbegin(), cursor.cbegin())));

    return index;
  }
  
  ir::Index Parser::Close(ir::Index index) {
    if (symbols.IsStructured(index)) {
      uint32_t end = static_cast<uint32_t>(symbols.Count() - 1);
      uint64_t value = symbols.Payload(index);
      value = (value & 0x00000000FFFFFFFF) | (static_cast<uint64_t>(end) << 32);
      symbols.Payload(index, value);
    }

    symbols.LastToken(index, static_cast<uint32_t>(std::distance(tokens.cbegin(), cursor.cbegin()) - 1));
    return index;
  }

  Parser::Parser(std::vector<ir::Token>&& tokens, std::vector<uint32_t>&& data)
    : tokens{std::move(tokens)}
    , data{std::move(data)}
    , symbols{}
    , instructions{}
    , cursor{tokens}
  {
    WhiteSpace(ir::Index{}); // Skip any leading trivial tokens
  }

  bool Parser::None(ir::Index parent) {
    cursor.Advance(); // Eat the `None` token
    return true;
  }

  bool Parser::Spaces(ir::Index parent) {
    cursor.Advance(); // Eat the `Spaces*` token
    return true;
  }

  bool Parser::Tabs(ir::Index parent) {
    cursor.Advance(); // Eat the `Tabs*` token
    return true;
  }

  bool Parser::NewLine(ir::Index parent) {
    cursor.Advance();
    return true;
  }

  bool Parser::Comment(ir::Index parent) {
    // TODO
    return true;
  }

  bool Parser::MultiLineComment(ir::Index parent) {
    // TODO
    return true;
  }

  bool Parser::WhiteSpaceHelper(ir::Index parent) {
    switch (cursor.Peek()) {
      case ir::Token::VerticalTab:
      case ir::Token::FormFeed:
      case ir::Token::None: return None(parent);

      case ir::Token::Spaces0:
      case ir::Token::Spaces1:
      case ir::Token::Spaces2:
      case ir::Token::Spaces3:
      case ir::Token::Spaces4:
      case ir::Token::Spaces5:
      case ir::Token::Spaces6:
      case ir::Token::Spaces7:
      case ir::Token::Spaces8:
      case ir::Token::Spaces9:
      case ir::Token::SpacesA:
      case ir::Token::SpacesB:
      case ir::Token::SpacesC:
      case ir::Token::SpacesD:
      case ir::Token::SpacesE:
      case ir::Token::SpacesF: return Spaces(parent);

      case ir::Token::Tabs0:
      case ir::Token::Tabs1:
      case ir::Token::Tabs2:
      case ir::Token::Tabs3:
      case ir::Token::Tabs4:
      case ir::Token::Tabs5:
      case ir::Token::Tabs6:
      case ir::Token::Tabs7:
      case ir::Token::Tabs8:
      case ir::Token::Tabs9:
      case ir::Token::TabsA:
      case ir::Token::TabsB:
      case ir::Token::TabsC:
      case ir::Token::TabsD:
      case ir::Token::TabsE:
      case ir::Token::TabsF: return Tabs(parent);

      case ir::Token::LineFeed:
      case ir::Token::CarriageReturnLineFeed:
      case ir::Token::CarriageReturn:
      case ir::Token::LineSeparator:
      case ir::Token::ParagraphSeparator: return NewLine(parent);

      case ir::Token::CommentOpen: return Comment(parent);
      case ir::Token::MultiLineCommentOpen: return MultiLineComment(parent);
      default: return; // Not a trivial token
    }
  }

  bool Parser::WhiteSpace(ir::Index parent) {
    size_t count = 0;
    while (IsTrivialToken(cursor.Peek()) && WhiteSpaceHelper(parent)) {
      ++count;
    }

    return count > 0;
  }

  ir::Index Parser::Expect(ir::Token token, ir::Error error, ir::Index parent) {
    if (!Match(token)) {
      return ReportError(error, parent);
    }

    return ir::Index{};
  }

  uint32_t Parser::Instruct(ir::Opcode opcode, ir::Index res, ir::Index lhs, ir::Index rhs) {
    uint32_t index = static_cast<uint32_t>(instructions.size());
    instructions.emplace_back(opcode, res, lhs, rhs);
    return index;
  }

  ir::Index Parser::Undefined(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Undefined, parent);

    if (Match(ir::Token::Undefined)) {
      Close(symbol);
      WhiteSpace(parent);
      Instruct(ir::Opcode::Construct, symbol, symbol);
      Instruct(ir::Opcode::Destruct, symbol, symbol);
    } else {
      return ReportError(ir::Error::ParserExpectedUndefinedLiteral, symbol);
    }

    return symbol;
  }

  ir::Index Parser::Null(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Void, parent);

    if (cursor.Match(ir::Token::Null)) {
      Close(symbol); // Don't include the whitespace as a child
      WhiteSpace(parent);
      Instruct(ir::Opcode::Construct, symbol, symbol);
      Instruct(ir::Opcode::Destruct, symbol, symbol);
    } else {
      return ReportError(ir::Error::ParserExpectedNullLiteral, symbol);
    }

    return symbol;
  }

  ir::Index Parser::Boolean(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Boolean, parent);

    if (cursor.Match(ir::Token::True)) {
      symbols.Payload(symbol, static_cast<uint64_t>(true));
    } else if (cursor.Match(ir::Token::False)) {
      symbols.Payload(symbol, static_cast<uint64_t>(false));
    } else {
      return ReportError(ir::Error::ParserExpectedBooleanLiteral, symbol);
    }

    Close(symbol); // Don't include the whitespace as a child
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Character(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Character, parent);

    if (!cursor.Match(ir::Token::CharOpen)) {
      return ReportError(ir::Error::ParserExpectedCharacterOpenQuote, symbol);
    }

    uint32_t local_position = character_position;
    char32_t code_point = U'\0';
    bool has_content = false;

    if (!cursor.Done()) {
      uint32_t run_length = 0;
      if (ConsumeCharactersRun(cursor, run_length)) {
        if (run_length == 0 || run_length != 1) {
          return ReportError(ir::Error::CharacterLiteralExpectedContent, symbol);
        }

        if (local_position + run_length > characters.size()) {
          return ReportError(ir::Error::CharacterLiteralExpectedContent, symbol);
        }

        code_point = characters[local_position];
        local_position += run_length;
        has_content = true;
      } else if (IsEscapeToken(cursor.Peek())) {
        cursor.Advance();

        if (local_position >= characters.size()) {
          return ReportError(ir::Error::CharacterLiteralExpectedContent, symbol);
        }

        code_point = characters[local_position];
        local_position += 1;
        has_content = true;
      }
    }

    if (!has_content) {
      return ReportError(ir::Error::CharacterLiteralExpectedContent, symbol);
    }

    if (!cursor.Match(ir::Token::CharClose)) {
      ReportError(ir::Error::ParserExpectedCharacterCloseQuote, symbol);
    }

    symbols.Payload(symbol, static_cast<uint64_t>(code_point));
    character_position = local_position;

    Close(symbol); // Don't include the whitespace as a child
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Hex(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Integer, parent);

    if (!cursor.Match(ir::Token::HexStart)) {
      return ReportError(ir::Error::ParserExpectedHexStart, symbol);
    }

    if (!PackHexNibblesToLimbs(cursor, limbs, symbols, symbol)) {
      return ReportError(ir::Error::ParserExpectedDecimal, symbol);
    }

    Close(symbol);
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Octal(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Integer, parent);

    if (!cursor.Match(ir::Token::OctalStart)) {
      return ReportError(ir::Error::ParserExpectedOctalStart, symbol);
    }

    // Lexer already emitted octal as hex-nibble tokens; reuse the same packing.
    if (!PackHexNibblesToLimbs(cursor, limbs, symbols, symbol)) {
      return ReportError(ir::Error::ParserExpectedDecimal, symbol);
    }

    Close(symbol);
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Binary(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Integer, parent);

    if (!cursor.Match(ir::Token::BinaryStart)) {
      return ReportError(ir::Error::ParserExpectedBinaryStart, symbol);
    }

    // Lexer already emitted binary as hex-nibble tokens; reuse the same packing.
    if (!PackHexNibblesToLimbs(cursor, limbs, symbols, symbol)) {
      return ReportError(ir::Error::ParserExpectedDecimal, symbol);
    }

    Close(symbol);
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Decimal(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Integer, parent);
    std::vector<uint32_t> significand;
    bool has_digits = false;
    size_t fractional_digits = 0;

    auto consume_runs = [&](bool is_fraction) {
      bool consumed_any = false;
      while (!cursor.Done()) {
        // Skip underscores between runs
        while (cursor.Peek() == ir::Token::Underscore) {
          cursor.Advance();
        }

        int first = DigitValue(cursor.Peek());
        if (first < 0) break; // no more runs

        // Parse one run: count leading zero digits and build value (u64 or big-int fallback)
        size_t leading_zero_digits = 0;
        bool seen_nonzero = false;
        uint64_t run64 = 0;
        bool has_big = false;
        std::vector<uint32_t> run_big;

        auto add_small_to_vec = [&](std::vector<uint32_t>& vec, uint32_t small) {
          if (small == 0) return;
          // AddMagnitude with a single-word vector to avoid duplicating add logic
          std::vector<uint32_t> tmp{small};
          AddMagnitude(vec, tmp);
        };

        while (!cursor.Done()) {
          ir::Token t = cursor.Peek();
          if (t == ir::Token::Underscore || t == ir::Token::Dot || t == ir::Token::Exponent) {
            break; // end of this run
          }
          int dv = DigitValue(t);
          if (dv < 0) break;
          cursor.Advance();

          if (!seen_nonzero) {
            if (dv == 0) {
              leading_zero_digits += 1; // source zeros are kept as digits
              continue;
            }
            seen_nonzero = true;
          }

          if (!has_big) {
            if (run64 > (std::numeric_limits<uint64_t>::max() >> 4)) {
              // Promote to big-int
              if (run64 != 0) {
                if (run64 <= 0xFFFFFFFFull) {
                  run_big.push_back(static_cast<uint32_t>(run64));
                } else {
                  run_big.push_back(static_cast<uint32_t>(run64 >> 32u));
                  run_big.push_back(static_cast<uint32_t>(run64 & 0xFFFFFFFFu));
                }
              }
              has_big = true;
              // incorporate current nibble
              if (!run_big.empty()) MultiplySmall(run_big, 16u);
              add_small_to_vec(run_big, static_cast<uint32_t>(dv));
            } else {
              run64 = (run64 << 4) | static_cast<uint64_t>(dv);
            }
          } else {
            MultiplySmall(run_big, 16u);
            add_small_to_vec(run_big, static_cast<uint32_t>(dv));
          }
        }

        // Determine decimal digit count for the numeric value part of the run
        size_t numeric_digits = 0;
        if (has_big) {
          numeric_digits = DecimalDigitCount(run_big);
        } else if (seen_nonzero) {
          // Use DecimalDigitCount fast path by creating a tiny vector
          std::vector<uint32_t> tmp;
          if (run64 <= 0xFFFFFFFFull) {
            tmp.push_back(static_cast<uint32_t>(run64));
          } else {
            tmp.push_back(static_cast<uint32_t>(run64 >> 32u));
            tmp.push_back(static_cast<uint32_t>(run64 & 0xFFFFFFFFu));
          }
          numeric_digits = DecimalDigitCount(tmp);
        } else {
          // Entire run was zeros; numeric value contributes no additional digits beyond leading zeros
          numeric_digits = 0;
        }

        size_t run_digits = leading_zero_digits + numeric_digits;

        // Merge into the significand: S = S * 10^run_digits + run_value
        if (run_digits != 0) {
          MultiplyPow10Vec(significand, run_digits);
          if (has_big) {
            if (!run_big.empty()) {
              AddMagnitude(significand, run_big);
            }
          } else if (seen_nonzero) {
            std::vector<uint32_t> tmp;
            if (run64 <= 0xFFFFFFFFull) {
              tmp.push_back(static_cast<uint32_t>(run64));
            } else {
              tmp.push_back(static_cast<uint32_t>(run64 >> 32u));
              tmp.push_back(static_cast<uint32_t>(run64 & 0xFFFFFFFFu));
            }
            AddMagnitude(significand, tmp);
          }

          if (is_fraction) {
            fractional_digits += run_digits;
          }

          has_digits = true;
          consumed_any = true;
        }
      }
      return consumed_any;
    };

    consume_runs(false);

    bool is_float = false;
    if (cursor.Peek() == ir::Token::Dot) {
      is_float = true;
      cursor.Advance();
      consume_runs(true);
    }

    bool exponent_negative = false;
    uint64_t exponent_abs = 0;
    bool exponent_overflow = false;

    bool exponent_digits = false;
    if (cursor.Peek() == ir::Token::Exponent) {
      is_float = true;
      cursor.Advance();

      if (cursor.Peek() == ir::Token::Plus) {
        cursor.Advance();
      } else if (cursor.Peek() == ir::Token::Minus) {
        exponent_negative = true;
        cursor.Advance();
      }

      // Consume exponent runs, accumulating exponent_abs in base-10
      while (!cursor.Done()) {
        // Skip underscores between runs
        while (cursor.Peek() == ir::Token::Underscore) {
          cursor.Advance();
        }

        int first = DigitValue(cursor.Peek());
        if (first < 0) break; // no more digits

        // Parse a run into uint64 only (overflow => fail)
        size_t leading_zero_digits = 0;
        bool seen_nonzero = false;
        uint64_t run64 = 0;
        bool run_overflow = false;

        while (!cursor.Done()) {
          ir::Token t = cursor.Peek();
          if (t == ir::Token::Underscore || t == ir::Token::Dot || t == ir::Token::Exponent) {
            break;
          }
          int dv = DigitValue(t);
          if (dv < 0) break;
          cursor.Advance();

          if (!seen_nonzero) {
            if (dv == 0) {
              leading_zero_digits += 1;
              continue;
            }
            seen_nonzero = true;
          }

          if (run64 > (std::numeric_limits<uint64_t>::max() >> 4)) {
            run_overflow = true;
            break;
          }
          run64 = (run64 << 4) | static_cast<uint64_t>(dv);
        }

        if (run_overflow) { exponent_overflow = true; break; }

        size_t numeric_digits = 0;
        if (seen_nonzero) {
          // Fast 64-bit digit count using a small table
          static constexpr uint64_t P10[20] = {
            1ull,
            10ull,
            100ull,
            1000ull,
            10000ull,
            100000ull,
            1000000ull,
            10000000ull,
            100000000ull,
            1000000000ull,
            10000000000ull,
            100000000000ull,
            1000000000000ull,
            10000000000000ull,
            100000000000000ull,
            1000000000000000ull,
            10000000000000000ull,
            100000000000000000ull,
            1000000000000000000ull,
            10000000000000000000ull
          };
          size_t d = 1;
          while (d < 20 && run64 >= P10[d]) ++d;
          numeric_digits = d;
        } else {
          numeric_digits = 0; // all zeros
        }

        size_t run_digits = leading_zero_digits + numeric_digits;
        if (!MultiplyPow10(exponent_abs, run_digits)) { exponent_overflow = true; break; }

        if (seen_nonzero) {
          if (run64 > std::numeric_limits<uint64_t>::max() - exponent_abs) { exponent_overflow = true; break; }
          exponent_abs += run64;
        }

        exponent_digits = true;
      }

      if (!exponent_digits || exponent_overflow) {
        return ReportError(ir::Error::ParserExpectedDecimal, symbol);
      }
    }

    if (!has_digits) {
      return ReportError(ir::Error::ParserExpectedDecimal, symbol);
    }

    if (fractional_digits > static_cast<size_t>(std::numeric_limits<int64_t>::max())) {
      return ReportError(ir::Error::ParserExpectedDecimal, symbol);
    }

    if (!is_float) {
      StoreMagnitude(symbols, symbol, limbs, significand);
      Close(symbol);
      WhiteSpace(parent);
      Instruct(ir::Opcode::Construct, symbol, symbol);
      Instruct(ir::Opcode::Destruct, symbol, symbol);
      return symbol;
    }

    symbols.Type(symbol, ir::symbol::Type::Float);

    int64_t exponent_total = 0;
    if (exponent_abs > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + (exponent_negative ? 1ull : 0ull)) {
      return ReportError(ir::Error::ParserExpectedDecimal, symbol);
    }

    exponent_total = exponent_negative ? -static_cast<int64_t>(exponent_abs) : static_cast<int64_t>(exponent_abs);

    int64_t fractional_shift = static_cast<int64_t>(fractional_digits);
    if (fractional_shift > 0 && exponent_total < std::numeric_limits<int64_t>::min() + fractional_shift) {
      return ReportError(ir::Error::ParserExpectedDecimal, symbol);
    }

    exponent_total -= fractional_shift;

    StoreFloat(symbols, symbol, limbs, significand, exponent_total);

    Close(symbol);
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::String(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::String, parent);

    if (!cursor.Match(ir::Token::StringOpen)) {
      return ReportError(ir::Error::ParserExpectedStringOpenQuote, symbol);
    }

    uint32_t slice_start = character_position;
    uint32_t local_position = slice_start;
    uint32_t length = 0;

    // Accumulate raw runs and escapes; each token corresponds to UTF-32 code points captured by the lexer.
    while (true) {
      if (cursor.Done()) {
        return ReportError(ir::Error::StringLiteralExpectedClosingDoubleQuote, symbol);
      }

      ir::Token token = cursor.Peek();

      uint32_t run = 0;
      if (ConsumeCharactersRun(cursor, run)) {
        if (run > 0) {
          if (local_position + run > characters.size()) {
            return ReportError(ir::Error::StringLiteralExpectedClosingDoubleQuote, symbol);
          }

          local_position += run;
          length += run;
        }
        continue;
      }

      if (IsEscapeToken(token)) {
        cursor.Advance();

        if (local_position >= characters.size()) {
          return ReportError(ir::Error::StringLiteralExpectedClosingDoubleQuote, symbol);
        }

        local_position += 1;
        length += 1;
        continue;
      }

      if (token == ir::Token::StringClose) {
        break;
      }

      return ReportError(ir::Error::StringLiteralExpectedClosingDoubleQuote, symbol);
    }

    if (!cursor.Match(ir::Token::StringClose)) {
      ReportError(ir::Error::ParserExpectedStringCloseQuote, symbol);
    }

    symbols.Payload(symbol, slice_start, length);
    character_position = local_position;

    Close(symbol);
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Array(ir::Index parent) {
    // A required `ArrayOpen` token
    // Create an `Array` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `ArrayClose` token
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Array, parent);
    Push(symbol);
    
    if (!Match(ir::Token::ArrayOpen)) {
      return ReportError(ir::Error::ParserExpectedArrayOpenBracket, symbol);
    }

    if (!Match(ir::Token::ArrayClose)) {
      while (true) {
        Expression(symbol);

        if (Match(ir::Token::Comma)) {
          if (Match(ir::Token::ArrayClose)) {
            break;
          }
          continue;
        }

        if (!Match(ir::Token::ArrayClose)) {
          ReportError(ir::Error::ParserExpectedArrayCloseBracket, symbol);
        }
        break;
      }
    }

    Pop(symbol);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Enum(ir::Index parent) {
    // A required `EnumOpen` token
    // Create an `Enum` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `EnumClose` token
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Enum, parent);
    Push(symbol);

    if (!Match(ir::Token::EnumOpen)) {
      return ReportError(ir::Error::ParserExpectedEnumOpenChevron, symbol);
    }

    if (!Match(ir::Token::EnumClose)) {
      while (true) {
        Expression(symbol);

        if (Match(ir::Token::Comma)) {
          if (Match(ir::Token::EnumClose)) {
            break;
          }
          continue;
        }

        if (!Match(ir::Token::EnumClose)) {
          ReportError(ir::Error::ParserExpectedEnumCloseChevron, symbol);
        }
        break;
      }
    }

    Pop(symbol);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Tuple(ir::Index parent) {
    // A required `TupleOpen` token
    // Create a `Tuple` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `TupleClose` token
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Tuple, parent);
    Push(symbol);

    if (!Match(ir::Token::TupleOpen)) {
      return ReportError(ir::Error::ParserExpectedTupleOpenParen, symbol);
    }

    if (!Match(ir::Token::TupleClose)) {
      while (true) {
        Expression(symbol);

        if (Match(ir::Token::Comma)) {
          if (Match(ir::Token::TupleClose)) {
            break;
          }
          continue;
        }

        if (!Match(ir::Token::TupleClose)) {
          ReportError(ir::Error::ParserExpectedTupleCloseParen, symbol);
        }
        break;
      }
    }

    Pop(symbol);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Object(ir::Index parent) {
    // A required `ObjectOpen` token
    // Create an `Object` symbol to serve as the parent of the subsequent `ObjectDeclaration` groups
    // Zero or more `ObjectDeclaration` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `ObjectClose` token
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Object, parent);
    Push(symbol);

    if (!Match(ir::Token::ObjectOpen)) {
      return ReportError(ir::Error::ParserExpectedObjectOpenBrace, symbol);
    }

    if (!Match(ir::Token::ObjectClose)) {
      while (true) {
        ir::Index declaration = ObjectDeclaration(symbol);

        if (!declaration) {
          ReportError(ir::Error::ParserExpectedObjectDeclaration, symbol);
        }

        if (Match(ir::Token::Comma)) {
          if (Match(ir::Token::ObjectClose)) {
            break;
          }
          continue;
        }

        if (!Match(ir::Token::ObjectClose)) {
          ReportError(ir::Error::ParserExpectedObjectCloseBrace, symbol);
        }
        break;
      }
    }

    Pop(symbol);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::TemplateString(ir::Index parent) {
    ir::Index tpl = Open(ir::symbol::Type::TemplateString, parent);

    if (!cursor.Match(ir::Token::TemplateStringOpen)) {
      return ReportError(ir::Error::TemplateStringLiteralExpectedClosingBacktick, tpl);
    }

    uint32_t local_position = character_position;
    uint32_t segment_start = local_position;

    auto emit_segment = [&](bool force) {
      uint32_t segment_length = local_position - segment_start;
      if (!force && segment_length == 0) {
        return;
      }

      ir::Index str = Create(ir::symbol::Type::String, tpl);
      symbols.Payload(str, segment_start, segment_length);
      Instruct(ir::Opcode::Construct, str, str);
      Instruct(ir::Opcode::Destruct, str, str);

      segment_start = local_position;
    };

    bool closed = false;

    while (!cursor.Done()) {
      ir::Token token = cursor.Peek();

      if (token == ir::Token::TemplateStringClose) {
        cursor.Advance();
        emit_segment(true);
        closed = true;
        break;
      }

      if (token == ir::Token::TemplateStringExpressionOpen) {
        cursor.Advance();
        emit_segment(true);

        Expression(tpl);

        while (IsTrivialToken(cursor.Peek())) {
          WhiteSpace(tpl);
        }

        if (!cursor.Match(ir::Token::TemplateStringExpressionClose)) {
          ReportError(ir::Error::TemplateStringLiteralExpectedClosingBrace, tpl);
        }

        segment_start = local_position;
        continue;
      }

      uint32_t run = 0;
      if (ConsumeCharactersRun(cursor, run)) {
        if (run > 0) {
          if (local_position + run > characters.size()) {
            return ReportError(ir::Error::TemplateStringLiteralExpectedClosingBacktick, tpl);
          }
          local_position += run;
        }
        continue;
      }

      if (IsEscapeToken(token)) {
        cursor.Advance();
        if (local_position >= characters.size()) {
          return ReportError(ir::Error::TemplateStringLiteralExpectedClosingBacktick, tpl);
        }
        local_position += 1;
        continue;
      }

      if (IsNewlineToken(token)) {
        cursor.Advance();
        uint32_t newline_len = (token == ir::Token::CarriageReturnLineFeed) ? 2u : 1u;
        if (local_position + newline_len > characters.size()) {
          return ReportError(ir::Error::TemplateStringLiteralExpectedClosingBacktick, tpl);
        }
        local_position += newline_len;
        continue;
      }

      ReportError(ir::Error::TemplateStringLiteralExpectedClosingBacktick, tpl);
      cursor.Advance();
      break;
    }

    if (!closed) {
      emit_segment(false);
      ReportError(ir::Error::TemplateStringLiteralExpectedClosingBacktick, tpl);
    }

    character_position = local_position;

    Close(tpl);
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, tpl, tpl);
    Instruct(ir::Opcode::Destruct, tpl, tpl);
    return tpl;
  }

  ir::Index Parser::Function(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::Value(ir::Index parent) {
    switch (cursor.Peek()) {
      // Any of these have to be an identifier, since they aren't inside a string/character literal
      case ir::Token::Characters0:
      case ir::Token::Characters1:
      case ir::Token::Characters2:
      case ir::Token::Characters3:
      case ir::Token::Characters4:
      case ir::Token::Characters5:
      case ir::Token::Characters6:
      case ir::Token::Characters7:
      case ir::Token::Characters8:
      case ir::Token::Characters9:
      case ir::Token::CharactersA:
      case ir::Token::CharactersB:
      case ir::Token::CharactersC:
      case ir::Token::CharactersD:
      case ir::Token::CharactersE:
      case ir::Token::CharactersF:
      case ir::Token::EscapeASCII:
      case ir::Token::EscapeHex:
      case ir::Token::EscapeUnicode:
      case ir::Token::EscapeUnicodeBraced1:
      case ir::Token::EscapeUnicodeBraced2:
      case ir::Token::EscapeUnicodeBraced3:
      case ir::Token::EscapeUnicodeBraced4:
      case ir::Token::EscapeUnicodeBraced5:
      case ir::Token::EscapeUnicodeBraced6: return Identifier(parent);

      // Any of these have to be a number. (Floats cannot start with a dot.)
      case ir::Token::Digits0:
      case ir::Token::Digits1:
      case ir::Token::Digits2:
      case ir::Token::Digits3:
      case ir::Token::Digits4:
      case ir::Token::Digits5:
      case ir::Token::Digits6:
      case ir::Token::Digits7:
      case ir::Token::Digits8:
      case ir::Token::Digits9:
      case ir::Token::DigitsA:
      case ir::Token::DigitsB:
      case ir::Token::DigitsC:
      case ir::Token::DigitsD:
      case ir::Token::DigitsE:
      case ir::Token::DigitsF: return Decimal(parent);

      case ir::Token::HexStart: return Hex(parent);
      case ir::Token::OctalStart: return Octal(parent);
      case ir::Token::BinaryStart: return Binary(parent);

      case ir::Token::True:
      case ir::Token::False: return Boolean(parent);

      case ir::Token::Null: return Null(parent);
      case ir::Token::Undefined: return Undefined(parent);
      case ir::Token::This: return This(parent);
      case ir::Token::That: return That(parent);
      case ir::Token::StringOpen: return String(parent);
      case ir::Token::CharOpen: return Character(parent);
      case ir::Token::ArrayOpen: return Array(parent);
      case ir::Token::EnumOpen: return Enum(parent);
      case ir::Token::TupleOpen: return Tuple(parent);
      case ir::Token::ObjectOpen: return Object(parent);
      case ir::Token::TemplateStringOpen: return TemplateString(parent);

      // Functions may start with a capture list or parameter list
      case ir::Token::CaptureOpen:
      case ir::Token::ParameterOpen: return Function(parent);

      default: return ir::Index{};
    }
  }

  ir::Index Parser::Identifier(ir::Index parent) {
    ir::Index symbol = Open(ir::symbol::Type::Identifier, parent);

    uint32_t slice_start = character_position;
    uint32_t local_position = slice_start;
    uint32_t length = 0;

    while (!cursor.Done()) {
      ir::Token token = cursor.Peek();

      uint32_t run = 0;
      if (ConsumeCharactersRun(cursor, run)) {
        if (run > 0) {
          if (local_position + run > characters.size()) {
            return ReportError(ir::Error::ParserExpectedPrimaryExpression, symbol);
          }

          local_position += run;
          length += run;
        }
        continue;
      }

      if (IsEscapeToken(token)) {
        cursor.Advance();

        if (local_position >= characters.size()) {
          return ReportError(ir::Error::ParserExpectedPrimaryExpression, symbol);
        }

        local_position += 1;
        length += 1;
        continue;
      }

      break;
    }

    if (length == 0) {
      return ReportError(ir::Error::ParserExpectedPrimaryExpression, symbol);
    }

    symbols.Payload(symbol, slice_start, length);
    character_position = local_position;

    Close(symbol);
    WhiteSpace(parent);
    Instruct(ir::Opcode::Construct, symbol, symbol);
    Instruct(ir::Opcode::Destruct, symbol, symbol);
    return symbol;
  }

  ir::Index Parser::Destructuring(ir::Index parent) {
    switch (cursor.Peek()) {
      case ir::Token::ArrayOpen: return Array(parent);
      case ir::Token::TupleOpen: return Tuple(parent);
      case ir::Token::ObjectOpen: return Object(parent);
      default:
        return ReportError(ir::Error::ParserExpectedDestructuringPattern, parent);
    }
  }

  ir::Index Parser::Error(ir::Index parent) {
    if (!cursor.Match(ir::Token::Error)) {
      return ReportError(ir::Error::ParserExpectedPrimaryExpression, parent);
    }
    return NextSymbol();
  }

  ir::Index Parser::Expression(ir::Index parent) {
    WhiteSpace(parent);

    return ParseBinary(parent, 0);
  }

  ir::Index Parser::ObjectDeclaration(ir::Index parent) {
    ir::Token token = cursor.Peek();

    if (token == ir::Token::Comma) {
      return ir::Index{};
    }

    ir::Index key;

    switch (token) {
      case ir::Token::Identifier:
        key = Identifier(parent);
        break;
      case ir::Token::StringOpen:
        key = String(parent);
        break;
      case ir::Token::CharOpen:
        key = Character(parent);
        break;
      default:
        return Expression(parent);
    }

    if (Match(ir::Token::Colon)) {
      return Expression(parent);
    }

    return key;
  }

  ir::Index Parser::Declaration(ir::Index parent) {
    // A `Declaration` is different from the `DeclarationStatement` statement, as it can appear in other places such as function parameters or destructured objects/arrays
    // It has slightly different rules and does not end with a semicolon

    // *Zero* or more modifier tokens, such as `Let`, `Const`, `Mutable`, `Static`, `Private`, `Public`, or `Protected`
    // A required `Identifier` group OR a `Destructuring` group
    // An optional type annotation, beginning with a `TypeStart` token, then a required `Expression` group
    // An optional assignment, beginning with an `Assign` token, then a required `Expression` group

    // NOTE: Unsure what we return here. If there was an expression, we return that symbol. If not, do we create a new `Declaration` symbol? Or just return the identifier/destructuring symbol? This will need more thought.

    while (IsModifierToken(cursor.Peek())) {
      Advance();
    }

    ir::Index target;

    if (cursor.Peek() == ir::Token::Identifier) {
      target = Identifier(parent);
    } else {
      target = Destructuring(parent);
    }

    if (Match(ir::Token::TypeStart)) {
      Expression(parent);
    }

    if (Match(ir::Token::Assign)) {
      return Expression(parent);
    }

    return target;
  }

  // Statements
  ir::Index Parser::ImportStatement(ir::Index parent) {
    // A required `Import` token
    // An optional `Identifier` or a destructured object, this can be skipped to import the module for side effects only
    // A required `From` token
    // A required `String` group
    // An optional `In` token, followed by a required `String` group for a target package
    // A required `Semicolon` token

    ir::Index symbol = Create(ir::symbol::Type::Import, parent);
    Push(symbol);

    if (!Match(ir::Token::Import)) {
      return ReportError(ir::Error::ParserExpectedImportKeyword, symbol);
    }

    if (cursor.Peek() == ir::Token::Identifier) {
      Identifier(symbol);
    } else if (cursor.Peek() == ir::Token::ObjectOpen) {
      Destructuring(symbol);
    }

    if (!Match(ir::Token::From)) {
      return ReportError(ir::Error::ParserExpectedFromAfterImport, symbol);
    }

    String(symbol);

    if (Match(ir::Token::In)) {
      String(symbol);
    }

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterImport, symbol);
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::Permissions(ir::Index parent) {
    // A required `With` token
    // A required `Array` group
    // A required `In` token
    // A required `String` group specifying the package name this permission is granting access to
    if (!Match(ir::Token::With)) {
      return ReportError(ir::Error::ParserExpectedWithKeyword, parent);
    }

    Array(parent);

    if (!Match(ir::Token::In)) {
      return ReportError(ir::Error::ParserExpectedInAfterPermissions, parent);
    }

    String(parent);
    return ir::Index{};
  }

  ir::Index Parser::RegisterStatement(ir::Index parent) {
    // A required `Register` token
    // A required `String` group naming the package
    // A required `From` token
    // A required `String` group setting the package path/url
    // Zero or more `Permissions` groups
    // A required `Semicolon` token

    ir::Index symbol = Create(ir::symbol::Type::Register, parent);
    Push(symbol);

    if (!Match(ir::Token::Register)) {
      return ReportError(ir::Error::ParserExpectedRegisterKeyword, symbol);
    }

    String(symbol);

    if (!Match(ir::Token::From)) {
      return ReportError(ir::Error::ParserExpectedFromAfterRegister, symbol);
    }

    String(symbol);

    while (cursor.Peek() == ir::Token::With) {
      Permissions(symbol);
    }

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterRegister, symbol);
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::BreakStatement(ir::Index parent) {
    if (!cursor.Match(ir::Token::Break)) {
      return ReportError(ir::Error::ParserExpectedBreakKeyword, parent);
    }

    WhiteSpace(parent);

    size_t depth = 1;
    while (cursor.Match(ir::Token::Break)) {
      depth += 1;
      WhiteSpace(parent);
    }

    if (depth > break_targets.size()) {
      ReportError(ir::Error::ParserBreakOutsideLoop, parent);
      depth = break_targets.size();
    }

    ir::Index target{};
    if (depth != 0) {
      target = break_targets[break_targets.size() - depth];
    }

    if (!cursor.Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterBreak, parent);
    }

    if (target) {
      Instruct(ir::Opcode::Jump, target);
    }

    return ir::Index{};
  }

  ir::Index Parser::ContinueStatement(ir::Index parent) {
    if (!cursor.Match(ir::Token::Continue)) {
      return ReportError(ir::Error::ParserExpectedContinueKeyword, parent);
    }

    WhiteSpace(parent);

    size_t depth = 1;
    while (cursor.Match(ir::Token::Continue)) {
      depth += 1;
      WhiteSpace(parent);
    }

    if (depth > continue_targets.size()) {
      ReportError(ir::Error::ParserContinueOutsideLoop, parent);
      depth = continue_targets.size();
    }

    ir::Index target{};
    if (depth != 0) {
      target = continue_targets[continue_targets.size() - depth];
    }

    if (!cursor.Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterContinue, parent);
    }

    if (target) {
      Instruct(ir::Opcode::Jump, target);
    }

    return ir::Index{};
  }

  ir::Index Parser::ReturnStatement(ir::Index parent) {
    // A required `Return` token
    // An optional `Expression` group
    // A required `Semicolon` token
    if (cursor.Match(ir::Token::Return)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::ParserExpectedReturnKeyword, parent);
    }

    ir::Index value{};
    if (cursor.Peek() != ir::Token::Semicolon) {
      value = Expression(parent);
    }

    if (cursor.Match(ir::Token::Semicolon)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::ParserExpectedSemicolonAfterReturn, parent);
    }

    Instruct(ir::Opcode::Return, value);

    return value;
  }

  ir::Index Parser::DoStatement(ir::Index parent) {
    // NOTE: In Warble, `do` statements are just scopes that exist to group code.
    // They do not have any looping behavior like in some other languages.
    // A Warble `do` is equivalent to an optimized `if (true) { ... }` statement.
    // This is because Warble interprets a plain `{ ... }` as an object literal, *not* a scope block.
    ir::Index statement = Open(ir::symbol::Type::Do, parent);

    if (!cursor.Match(ir::Token::Do)) {
      return ReportError(ir::Error::ParserExpectedDoKeyword, statement);
    }

    WhiteSpace(statement);

    Scope(statement);

    cursor.Match(ir::Token::Semicolon);

    return Close(statement);
  }

  ir::Index Parser::WhileStatement(ir::Index parent) {
    // while (condition) { body } [;]
    ir::Index statement = Open(ir::symbol::Type::While, parent);

    if (!cursor.Match(ir::Token::While)) {
      return ReportError(ir::Error::ParserExpectedWhileKeyword, statement);
    }

    WhiteSpace(statement);

    ir::Index condition_label = Create(ir::symbol::Type::Label, statement);
    ir::Index exit = Create(ir::symbol::Type::Label, statement);

    break_targets.push_back(exit);
    continue_targets.push_back(condition_label);

    Instruct(ir::Opcode::Label, condition_label);

    ir::Index cond = Condition(statement);

    WhiteSpace(statement);

    Instruct(ir::Opcode::Branch, cond, exit); // Exit when condition is falsy

    Scope(statement);

    Instruct(ir::Opcode::Jump, condition_label);

    cursor.Match(ir::Token::Semicolon);

    Instruct(ir::Opcode::Label, exit);

    ir::Index closed = Close(statement);

    continue_targets.pop_back();
    break_targets.pop_back();

    return closed;
  }

  ir::Index Parser::RepeatStatement(ir::Index parent) {
    // repeat { body } [while (condition)] [;]
    ir::Index statement = Open(ir::symbol::Type::Repeat, parent);

    if (!cursor.Match(ir::Token::Repeat)) {
      return ReportError(ir::Error::ParserExpectedRepeatKeyword, statement);
    }

    WhiteSpace(statement);

    // Loop entry/exit labels — continue label initially matches loop entry
    ir::Index entry = Create(ir::symbol::Type::Label, statement);
    ir::Index exit = Create(ir::symbol::Type::Label, statement);
    ir::Index continue_label = Create(ir::symbol::Type::Label, statement);

    break_targets.push_back(exit);
    continue_targets.push_back(continue_label);

    Instruct(ir::Opcode::Label, entry);
    uint32_t continue_label_instruction = Instruct(ir::Opcode::Label, continue_label);

    Scope(statement);

    WhiteSpace(statement);

    if (cursor.Match(ir::Token::While)) {
      // repeat ... while (condition) — condition executes after the body
      symbols.Type(statement, ir::symbol::Type::RepeatWhile);
      WhiteSpace(statement);

      if (continue_label_instruction < instructions.size()) {
        instructions.erase(instructions.begin() + continue_label_instruction);
      }

      continue_label_instruction = Instruct(ir::Opcode::Label, continue_label);

      ir::Index cond = Condition(statement);

      WhiteSpace(statement);

      Instruct(ir::Opcode::Fork, cond, exit, entry); // if condition falsy, exit loop, otherwise jump back to start
    } else {
      Instruct(ir::Opcode::Jump, entry); // unconditional loop
    }

    cursor.Match(ir::Token::Semicolon);

    Instruct(ir::Opcode::Label, exit);

    ir::Index closed = Close(statement);

    continue_targets.pop_back();
    break_targets.pop_back();

    return closed;
  }

  ir::Index Parser::ForStatement(ir::Index parent) {
    // NOTE: Iteration protocol wiring is pending. This implementation focuses on
    // parsing structure so we can revisit the instruction emission later.
    ir::Index statement = Open(ir::symbol::Type::For, parent);

    if (!cursor.Match(ir::Token::For)) {
      return ReportError(ir::Error::ParserExpectedForKeyword, statement);
    }

    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::ConditionOpen)) {
      ReportError(ir::Error::ParserExpectedConditionOpen, statement);
    } else {
      WhiteSpace(statement);
    }

    ir::Index binding = Declaration(statement);
    (void)binding;

    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::In)) {
      ReportError(ir::Error::ParserExpectedInAfterFor, statement);
    } else {
      WhiteSpace(statement);
    }

    ir::Index iterable = Expression(statement);
    (void)iterable;

    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::ConditionClose)) {
      ReportError(ir::Error::ParserExpectedForConditionClose, statement);
    }

    WhiteSpace(statement);

    ir::Index body = Create(ir::symbol::Type::Label, statement);
    ir::Index exit = Create(ir::symbol::Type::Label, statement);

    break_targets.push_back(exit);
    continue_targets.push_back(body);

    Instruct(ir::Opcode::Label, body);

    Scope(statement);

    WhiteSpace(statement);

    cursor.Match(ir::Token::Semicolon);

    Instruct(ir::Opcode::Label, exit);

    continue_targets.pop_back();
    break_targets.pop_back();

    return Close(statement);
  }

  ir::Index Parser::ElseStatement(ir::Index parent) {
    ir::Index statement = Open(ir::symbol::Type::Else, parent);
    ir::Index exit = Create(ir::symbol::Type::Label, statement); // Create the exit label upfront

    if (cursor.Match(ir::Token::Else)) {
      WhiteSpace(statement);
    } else {
      return ReportError(ir::Error::ParserExpectedElseKeyword, statement);
    }

    bool has_if = false;
    if (cursor.Match(ir::Token::If)) {
      has_if = true;
      WhiteSpace(statement);
      symbols.Type(statement, ir::symbol::Type::ElseIf); // Upgrade to ElseIf

      ir::Index cond = Condition(statement);
      // Branch semantics here: jump to `exit` when condition is falsy (skip this else-if block)
      Instruct(ir::Opcode::Branch, cond, exit);
    }

    Scope(statement);

    uint32_t join_index = 0;
    if (has_if) {
      join_index = Instruct(ir::Opcode::Jump); // Placeholder
    }

    Instruct(ir::Opcode::Label, exit); // Anchor the exit label here
    Close(statement);

    if (has_if && cursor.Peek() == ir::Token::Else) {
      ir::Index end = ElseStatement(parent); // Gets the last exit label of the else-if chain
      instructions[join_index].Operand(0, end); // Patch jump to the end of the else-if chain
      return end;
    }

    // If this was an else-if with no trailing else, patch its tail jump to its own exit label
    if (has_if) {
      instructions[join_index].Operand(0, exit);
    }
    
    return exit;
  }

  ir::Index Parser::IfStatement(ir::Index parent) {
    ir::Index statement = Open(ir::symbol::Type::If, parent);
    ir::Index exit = Create(ir::symbol::Type::Label, statement); // Create the exit label upfront

    if (cursor.Match(ir::Token::If)) {
      WhiteSpace(statement);
    } else {
      return ReportError(ir::Error::ParserExpectedIfKeyword, statement);
    }

    ir::Index cond = Condition(statement);
    Instruct(ir::Opcode::Branch, cond, exit); // If the condition fails, jump to exit

    Scope(statement);
    uint32_t join_index = Instruct(ir::Opcode::Jump); // Placeholder
    Instruct(ir::Opcode::Label, exit); // Anchor the exit label here to handle the else

    Close(statement);

    // Optional ELSE
    if (cursor.Peek() == ir::Token::Else) {
      ir::Index end = ElseStatement(parent); // Gets the last exit label of the else-if chain
      instructions[join_index].Operand(0, end); // Patch jump to the end of the else-if chain
    } else {
      instructions[join_index].Operand(0, exit); // Patch jump to the exit label
    }

    cursor.Match(ir::Token::Semicolon);
    return statement;
  }

  ir::Index Parser::IsStatement(ir::Index parent, ir::Index subject, ir::Index when_end) {
    ir::Index entry = Create(ir::symbol::Type::Label, parent);
    ir::Index exit = Create(ir::symbol::Type::Label, parent);

    if (cursor.Match(ir::Token::Is)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::ParserExpectedIsKeyword, parent);
    }

    ir::Index cond = Condition(parent);
    Instruct(ir::Opcode::BranchIs, subject, cond, entry); // If the `is` check passes, jump to entry

    while (!cursor.Done()) {
      if (cursor.Match(ir::Token::Is)) {
        WhiteSpace(parent);
        ir::Index cond = Condition(parent);
        Instruct(ir::Opcode::BranchIs, subject, cond, entry); // If the `is` check passes, jump to entry
      } else if (cursor.Match(ir::Token::Has)) {
        WhiteSpace(parent);
        ir::Index cond = Condition(parent);
        Instruct(ir::Opcode::BranchHas, subject, cond, entry); // If the `has` check passes, jump to entry
      } else if (cursor.Match(ir::Token::From)) {
        WhiteSpace(parent);
        ir::Index cond = Condition(parent);
        Instruct(ir::Opcode::BranchFrom, subject, cond, entry); // If the `from` check passes, jump to entry
      } else {
        break;
      }
    }

    Instruct(ir::Opcode::Jump, exit); // If none of the checks passed, jump to exit
    Instruct(ir::Opcode::Label, entry); // Bind it here at the start of the scope
    Scope(parent);
    Instruct(ir::Opcode::Jump, when_end); // After executing body, skip remaining arms
    Instruct(ir::Opcode::Label, exit); // Bind it here at the end of the scope

    return exit;
  }

  ir::Index Parser::HasStatement(ir::Index parent, ir::Index subject, ir::Index when_end) {
    // has (cond1) [has|is|from (condN)] { body }
    ir::Index entry = Create(ir::symbol::Type::Label, parent);
    ir::Index exit = Create(ir::symbol::Type::Label, parent);

    if (cursor.Match(ir::Token::Has)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::ParserExpectedHasKeyword, parent);
    }

    ir::Index cond = Condition(parent);
    Instruct(ir::Opcode::BranchHas, subject, cond, entry); // Jump to entry if it passes

    while (!cursor.Done()) {
      if (cursor.Match(ir::Token::Is)) {
        WhiteSpace(parent);
        ir::Index cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchIs, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::Has)) {
        WhiteSpace(parent);
        ir::Index cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchHas, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::From)) {
        WhiteSpace(parent);
        ir::Index cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchFrom, subject, cond2, entry);
      } else {
        break;
      }
    }

    Instruct(ir::Opcode::Jump, exit);
    Instruct(ir::Opcode::Label, entry);
    Scope(parent);
    Instruct(ir::Opcode::Jump, when_end);
    Instruct(ir::Opcode::Label, exit);
    return exit;
  }

  ir::Index Parser::FromStatement(ir::Index parent, ir::Index subject, ir::Index when_end) {
    // from (cond1) [has|is|from (condN)] { body }
    ir::Index entry = Create(ir::symbol::Type::Label, parent);
    ir::Index exit = Create(ir::symbol::Type::Label, parent);

    if (cursor.Match(ir::Token::From)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::ParserExpectedFromKeyword, parent);
    }

    ir::Index cond = Condition(parent);
    Instruct(ir::Opcode::BranchFrom, subject, cond, entry); // Jump to entry if it passes

    while (!cursor.Done()) {
      if (cursor.Match(ir::Token::Is)) {
        WhiteSpace(parent);
        ir::Index cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchIs, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::Has)) {
        WhiteSpace(parent);
        ir::Index cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchHas, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::From)) {
        WhiteSpace(parent);
        ir::Index cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchFrom, subject, cond2, entry);
      } else {
        break;
      }
    }

    Instruct(ir::Opcode::Jump, exit);
    Instruct(ir::Opcode::Label, entry);
    Scope(parent);
    Instruct(ir::Opcode::Jump, when_end);
    Instruct(ir::Opcode::Label, exit);
    return exit;
  }

  ir::Index Parser::DefaultStatement(ir::Index parent, ir::Index when_end) {
    if (!cursor.Match(ir::Token::Default)) {
      return ReportError(ir::Error::ParserExpectedDefaultKeyword, parent);
    }

    WhiteSpace(parent);

    Scope(parent);

    Instruct(ir::Opcode::Jump, when_end);

    return ir::Index{};
  }

  ir::Index Parser::WhenStatement(ir::Index parent) {
    // Simplified: default must be last; each arm has a body; no cross-arm fallthrough.
    ir::Index statement = Open(ir::symbol::Type::When, parent);

    if (!cursor.Match(ir::Token::When)) {
      return ReportError(ir::Error::ParserExpectedWhenKeyword, statement);
    }

    WhiteSpace(statement);

    ir::Index subject = Condition(statement);
    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::ScopeOpen)) {
      ReportError(ir::Error::ParserExpectedWhenScopeOpen, statement);
    }
    WhiteSpace(statement);

    ir::Index when_end = Create(ir::symbol::Type::Label, statement);
    bool default_seen = false;

    while (!cursor.Done() && cursor.Peek() != ir::Token::ScopeClose) {
      if (cursor.Peek() == ir::Token::Default) {
        // default must be last
        if (default_seen) {
          ReportError(ir::Error::ParserDuplicateDefaultInWhen, statement);
        }
        default_seen = true;

        DefaultStatement(statement, when_end);
        WhiteSpace(statement);

        // No more arms allowed after default
        break;
      }

      switch (cursor.Peek()) {
        case ir::Token::Is:
          IsStatement(statement, subject, when_end);
          break;
        case ir::Token::Has:
          HasStatement(statement, subject, when_end);
          break;
        case ir::Token::From:
          FromStatement(statement, subject, when_end);
          break;
        default: {
          ReportError(ir::Error::ParserExpectedWhenArm, statement);
          cursor.Advance();
          WhiteSpace(statement);
          continue;
        }
      }

      WhiteSpace(statement);
    }

    if (!cursor.Match(ir::Token::ScopeClose)) {
      ReportError(ir::Error::ParserExpectedWhenScopeClose, statement);
    }
    WhiteSpace(statement);
    cursor.Match(ir::Token::Semicolon);

    Instruct(ir::Opcode::Label, when_end);
    return Close(statement);
  }

  ir::Index Parser::DeclarationStatement(ir::Index parent) {
    // One or more modifier tokens, such as `Let`, `Const`, `Mutable`, `Static`, `Private`, `Public`, `Protected`, or `Export`
    // A required identifier
    // An optional type annotation, beginning with a `TypeStart` token then a required expression
    // A required assignment, beginning with `Assign` token then an `Expression` group
    // A required `Semicolon` token
    // Return the expression symbol created by the assignment

    // NOTE: We do not create a symbol for the declaration itself, we use the symbol from the expression
    while (IsModifierToken(cursor.Peek())) {
      cursor.Advance();
      WhiteSpace(parent);
    }

    ir::Index target = Identifier(parent);

    if (cursor.Match(ir::Token::TypeStart)) {
      WhiteSpace(parent);
      Expression(parent);
    }

    bool has_assign = cursor.Match(ir::Token::Assign);
    if (!has_assign) {
      return ReportError(ir::Error::ParserExpectedInitializerInDeclaration, parent);
    }

    ir::Index value = Expression(parent);

    if (cursor.Match(ir::Token::Semicolon)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::ParserExpectedSemicolonAfterDeclaration, parent);
    }

    return value;
  }

  ir::Index Parser::ExpressionStatement(ir::Index parent) {
    // A required `Expression` group
    // A required `Semicolon` token
    // Return the expression symbol

    ir::Index value = Expression(parent);

    if (!cursor.Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterExpression, parent);
    }

    return value;
  }

  ir::Index Parser::Statement(ir::Index parent) {
    switch (cursor.Peek()) {
      case ir::Token::Import: return ImportStatement(parent);
      case ir::Token::Register: return RegisterStatement(parent);
      case ir::Token::Break: return BreakStatement(parent);
      case ir::Token::Continue: return ContinueStatement(parent);
      case ir::Token::Return: return ReturnStatement(parent);
      case ir::Token::Do: return DoStatement(parent);
      case ir::Token::While: return WhileStatement(parent);
      case ir::Token::Repeat: return RepeatStatement(parent);
      case ir::Token::For: return ForStatement(parent);
      case ir::Token::If: return IfStatement(parent);
      case ir::Token::When: return WhenStatement(parent);

      // Modifier keywords that begin a declaration statement
      case ir::Token::Let:
      case ir::Token::Const:
      case ir::Token::Mutable:
      case ir::Token::Static:
      case ir::Token::Private:
      case ir::Token::Public:
      case ir::Token::Protected:
      case ir::Token::Export: return DeclarationStatement(parent);
      default: return ExpressionStatement(parent);
    }
  }

  ir::Index Parser::Scope(ir::Index parent) {
    if (cursor.Match(ir::Token::ScopeOpen)) {
      WhiteSpace(parent);
    } else {
      ReportError(ir::Error::ParserExpectedScopeOpen, parent);
    }

    while (!cursor.Done() && cursor.Peek() != ir::Token::ScopeClose) {
      Statement(parent);
      WhiteSpace(parent);
    }

    if (cursor.Match(ir::Token::ScopeClose)) {
      WhiteSpace(parent);
    } else {
      ReportError(ir::Error::ParserExpectedScopeClose, parent);
    }

    return ir::Index{};
  }

  ir::Index Parser::Condition(ir::Index parent) {
    // A required `ConditionOpen` token
    // A required `Expression` group
    // A required `ConditionClose` token
    // Return the expression symbol

    if (!cursor.Match(ir::Token::ConditionOpen)) {
      ReportError(ir::Error::ParserExpectedConditionOpen, parent);
    }

    ir::Index expr = Expression(parent);

    if (!cursor.Match(ir::Token::ConditionClose)) {
      ReportError(ir::Error::ParserExpectedConditionClose, parent);
    }

    return expr;
  }
};