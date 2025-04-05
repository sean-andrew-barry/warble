export module lexical.cursor.token;

import lexical.cursor;
import lexical.token;
import lexical.tokens;
import utility.print;
import <vector>;
import <bitset>;
import <initializer_list>;
import <string_view>;
import <stdexcept>;
import <unordered_map>;
import <utility>;
import <array>;
import <format>;

namespace lexical::cursor {
  using Pair = std::pair<std::string_view, lexical::Tokens>;
  
  consteval std::bitset<256> MakeBitset(std::initializer_list<lexical::Tokens> list) {
    std::bitset<256> bits;

    for (auto token : list) {
      bits.set(static_cast<uint8_t>(token));
    }

    return bits;
  }

  template<size_t N>
  consteval std::bitset<256> MakeBitset(const std::array<Pair, N>& list) {
    std::bitset<256> bits;

    for (const auto& [string, token] : list) {
      bits.set(static_cast<uint8_t>(token));
    }

    return bits;
  }

  inline constexpr auto TRIVIAL_SET = MakeBitset({
    lexical::Tokens::NONE,
    lexical::Tokens::COMMENT,
    lexical::Tokens::WHITESPACE,
    lexical::Tokens::NEWLINE,
    lexical::Tokens::COMMENT_CLOSE,
    lexical::Tokens::COMMENT_OPEN,
    lexical::Tokens::MULTI_LINE_COMMENT_CLOSE,
    lexical::Tokens::MULTI_LINE_COMMENT_OPEN,
  });

  inline constexpr auto LITERAL_SET = MakeBitset({
    lexical::Tokens::UNDEFINED,
    lexical::Tokens::_NULL,
    lexical::Tokens::FALSE,
    lexical::Tokens::TRUE,
    lexical::Tokens::NUMBER,
    lexical::Tokens::INTEGER,
    lexical::Tokens::DECIMAL,
    lexical::Tokens::HEX_INTEGER,
    lexical::Tokens::HEX_NUMBER,
    lexical::Tokens::OCTAL_INTEGER,
    lexical::Tokens::OCTAL_NUMBER,
    lexical::Tokens::BINARY_INTEGER,
    lexical::Tokens::BINARY_NUMBER,
  });

  inline constexpr auto ESCAPE_SET = MakeBitset({
    lexical::Tokens::ESCAPE_NEWLINE, // (\n) - A newline character.
    lexical::Tokens::ESCAPE_TAB, // (\t) - A tab character.
    lexical::Tokens::ESCAPE_BACKSPACE, // (\b) - A backspace.
    lexical::Tokens::ESCAPE_RETURN, // (\r) - A carriage return.
    lexical::Tokens::ESCAPE_FORM_FEED, // (\f) - A form feed.
    lexical::Tokens::ESCAPE_LITERAL, // (\X) - A literal character
    lexical::Tokens::ESCAPE_UNICODE_CODEPOINT, // (\u{XXXXX...}) - A variable length Unicode code point
    lexical::Tokens::ESCAPE_UNICODE_SHORT, // (\uXXXX) - A Unicode character using exactly four hexadecimal digits.
    lexical::Tokens::ESCAPE_HEX_CODE, // (\xXX) - A character specified by a hexadecimal value of exactly two digits.
  });

  constexpr auto CONTEXTUAL_ARRAY = std::to_array<Pair>({
    {"super", lexical::Tokens::SUPER},
    {"this", lexical::Tokens::THIS},
    {"self", lexical::Tokens::SELF},
  });

  constexpr auto IMPLICIT_ARRAY = std::to_array<Pair>({
    {"->", lexical::Tokens::ARROW}, // x -> y
    {"(", lexical::Tokens::PARAMETER_OPEN}, // x(){ return y; }
    {"[", lexical::Tokens::CAPTURE_OPEN}, // x[]{ return y; }
    {"(", lexical::Tokens::TUPLE_OPEN}, // x(y)
    {"[", lexical::Tokens::ARRAY_OPEN}, // x[y]
    {"{", lexical::Tokens::OBJECT_OPEN}, // x{y}
    {"<", lexical::Tokens::ENUM_OPEN}, // x<y>
    {"'", lexical::Tokens::CHAR_OPEN}, // x'y'
    {"\"", lexical::Tokens::STRING_OPEN}, // x"y"
    {"`", lexical::Tokens::TEMPLATE_STRING_OPEN}, // x`y`
  });

  constexpr auto BINARY_ARRAY = std::to_array<Pair>({
    {"with", lexical::Tokens::WITH},
    {"use", lexical::Tokens::USE},
    {"+", lexical::Tokens::ADD},
    {"=", lexical::Tokens::ASSIGN},
    {"=>", lexical::Tokens::CONSTRUCTOR},
    {"/", lexical::Tokens::DIVIDE},
    {"=?", lexical::Tokens::EMPLACE},
    {"==", lexical::Tokens::EQUAL},
    {"**", lexical::Tokens::EXPONENT},
    {">=", lexical::Tokens::GREATER_OR_EQUAL},
    {">", lexical::Tokens::GREATER},
    {"..", lexical::Tokens::EXCLUSIVE_RANGE},
    {"...", lexical::Tokens::INCLUSIVE_RANGE},
    {"<=", lexical::Tokens::LESSER_OR_EQUAL},
    {"<", lexical::Tokens::LESSER},
    {".", lexical::Tokens::MEMBER_ACCESS},
    {"%", lexical::Tokens::MODULO},
    {"*", lexical::Tokens::MULTIPLY},
    {"!=", lexical::Tokens::NOT_EQUAL},
    {"?=", lexical::Tokens::ASSIGN_OPTIONAL},
    {"?.", lexical::Tokens::OPTIONAL_MEMBER_ACCESS},
    {"&&", lexical::Tokens::AND},
    {"||", lexical::Tokens::OR},
    {"!!", lexical::Tokens::TRUTHY_AND},
    {"??", lexical::Tokens::TRUTHY_OR},
    {"-", lexical::Tokens::SUBTRACT},
    {"::", lexical::Tokens::STATIC_MEMBER_ACCESS},
    {"<<<", lexical::Tokens::TRIPLE_LEFT_SHIFT},
    {">>>", lexical::Tokens::TRIPLE_RIGHT_SHIFT},
    {"<=>", lexical::Tokens::THREE_WAY_COMPARISON},
    {"?", lexical::Tokens::TERNARY_CONDITION},
    {":", lexical::Tokens::TERNARY_ALTERNATE},
    {"===", lexical::Tokens::ASSERT_EQUAL},
    {">==", lexical::Tokens::ASSERT_GREATER_OR_EQUAL},
    {"<==", lexical::Tokens::ASSERT_LESSER_OR_EQUAL},
    {"!==", lexical::Tokens::ASSERT_NOT_EQUAL},
    {"+=", lexical::Tokens::ASSIGN_ADD},
    {"/=", lexical::Tokens::ASSIGN_DIVIDE},
    {"**=", lexical::Tokens::ASSIGN_EXPONENT},
    {"%=", lexical::Tokens::ASSIGN_MODULO},
    {"*=", lexical::Tokens::ASSIGN_MULTIPLY},
    {"-=", lexical::Tokens::ASSIGN_SUBTRACT},
    {"&&=", lexical::Tokens::ASSIGN_AND},
    {"||=", lexical::Tokens::ASSIGN_OR},
    {"!!=", lexical::Tokens::ASSIGN_TRUTHY_AND},
    {"??=", lexical::Tokens::ASSIGN_TRUTHY_OR},
    {"<<", lexical::Tokens::BITWISE_LEFT_SHIFT},
    {">>", lexical::Tokens::BITWISE_RIGHT_SHIFT},
    {"<<<", lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT},
    {">>>", lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT},
    {"<<<=", lexical::Tokens::BITWISE_ASSIGN_TRIPLE_LEFT_SHIFT},
    {">>>=", lexical::Tokens::BITWISE_ASSIGN_TRIPLE_RIGHT_SHIFT},
    {"<<=", lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT},
    {">>=", lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT},
    {"&", lexical::Tokens::BITWISE_AND},
    {"^", lexical::Tokens::BITWISE_XOR},
    {"|", lexical::Tokens::BITWISE_OR},
    {"&=", lexical::Tokens::BITWISE_ASSIGN_AND},
    {"^=", lexical::Tokens::BITWISE_ASSIGN_XOR},
    {"|=", lexical::Tokens::BITWISE_ASSIGN_OR},
  });

  constexpr auto UNARY_PREFIX_ARRAY = std::to_array<Pair>({
    {"yield", lexical::Tokens::YIELD},
    {"expected", lexical::Tokens::EXPECTED},
    {"unexpected", lexical::Tokens::UNEXPECTED},
    {"await", lexical::Tokens::AWAIT},
    {"!", lexical::Tokens::CONTRACT},
    {"@", lexical::Tokens::COPY},
    {"--", lexical::Tokens::DECREMENT},
    {"++", lexical::Tokens::INCREMENT},
    {"#", lexical::Tokens::BORROW},
    {"-", lexical::Tokens::NEGATIVE},
    {"!", lexical::Tokens::NOT},
    {"?", lexical::Tokens::OPTIONAL},
    {"+", lexical::Tokens::POSITIVE},
    {"&", lexical::Tokens::REFERENCE},
    {"^", lexical::Tokens::OPTIONAL_REFERENCE},
    {"...", lexical::Tokens::SPREAD},
    {"$", lexical::Tokens::SYMBOL},
    {"*", lexical::Tokens::VIRTUAL},
    {"=", lexical::Tokens::MOVE},
    {"~", lexical::Tokens::BITWISE_NOT},
  });

  constexpr auto UNARY_POSTFIX_ARRAY = std::to_array<Pair>({
    {"!", lexical::Tokens::POSTFIX_CONTRACT},
    {"@", lexical::Tokens::POSTFIX_COPY},
    {"--", lexical::Tokens::POSTFIX_DECREMENT},
    {"++", lexical::Tokens::POSTFIX_INCREMENT},
    {"#", lexical::Tokens::POSTFIX_BORROW},
    {"?", lexical::Tokens::POSTFIX_OPTIONAL},
    {"&", lexical::Tokens::POSTFIX_REFERENCE},
    {"...", lexical::Tokens::POSTFIX_SPREAD},
    {"$", lexical::Tokens::POSTFIX_SYMBOL},
    {"*", lexical::Tokens::POSTFIX_VIRTUAL},
  });

  constexpr auto STATEMENT_ARRAY = std::to_array<Pair>({
    {"as", lexical::Tokens::AS},
    {"break", lexical::Tokens::BREAK},
    {"continue", lexical::Tokens::CONTINUE},
    {"cover", lexical::Tokens::COVER},
    {"default", lexical::Tokens::DEFAULT},
    {"do", lexical::Tokens::DO},
    {"else", lexical::Tokens::ELSE},
    {"for", lexical::Tokens::FOR},
    {"register", lexical::Tokens::REGISTER},
    {"from", lexical::Tokens::FROM},
    {"if", lexical::Tokens::IF},
    {"return", lexical::Tokens::RETURN},
    {"match", lexical::Tokens::MATCH},
    {"is", lexical::Tokens::IS},
    {"in", lexical::Tokens::IN},
    {"import", lexical::Tokens::IMPORT},
    {"transform", lexical::Tokens::TRANSFORM},
    {"while", lexical::Tokens::WHILE},
    {"case", lexical::Tokens::CASE},
  });

  constexpr auto MODIFIER_ARRAY = std::to_array<Pair>({
    {"async", lexical::Tokens::ASYNC},
    {"let", lexical::Tokens::LET},
    {"const", lexical::Tokens::CONST},
    {"comtime", lexical::Tokens::COMTIME},
    // {"compiler", lexical::Tokens::COMPILER},
    {"export", lexical::Tokens::EXPORT},
    {"runtime", lexical::Tokens::RUNTIME},
    {"immutable", lexical::Tokens::IMMUTABLE},
    {"mutable", lexical::Tokens::MUTABLE},
    {"private", lexical::Tokens::PRIVATE},
    {"protected", lexical::Tokens::PROTECTED},
    {"public", lexical::Tokens::PUBLIC},
    {"static", lexical::Tokens::STATIC},
  });

  constexpr auto TYPE_ARRAY = std::to_array<Pair>({
    {"auto", lexical::Tokens::AUTO},
    {"array", lexical::Tokens::ARRAY},
    {"bool", lexical::Tokens::BOOL},
    {"class", lexical::Tokens::CLASS},
    {"compiler", lexical::Tokens::COMPILER},
    {"byte", lexical::Tokens::BYTE},
    {"char", lexical::Tokens::CHAR},
    {"char8", lexical::Tokens::CHAR8},
    {"char16", lexical::Tokens::CHAR16},
    {"char32", lexical::Tokens::CHAR32},
    {"enum", lexical::Tokens::ENUM},
    {"float", lexical::Tokens::FLOAT},
    {"float16", lexical::Tokens::FLOAT16},
    {"float32", lexical::Tokens::FLOAT32},
    {"float64", lexical::Tokens::FLOAT64},
    {"function", lexical::Tokens::FUNCTION},
    {"int", lexical::Tokens::INT},
    {"int8", lexical::Tokens::INT8},
    {"int16", lexical::Tokens::INT16},
    {"int32", lexical::Tokens::INT32},
    {"int64", lexical::Tokens::INT64},
    {"object", lexical::Tokens::OBJECT},
    {"string", lexical::Tokens::STRING},
    {"tuple", lexical::Tokens::TUPLE},
    {"uint", lexical::Tokens::UINT},
    {"uint8", lexical::Tokens::UINT8},
    {"uint16", lexical::Tokens::UINT16},
    {"uint32", lexical::Tokens::UINT32},
    {"uint64", lexical::Tokens::UINT64},
    {"variant", lexical::Tokens::VARIANT},
    {"void", lexical::Tokens::VOID},
  });

  constexpr auto SYMBOL_ARRAY = std::to_array<Pair>({
    {"[", lexical::Tokens::ARRAY_OPEN},
    {"]", lexical::Tokens::ARRAY_CLOSE},
    {"->", lexical::Tokens::ARROW}, // ->
    {"[", lexical::Tokens::CAPTURE_OPEN},
    {"]", lexical::Tokens::CAPTURE_CLOSE},
    {"'", lexical::Tokens::CHAR_OPEN},
    {"'", lexical::Tokens::CHAR_CLOSE},
    {",", lexical::Tokens::COMMA},
    {"[", lexical::Tokens::COMPUTED_PROPERTY_OPEN},
    {"]", lexical::Tokens::COMPUTED_PROPERTY_CLOSE},
    {"<", lexical::Tokens::ENUM_OPEN},
    {">", lexical::Tokens::ENUM_CLOSE},
    {"{", lexical::Tokens::IMPORTS_OPEN},
    {"}", lexical::Tokens::IMPORTS_CLOSE},
    {"{", lexical::Tokens::OBJECT_OPEN},
    {"}", lexical::Tokens::OBJECT_CLOSE},
    {"(", lexical::Tokens::PARAMETER_OPEN},
    {")", lexical::Tokens::PARAMETER_CLOSE},
    {":", lexical::Tokens::INLINE_SCOPE_START},
    {"{", lexical::Tokens::SCOPE_OPEN},
    {"}", lexical::Tokens::SCOPE_CLOSE},
    {";", lexical::Tokens::SEMICOLON},
    {"`", lexical::Tokens::TEMPLATE_STRING_OPEN},
    {"`", lexical::Tokens::TEMPLATE_STRING_CLOSE},
    {"{", lexical::Tokens::TEMPLATE_STRING_EXPRESSION_OPEN},
    {"}", lexical::Tokens::TEMPLATE_STRING_EXPRESSION_CLOSE},
    {"\"", lexical::Tokens::STRING_CLOSE},
    {"\"", lexical::Tokens::STRING_OPEN},
    {")", lexical::Tokens::TUPLE_CLOSE},
    {"(", lexical::Tokens::TUPLE_OPEN},
    {":", lexical::Tokens::TYPE_START}, // :
    {"*", lexical::Tokens::WILDCARD}, // *
  });

  constexpr auto MISC_ARRAY = std::to_array<Pair>({
    {"identifier", lexical::Tokens::IDENTIFIER},
    {"text", lexical::Tokens::TEXT},
    {"error", lexical::Tokens::ERROR},
  });

  inline constexpr auto CONTEXTUAL_SET = MakeBitset(CONTEXTUAL_ARRAY);
  inline constexpr auto IMPLICIT_SET = MakeBitset(IMPLICIT_ARRAY);
  inline constexpr auto BINARY_SET = MakeBitset(BINARY_ARRAY);
  inline constexpr auto UNARY_PREFIX_SET = MakeBitset(UNARY_PREFIX_ARRAY);
  inline constexpr auto UNARY_POSTFIX_SET = MakeBitset(UNARY_POSTFIX_ARRAY);
  inline constexpr auto UNARY_SET = UNARY_PREFIX_SET | UNARY_POSTFIX_SET;
  inline constexpr auto STATEMENT_SET = MakeBitset(STATEMENT_ARRAY);
  inline constexpr auto MODIFIER_SET = MakeBitset(MODIFIER_ARRAY);
  inline constexpr auto TYPE_SET = MakeBitset(TYPE_ARRAY);
  inline constexpr auto SYMBOL_SET = MakeBitset(SYMBOL_ARRAY);
  inline constexpr auto MISC_SET = MakeBitset(MISC_ARRAY);

  inline constexpr auto IDENTIFIABLE_SET = MakeBitset({
    lexical::Tokens::IDENTIFIER,
  }) | CONTEXTUAL_SET | TYPE_SET;

  inline constexpr auto DECLARABLE_SET = MakeBitset({
    lexical::Tokens::COMPUTED_PROPERTY_OPEN,
    lexical::Tokens::SPREAD,
  }) | MODIFIER_SET | IDENTIFIABLE_SET;

  const auto WORD_MAP = []{
    std::unordered_map<std::string_view, lexical::Tokens> map;

    map.reserve(CONTEXTUAL_ARRAY.size() + STATEMENT_ARRAY.size() + TYPE_ARRAY.size() + MODIFIER_ARRAY.size());
    map.insert(CONTEXTUAL_ARRAY.cbegin(), CONTEXTUAL_ARRAY.cend());
    map.insert(STATEMENT_ARRAY.cbegin(), STATEMENT_ARRAY.cend());
    map.insert(TYPE_ARRAY.cbegin(), TYPE_ARRAY.cend());
    map.insert(MODIFIER_ARRAY.cbegin(), MODIFIER_ARRAY.cend());

    return map;
  }();

  inline constexpr lexical::Token end{lexical::Tokens::NONE};
  constexpr const lexical::Token& END() { return end; }

  export class Token : public lexical::Cursor<std::vector<lexical::Token>, END> {
  private:
  public:
    using Super::Super;
    using Super::Peek;
    // using Super::Retreat;

    constexpr Token(const std::vector<lexical::Token>& tokens)
      : Super{tokens}
    {
      while (IsTrivial()) {
        // utility::Debug("Skipping trivial token");
        Super::Advance(1);
      }
    }

    constexpr void Advance(size_t n = 1) {
      while (n > 0 && Valid()) {
        Super::Advance(1);
        if (!IsTrivial()) {
          --n;
        }
      }
    }

    void Retreat(Iterator to) {
      throw std::runtime_error("Retreat is not valid in a Token cursor");
    }

    constexpr decltype(auto) Take() {
      auto current = Super::Peek();
      Advance(1); // Take has to be overridden so that we call the correct Advance
      
      return current;
    }

    decltype(auto) Peek(size_t n) const {
      auto iter = cbegin();

      while (true) {
        if (iter == cend()) return END();
        
        if (n == 0) {
          if (IsTrivial(iter->Type())) {
            ++iter;
          }

          break;
        }

        if (!IsTrivial(iter->Type())) {
          --n; // Only decrement `n` for non-trivial tokens
        }

        ++iter;
      }

      return *iter;
    }

    constexpr bool Check(const lexical::Tokens type) const {
      return Super::Peek().Type() == type;
    }

    constexpr bool Match(const lexical::Tokens type) {
      if (Super::Peek().Type() == type) {
        Advance();
        return true;
      } else {
        return false;
      }
    }

    constexpr bool IsTrivial() const { return IsTrivial(Peek().Type()); }
    constexpr bool IsLiteral() const { return IsLiteral(Peek().Type()); }
    constexpr bool IsEscape() const { return IsEscape(Peek().Type()); }
    constexpr bool IsContextual() const { return IsContextual(Peek().Type()); }
    constexpr bool IsImplicit() const { return IsImplicit(Peek().Type()); }
    constexpr bool IsBinary() const { return IsBinary(Peek().Type()); }
    constexpr bool IsUnaryPrefix() const { return IsUnaryPrefix(Peek().Type()); }
    constexpr bool IsUnaryPostfix() const { return IsUnaryPostfix(Peek().Type()); }
    constexpr bool IsUnary() const { return IsUnary(Peek().Type()); }
    constexpr bool IsStatement() const { return IsStatement(Peek().Type()); }
    constexpr bool IsModifier() const { return IsModifier(Peek().Type()); }
    constexpr bool IsType() const { return IsType(Peek().Type()); }
    constexpr bool IsSymbol() const { return IsSymbol(Peek().Type()); }
    constexpr bool IsMisc() const { return IsMisc(Peek().Type()); }
    constexpr bool IsIdentifiable() const { return IsIdentifiable(Peek().Type()); }
    constexpr bool IsDeclarable() const { return IsDeclarable(Peek().Type()); }
    constexpr bool IsRightAssociative() const { return IsRightAssociative(Peek().Type()); }
    constexpr int Precedence() const { return Precedence(Peek().Type()); }
    constexpr std::string_view Name() const { return Name(Peek().Type()); }

    constexpr static bool IsTrivial(lexical::Tokens t) { return TRIVIAL_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsLiteral(lexical::Tokens t) { return LITERAL_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsEscape(lexical::Tokens t) { return ESCAPE_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsContextual(lexical::Tokens t) { return CONTEXTUAL_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsImplicit(lexical::Tokens t) { return IMPLICIT_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsBinary(lexical::Tokens t) { return BINARY_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsUnaryPrefix(lexical::Tokens t) { return UNARY_PREFIX_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsUnaryPostfix(lexical::Tokens t) { return UNARY_POSTFIX_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsUnary(lexical::Tokens t) { return UNARY_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsStatement(lexical::Tokens t) { return STATEMENT_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsModifier(lexical::Tokens t) { return MODIFIER_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsType(lexical::Tokens t) { return TYPE_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsSymbol(lexical::Tokens t) { return SYMBOL_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsMisc(lexical::Tokens t) { return MISC_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsIdentifiable(lexical::Tokens t) { return IDENTIFIABLE_SET[static_cast<uint8_t>(t)]; }
    constexpr static bool IsDeclarable(lexical::Tokens t) { return DECLARABLE_SET[static_cast<uint8_t>(t)]; }

    constexpr static bool IsRightAssociative(lexical::Tokens t) {
      using enum lexical::Tokens;

      switch (t) {
        case EXPONENT: // x ** y
        case ASSIGN: // x = y
        case ASSIGN_AND: // x &&= y
        case ASSIGN_OR: // x ||= y
        case ASSIGN_TRUTHY_AND: // x !!= y
        case ASSIGN_TRUTHY_OR: // x ??= y
        case ASSIGN_OPTIONAL: // x ?= y
        case ASSIGN_ADD: // x += y
        case ASSIGN_SUBTRACT: // x -= y
        case ASSIGN_MULTIPLY: // x *= y
        case ASSIGN_DIVIDE: // x /= y
        case ASSIGN_MODULO: // x %= y
        case ASSIGN_EXPONENT: // x **= y
        case BITWISE_ASSIGN_AND: // x &= y
        case BITWISE_ASSIGN_XOR: // x ^= y
        case BITWISE_ASSIGN_OR: // x |= y
        case BITWISE_ASSIGN_LEFT_SHIFT: // x <<= y
        case BITWISE_ASSIGN_RIGHT_SHIFT: // x >>= y
        case BITWISE_ASSIGN_TRIPLE_LEFT_SHIFT: // x <<<= y
        case BITWISE_ASSIGN_TRIPLE_RIGHT_SHIFT: // x >>>= y
        case TERNARY_CONDITION: // x ? y : z
          return true;
        default:
          return false;
      }
    }

    constexpr static int Precedence(lexical::Tokens t) {
      switch (t) {
        // Property access
        case lexical::Tokens::MEMBER_ACCESS: // x.y
        case lexical::Tokens::STATIC_MEMBER_ACCESS: // x::y
        case lexical::Tokens::OPTIONAL_MEMBER_ACCESS: // x?.y
          return 20;

        // Function calls
        case lexical::Tokens::ARROW: // x -> y
        case lexical::Tokens::PARAMETER_OPEN: // x(){ return y; }
        case lexical::Tokens::CAPTURE_OPEN: // x[]{ return y; }
        case lexical::Tokens::TUPLE_OPEN: // x(y)
        case lexical::Tokens::ARRAY_OPEN: // x[y]
        case lexical::Tokens::OBJECT_OPEN: // x{y}
        case lexical::Tokens::ENUM_OPEN: // x<y>
        case lexical::Tokens::CHAR_OPEN: // x'y'
        case lexical::Tokens::STRING_OPEN: // x"y"
        case lexical::Tokens::TEMPLATE_STRING_OPEN: // x`y`
          return 19;

        // Unary operators
        case lexical::Tokens::BITWISE_NOT: // ~x
        case lexical::Tokens::INCREMENT: // ++x
        case lexical::Tokens::POSITIVE: // +x
        case lexical::Tokens::DECREMENT: // --x
        case lexical::Tokens::NEGATIVE: // -x
        case lexical::Tokens::VIRTUAL: // *x
        case lexical::Tokens::REFERENCE: // &x
        case lexical::Tokens::OPTIONAL_REFERENCE: // ^x
        case lexical::Tokens::SYMBOL: // $x
        case lexical::Tokens::BORROW: // #x
        case lexical::Tokens::COPY: // @x
        case lexical::Tokens::NOT: // !x
          return 18;

        // Unary keywords
        case lexical::Tokens::AWAIT: // await x
        case lexical::Tokens::EXPECTED: // expected x
        case lexical::Tokens::UNEXPECTED: // unexpected x
          return 17;

        case lexical::Tokens::EXPONENT: // x ** y
          return 16;

        // Multiplicative
        case lexical::Tokens::MULTIPLY: // x * y
        case lexical::Tokens::DIVIDE: // x / y
        case lexical::Tokens::MODULO: // x % y
          return 15;

        // Additive
        case lexical::Tokens::ADD: // x + y
        case lexical::Tokens::SUBTRACT: // x - y
          return 14;

        // Shift
        case lexical::Tokens::BITWISE_LEFT_SHIFT: // x << y
        case lexical::Tokens::BITWISE_RIGHT_SHIFT: // x >> y
        case lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT: // x <<< y
        case lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT: // x >>> y
          return 13;

        // case THREE_WAY_COMPARISON: // x <=> y
        //   return 12;

        // Relational
        case lexical::Tokens::LESSER: // x < y
        case lexical::Tokens::LESSER_OR_EQUAL: // x <= y
        case lexical::Tokens::GREATER: // x > y
        case lexical::Tokens::GREATER_OR_EQUAL: // x >= y
        case lexical::Tokens::ASSERT_LESSER_OR_EQUAL: // x <== y
        case lexical::Tokens::ASSERT_GREATER_OR_EQUAL: // x >== y
          return 11;

        // Equality
        case lexical::Tokens::EQUAL: // x == y
        case lexical::Tokens::NOT_EQUAL: // x != y
        case lexical::Tokens::ASSERT_EQUAL: // x === y
        case lexical::Tokens::ASSERT_NOT_EQUAL: // x !== y
          return 10;

        // Logical bitwise
        case lexical::Tokens::BITWISE_AND: // x & y
          return 9;
        case lexical::Tokens::BITWISE_XOR: // x ^ y
          return 8;
        case lexical::Tokens::BITWISE_OR: // x | y
          return 7;

        // Logical
        case lexical::Tokens::AND: // x && y
        case lexical::Tokens::TRUTHY_AND: // x !! y
          return 6;
        case lexical::Tokens::OR: // x || y
        case lexical::Tokens::TRUTHY_OR: // x ?? y
          return 5;

        // Structure
        case lexical::Tokens::EXCLUSIVE_RANGE: // x..y
        case lexical::Tokens::INCLUSIVE_RANGE: // x...y
          return 4;

        // Assignment
        case lexical::Tokens::ASSIGN: // x = y
        case lexical::Tokens::ASSIGN_AND: // x &&= y
        case lexical::Tokens::ASSIGN_OR: // x ||= y
        case lexical::Tokens::ASSIGN_TRUTHY_AND: // !!=
        case lexical::Tokens::ASSIGN_TRUTHY_OR: // ??=
        case lexical::Tokens::ASSIGN_OPTIONAL: // x ?= y
        case lexical::Tokens::ASSIGN_ADD: // x += y
        case lexical::Tokens::ASSIGN_SUBTRACT: // x -= y
        case lexical::Tokens::ASSIGN_MULTIPLY: // x *= y
        case lexical::Tokens::ASSIGN_DIVIDE: // x /= y
        case lexical::Tokens::ASSIGN_MODULO: // x %= y
        case lexical::Tokens::ASSIGN_EXPONENT: // x **= y
        case lexical::Tokens::BITWISE_ASSIGN_AND: // x &= y
        case lexical::Tokens::BITWISE_ASSIGN_XOR: // x ^= y
        case lexical::Tokens::BITWISE_ASSIGN_OR: // x |= y
        case lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT: // x <<= y
        case lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT: // x >>= y
        case lexical::Tokens::BITWISE_ASSIGN_TRIPLE_LEFT_SHIFT: // x <<<= y
        case lexical::Tokens::BITWISE_ASSIGN_TRIPLE_RIGHT_SHIFT: // x >>>= y
          return 3;

        case lexical::Tokens::TERNARY_CONDITION: // x ? y : z
        case lexical::Tokens::YIELD: // yield x
        case lexical::Tokens::SPREAD: // ...x
          return 2;

        // case COMMA:
        //   return 1;
        case lexical::Tokens::USE:
          return 1;

        default:
          return -1; // NONE or any unhandled case
      }
    }

    constexpr static std::string_view Name(lexical::Tokens type) {
      switch (type) {
        case lexical::Tokens::NONE: return "NONE";
        case lexical::Tokens::COMMENT: return "COMMENT";
        case lexical::Tokens::WHITESPACE: return "WHITESPACE";
        case lexical::Tokens::NEWLINE: return "NEWLINE";
        case lexical::Tokens::COMMENT_CLOSE: return "COMMENT_CLOSE";
        case lexical::Tokens::COMMENT_OPEN: return "COMMENT_OPEN";
        case lexical::Tokens::MULTI_LINE_COMMENT_CLOSE: return "MULTI_LINE_COMMENT_CLOSE";
        case lexical::Tokens::MULTI_LINE_COMMENT_OPEN: return "MULTI_LINE_COMMENT_OPEN";
        case lexical::Tokens::UNDEFINED: return "UNDEFINED";
        case lexical::Tokens::_NULL: return "NULL";
        case lexical::Tokens::FALSE: return "FALSE";
        case lexical::Tokens::TRUE: return "TRUE";
        case lexical::Tokens::NUMBER: return "NUMBER";
        case lexical::Tokens::INTEGER: return "INTEGER";
        case lexical::Tokens::DECIMAL: return "DECIMAL";
        case lexical::Tokens::HEX_INTEGER: return "HEX_INTEGER";
        case lexical::Tokens::HEX_NUMBER: return "HEX_NUMBER";
        case lexical::Tokens::OCTAL_INTEGER: return "OCTAL_INTEGER";
        case lexical::Tokens::OCTAL_NUMBER: return "OCTAL_NUMBER";
        case lexical::Tokens::BINARY_INTEGER: return "BINARY_INTEGER";
        case lexical::Tokens::BINARY_NUMBER: return "BINARY_NUMBER";
        case lexical::Tokens::ESCAPE_NEWLINE: return "ESCAPE_NEWLINE";
        case lexical::Tokens::ESCAPE_TAB: return "ESCAPE_TAB";
        case lexical::Tokens::ESCAPE_BACKSPACE: return "ESCAPE_BACKSPACE";
        case lexical::Tokens::ESCAPE_RETURN: return "ESCAPE_RETURN";
        case lexical::Tokens::ESCAPE_FORM_FEED: return "ESCAPE_FORM_FEED";
        case lexical::Tokens::ESCAPE_LITERAL: return "ESCAPE_LITERAL";
        case lexical::Tokens::ESCAPE_UNICODE_CODEPOINT: return "ESCAPE_UNICODE_CODEPOINT";
        case lexical::Tokens::ESCAPE_UNICODE_SHORT: return "ESCAPE_UNICODE_SHORT";
        case lexical::Tokens::ESCAPE_HEX_CODE: return "ESCAPE_HEX_CODE";
        case lexical::Tokens::SUPER: return "SUPER";
        case lexical::Tokens::THIS: return "THIS";
        case lexical::Tokens::SELF: return "SELF";
        case lexical::Tokens::WITH: return "WITH";
        case lexical::Tokens::USE: return "USE";
        case lexical::Tokens::ADD: return "ADD";
        case lexical::Tokens::AND: return "AND";
        case lexical::Tokens::ASSIGN: return "ASSIGN";
        case lexical::Tokens::TRUTHY_AND: return "TRUTHY_AND";
        case lexical::Tokens::TRUTHY_OR: return "TRUTHY_OR";
        case lexical::Tokens::CONSTRUCTOR: return "CONSTRUCTOR";
        case lexical::Tokens::DIVIDE: return "DIVIDE";
        case lexical::Tokens::EMPLACE: return "EMPLACE";
        case lexical::Tokens::EQUAL: return "EQUAL";
        case lexical::Tokens::EXPONENT: return "EXPONENT";
        case lexical::Tokens::GREATER_OR_EQUAL: return "GREATER_OR_EQUAL";
        case lexical::Tokens::GREATER: return "GREATER";
        case lexical::Tokens::EXCLUSIVE_RANGE: return "EXCLUSIVE_RANGE";
        case lexical::Tokens::INCLUSIVE_RANGE: return "INCLUSIVE_RANGE";
        case lexical::Tokens::LESSER_OR_EQUAL: return "LESSER_OR_EQUAL";
        case lexical::Tokens::LESSER: return "LESSER";
        case lexical::Tokens::MEMBER_ACCESS: return "MEMBER_ACCESS";
        case lexical::Tokens::MODULO: return "MODULO";
        case lexical::Tokens::MULTIPLY: return "MULTIPLY";
        case lexical::Tokens::NOT_EQUAL: return "NOT_EQUAL";
        case lexical::Tokens::ASSIGN_OPTIONAL: return "ASSIGN_OPTIONAL";
        case lexical::Tokens::OPTIONAL_MEMBER_ACCESS: return "OPTIONAL_MEMBER_ACCESS";
        case lexical::Tokens::OR: return "OR";
        case lexical::Tokens::SUBTRACT: return "SUBTRACT";
        case lexical::Tokens::STATIC_MEMBER_ACCESS: return "STATIC_MEMBER_ACCESS";
        case lexical::Tokens::TRIPLE_LEFT_SHIFT: return "TRIPLE_LEFT_SHIFT";
        case lexical::Tokens::TRIPLE_RIGHT_SHIFT: return "TRIPLE_RIGHT_SHIFT";
        case lexical::Tokens::THREE_WAY_COMPARISON: return "THREE_WAY_COMPARISON";
        case lexical::Tokens::TERNARY_CONDITION: return "TERNARY_CONDITION";
        case lexical::Tokens::TERNARY_ALTERNATE: return "TERNARY_ALTERNATE";
        case lexical::Tokens::ASSERT_EQUAL: return "ASSERT_EQUAL";
        case lexical::Tokens::ASSERT_GREATER_OR_EQUAL: return "ASSERT_GREATER_OR_EQUAL";
        case lexical::Tokens::ASSERT_LESSER_OR_EQUAL: return "ASSERT_LESSER_OR_EQUAL";
        case lexical::Tokens::ASSERT_NOT_EQUAL: return "ASSERT_NOT_EQUAL";
        case lexical::Tokens::ASSIGN_ADD: return "ASSIGN_ADD";
        case lexical::Tokens::ASSIGN_DIVIDE: return "ASSIGN_DIVIDE";
        case lexical::Tokens::ASSIGN_EXPONENT: return "ASSIGN_EXPONENT";
        case lexical::Tokens::ASSIGN_MODULO: return "ASSIGN_MODULO";
        case lexical::Tokens::ASSIGN_MULTIPLY: return "ASSIGN_MULTIPLY";
        case lexical::Tokens::ASSIGN_SUBTRACT: return "ASSIGN_SUBTRACT";
        case lexical::Tokens::ASSIGN_AND: return "ASSIGN_AND";
        case lexical::Tokens::ASSIGN_OR: return "ASSIGN_OR";
        case lexical::Tokens::ASSIGN_TRUTHY_AND: return "ASSIGN_TRUTHY_AND";
        case lexical::Tokens::ASSIGN_TRUTHY_OR: return "ASSIGN_TRUTHY_OR";
        case lexical::Tokens::BITWISE_NOT: return "BITWISE_NOT";
        case lexical::Tokens::BITWISE_LEFT_SHIFT: return "BITWISE_LEFT_SHIFT";
        case lexical::Tokens::BITWISE_RIGHT_SHIFT: return "BITWISE_RIGHT_SHIFT";
        case lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT: return "BITWISE_TRIPLE_LEFT_SHIFT";
        case lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT: return "BITWISE_TRIPLE_RIGHT_SHIFT";
        case lexical::Tokens::BITWISE_ASSIGN_TRIPLE_LEFT_SHIFT: return "BITWISE_ASSIGN_TRIPLE_LEFT_SHIFT";
        case lexical::Tokens::BITWISE_ASSIGN_TRIPLE_RIGHT_SHIFT: return "BITWISE_ASSIGN_TRIPLE_RIGHT_SHIFT";
        case lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT: return "BITWISE_ASSIGN_LEFT_SHIFT";
        case lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT: return "BITWISE_ASSIGN_RIGHT_SHIFT";
        case lexical::Tokens::BITWISE_AND: return "BITWISE_AND";
        case lexical::Tokens::BITWISE_XOR: return "BITWISE_XOR";
        case lexical::Tokens::BITWISE_OR: return "BITWISE_OR";
        case lexical::Tokens::BITWISE_ASSIGN_AND: return "BITWISE_ASSIGN_AND";
        case lexical::Tokens::BITWISE_ASSIGN_XOR: return "BITWISE_ASSIGN_XOR";
        case lexical::Tokens::BITWISE_ASSIGN_OR: return "BITWISE_ASSIGN_OR";
        case lexical::Tokens::YIELD: return "YIELD";
        case lexical::Tokens::EXPECTED: return "EXPECTED";
        case lexical::Tokens::UNEXPECTED: return "UNEXPECTED";
        case lexical::Tokens::AWAIT: return "AWAIT";
        case lexical::Tokens::CONTRACT: return "CONTRACT";
        case lexical::Tokens::COPY: return "COPY";
        case lexical::Tokens::DECREMENT: return "DECREMENT";
        case lexical::Tokens::INCREMENT: return "INCREMENT";
        case lexical::Tokens::BORROW: return "BORROW";
        case lexical::Tokens::NEGATIVE: return "NEGATIVE";
        case lexical::Tokens::NOT: return "NOT";
        case lexical::Tokens::OPTIONAL: return "OPTIONAL";
        case lexical::Tokens::POSITIVE: return "POSITIVE";
        case lexical::Tokens::REFERENCE: return "REFERENCE";
        case lexical::Tokens::OPTIONAL_REFERENCE: return "OPTIONAL_REFERENCE";
        case lexical::Tokens::SPREAD: return "SPREAD";
        case lexical::Tokens::SYMBOL: return "SYMBOL";
        case lexical::Tokens::VIRTUAL: return "VIRTUAL";
        case lexical::Tokens::MOVE: return "MOVE";
        case lexical::Tokens::POSTFIX_CONTRACT: return "POSTFIX_CONTRACT";
        case lexical::Tokens::POSTFIX_COPY: return "POSTFIX_COPY";
        case lexical::Tokens::POSTFIX_DECREMENT: return "POSTFIX_DECREMENT";
        case lexical::Tokens::POSTFIX_INCREMENT: return "POSTFIX_INCREMENT";
        case lexical::Tokens::POSTFIX_BORROW: return "POSTFIX_BORROW";
        case lexical::Tokens::POSTFIX_OPTIONAL: return "POSTFIX_OPTIONAL";
        case lexical::Tokens::POSTFIX_REFERENCE: return "POSTFIX_REFERENCE";
        case lexical::Tokens::POSTFIX_SPREAD: return "POSTFIX_SPREAD";
        case lexical::Tokens::POSTFIX_SYMBOL: return "POSTFIX_SYMBOL";
        case lexical::Tokens::POSTFIX_VIRTUAL: return "POSTFIX_VIRTUAL";
        case lexical::Tokens::AS: return "AS";
        case lexical::Tokens::ASYNC: return "ASYNC";
        case lexical::Tokens::BREAK: return "BREAK";
        case lexical::Tokens::CONTINUE: return "CONTINUE";
        case lexical::Tokens::COVER: return "COVER";
        case lexical::Tokens::DEFAULT: return "DEFAULT";
        case lexical::Tokens::DO: return "DO";
        case lexical::Tokens::ELSE: return "ELSE";
        case lexical::Tokens::FOR: return "FOR";
        case lexical::Tokens::REGISTER: return "REGISTER";
        case lexical::Tokens::FROM: return "FROM";
        case lexical::Tokens::IF: return "IF";
        case lexical::Tokens::RETURN: return "RETURN";
        case lexical::Tokens::MATCH: return "MATCH";
        case lexical::Tokens::IS: return "IS";
        case lexical::Tokens::IN: return "IN";
        case lexical::Tokens::IMPORT: return "IMPORT";
        case lexical::Tokens::TRANSFORM: return "TRANSFORM";
        case lexical::Tokens::WHILE: return "WHILE";
        case lexical::Tokens::CASE: return "CASE";
        case lexical::Tokens::LET: return "LET";
        case lexical::Tokens::CONST: return "CONST";
        case lexical::Tokens::COMTIME: return "COMTIME";
        case lexical::Tokens::COMPILER: return "COMPILER";
        case lexical::Tokens::EXPORT: return "EXPORT";
        case lexical::Tokens::RUNTIME: return "RUNTIME";
        case lexical::Tokens::IMMUTABLE: return "IMMUTABLE";
        case lexical::Tokens::MUTABLE: return "MUTABLE";
        case lexical::Tokens::PRIVATE: return "PRIVATE";
        case lexical::Tokens::PROTECTED: return "PROTECTED";
        case lexical::Tokens::PUBLIC: return "PUBLIC";
        case lexical::Tokens::STATIC: return "STATIC";
        case lexical::Tokens::AUTO: return "AUTO";
        case lexical::Tokens::ARRAY: return "ARRAY";
        case lexical::Tokens::BOOL: return "BOOL";
        case lexical::Tokens::CLASS: return "CLASS";
        case lexical::Tokens::BYTE: return "BYTE";
        case lexical::Tokens::CHAR: return "CHAR";
        case lexical::Tokens::CHAR8: return "CHAR8";
        case lexical::Tokens::CHAR16: return "CHAR16";
        case lexical::Tokens::CHAR32: return "CHAR32";
        case lexical::Tokens::ENUM: return "ENUM";
        case lexical::Tokens::FLOAT: return "FLOAT";
        case lexical::Tokens::FLOAT16: return "FLOAT16";
        case lexical::Tokens::FLOAT32: return "FLOAT32";
        case lexical::Tokens::FLOAT64: return "FLOAT64";
        case lexical::Tokens::FUNCTION: return "FUNCTION";
        case lexical::Tokens::INT: return "INT";
        case lexical::Tokens::INT8: return "INT8";
        case lexical::Tokens::INT16: return "INT16";
        case lexical::Tokens::INT32: return "INT32";
        case lexical::Tokens::INT64: return "INT64";
        case lexical::Tokens::OBJECT: return "OBJECT";
        case lexical::Tokens::STRING: return "STRING";
        case lexical::Tokens::TUPLE: return "TUPLE";
        case lexical::Tokens::UINT: return "UINT";
        case lexical::Tokens::UINT8: return "UINT8";
        case lexical::Tokens::UINT16: return "UINT16";
        case lexical::Tokens::UINT32: return "UINT32";
        case lexical::Tokens::UINT64: return "UINT64";
        case lexical::Tokens::VARIANT: return "VARIANT";
        case lexical::Tokens::VOID: return "VOID";
        case lexical::Tokens::DESTRUCTURED_ARRAY_OPEN: return "DESTRUCTURED_ARRAY_OPEN";
        case lexical::Tokens::DESTRUCTURED_ARRAY_CLOSE: return "DESTRUCTURED_ARRAY_CLOSE";
        case lexical::Tokens::DESTRUCTURED_ENUM_OPEN: return "DESTRUCTURED_ENUM_OPEN";
        case lexical::Tokens::DESTRUCTURED_ENUM_CLOSE: return "DESTRUCTURED_ENUM_CLOSE";
        case lexical::Tokens::DESTRUCTURED_OBJECT_OPEN: return "DESTRUCTURED_OBJECT_OPEN";
        case lexical::Tokens::DESTRUCTURED_OBJECT_CLOSE: return "DESTRUCTURED_OBJECT_CLOSE";
        case lexical::Tokens::DESTRUCTURED_TUPLE_OPEN: return "DESTRUCTURED_TUPLE_OPEN";
        case lexical::Tokens::DESTRUCTURED_TUPLE_CLOSE: return "DESTRUCTURED_TUPLE_CLOSE";
        case lexical::Tokens::ARRAY_CLOSE: return "ARRAY_CLOSE";
        case lexical::Tokens::ARRAY_OPEN: return "ARRAY_OPEN";
        case lexical::Tokens::CHAR_CLOSE: return "CHAR_CLOSE";
        case lexical::Tokens::CHAR_OPEN: return "CHAR_OPEN";
        case lexical::Tokens::COMMA: return "COMMA";
        case lexical::Tokens::COMPUTED_PROPERTY_CLOSE: return "COMPUTED_PROPERTY_CLOSE";
        case lexical::Tokens::COMPUTED_PROPERTY_OPEN: return "COMPUTED_PROPERTY_OPEN";
        case lexical::Tokens::CONDITION_OPEN: return "CONDITION_OPEN";
        case lexical::Tokens::CONDITION_CLOSE: return "CONDITION_CLOSE";
        case lexical::Tokens::ENUM_CLOSE: return "ENUM_CLOSE";
        case lexical::Tokens::ENUM_OPEN: return "ENUM_OPEN";
        case lexical::Tokens::IMPORTS_CLOSE: return "IMPORTS_CLOSE";
        case lexical::Tokens::IMPORTS_OPEN: return "IMPORTS_OPEN";
        case lexical::Tokens::OBJECT_CLOSE: return "OBJECT_CLOSE";
        case lexical::Tokens::OBJECT_OPEN: return "OBJECT_OPEN";
        case lexical::Tokens::ARROW: return "ARROW";
        case lexical::Tokens::CAPTURE_OPEN: return "CAPTURE_OPEN";
        case lexical::Tokens::CAPTURE_CLOSE: return "CAPTURE_CLOSE";
        case lexical::Tokens::PARAMETER_OPEN: return "PARAMETER_OPEN";
        case lexical::Tokens::PARAMETER_CLOSE: return "PARAMETER_CLOSE";
        case lexical::Tokens::RETURN_TYPE_OPEN: return "RETURN_TYPE_OPEN";
        case lexical::Tokens::RETURN_TYPE_CLOSE: return "RETURN_TYPE_CLOSE";
        case lexical::Tokens::INLINE_SCOPE_START: return "INLINE_SCOPE_START";
        case lexical::Tokens::SCOPE_OPEN: return "SCOPE_OPEN";
        case lexical::Tokens::SCOPE_CLOSE: return "SCOPE_CLOSE";
        case lexical::Tokens::SEMICOLON: return "SEMICOLON";
        case lexical::Tokens::TEMPLATE_STRING_OPEN: return "TEMPLATE_STRING_OPEN";
        case lexical::Tokens::TEMPLATE_STRING_CLOSE: return "TEMPLATE_STRING_CLOSE";
        case lexical::Tokens::TEMPLATE_STRING_EXPRESSION_OPEN: return "TEMPLATE_STRING_EXPRESSION_OPEN";
        case lexical::Tokens::TEMPLATE_STRING_EXPRESSION_CLOSE: return "TEMPLATE_STRING_EXPRESSION_CLOSE";
        case lexical::Tokens::STRING_CLOSE: return "STRING_CLOSE";
        case lexical::Tokens::STRING_OPEN: return "STRING_OPEN";
        case lexical::Tokens::TUPLE_OPEN: return "TUPLE_OPEN";
        case lexical::Tokens::TUPLE_CLOSE: return "TUPLE_CLOSE";
        case lexical::Tokens::TYPE_START: return "TYPE_START";
        case lexical::Tokens::WILDCARD: return "WILDCARD";
        case lexical::Tokens::IDENTIFIER: return "IDENTIFIER";
        case lexical::Tokens::TEXT: return "TEXT";
        case lexical::Tokens::ERROR: return "ERROR";
        default: return "Unknown Token";
      }
    }

    static lexical::Tokens FindKeyword(const std::string_view view) {
      auto iter = WORD_MAP.find(view);

      if (iter != WORD_MAP.end()) {
        return iter->second;
      } else {
        return lexical::Tokens::IDENTIFIER;
      }
    }
  };
};

export template<>
struct std::formatter<lexical::Tokens> : std::formatter<std::string_view> {
  // Parse is inherited from std::formatter<std::string_view>.
  template <typename FormatContext>
  auto format(lexical::Tokens t, FormatContext& ctx) {
    // Use your Token::Name function to get the string representation.
    return std::formatter<std::string_view>::format(lexical::cursor::Token::Name(t), ctx);
  }
};