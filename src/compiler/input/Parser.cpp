import compiler.input.Parser;
import compiler.ir.Symbols;
import compiler.ir.Error;
import compiler.ir.Opcode;
import compiler.ir.symbol.Kind;
import compiler.ir.symbol.Flag;
import compiler.ir.Token;
import compiler.ir.Symbol;
import compiler.text.cursor.String;
import compiler.text.Unicode;
import compiler.utility.Print;
import <cstdint>;
import <algorithm>;
import <limits>;
import <vector>;
import <iterator>;
import <utility>;
import <cstddef>;
import <stdexcept>;

namespace compiler::input {
  void Parser::PushDeclarationScope(ir::Symbol scope) {
    declarations.push_back(scope);
  }

  void Parser::PopDeclarationScope(ir::Symbol scope) {
    while (!declarations.empty()) {
      const ir::Symbol top = declarations.back();
      declarations.pop_back();
      if (top == scope) {
        return;
      }
    }
  }

  void Parser::PushScopeLevelDeclaration(ir::Symbol parent, ir::Symbol declaration) {
    if (!declaration) return;
    if (!symbols.IsScope(parent)) return;
    declarations.push_back(declaration);
  }

  int Parser::Precedence(ir::Token token) const {
    switch (token) {
      // Unary postfix operators
      case ir::Token::Guard: // x?
        return 19;

      // Property / member access
      case ir::Token::MemberReference: // x.y
      case ir::Token::MutableMemberReference: // x:y
      case ir::Token::StaticMemberReference: // x::y
        return 18;

      // Function calls
      case ir::Token::ParameterOpen: // x(){ return y; }
      case ir::Token::CaptureOpen: // x[]{ return y; }
      // case ir::Token::ArrowHead: // x() => y - TODO: Unsure about this
      case ir::Token::TupleOpen: // x(y)
      case ir::Token::ArrayOpen: // x[y]
      case ir::Token::ObjectOpen: // x{y}
      case ir::Token::EnumOpen: // x<y>
      case ir::Token::CharacterOpen: // x'y'
      case ir::Token::StringOpen: // x"y"
      case ir::Token::TemplateStringOpen: // x"y" - uses the same syntax as strings
        return 17;

      // Unary prefix operators
      case ir::Token::BitwiseNot: // ~x
      case ir::Token::Increment: // ++x
      case ir::Token::Positive: // +x
      case ir::Token::Decrement: // --x
      case ir::Token::Negative: // -x
      case ir::Token::ReferenceOf: // &x
      case ir::Token::MutableReferenceOf: // *x
      case ir::Token::SymbolOf: // $x
      case ir::Token::LengthOf: // #x
      case ir::Token::CopyOf: // @x
      case ir::Token::Not: // !x
      case ir::Token::Spread: // ...x (prefix spread)
        return 16;

      // Unary prefix keyword operators
      case ir::Token::Await: // await x
      case ir::Token::Expect: // expect x
      case ir::Token::Assume: // assume x
      case ir::Token::Comtime: // comtime x
      case ir::Token::Runtime: // runtime x
        return 15;

      // Multiplicative
      case ir::Token::Multiply: // x * y
      case ir::Token::Divide: // x / y
      case ir::Token::Modulo: // x % y
        return 14;

      // Additive
      case ir::Token::Add: // x + y
      case ir::Token::Subtract: // x - y
        return 13;

      // Shifts
      case ir::Token::BitwiseLeftShift: // x << y
      case ir::Token::BitwiseRightShift: // x >> y
      case ir::Token::BitwiseTripleLeftShift: // x <<< y
      case ir::Token::BitwiseTripleRightShift: // x >>> y
        return 12;

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
        return 6;
      case ir::Token::Or: // x || y
        return 5;

      // Range construction
      case ir::Token::Range: // x..y
        return 4;

      case ir::Token::Pipeline: // x -> y
        return 3;

      // Assignment family
      case ir::Token::Assign: // =
      case ir::Token::AssignAnd: // &&=
      case ir::Token::AssignOr: // ||=
      case ir::Token::AssignAdd: // +=
      case ir::Token::AssignSubtract: // -=
      case ir::Token::AssignMultiply: // *=
      case ir::Token::AssignDivide: // /=
      case ir::Token::AssignModulo: // %=
      case ir::Token::BitwiseAssignAnd: // &=
      case ir::Token::BitwiseAssignXor: // ^=
      case ir::Token::BitwiseAssignOr: // |=
      case ir::Token::BitwiseAssignLeftShift: // <<=
      case ir::Token::BitwiseAssignRightShift: // >>=
      case ir::Token::BitwiseAssignTripleLeftShift: // <<<=
      case ir::Token::BitwiseAssignTripleRightShift: // >>>=
        return 2;

      default:
        return -1; // None / unhandled token has no precedence
    }
  }

  bool Parser::IsRightAssociative(ir::Token token) const {
    switch (token) {
      case ir::Token::Assign: // x = y
      case ir::Token::AssignAnd: // x &&= y
      case ir::Token::AssignOr: // x ||= y
      case ir::Token::AssignAdd: // x += y
      case ir::Token::AssignSubtract: // x -= y
      case ir::Token::AssignMultiply: // x *= y
      case ir::Token::AssignDivide: // x /= y
      case ir::Token::AssignModulo: // x %= y
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

  bool Parser::Match(ir::Token token, ir::Symbol parent) {
    if (cursor.Match(token)) {
      WhiteSpace(parent);
      return true;
    }

    return false;
  }

  bool Parser::Expect(ir::Token token, ir::Symbol parent) {
    if (cursor.Match(token)) {
      WhiteSpace(parent);
      return true;
    }

    if (cursor.Match(ir::Token::Error)) {
      WhiteSpace(parent);
      return false;
    }

    throw compiler::utility::Error("Expected token ", token, " not found for symbol ", parent, ". Actually found ", cursor.Peek(), ".");
  }

  void Parser::SlideDrop(ir::Symbol symbol) {
    if (instructions.empty()) return;

    int found = -1;
    for (int i = static_cast<int>(instructions.size()) - 1; i >= 0; --i) {
      const auto& ins = instructions[static_cast<size_t>(i)];
      if (!ins.Is(ir::Opcode::Drop)) continue;
      if (ins.Primary() == symbol || ins.Secondary() == symbol || ins.Result() == symbol) {
        found = i;
        break;
      }
    }

    if (found < 0) return;

    ir::Instruction moved = instructions[static_cast<size_t>(found)];
    instructions.erase(instructions.begin() + found);
    instructions.push_back(moved);
  }

  std::pair<uint32_t, bool> Parser::ConsumeTextRun() {
    auto characters_digit = [](ir::Token token) constexpr -> uint32_t {
      return static_cast<uint32_t>(token) - static_cast<uint32_t>(ir::Token::Characters0);
    };

    uint32_t length = 0;
    bool ok = true;

    while (!cursor.Done()) {
      const ir::Token token = cursor.Peek();

      if (IsCharactersToken(token)) {
        uint32_t run_length = 0;
        while (!cursor.Done() && IsCharactersToken(cursor.Peek())) {
          run_length = (run_length << 4) | characters_digit(cursor.Peek());
          cursor.Advance();
        }

        const size_t available = (data_index < data.size()) ? (data.size() - data_index) : 0;
        const size_t to_consume = std::min(static_cast<size_t>(run_length), available);
        data_index += to_consume;
        length += static_cast<uint32_t>(to_consume);

        if (to_consume != static_cast<size_t>(run_length)) {
          ok = false;
        }

        continue;
      }

      if (IsEscapeToken(token)) {
        cursor.Advance();
        if (data_index < data.size()) {
          data_index += 1;
          length += 1;
        } else {
          ok = false;
        }
        continue;
      }

      break;
    }

    return {length, ok};
  }

  ir::Symbol Parser::MakeTemporary(ir::Symbol parent, ir::Symbol lhs, ir::Symbol rhs) {
    (void)lhs;
    (void)rhs;
    ir::Symbol result = Create(ir::symbol::Kind::Auto, parent);

    return result;
  }

  ir::Symbol Parser::Lookup(ir::Symbol parent) {
    const uint32_t token_start = static_cast<uint32_t>(tokens.size() - cursor.Size());
    const uint32_t needle_start = static_cast<uint32_t>(data_index);
    const auto [needle_len, ok] = ConsumeTextRun();

    if (!ok) {
      ir::Symbol error = ReportError(ir::Error::ParserIdentifierExpectedCharacterData, parent);
      symbols.Token(error, token_start);
      WhiteSpace(parent);
      return error;
    }

    if (needle_len == 0) {
      ir::Symbol error = ReportError(ir::Error::ParserIdentifierExpectedContent, parent);
      symbols.Token(error, token_start);
      WhiteSpace(parent);
      return error;
    }

    auto slice_equals = [&](uint32_t a_start, uint32_t a_len, uint32_t b_start, uint32_t b_len) -> bool {
      if (a_len != b_len) return false;
      if (a_start > data.size() || b_start > data.size()) return false;
      if (static_cast<size_t>(a_start) + a_len > data.size()) return false;
      if (static_cast<size_t>(b_start) + b_len > data.size()) return false;
      for (uint32_t i = 0; i < a_len; ++i) {
        if (data[static_cast<size_t>(a_start) + i] != data[static_cast<size_t>(b_start) + i]) {
          return false;
        }
      }
      return true;
    };

    for (size_t i = declarations.size(); i-- > 0;) {
      const ir::Symbol candidate = declarations[i];
      if (!candidate) continue;

      const ir::Symbol candidate_name = symbols.Name(candidate);
      if (!candidate_name) continue;
      if (symbols.Kind(candidate_name) != ir::symbol::Kind::Identifier) continue;

      const uint32_t candidate_start = symbols.PayloadLow(candidate_name);
      const uint32_t candidate_len = symbols.PayloadHigh(candidate_name);
      if (candidate_len != needle_len) continue;

      if (slice_equals(needle_start, needle_len, candidate_start, candidate_len)) {
        WhiteSpace(parent);
        return candidate;
      }
    }

    ir::Symbol error = ReportError(ir::Error::ParserIdentifierNotDeclared, parent);
    symbols.Token(error, token_start);
    WhiteSpace(parent);
    return error;
  }

  ir::Symbol Parser::ParsePrimary(ir::Symbol parent) {
    switch (cursor.Peek()) {
      case ir::Token::Undefined: return UndefinedLiteral(parent);
      case ir::Token::Null: return NullLiteral(parent);
      case ir::Token::This: return ThisLiteral(parent);
      case ir::Token::That: return ThatLiteral(parent);
      case ir::Token::True:
      case ir::Token::False: return BooleanLiteral(parent);
      case ir::Token::CharacterOpen: return CharacterLiteral(parent);
      case ir::Token::StringOpen: return StringLiteral(parent);
      case ir::Token::TemplateStringOpen: return TemplateStringLiteral(parent);

      case ir::Token::ArrayOpen: return ArrayLiteral(parent);
      case ir::Token::EnumOpen: return EnumLiteral(parent);
      case ir::Token::TupleOpen: return TupleLiteral(parent);
      case ir::Token::ObjectOpen: return ObjectLiteral(parent);

      // Functions may start with a capture list, parameter list, or arrow head
      case ir::Token::CaptureOpen:
      case ir::Token::ParameterOpen:
      case ir::Token::ArrowHead: return FunctionLiteral(parent);

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
      case ir::Token::DigitsF: return DecimalLiteral(parent);

      case ir::Token::HexStart: return HexLiteral(parent);
      case ir::Token::OctalStart: return OctalLiteral(parent);
      case ir::Token::BinaryStart: return BinaryLiteral(parent);

      // Must be an identifier
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
      case ir::Token::EscapeUnicodeBraced6: return Lookup(parent);

      default: return undefined; // Not a primary expression
    }
  }

  ir::Symbol Parser::ParseUnaryPrefix(ir::Symbol parent) {
    const ir::Token op = cursor.Peek();

    if (IsUnaryPrefixToken(op)) {
      cursor.Advance();
      WhiteSpace(parent);

      ir::Symbol operand = ParseUnaryPrefix(parent);
      ir::Symbol result = Copy(operand, parent);

      Instruct(UnaryPrefixOpcode(op), result, operand);

      SlideDrop(operand);
      SlideDrop(result);
      return result;
    }

    return ParsePrimary(parent);
  }

  ir::Symbol Parser::ParseBinary(ir::Symbol parent, int min_precedence) {
    ir::Symbol lhs = ParseUnaryPrefix(parent);

    while (true) {
      const ir::Token op = cursor.Peek();
      const int precedence = Precedence(op);
      if (precedence < min_precedence) break;

      // Postfix/call-like operators
      switch (op) {
        case ir::Token::MemberReference:
        case ir::Token::MutableMemberReference:
        case ir::Token::StaticMemberReference: {
          cursor.Advance();
          WhiteSpace(parent);
          ir::Symbol member = Identifier(parent);
          ir::Symbol result = MakeTemporary(parent, lhs, member);
          Instruct(ir::Opcode::Select, result, lhs, member);
          SlideDrop(lhs);
          SlideDrop(member);
          SlideDrop(result);
          lhs = result;
          continue;
        }

        case ir::Token::TupleOpen: {
          ir::Symbol args = TupleLiteral(parent);
          ir::Symbol result = MakeTemporary(parent, lhs, args);
          Instruct(ir::Opcode::Call, result, lhs, args);
          SlideDrop(lhs);
          SlideDrop(args);
          SlideDrop(result);
          lhs = result;
          continue;
        }

        case ir::Token::ArrayOpen: {
          ir::Symbol args = ArrayLiteral(parent);
          ir::Symbol result = MakeTemporary(parent, lhs, args);
          Instruct(ir::Opcode::Call, result, lhs, args);
          SlideDrop(lhs);
          SlideDrop(args);
          SlideDrop(result);
          lhs = result;
          continue;
        }

        case ir::Token::ObjectOpen: {
          ir::Symbol args = ObjectLiteral(parent);
          ir::Symbol result = MakeTemporary(parent, lhs, args);
          Instruct(ir::Opcode::Call, result, lhs, args);
          SlideDrop(lhs);
          SlideDrop(args);
          SlideDrop(result);
          lhs = result;
          continue;
        }

        case ir::Token::EnumOpen: {
          ir::Symbol args = EnumLiteral(parent);
          ir::Symbol result = MakeTemporary(parent, lhs, args);
          Instruct(ir::Opcode::Call, result, lhs, args);
          SlideDrop(lhs);
          SlideDrop(args);
          SlideDrop(result);
          lhs = result;
          continue;
        }

        case ir::Token::StringOpen: {
          ir::Symbol arg = StringLiteral(parent);
          ir::Symbol result = MakeTemporary(parent, lhs, arg);
          Instruct(ir::Opcode::Call, result, lhs, arg);
          SlideDrop(lhs);
          SlideDrop(arg);
          SlideDrop(result);
          lhs = result;
          continue;
        }

        case ir::Token::TemplateStringOpen: {
          ir::Symbol arg = TemplateStringLiteral(parent);
          ir::Symbol result = MakeTemporary(parent, lhs, arg);
          Instruct(ir::Opcode::Call, result, lhs, arg);
          SlideDrop(lhs);
          SlideDrop(arg);
          SlideDrop(result);
          lhs = result;
          continue;
        }

        default:
          break;
      }

      // If we get here it has a precedence
      if (IsBinaryToken(op)) {
        // So only consume it if it's a binary operator
        // This way we don't consume call-like operators here
        cursor.Advance();
        WhiteSpace(parent);
      }

      ir::Symbol rhs = ParseBinary(parent, precedence + (IsRightAssociative(op) ? 0 : 1));
      ir::Symbol result = Copy(rhs, parent);

      Instruct(BinaryOpcode(op), result, lhs, rhs);

      SlideDrop(lhs);
      SlideDrop(rhs);
      SlideDrop(result);
      lhs = result;
    }

    return lhs;
  }

  ir::Opcode Parser::UnaryPrefixOpcode(ir::Token token) const {
    switch (token) {
      case ir::Token::Negative: return ir::Opcode::Negate;
      case ir::Token::Positive: return ir::Opcode::Move;
      case ir::Token::Not: return ir::Opcode::Not;
      case ir::Token::BitwiseNot: return ir::Opcode::BitwiseNot;
      case ir::Token::ReferenceOf:
      case ir::Token::MutableReferenceOf: return ir::Opcode::AddressOf;
      case ir::Token::SymbolOf: return ir::Opcode::SymbolOf;
      case ir::Token::CopyOf: return ir::Opcode::CopyOf;
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
      case ir::Token::BitwiseAnd: return ir::Opcode::BitwiseAnd;
      case ir::Token::BitwiseOr: return ir::Opcode::BitwiseOr;
      case ir::Token::BitwiseXor: return ir::Opcode::BitwiseExclusiveOr;
      case ir::Token::BitwiseLeftShift: return ir::Opcode::BitwiseShiftLeft;
      case ir::Token::BitwiseRightShift: return ir::Opcode::BitwiseShiftRight;
      case ir::Token::And: return ir::Opcode::And;
      case ir::Token::Or: return ir::Opcode::Or;
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
  ir::Symbol Parser::Create(ir::symbol::Kind type, ir::Symbol parent) {
    ir::Symbol index = symbols.Add(type);
    symbols.Parent(index, parent);
    symbols.Token(index, static_cast<uint32_t>(tokens.size() - cursor.Size()));

    return index;
  }

  ir::Symbol Parser::Copy(ir::Symbol source, ir::Symbol parent) {
    ir::Symbol index = symbols.Copy(source);
    symbols.Parent(index, parent);
    symbols.Token(index, static_cast<uint32_t>(tokens.size() - cursor.Size()));

    return index;
  }

  ir::Symbol Parser::ReportError(ir::Error error, ir::Symbol parent) {
    ir::Symbol index = Create(ir::symbol::Kind::Error, parent);
    symbols.Payload(index, static_cast<uint64_t>(error));
    return index;
  }

  ir::Symbol Parser::Open(ir::symbol::Kind type, ir::Symbol parent) {
    ir::Symbol index = symbols.Add(type);
    symbols.Parent(index, parent);
    symbols.Token(index, static_cast<uint32_t>(tokens.size() - cursor.Size()));

    // Add the index of the symbol and the length, currently 0
    symbols.Payload(index, index.Row(), 0u);

    return index;
  }
  
  ir::Symbol Parser::Close(ir::Symbol index) {
    // Update the payload to store the length of the children
    uint32_t length = static_cast<uint32_t>(symbols.Count() - 1) - index.Row();
    symbols.Payload(index, index.Row(), length);

    return index;
  }

  Parser::Parser(std::vector<ir::Token>&& tokens, std::vector<uint32_t>&& data)
    : tokens{std::move(tokens)}
    , data{std::move(data)}
    , symbols{}
    , instructions{}
    , cursor{this->tokens}
    , undefined{symbols.Add(ir::symbol::Kind::Undefined)} // Reserve index 0 for `undefined` symbol
    , unresolved{symbols.Add(ir::symbol::Kind::Unresolved)} // Reserve index 1 for `unresolved` symbol
    , identity{symbols.Add(ir::symbol::Kind::Auto)} // Reserve index 2 for `auto` symbol
  {
  }

  ir::Symbol Parser::Parse() {
    ir::Symbol mod = Open(ir::symbol::Kind::Module, ir::Symbol{});
    PushDeclarationScope(mod);

    WhiteSpace(mod); // Skip any leading trivial tokens

    while (!cursor.Done()) {
      const size_t remaining_before = cursor.Size();
      (void)Statement(mod);

      if (cursor.Size() == remaining_before) {
        // Hard guarantee: never allow an infinite loop.
        compiler::utility::Print("Parser skipping token ", cursor.Size(), ": " , cursor.Peek(), ".");
        cursor.Advance();
      }
    }

    PopDeclarationScope(mod);
    Close(mod);
    return mod;
  }

  bool Parser::None(ir::Symbol) {
    if (cursor.Done()) return false;
    cursor.Advance(); // Eat the `None` token
    return true;
  }

  bool Parser::Spaces(ir::Symbol) {
    cursor.Advance(); // Eat the `Spaces*` token
    return true;
  }

  bool Parser::Tabs(ir::Symbol) {
    cursor.Advance(); // Eat the `Tabs*` token
    return true;
  }

  bool Parser::NewLine(ir::Symbol) {
    cursor.Advance();
    return true;
  }

  bool Parser::Comment(ir::Symbol) {
    if (!cursor.Match(ir::Token::CommentOpen)) return false;

    while (!cursor.Done()) {
      ir::Token token = cursor.Peek();

      if (token == ir::Token::CommentClose) {
        cursor.Advance();
        return true;
      }

      if (IsCharactersToken(token) || IsEscapeToken(token)) {
        (void)ConsumeTextRun();
        continue;
      }

      // Comments may (optionally) contain explicit line-break tokens; keep data_index aligned.
      switch (token) {
        case ir::Token::CarriageReturn:
        case ir::Token::LineFeed:
        case ir::Token::LineSeparator:
        case ir::Token::ParagraphSeparator: {
          cursor.Advance();
          if (data_index < data.size()) {
            data_index += 1;
          }
          continue;
        }
        case ir::Token::CarriageReturnLineFeed: {
          cursor.Advance();
          const size_t available = (data_index < data.size()) ? (data.size() - data_index) : 0;
          data_index += std::min<size_t>(2, available);
          continue;
        }
        default:
          break;
      }

      cursor.Advance();
    }

    return true;
  }

  bool Parser::MultiLineComment(ir::Symbol) {
    if (!cursor.Match(ir::Token::MultiLineCommentOpen)) return false;

    while (!cursor.Done()) {
      ir::Token token = cursor.Peek();

      if (token == ir::Token::MultiLineCommentClose) {
        cursor.Advance();
        return true;
      }

      if (IsCharactersToken(token) || IsEscapeToken(token)) {
        (void)ConsumeTextRun();
        continue;
      }

      // The lexer may emit explicit line-break tokens inside multi-line comments.
      switch (token) {
        case ir::Token::CarriageReturn:
        case ir::Token::LineFeed:
        case ir::Token::LineSeparator:
        case ir::Token::ParagraphSeparator: {
          cursor.Advance();
          if (data_index < data.size()) {
            data_index += 1;
          }
          continue;
        }
        case ir::Token::CarriageReturnLineFeed: {
          cursor.Advance();
          const size_t available = (data_index < data.size()) ? (data.size() - data_index) : 0;
          data_index += std::min<size_t>(2, available);
          continue;
        }
        default:
          break;
      }

      cursor.Advance();
    }

    return true;
  }

  bool Parser::WhiteSpaceHelper(ir::Symbol parent) {
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
      default: return false; // Not a trivial token
    }
  }

  bool Parser::WhiteSpace(ir::Symbol parent) {
    size_t count = 0;
    while (!cursor.Done() && IsTrivialToken(cursor.Peek()) && WhiteSpaceHelper(parent)) {
      ++count;
    }

    return count > 0;
  }

  uint32_t Parser::Instruct(ir::Opcode opcode, ir::Symbol res, ir::Symbol lhs, ir::Symbol rhs) {
    uint32_t index = static_cast<uint32_t>(instructions.size());
    instructions.emplace_back(opcode, res, lhs, rhs);
    return index;
  }

  ir::Symbol Parser::UndefinedLiteral(ir::Symbol parent) {
    ir::Symbol symbol = Create(ir::symbol::Kind::Undefined, parent);
    Expect(ir::Token::Undefined, symbol);

    return symbol;
  }

  ir::Symbol Parser::NullLiteral(ir::Symbol parent) {
    ir::Symbol symbol = Create(ir::symbol::Kind::Null, parent);
    Expect(ir::Token::Null, symbol);

    return symbol;
  }

  ir::Symbol Parser::ThisLiteral(ir::Symbol parent) {
    if (topics.size() < 1) {
      ir::Symbol symbol = Create(ir::symbol::Kind::Error, parent);
      symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserInvalidTopicAccess));
      WhiteSpace(parent);
      return symbol;
    }

    Expect(ir::Token::This, parent);

    return topics.back();
  }

  ir::Symbol Parser::ThatLiteral(ir::Symbol parent) {
    if (topics.size() < 2) {
      ir::Symbol symbol = Create(ir::symbol::Kind::Error, parent);
      symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserInvalidTopicAccess));
      WhiteSpace(parent);
      return symbol;
    }

    Expect(ir::Token::That, parent);

    return topics[topics.size() - 2];
  }

  ir::Symbol Parser::BooleanLiteral(ir::Symbol parent) {
    ir::Symbol symbol = Create(ir::symbol::Kind::Boolean, parent);

    if (cursor.Match(ir::Token::True)) {
      symbols.Payload(symbol, static_cast<uint64_t>(true));
    } else if (cursor.Match(ir::Token::False)) {
      symbols.Payload(symbol, static_cast<uint64_t>(false));
    } else if (cursor.Match(ir::Token::Error)) {
      // ... Not sure?
    }

    WhiteSpace(parent);

    return symbol;
  }

  ir::Symbol Parser::CharacterLiteral(ir::Symbol parent) {
    ir::Symbol symbol = Create(ir::symbol::Kind::Character, parent);

    Expect(ir::Token::CharacterOpen, symbol);

    switch (cursor.Peek()) {
      case ir::Token::Characters1:
      case ir::Token::EscapeASCII:
      case ir::Token::EscapeHex:
      case ir::Token::EscapeUnicode:
      case ir::Token::EscapeUnicodeBraced1:
      case ir::Token::EscapeUnicodeBraced2:
      case ir::Token::EscapeUnicodeBraced3:
      case ir::Token::EscapeUnicodeBraced4:
      case ir::Token::EscapeUnicodeBraced5:
      case ir::Token::EscapeUnicodeBraced6:
        cursor.Advance(); // Consume the content token

        if (data_index < data.size()) {
          symbols.Payload(symbol, static_cast<uint64_t>(data[data_index++])); // Consume the data
        } else {
          throw compiler::utility::Error("Character literal missing character data for symbol ", symbol, ".");
        }
        
        Expect(ir::Token::CharacterClose, symbol);
        break;
      case ir::Token::Error:
        cursor.Advance(); // Consume the error token
        WhiteSpace(parent);
        break;
    }

    return symbol;
  }

  ir::Symbol Parser::CanonicalIntegerLiteral(
    ir::Token start_token,
    ir::Error expected_start,
    ir::Error expected_limb_count,
    ir::Error expected_limb_data,
    ir::Symbol parent
  ) {
    ir::Symbol symbol = Create(ir::symbol::Kind::Integer, parent);

    auto digits_value = [](ir::Token token) constexpr -> uint32_t {
      return static_cast<uint32_t>(token) - static_cast<uint32_t>(ir::Token::Digits0);
    };

    if (!cursor.Match(start_token)) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(expected_start));

      WhiteSpace(parent);
      return symbol;
    }

    // Skip the format-mask tokens (Digits* and underscores) until we reach Limbs.
    while (!cursor.Done()) {
      ir::Token token = cursor.Peek();
      if (token == ir::Token::Underscore) {
        cursor.Advance();
        continue;
      }
      if (IsDigitsToken(token)) {
        cursor.Advance();
        continue;
      }
      break;
    }

    uint32_t limb_count = 1;
    if (cursor.Match(ir::Token::Limbs)) {
      limb_count = 0;
      size_t digits_consumed = 0;
      while (!cursor.Done() && IsDigitsToken(cursor.Peek())) {
        limb_count = (limb_count << 4) | digits_value(cursor.Peek());
        cursor.Advance();
        digits_consumed += 1;
      }

      if (digits_consumed == 0) {
        symbols.Kind(symbol, ir::symbol::Kind::Error);
        symbols.Payload(symbol, static_cast<uint64_t>(expected_limb_count));
        limb_count = 0;
      }
    } else {
      // Lexer docs: if Limbs is absent, limb count is implicitly 1.
      limb_count = 1;
    }

    const uint32_t slice_start = static_cast<uint32_t>(data_index);
    const size_t available = (data_index < data.size()) ? (data.size() - data_index) : 0;
    const size_t to_consume = std::min(static_cast<size_t>(limb_count), available);
    data_index += to_consume;

    if (to_consume != static_cast<size_t>(limb_count)) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(expected_limb_data));
    } else if (symbols.Kind(symbol) != ir::symbol::Kind::Error) {
      symbols.Payload(symbol, slice_start, limb_count);
    }

    WhiteSpace(parent);
    return symbol;
  }

  ir::Symbol Parser::HexLiteral(ir::Symbol parent) {
    return CanonicalIntegerLiteral(ir::Token::HexStart,
                                   ir::Error::ParserExpectedHexStart,
                                   ir::Error::ParserHexLiteralExpectedLimbCount,
                                   ir::Error::ParserHexLiteralExpectedLimbData,
                                   parent);
  }

  ir::Symbol Parser::OctalLiteral(ir::Symbol parent) {
    return CanonicalIntegerLiteral(ir::Token::OctalStart,
                                   ir::Error::ParserExpectedOctalStart,
                                   ir::Error::ParserOctalLiteralExpectedLimbCount,
                                   ir::Error::ParserOctalLiteralExpectedLimbData,
                                   parent);
  }

  ir::Symbol Parser::BinaryLiteral(ir::Symbol parent) {
    return CanonicalIntegerLiteral(ir::Token::BinaryStart,
                                   ir::Error::ParserExpectedBinaryStart,
                                   ir::Error::ParserBinaryLiteralExpectedLimbCount,
                                   ir::Error::ParserBinaryLiteralExpectedLimbData,
                                   parent);
  }

  ir::Symbol Parser::DecimalLiteral(ir::Symbol parent) {
    ir::Symbol symbol = Create(ir::symbol::Kind::Integer, parent);

    auto digits_value = [](ir::Token token) constexpr -> uint32_t {
      return static_cast<uint32_t>(token) - static_cast<uint32_t>(ir::Token::Digits0);
    };

    // Decimal literals have no prefix token; they begin with at least one Digits* token.
    if (!IsDigitsToken(cursor.Peek())) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserDecimalLiteralExpectedDigitRun));

      WhiteSpace(parent);
      return symbol;
    }

    bool is_float = false;

    // Skip decimal format tokens until Limbs.
    while (!cursor.Done()) {
      ir::Token token = cursor.Peek();

      if (IsDigitsToken(token) || token == ir::Token::Underscore) {
        cursor.Advance();
        continue;
      }

      if (token == ir::Token::Dot) {
        is_float = true;
        cursor.Advance();
        continue;
      }

      if (token == ir::Token::Exponent) {
        is_float = true;
        cursor.Advance();
        continue;
      }

      // Exponent sign markers are part of the format-mask.
      if (token == ir::Token::Positive || token == ir::Token::Negative) {
        cursor.Advance();
        continue;
      }

      break;
    }

    uint32_t limb_count = 0;
    if (cursor.Match(ir::Token::Limbs)) {
      size_t digits_consumed = 0;
      while (!cursor.Done() && IsDigitsToken(cursor.Peek())) {
        limb_count = (limb_count << 4) | digits_value(cursor.Peek());
        cursor.Advance();
        digits_consumed += 1;
      }

      if (digits_consumed == 0) {
        symbols.Kind(symbol, ir::symbol::Kind::Error);
        symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserDecimalLiteralExpectedLimbCount));
        limb_count = 0;
      }
    } else {
      // Future compact stream: if Limbs is absent, infer the default.
      limb_count = is_float ? 3u : 1u;
    }

    // Adjust kind after we know whether we saw floating-point formatting.
    if (symbols.Kind(symbol) != ir::symbol::Kind::Error) {
      symbols.Kind(symbol, is_float ? ir::symbol::Kind::Float : ir::symbol::Kind::Integer);
    }

    const uint32_t slice_start = static_cast<uint32_t>(data_index);
    const size_t available = (data_index < data.size()) ? (data.size() - data_index) : 0;
    const size_t to_consume = std::min(static_cast<size_t>(limb_count), available);
    data_index += to_consume;

    if (to_consume != static_cast<size_t>(limb_count)) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserDecimalLiteralExpectedLimbData));
    } else if (symbols.Kind(symbol) != ir::symbol::Kind::Error) {
      symbols.Payload(symbol, slice_start, limb_count);
    }

    WhiteSpace(parent);
    return symbol;
  }

  ir::Symbol Parser::StringLiteral(ir::Symbol parent) {
    ir::Symbol symbol = Create(ir::symbol::Kind::String, parent);

    if (cursor.Match(ir::Token::StringOpen)) {
      const uint32_t slice_start = static_cast<uint32_t>(data_index);
      uint32_t length = 0;
      bool closed = false;

      while (!cursor.Done()) {
        ir::Token token = cursor.Peek();

        if (token == ir::Token::StringClose) {
          cursor.Advance();
          closed = true;
          break;
        }

        if (IsCharactersToken(token) || IsEscapeToken(token)) {
          const auto [run_length, ok] = ConsumeTextRun();
          length += run_length;
          if (!ok) {
            symbols.Kind(symbol, ir::symbol::Kind::Error);
            symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserStringLiteralExpectedCharacterData));
            break;
          }
          continue;
        }

        // The lexer may emit explicit line-break tokens inside a string (and store the characters).
        switch (token) {
          case ir::Token::CarriageReturn:
          case ir::Token::LineFeed:
          case ir::Token::LineSeparator:
          case ir::Token::ParagraphSeparator: {
            cursor.Advance();
            if (data_index < data.size()) {
              data_index += 1;
              length += 1;
            } else {
              symbols.Kind(symbol, ir::symbol::Kind::Error);
              symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserStringLiteralExpectedCharacterData));
            }
            continue;
          }
          case ir::Token::CarriageReturnLineFeed: {
            cursor.Advance();
            const size_t available = (data_index < data.size()) ? (data.size() - data_index) : 0;
            const size_t to_consume = std::min<size_t>(2, available);
            data_index += to_consume;
            length += static_cast<uint32_t>(to_consume);
            if (to_consume != 2) {
              symbols.Kind(symbol, ir::symbol::Kind::Error);
              symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserStringLiteralExpectedCharacterData));
            }
            continue;
          }
          default:
            break;
        }

        symbols.Kind(symbol, ir::symbol::Kind::Error);
        symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserStringLiteralExpectedClosingDoubleQuote));
        break;
      }

      if (symbols.Kind(symbol) != ir::symbol::Kind::Error) {
        if (!closed) {
          symbols.Kind(symbol, ir::symbol::Kind::Error);
          symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserStringLiteralExpectedClosingDoubleQuote));
        } else {
          // Payload format: (start, length)
          symbols.Payload(symbol, slice_start, length);
        }
      }
    } else {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserStringLiteralExpectedOpeningDoubleQuote));
    }

    WhiteSpace(parent);
    return symbol;
  }

  ir::Symbol Parser::TemplateStringLiteral(ir::Symbol parent) {
    ir::Symbol tpl = Open(ir::symbol::Kind::TemplateString, parent);

    if (!cursor.Match(ir::Token::TemplateStringOpen)) {
      symbols.Kind(tpl, ir::symbol::Kind::Error);
      symbols.Payload(tpl, static_cast<uint64_t>(ir::Error::ParserTemplateStringLiteralExpectedOpeningBacktick));

      Close(tpl);
      WhiteSpace(parent);
      // No lifetime if we never constructed.
      return tpl;
    }

    uint32_t segment_start = static_cast<uint32_t>(data_index);
    bool closed = false;

    auto emit_segment = [&](bool force) {
      const uint32_t segment_end = static_cast<uint32_t>(data_index);
      const uint32_t segment_length = segment_end - segment_start;
      if (!force && segment_length == 0) {
        return;
      }

      ir::Symbol str = Create(ir::symbol::Kind::String, tpl);
      symbols.Payload(str, segment_start, segment_length);
      // Template string segments are container members; no direct lifetime.

      segment_start = static_cast<uint32_t>(data_index);
    };

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

        const size_t before = tokens.size() - cursor.Size();
        Expression(tpl);
        const size_t after = tokens.size() - cursor.Size();

        if (before == after) {
          // Avoid infinite loops while Expression() is still a stub.
          symbols.Kind(tpl, ir::symbol::Kind::Error);
          symbols.Payload(tpl, static_cast<uint64_t>(ir::Error::TemplateStringLiteralExpectedExpression));
        }

        while (IsTrivialToken(cursor.Peek())) {
          WhiteSpace(tpl);
        }

        if (!cursor.Match(ir::Token::TemplateStringExpressionClose)) {
          symbols.Kind(tpl, ir::symbol::Kind::Error);
          symbols.Payload(tpl, static_cast<uint64_t>(ir::Error::TemplateStringLiteralExpectedClosingBrace));
        }

        segment_start = static_cast<uint32_t>(data_index);
        continue;
      }

      if (IsCharactersToken(token) || IsEscapeToken(token)) {
        const auto [_, ok] = ConsumeTextRun();
        if (!ok) {
          symbols.Kind(tpl, ir::symbol::Kind::Error);
          symbols.Payload(tpl, static_cast<uint64_t>(ir::Error::ParserTemplateStringLiteralExpectedCharacterData));
          break;
        }
        continue;
      }

      switch (token) {
        case ir::Token::CarriageReturn:
        case ir::Token::LineFeed:
        case ir::Token::LineSeparator:
        case ir::Token::ParagraphSeparator: {
          cursor.Advance();
          if (data_index < data.size()) {
            data_index += 1;
          } else {
            symbols.Kind(tpl, ir::symbol::Kind::Error);
            symbols.Payload(tpl, static_cast<uint64_t>(ir::Error::ParserTemplateStringLiteralExpectedCharacterData));
          }
          continue;
        }
        case ir::Token::CarriageReturnLineFeed: {
          cursor.Advance();
          const size_t available = (data_index < data.size()) ? (data.size() - data_index) : 0;
          const size_t to_consume = std::min<size_t>(2, available);
          data_index += to_consume;
          if (to_consume != 2) {
            symbols.Kind(tpl, ir::symbol::Kind::Error);
            symbols.Payload(tpl, static_cast<uint64_t>(ir::Error::ParserTemplateStringLiteralExpectedCharacterData));
          }
          continue;
        }
        default:
          break;
      }

      symbols.Kind(tpl, ir::symbol::Kind::Error);
      symbols.Payload(tpl, static_cast<uint64_t>(ir::Error::TemplateStringLiteralExpectedClosingBacktick));
      break;
    }

    if (symbols.Kind(tpl) != ir::symbol::Kind::Error && !closed) {
      symbols.Kind(tpl, ir::symbol::Kind::Error);
      symbols.Payload(tpl, static_cast<uint64_t>(ir::Error::TemplateStringLiteralExpectedClosingBacktick));
    }

    if (symbols.IsScope(parent)) {
      Instruct(ir::Opcode::Drop, tpl);
    }

    Close(tpl);
    WhiteSpace(parent);
    return tpl;
  }

  ir::Symbol Parser::ArrayLiteral(ir::Symbol parent) {
    // A required `ArrayOpen` token
    // Create an `Array` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `ArrayClose` token
    // Return the symbol

    ir::Symbol symbol = Open(ir::symbol::Kind::Array, parent);

    const auto report_error = [&](ir::Error error) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(error));
    };

    do {
      if (!cursor.Match(ir::Token::ArrayOpen)) {
        report_error(ir::Error::ParserArrayLiteralExpectedOpeningBracket);
        break;
      }

      // Empty arrays are legal.
      if (cursor.Match(ir::Token::ArrayClose)) {
        break;
      }

      while (true) {
        // Disallow commas without an expression, e.g. `[,]` or `[1,,2]`.
        if (cursor.Peek() == ir::Token::Comma) {
          report_error(ir::Error::ParserArrayLiteralUnexpectedComma);
          cursor.Advance();

          // Best-effort recovery: allow closing bracket after the bad comma.
          if (cursor.Match(ir::Token::ArrayClose)) {
            break;
          }
          continue;
        }

        // After a comma, a closing bracket is only valid as a trailing comma.
        if (cursor.Peek() == ir::Token::ArrayClose) {
          report_error(ir::Error::ParserArrayLiteralExpectedExpression);
          cursor.Advance();
          break;
        }

        Expression(symbol);

        if (cursor.Match(ir::Token::Comma)) {
          if (cursor.Match(ir::Token::ArrayClose)) {
            break;
          }
          continue;
        }

        if (cursor.Match(ir::Token::ArrayClose)) {
          break;
        }

        report_error(ir::Error::ParserArrayLiteralExpectedCommaOrClosingBracket);

        // Ensure forward progress on malformed input.
        if (!cursor.Done()) {
          cursor.Advance();
        } else {
          break;
        }
      }
    } while (false);

    if (symbols.IsScope(parent)) {
      Instruct(ir::Opcode::Drop, symbol);
    }

    Close(symbol);
    WhiteSpace(parent);
    return symbol;
  }

  ir::Symbol Parser::EnumLiteral(ir::Symbol parent) {
    // A required `EnumOpen` token
    // Create an `Enum` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `EnumClose` token
    // Return the symbol

    ir::Symbol symbol = Open(ir::symbol::Kind::Enum, parent);

    const auto report_error = [&](ir::Error error) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(error));
    };

    do {
      if (!cursor.Match(ir::Token::EnumOpen)) {
        report_error(ir::Error::ParserEnumLiteralExpectedOpeningChevron);
        break;
      }

      // Empty enums are legal.
      if (cursor.Match(ir::Token::EnumClose)) {
        break;
      }

      while (true) {
        // Disallow commas without an expression, e.g. `<,>` or `<a,,b>`.
        if (cursor.Peek() == ir::Token::Comma) {
          report_error(ir::Error::ParserEnumLiteralUnexpectedComma);
          cursor.Advance();

          // Best-effort recovery: allow closing chevron after the bad comma.
          if (cursor.Match(ir::Token::EnumClose)) {
            break;
          }
          continue;
        }

        // After a comma, a closing chevron is only valid as a trailing comma.
        if (cursor.Peek() == ir::Token::EnumClose) {
          report_error(ir::Error::ParserEnumLiteralExpectedExpression);
          cursor.Advance();
          break;
        }

        Expression(symbol);

        if (cursor.Match(ir::Token::Comma)) {
          if (cursor.Match(ir::Token::EnumClose)) {
            break;
          }
          continue;
        }

        if (cursor.Match(ir::Token::EnumClose)) {
          break;
        }

        report_error(ir::Error::ParserEnumLiteralExpectedCommaOrClosingChevron);

        // Ensure forward progress on malformed input.
        if (!cursor.Done()) {
          cursor.Advance();
        } else {
          break;
        }
      }
    } while (false);

    if (symbols.IsScope(parent)) {
      Instruct(ir::Opcode::Drop, symbol);
    }

    Close(symbol);
    WhiteSpace(parent);
    return symbol;
  }

  ir::Symbol Parser::TupleLiteral(ir::Symbol parent) {
    // A required `TupleOpen` token
    // Create a `Tuple` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `TupleClose` token
    // Return the symbol

    ir::Symbol symbol = Open(ir::symbol::Kind::Tuple, parent);

    const auto report_error = [&](ir::Error error) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(error));
    };

    do {
      if (!cursor.Match(ir::Token::TupleOpen)) {
        report_error(ir::Error::ParserTupleLiteralExpectedOpeningParen);
        break;
      }

      // Empty tuples are legal.
      if (cursor.Match(ir::Token::TupleClose)) {
        break;
      }

      while (true) {
        // Disallow commas without an expression, e.g. `(,)` or `(a,,b)`.
        if (cursor.Peek() == ir::Token::Comma) {
          report_error(ir::Error::ParserTupleLiteralUnexpectedComma);
          cursor.Advance();

          // Best-effort recovery: allow closing paren after the bad comma.
          if (cursor.Match(ir::Token::TupleClose)) {
            break;
          }
          continue;
        }

        // After a comma, a closing paren is only valid as a trailing comma.
        if (cursor.Peek() == ir::Token::TupleClose) {
          report_error(ir::Error::ParserTupleLiteralExpectedExpression);
          cursor.Advance();
          break;
        }

        Expression(symbol);

        if (cursor.Match(ir::Token::Comma)) {
          if (cursor.Match(ir::Token::TupleClose)) {
            break;
          }
          continue;
        }

        if (cursor.Match(ir::Token::TupleClose)) {
          break;
        }

        report_error(ir::Error::ParserTupleLiteralExpectedCommaOrClosingParen);

        // Ensure forward progress on malformed input.
        if (!cursor.Done()) {
          cursor.Advance();
        } else {
          break;
        }
      }
    } while (false);

    if (symbols.IsScope(parent)) {
      Instruct(ir::Opcode::Drop, symbol);
    }

    Close(symbol);
    WhiteSpace(parent);
    return symbol;
  }

  ir::Symbol Parser::ObjectLiteral(ir::Symbol parent) {
    // A required `ObjectOpen` token
    // Create an `Object` symbol to serve as the parent of the subsequent `Declaration` groups
    // Zero or more `Declaration` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `ObjectClose` token
    // Return the symbol

    ir::Symbol symbol = Open(ir::symbol::Kind::Object, parent);
    topics.push_back(symbol); // Add the object to the topic stack, allowing `this` to refer to it

    const auto report_error = [&](ir::Error error) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(error));
    };

    do {
      if (!cursor.Match(ir::Token::ObjectOpen)) {
        report_error(ir::Error::ParserObjectLiteralExpectedOpeningBrace);
        break;
      }

      // Empty objects are legal.
      if (cursor.Match(ir::Token::ObjectClose)) {
        break;
      }

      while (true) {
        // Disallow commas without a declaration, e.g. `{,}` or `{a = 1,, b = 2}`.
        if (cursor.Peek() == ir::Token::Comma) {
          report_error(ir::Error::ParserObjectLiteralUnexpectedComma);
          cursor.Advance();

          // Best-effort recovery: allow closing brace after the bad comma.
          if (cursor.Match(ir::Token::ObjectClose)) {
            break;
          }
          continue;
        }

        if (cursor.Peek() == ir::Token::ObjectClose) {
          report_error(ir::Error::ParserObjectLiteralExpectedDeclaration);
          cursor.Advance();
          break;
        }

        Declaration(symbol);

        if (cursor.Match(ir::Token::Comma)) {
          if (cursor.Match(ir::Token::ObjectClose)) {
            break;
          }
          continue;
        }

        if (cursor.Match(ir::Token::ObjectClose)) {
          break;
        }

        report_error(ir::Error::ParserObjectLiteralExpectedCommaOrClosingBrace);

        // Ensure forward progress on malformed input.
        if (!cursor.Done()) {
          cursor.Advance();
        } else {
          report_error(ir::Error::ParserObjectLiteralExpectedClosingBrace);
          break;
        }
      }
    } while (false);

    if (symbols.IsScope(parent)) {
      Instruct(ir::Opcode::Drop, symbol);
    }

    topics.pop_back();
    Close(symbol);
    WhiteSpace(parent);
    return symbol;
  }

  ir::Symbol Parser::FunctionLiteral(ir::Symbol parent) {
    (void)parent;

    // TODO

    // if (symbols.IsScope(parent)) {
    //   Instruct(ir::Opcode::Drop, symbol);
    // }

    return ir::Symbol{};
  }

  ir::Symbol Parser::Identifier(ir::Symbol parent) {
    ir::Symbol symbol = Create(ir::symbol::Kind::Identifier, parent);

    const uint32_t slice_start = static_cast<uint32_t>(data_index);

    const auto [length, ok] = ConsumeTextRun();
    if (!ok) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserIdentifierExpectedCharacterData));
    }

    if (symbols.Kind(symbol) != ir::symbol::Kind::Error && length == 0) {
      symbols.Kind(symbol, ir::symbol::Kind::Error);
      symbols.Payload(symbol, static_cast<uint64_t>(ir::Error::ParserIdentifierExpectedContent));
    } else if (symbols.Kind(symbol) != ir::symbol::Kind::Error) {
      symbols.Payload(symbol, slice_start, length);
    }

    WhiteSpace(parent);
    return symbol;
  }

  // ir::Symbol Parser::Destructuring(ir::Symbol parent) {
  //   switch (cursor.Peek()) {
  //     case ir::Token::ArrayOpen: return Array(parent);
  //     case ir::Token::TupleOpen: return Tuple(parent);
  //     case ir::Token::ObjectOpen: return Object(parent);
  //     default:
  //       return ReportError(ir::Error::ParserExpectedDestructuringPattern, parent);
  //   }
  // }

  ir::Symbol Parser::Expression(ir::Symbol parent) {
    const auto start_it = cursor.cbegin();
    ir::Symbol expr = ParseBinary(parent, 0);

    // Defensive guarantee to avoid infinite loops on malformed input.
    if (cursor.cbegin() == start_it && !cursor.Done()) {
      throw compiler::utility::Error("Expression failed to progress at ", cursor.Peek(), " with parent ", parent, ".");
    }

    return expr;
  }

  ir::symbol::Flag Parser::Modifiers(ir::Symbol parent) {
    (void)parent;
    switch (cursor.Peek()) {
      case ir::Token::Const:
        cursor.Advance();
        return ir::symbol::Flag::Const;
      case ir::Token::Mut:
        cursor.Advance();
        return ir::symbol::Flag::Mut;
      case ir::Token::Let:
        cursor.Advance();
        return ir::symbol::Flag::Let;
      case ir::Token::Export:
        cursor.Advance();
        return ir::symbol::Flag::Export;
      case ir::Token::Public:
        cursor.Advance();
        return ir::symbol::Flag::None;
      case ir::Token::Private:
        cursor.Advance();
        return ir::symbol::Flag::Private;
      case ir::Token::Protected:
        cursor.Advance();
        return ir::symbol::Flag::Protected;
      default:
        return ir::symbol::Flag::None;
    }
  }

  ir::Symbol Parser::Declaration(ir::Symbol parent) {
    // Create an `Unresolved` symbol to serve as the declaration, we'll set the kind later
    ir::Symbol declaration = Create(ir::symbol::Kind::Unresolved, parent);

    while (!cursor.Done() && IsModifierToken(cursor.Peek())) {
      switch (cursor.Peek()) {
        case ir::Token::Const:
          cursor.Advance();
          symbols.Flag(declaration, ir::symbol::Flag::Const);
          break;
        case ir::Token::Mut:
          cursor.Advance();
          symbols.Flag(declaration, ir::symbol::Flag::Mut);
          break;
        case ir::Token::Let:
          cursor.Advance();
          symbols.Flag(declaration, ir::symbol::Flag::Let);
          break;
        case ir::Token::Export:
          cursor.Advance();
          symbols.Flag(declaration, ir::symbol::Flag::Export);
          break;
        case ir::Token::Public:
          cursor.Advance();
          symbols.Flag(declaration, ir::symbol::Flag::None);
          break;
        case ir::Token::Private:
          cursor.Advance();
          symbols.Flag(declaration, ir::symbol::Flag::Private);
          break;
        case ir::Token::Protected:
          cursor.Advance();
          symbols.Flag(declaration, ir::symbol::Flag::Protected);
          break;
        default:
          throw compiler::utility::Error("Unrecognized modifier token ", cursor.Peek(), " at declaration in ", parent, ".");
      }

      WhiteSpace(parent); // Don't parent to the declaration, it isn't structured
    }

    ir::Symbol name;
    // TODO: Handle destructuring
    if (IsCharactersToken(cursor.Peek()) || IsEscapeToken(cursor.Peek())) {
      name = Identifier(parent);
    } else {
      return ReportError(ir::Error::ParserDeclarationExpectedIdentifier, parent);
    }

    symbols.Name(declaration, name);

    ir::Symbol type = identity;
    if (Match(ir::Token::TypeStart, parent)) {
      type = Expression(parent);
    }

    Expect(ir::Token::Initialize, parent);

    ir::Symbol value = Expression(parent);

    Instruct(ir::Opcode::Call, declaration, type, value);
    // TODO: Set up the declaration's kind based on the type, making it a result of the call

    if (symbols.IsScope(parent)) {
      Instruct(ir::Opcode::Drop, declaration);
    }

    if (name) {
      PushScopeLevelDeclaration(parent, declaration);
    }

    return declaration;
  }

  // Statements
  ir::Symbol Parser::ImportStatement(ir::Symbol parent) {
    ir::Symbol statement = Open(ir::symbol::Kind::Import, parent);

    if (!cursor.Match(ir::Token::Import)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::ImportExpectedIdentifier));
      return Close(statement);
    }

    WhiteSpace(statement);

    // Optional binding (currently: identifier only; destructuring TBD)
    if ((cursor.Peek() >= ir::Token::Characters0 && cursor.Peek() <= ir::Token::CharactersF) || IsEscapeToken(cursor.Peek())) {
      Identifier(statement);
      WhiteSpace(statement);
    }

    if (!cursor.Match(ir::Token::From)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::ImportExpectedFromKeyword));
      return Close(statement);
    }

    WhiteSpace(statement);
    StringLiteral(statement);
    WhiteSpace(statement);

    // Optional target package selector: `in "pkg"`
    if (cursor.Match(ir::Token::In)) {
      WhiteSpace(statement);
      StringLiteral(statement);
      WhiteSpace(statement);
    }

    if (!cursor.Match(ir::Token::Semicolon)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::ImportExpectedSemicolon));
    }

    WhiteSpace(statement);
    return Close(statement);
  }

  ir::Symbol Parser::Permissions(ir::Symbol parent) {
    // with [permissions] in "package"
    if (!cursor.Match(ir::Token::With)) {
      return ReportError(ir::Error::RegisterExpectedIdentifierAfterWith, parent);
    }

    WhiteSpace(parent);
    ArrayLiteral(parent);
    WhiteSpace(parent);

    if (!cursor.Match(ir::Token::In)) {
      return ReportError(ir::Error::RegisterExpectedIdentifierAfterWith, parent);
    }

    WhiteSpace(parent);
    StringLiteral(parent);
    WhiteSpace(parent);
    return ir::Symbol{};
  }

  ir::Symbol Parser::RegisterStatement(ir::Symbol parent) {
    ir::Symbol statement = Open(ir::symbol::Kind::Register, parent);

    if (!cursor.Match(ir::Token::Register)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::RegisterExpectedIdentifier));
      return Close(statement);
    }

    WhiteSpace(statement);
    StringLiteral(statement);
    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::From)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::RegisterExpectedFromKeyword));
      return Close(statement);
    }

    WhiteSpace(statement);
    StringLiteral(statement);
    WhiteSpace(statement);

    while (!cursor.Done() && cursor.Peek() == ir::Token::With) {
      Permissions(statement);
      WhiteSpace(statement);
    }

    if (!cursor.Match(ir::Token::Semicolon)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::RegisterExpectedSemicolon));
    }

    WhiteSpace(statement);
    return Close(statement);
  }

  ir::Symbol Parser::BreakStatement(ir::Symbol parent) {
    if (!cursor.Match(ir::Token::Break)) {
      return ReportError(ir::Error::StatementFoundUnexpectedCharacter, parent);
    }

    WhiteSpace(parent);

    size_t depth = 1;
    while (cursor.Match(ir::Token::Break)) {
      ++depth;
      WhiteSpace(parent);
    }

    if (depth > break_targets.size()) {
      ReportError(ir::Error::ParserBreakOutsideLoop, parent);
      depth = break_targets.size();
    }

    ir::Symbol target{};
    if (depth != 0) {
      target = break_targets[break_targets.size() - depth];
    }

    if (!cursor.Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::BreakStatementExpectedSemicolon, parent);
    }

    WhiteSpace(parent);

    if (target) {
      Instruct(ir::Opcode::Jump, target);
    }

    return ir::Symbol{};
  }

  ir::Symbol Parser::ContinueStatement(ir::Symbol parent) {
    Expect(ir::Token::Continue, parent);

    size_t depth = 1;
    while (Match(ir::Token::Continue, parent)) {
      ++depth;
    }

    if (depth > continue_targets.size()) {
      ReportError(ir::Error::ParserContinueOutsideLoop, parent);
      depth = continue_targets.size();
    }

    ir::Symbol target{};
    if (depth != 0) {
      target = continue_targets[continue_targets.size() - depth];
    }

    Expect(ir::Token::Semicolon, parent);

    if (target) {
      Instruct(ir::Opcode::Jump, target);
    }

    return ir::Symbol{};
  }

  ir::Symbol Parser::ReturnStatement(ir::Symbol parent) {
    Expect(ir::Token::Return, parent);

    ir::Symbol value = undefined;
    if (cursor.Peek() != ir::Token::Semicolon) {
      value = Expression(parent);
    }

    Expect(ir::Token::Semicolon, parent);

    Instruct(ir::Opcode::Return, value);
    return value;
  }

  ir::Symbol Parser::YieldStatement(ir::Symbol parent) {
    Expect(ir::Token::Yield, parent);

    ir::Symbol value = undefined;
    if (cursor.Peek() != ir::Token::Semicolon) {
      value = Expression(parent);
    }

    Expect(ir::Token::Semicolon, parent);

    Instruct(ir::Opcode::Yield, value);
    return value;
  }

  ir::Symbol Parser::PanicStatement(ir::Symbol parent) {
    Expect(ir::Token::Panic, parent);

    ir::Symbol value = undefined;
    if (cursor.Peek() != ir::Token::Semicolon) {
      value = Expression(parent);
    }

    Expect(ir::Token::Semicolon, parent);

    Instruct(ir::Opcode::Panic, value);
    return value;
  }

  ir::Symbol Parser::DoStatement(ir::Symbol parent) {
    ir::Symbol statement = Open(ir::symbol::Kind::Do, parent);

    Expect(ir::Token::Do, statement);

    ScopeOrInlineStatement(statement);

    return Close(statement);
  }

  ir::Symbol Parser::WhileStatement(ir::Symbol parent) {
    // while (condition) { body } [;]
    ir::Symbol statement = Open(ir::symbol::Kind::While, parent);

    Expect(ir::Token::While, statement);

    ir::Symbol condition_label = Create(ir::symbol::Kind::Label, statement);
    ir::Symbol exit = Create(ir::symbol::Kind::Label, statement);

    break_targets.push_back(exit);
    continue_targets.push_back(condition_label);

    Instruct(ir::Opcode::Label, condition_label);
    ir::Symbol cond = Condition(statement);
    WhiteSpace(statement);

    Instruct(ir::Opcode::Branch, cond, exit);

    ScopeOrInlineStatement(statement);

    Instruct(ir::Opcode::Jump, condition_label);
    Instruct(ir::Opcode::Label, exit);

    continue_targets.pop_back();
    break_targets.pop_back();
    return Close(statement);
  }

  ir::Symbol Parser::RepeatStatement(ir::Symbol parent) {
    // repeat { body } [while (condition)] [;]
    ir::Symbol statement = Open(ir::symbol::Kind::Repeat, parent);

    Expect(ir::Token::Repeat, statement);

    ir::Symbol entry = Create(ir::symbol::Kind::Label, statement);
    ir::Symbol exit = Create(ir::symbol::Kind::Label, statement);
    ir::Symbol continue_label = Create(ir::symbol::Kind::Label, statement);

    break_targets.push_back(exit);
    continue_targets.push_back(continue_label);

    Instruct(ir::Opcode::Label, entry);
    uint32_t continue_label_instruction = Instruct(ir::Opcode::Label, continue_label);

    PushDeclarationScope(statement);
    Scope(statement);
    PopDeclarationScope(statement);

    if (cursor.Match(ir::Token::While)) {
      // repeat ... while (condition)
      symbols.Kind(statement, ir::symbol::Kind::RepeatWhile);
      WhiteSpace(statement);

      if (continue_label_instruction < instructions.size()) {
        instructions.erase(instructions.begin() + continue_label_instruction);
      }

      continue_label_instruction = Instruct(ir::Opcode::Label, continue_label);

      ir::Symbol cond = Condition(statement);
      Instruct(ir::Opcode::Fork, cond, exit, entry);
    } else {
      Instruct(ir::Opcode::Jump, entry);
    }

    Instruct(ir::Opcode::Label, exit);

    continue_targets.pop_back();
    break_targets.pop_back();
    return Close(statement);
  }

  ir::Symbol Parser::ForStatement(ir::Symbol parent) {
    // for (binding in iterable) { body } [;]
    ir::Symbol statement = Open(ir::symbol::Kind::For, parent);

    if (!cursor.Match(ir::Token::For)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::ForExpectedBinding));
      return Close(statement);
    }

    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::ConditionOpen)) {
      ReportError(ir::Error::ConditionExpectedClosingParenthesis, statement);
    } else {
      WhiteSpace(statement);
    }

    // Best-effort: treat the binding as a declaration.
    Declaration(statement);
    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::In)) {
      ReportError(ir::Error::ForExpectedInKeyword, statement);
    } else {
      WhiteSpace(statement);
    }

    Expression(statement);
    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::ConditionClose)) {
      ReportError(ir::Error::ConditionExpectedClosingParenthesis, statement);
    }

    WhiteSpace(statement);

    ir::Symbol body = Create(ir::symbol::Kind::Label, statement);
    ir::Symbol exit = Create(ir::symbol::Kind::Label, statement);

    break_targets.push_back(exit);
    continue_targets.push_back(body);

    Instruct(ir::Opcode::Label, body);

    PushDeclarationScope(statement);

    if (cursor.Peek() == ir::Token::ScopeOpen) {
      Scope(statement);
    } else {
      Statement(statement);
    }

    PopDeclarationScope(statement);

    WhiteSpace(statement);
    cursor.Match(ir::Token::Semicolon);
    WhiteSpace(statement);
    Instruct(ir::Opcode::Label, exit);

    continue_targets.pop_back();
    break_targets.pop_back();
    return Close(statement);
  }

  ir::Symbol Parser::ElseStatement(ir::Symbol parent) {
    ir::Symbol statement = Open(ir::symbol::Kind::Else, parent);
    
    Expect(ir::Token::Else, statement);
    ir::Symbol exit = Create(ir::symbol::Kind::Label, statement);

    bool has_if = false;
    if (cursor.Match(ir::Token::If)) {
      has_if = true;
      symbols.Kind(statement, ir::symbol::Kind::ElseIf);
      WhiteSpace(statement);

      ir::Symbol cond = Condition(statement);
      Instruct(ir::Opcode::Branch, cond, exit);
      WhiteSpace(statement);
    }

    ScopeOrInlineStatement(statement);

    uint32_t join_index = 0;
    if (has_if) {
      join_index = Instruct(ir::Opcode::Jump);
    }

    Instruct(ir::Opcode::Label, exit);
    Close(statement);

    if (has_if && cursor.Peek() == ir::Token::Else) {
      ir::Symbol end = ElseStatement(parent);
      instructions[join_index].SetOperand(0, end);
      return end;
    }

    if (has_if) {
      instructions[join_index].SetOperand(0, exit);
    }

    return exit;
  }

  ir::Symbol Parser::IfStatement(ir::Symbol parent) {
    ir::Symbol statement = Open(ir::symbol::Kind::If, parent);
    
    Expect(ir::Token::If, statement);
    
    ir::Symbol cond = Condition(statement);
    
    ir::Symbol exit = Create(ir::symbol::Kind::Label, statement);
    Instruct(ir::Opcode::Branch, cond, exit);

    // TODO: Handle patterns

    ScopeOrInlineStatement(statement);

    uint32_t join_index = Instruct(ir::Opcode::Jump);
    Instruct(ir::Opcode::Label, exit);
    Close(statement);

    if (cursor.Peek() == ir::Token::Else) {
      ir::Symbol end = ElseStatement(parent);
      instructions[join_index].SetOperand(0, end);
    } else {
      instructions[join_index].SetOperand(0, exit);
    }

    return statement;
  }

  ir::Symbol Parser::IsStatement(ir::Symbol parent, ir::Symbol subject, ir::Symbol when_end) {
    ir::Symbol entry = Create(ir::symbol::Kind::Label, parent);
    ir::Symbol exit = Create(ir::symbol::Kind::Label, parent);

    if (cursor.Match(ir::Token::Is)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::IsPatternExpectedCondition, parent);
    }

    ir::Symbol cond = Condition(parent);
    Instruct(ir::Opcode::BranchIs, subject, cond, entry);

    while (!cursor.Done()) {
      if (cursor.Match(ir::Token::Is)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchIs, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::Has)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchHas, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::From)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
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

  ir::Symbol Parser::HasStatement(ir::Symbol parent, ir::Symbol subject, ir::Symbol when_end) {
    ir::Symbol entry = Create(ir::symbol::Kind::Label, parent);
    ir::Symbol exit = Create(ir::symbol::Kind::Label, parent);

    if (cursor.Match(ir::Token::Has)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::HasPatternExpectedCondition, parent);
    }

    ir::Symbol cond = Condition(parent);
    Instruct(ir::Opcode::BranchHas, subject, cond, entry);

    while (!cursor.Done()) {
      if (cursor.Match(ir::Token::Is)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchIs, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::Has)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchHas, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::From)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
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

  ir::Symbol Parser::FromStatement(ir::Symbol parent, ir::Symbol subject, ir::Symbol when_end) {
    ir::Symbol entry = Create(ir::symbol::Kind::Label, parent);
    ir::Symbol exit = Create(ir::symbol::Kind::Label, parent);

    if (cursor.Match(ir::Token::From)) {
      WhiteSpace(parent);
    } else {
      return ReportError(ir::Error::FromPatternExpectedCondition, parent);
    }

    ir::Symbol cond = Condition(parent);
    Instruct(ir::Opcode::BranchFrom, subject, cond, entry);

    while (!cursor.Done()) {
      if (cursor.Match(ir::Token::Is)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchIs, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::Has)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
        Instruct(ir::Opcode::BranchHas, subject, cond2, entry);
      } else if (cursor.Match(ir::Token::From)) {
        WhiteSpace(parent);
        ir::Symbol cond2 = Condition(parent);
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

  ir::Symbol Parser::DefaultStatement(ir::Symbol parent, ir::Symbol when_end) {
    if (!cursor.Match(ir::Token::Default)) {
      return ReportError(ir::Error::StatementFoundUnexpectedCharacter, parent);
    }

    WhiteSpace(parent);
    Scope(parent);
    Instruct(ir::Opcode::Jump, when_end);
    return ir::Symbol{};
  }

  ir::Symbol Parser::WhenStatement(ir::Symbol parent) {
    ir::Symbol statement = Open(ir::symbol::Kind::When, parent);

    if (!cursor.Match(ir::Token::When)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::StatementFoundUnexpectedCharacter));
      return Close(statement);
    }

    WhiteSpace(statement);
    ir::Symbol subject = Condition(statement);
    WhiteSpace(statement);

    if (!cursor.Match(ir::Token::ScopeOpen)) {
      symbols.Kind(statement, ir::symbol::Kind::Error);
      symbols.Payload(statement, static_cast<uint64_t>(ir::Error::WhileExpectedScopeBlock));
      return Close(statement);
    }

    WhiteSpace(statement);
    PushDeclarationScope(statement);

    ir::Symbol when_end = Create(ir::symbol::Kind::Label, statement);
    bool default_seen = false;

    while (!cursor.Done() && cursor.Peek() != ir::Token::ScopeClose) {
      if (cursor.Peek() == ir::Token::Default) {
        if (default_seen) {
          ReportError(ir::Error::ParserDuplicateDefaultInWhen, statement);
        }
        default_seen = true;
        DefaultStatement(statement, when_end);
        WhiteSpace(statement);
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
        default:
          ReportError(ir::Error::ParserExpectedWhenArm, statement);
          cursor.Advance();
          WhiteSpace(statement);
          continue;
      }

      WhiteSpace(statement);
    }

    if (!cursor.Match(ir::Token::ScopeClose)) {
      ReportError(ir::Error::BlockExpectedClosingCurlyBrace, statement);
    }

    WhiteSpace(statement);
    cursor.Match(ir::Token::Semicolon);
    WhiteSpace(statement);
    Instruct(ir::Opcode::Label, when_end);
    PopDeclarationScope(statement);
    return Close(statement);
  }

  ir::Symbol Parser::DeclarationStatement(ir::Symbol parent) {
    ir::Symbol value = Declaration(parent);

    if (!cursor.Match(ir::Token::Semicolon)) {
      return ReportError(ir::Error::DeclarationStatementExpectedSemicolon, parent);
    }

    WhiteSpace(parent);
    return value;
  }

  ir::Symbol Parser::ExpressionStatement(ir::Symbol parent) {
    if (Match(ir::Token::Semicolon, parent)) {
      // Empty expression statement
      return undefined;
    }

    ir::Symbol value = Expression(parent);

    Expect(ir::Token::Semicolon, parent);

    return value;
  }

  ir::Symbol Parser::Statement(ir::Symbol parent) {
    switch (cursor.Peek()) {
      case ir::Token::Import: return ImportStatement(parent);
      case ir::Token::Register: return RegisterStatement(parent);
      case ir::Token::Break: return BreakStatement(parent);
      case ir::Token::Continue: return ContinueStatement(parent);
      case ir::Token::Return: return ReturnStatement(parent);
      case ir::Token::Yield: return YieldStatement(parent);
      case ir::Token::Panic: return PanicStatement(parent);
      case ir::Token::Do: return DoStatement(parent);
      case ir::Token::While: return WhileStatement(parent);
      case ir::Token::Repeat: return RepeatStatement(parent);
      case ir::Token::For: return ForStatement(parent);
      case ir::Token::If: return IfStatement(parent);
      case ir::Token::When: return WhenStatement(parent);
      case ir::Token::Const:
      case ir::Token::Let:
      case ir::Token::Mut:
      case ir::Token::Private:
      case ir::Token::Protected:
      case ir::Token::Public: return DeclarationStatement(parent);
      default: return ExpressionStatement(parent);
    }
  }

  ir::Symbol Parser::Scope(ir::Symbol parent) {
    Expect(ir::Token::ScopeOpen, parent);
    
    ir::Symbol last = undefined;
    while (!cursor.Done() && cursor.Peek() != ir::Token::ScopeClose) {
      last = Statement(parent);
    }
    
    Expect(ir::Token::ScopeClose, parent);

    return last;
  }

  ir::Symbol Parser::Condition(ir::Symbol parent) {
    Expect(ir::Token::ConditionOpen, parent);
    ir::Symbol expr = Expression(parent);
    Expect(ir::Token::ConditionClose, parent);

    return expr;
  }

  ir::Symbol Parser::ScopeOrInlineStatement(ir::Symbol parent) {
    PushDeclarationScope(parent);

    if (cursor.Peek() == ir::Token::ScopeOpen) {
      Scope(parent);
    } else {
      Statement(parent);
    }

    PopDeclarationScope(parent);

    return ir::Symbol{};
  }
}
