export module compiler.input.Parser;

import <vector>;
import <bitset>;
import <initializer_list>;
import <string_view>;
import <stdexcept>;
import <unordered_map>;
import <utility>;
import <array>;
import <format>;

import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Error;
import compiler.ir.Instruction;
import compiler.ir.Token;
import compiler.text.cursor.Token;

namespace compiler::input {
  export class Lexer; // Forward declaration

  using Pair = std::pair<std::string_view, ir::Token>;

  consteval std::bitset<256> MakeBitset(std::initializer_list<ir::Token> list) {
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

  // Basic trivia & comment related tokens
  inline constexpr auto TRIVIAL_SET = MakeBitset({
    ir::Token::None,
    ir::Token::Spaces0,
    ir::Token::Spaces1,
    ir::Token::Spaces2,
    ir::Token::Spaces3,
    ir::Token::Spaces4,
    ir::Token::Spaces5,
    ir::Token::Spaces6,
    ir::Token::Spaces7,
    ir::Token::Spaces8,
    ir::Token::Spaces9,
    ir::Token::SpacesA,
    ir::Token::SpacesB,
    ir::Token::SpacesC,
    ir::Token::SpacesD,
    ir::Token::SpacesE,
    ir::Token::SpacesF,
    ir::Token::Tabs0,
    ir::Token::Tabs1,
    ir::Token::Tabs2,
    ir::Token::Tabs3,
    ir::Token::Tabs4,
    ir::Token::Tabs5,
    ir::Token::Tabs6,
    ir::Token::Tabs7,
    ir::Token::Tabs8,
    ir::Token::Tabs9,
    ir::Token::TabsA,
    ir::Token::TabsB,
    ir::Token::TabsC,
    ir::Token::TabsD,
    ir::Token::TabsE,
    ir::Token::TabsF,
    ir::Token::LineFeed,
    ir::Token::CarriageReturnLineFeed,
    ir::Token::CarriageReturn,
    ir::Token::LineSeparator,
    ir::Token::ParagraphSeparator,
    ir::Token::VerticalTab,
    ir::Token::FormFeed,
    ir::Token::CommentOpen,
    ir::Token::MultiLineCommentOpen,
  });

  // Literal-like primary tokens (no longer distinguishing numeric forms here)
  inline constexpr auto LITERAL_SET = MakeBitset({
    ir::Token::Undefined,
    ir::Token::Null,
    ir::Token::False,
    ir::Token::True,
    ir::Token::Hex,    // numeric prefix forms
    ir::Token::Octal,
    ir::Token::Binary,
    ir::Token::Digit,  // generic digit token
    ir::Token::Character,
    ir::Token::StringOpen, // opening markers sometimes treated specially
    ir::Token::TemplateStringOpen,
  });

  inline constexpr auto ESCAPE_SET = MakeBitset({
    ir::Token::EscapeNewline,
    ir::Token::EscapeTab,
    ir::Token::EscapeBackspace,
    ir::Token::EscapeReturn,
    ir::Token::EscapeFormFeed,
    ir::Token::EscapeNull,
    ir::Token::EscapeCharacter,
    ir::Token::EscapeSyntax,
    ir::Token::EscapeUnicodeCodepointStart,
    ir::Token::EscapeUnicodeCodepointEnd,
    ir::Token::EscapeUnicodeShort,
    ir::Token::EscapeHexCode,
  });

  // Contextual keywords supported by current token set
  constexpr auto CONTEXTUAL_ARRAY = std::to_array<Pair>({
    {"this", ir::Token::This},
  });

  constexpr auto IMPLICIT_ARRAY = std::to_array<Pair>({
    {"->", ir::Token::Call},
    {"(", ir::Token::ParameterOpen},
    {"[", ir::Token::CaptureOpen},
    {"(", ir::Token::TupleOpen},
    {"[", ir::Token::ArrayOpen},
    {"{", ir::Token::ObjectOpen},
    {"<", ir::Token::EnumOpen},
    {"'", ir::Token::CharOpen},
    {"\"", ir::Token::StringOpen},
    {"`", ir::Token::TemplateStringOpen},
  });

  constexpr auto BINARY_ARRAY = std::to_array<Pair>({
    {"with", ir::Token::With},
    {"+", ir::Token::Add},
    {"=", ir::Token::Assign},
    {"=>", ir::Token::InlineFunctionArrow},
    {"/", ir::Token::Divide},
    {"==", ir::Token::Equal},
    {"**", ir::Token::Exponent},
    {">=", ir::Token::GreaterOrEqual},
    {">", ir::Token::Greater},
    {"..", ir::Token::ExclusiveRange},
    {"...", ir::Token::InclusiveRange},
    {"<=", ir::Token::LesserOrEqual},
    {"<", ir::Token::Lesser},
    {".", ir::Token::MemberAccess},
    {"%", ir::Token::Modulo},
    {"*", ir::Token::Multiply},
    {"!=", ir::Token::NotEqual},
    {"?=", ir::Token::AssignOptional},
    {"?.", ir::Token::MemberAccessOptional},
    {"&&", ir::Token::And},
    {"||", ir::Token::Or},
    {"!!", ir::Token::Wrap},
    {"??", ir::Token::Unwrap},
    {"-", ir::Token::Subtract},
    {":", ir::Token::MemberAccessStatic},
    {"<<<", ir::Token::TripleLeftShift},
    {">>>", ir::Token::TripleRightShift},
    {"===", ir::Token::AssertEqual},
    {">==", ir::Token::AssertGreaterOrEqual},
    {"<==", ir::Token::AssertLesserOrEqual},
    {"!==", ir::Token::AssertNotEqual},
    {"+=", ir::Token::AssignAdd},
    {"/=", ir::Token::AssignDivide},
    {"**=", ir::Token::AssignExponent},
    {"%=", ir::Token::AssignModulo},
    {"*=", ir::Token::AssignMultiply},
    {"-=", ir::Token::AssignSubtract},
    {"&&=", ir::Token::AssignAnd},
    {"||=", ir::Token::AssignOr},
    {"!!=", ir::Token::AssignTruthyAnd},
    {"??=", ir::Token::AssignTruthyOr},
    {"<<", ir::Token::BitwiseLeftShift},
    {">>", ir::Token::BitwiseRightShift},
    {"<<<", ir::Token::BitwiseTripleLeftShift},
    {">>>", ir::Token::BitwiseTripleRightShift},
    {"<<<=", ir::Token::BitwiseAssignTripleLeftShift},
    {">>>=", ir::Token::BitwiseAssignTripleRightShift},
    {"<<=", ir::Token::BitwiseAssignLeftShift},
    {">>=", ir::Token::BitwiseAssignRightShift},
    {"&", ir::Token::BitwiseAnd},
    {"^", ir::Token::BitwiseXor},
    {"|", ir::Token::BitwiseOr},
    {"&=", ir::Token::BitwiseAssignAnd},
    {"^=", ir::Token::BitwiseAssignXor},
    {"|=", ir::Token::BitwiseAssignOr},
  });

  constexpr auto UNARY_PREFIX_ARRAY = std::to_array<Pair>({
    {"yield", ir::Token::Yield},
    {"await", ir::Token::Await},
    {"@", ir::Token::Copy},
    {"--", ir::Token::Decrement},
    {"++", ir::Token::Increment},
    {"#", ir::Token::Counted},
    {"-", ir::Token::Negative},
    {"!", ir::Token::Not},
    {"+", ir::Token::Positive},
    {"&", ir::Token::Reference},
    {"*", ir::Token::MutableReference},
    {"=", ir::Token::Move},
    {"...", ir::Token::Spread},
    {"$", ir::Token::Symbol},
    {"~", ir::Token::BitwiseNot},
  });

  constexpr auto STATEMENT_ARRAY = std::to_array<Pair>({
    {"as", ir::Token::As},
    {"break", ir::Token::Break},
    {"continue", ir::Token::Continue},
    {"default", ir::Token::Default},
    {"do", ir::Token::Do},
    {"else", ir::Token::Else},
    {"for", ir::Token::For},
    {"register", ir::Token::Register},
    {"from", ir::Token::From},
    {"if", ir::Token::If},
    {"return", ir::Token::Return},
    {"match", ir::Token::Match},
    {"is", ir::Token::Is},
    {"in", ir::Token::In},
    {"import", ir::Token::Import},
    {"export", ir::Token::Export},
    {"while", ir::Token::While},
    {"repeat", ir::Token::Repeat},
    {"case", ir::Token::Case},
    {"let", ir::Token::Let}, // also acts as modifier sometimes
  });

  // Reduced modifier set reflecting current tokens
  constexpr auto MODIFIER_ARRAY = std::to_array<Pair>({
    {"let", ir::Token::Let},
    {"compiler", ir::Token::Compiler},
    {"export", ir::Token::Export},
  });

  // Type-like identifiers still treated specially
  constexpr auto TYPE_ARRAY = std::to_array<Pair>({
    {"auto", ir::Token::Auto},
    {"void", ir::Token::Void},
  });

  constexpr auto SYMBOL_ARRAY = std::to_array<Pair>({
    {"[", ir::Token::ArrayOpen},
    {"]", ir::Token::ArrayClose},
    {"->", ir::Token::Call},
    {"[", ir::Token::CaptureOpen},
    {"]", ir::Token::CaptureClose},
    {"'", ir::Token::CharOpen},
    {"'", ir::Token::CharClose},
    {",", ir::Token::Comma},
    {"<", ir::Token::EnumOpen},
    {">", ir::Token::EnumClose},
    {"{", ir::Token::ImportsOpen},
    {"}", ir::Token::ImportsClose},
    {"{", ir::Token::ObjectOpen},
    {"}", ir::Token::ObjectClose},
    {"(", ir::Token::ParameterOpen},
    {")", ir::Token::ParameterClose},
    {"{", ir::Token::ScopeOpen},
    {"}", ir::Token::ScopeClose},
    {";", ir::Token::Semicolon},
    {"`", ir::Token::TemplateStringOpen},
    {"`", ir::Token::TemplateStringClose},
    {"{", ir::Token::TemplateStringExpressionOpen},
    {"}", ir::Token::TemplateStringExpressionClose},
    {"\"", ir::Token::StringOpen},
    {"\"", ir::Token::StringClose},
    {")", ir::Token::TupleClose},
    {"(", ir::Token::TupleOpen},
    {":", ir::Token::TypeStart},
    {"*", ir::Token::Wildcard},
  });

  constexpr auto MISC_ARRAY = std::to_array<Pair>({
    {"identifier", ir::Token::Identifier},
    {"error", ir::Token::Error},
  });

  inline constexpr auto CONTEXTUAL_SET = MakeBitset(CONTEXTUAL_ARRAY);
  inline constexpr auto IMPLICIT_SET = MakeBitset(IMPLICIT_ARRAY);
  inline constexpr auto BINARY_SET = MakeBitset(BINARY_ARRAY);
  inline constexpr auto UNARY_PREFIX_SET = MakeBitset(UNARY_PREFIX_ARRAY);
  inline constexpr auto STATEMENT_SET = MakeBitset(STATEMENT_ARRAY);
  inline constexpr auto MODIFIER_SET = MakeBitset(MODIFIER_ARRAY);
  inline constexpr auto TYPE_SET = MakeBitset(TYPE_ARRAY);
  inline constexpr auto SYMBOL_SET = MakeBitset(SYMBOL_ARRAY);
  inline constexpr auto MISC_SET = MakeBitset(MISC_ARRAY);

  inline constexpr auto IDENTIFIABLE_SET = MakeBitset({
    ir::Token::Identifier,
  }) | CONTEXTUAL_SET | TYPE_SET;

  inline constexpr auto DECLARABLE_SET = MakeBitset({
    ir::Token::Spread,
  }) | MODIFIER_SET | IDENTIFIABLE_SET;

  const auto WORD_MAP = []{
    std::unordered_map<std::string_view, ir::Token> map;

  map.reserve(CONTEXTUAL_ARRAY.size() + STATEMENT_ARRAY.size() + TYPE_ARRAY.size() + MODIFIER_ARRAY.size());
    map.insert(CONTEXTUAL_ARRAY.cbegin(), CONTEXTUAL_ARRAY.cend());
    map.insert(STATEMENT_ARRAY.cbegin(), STATEMENT_ARRAY.cend());
    map.insert(TYPE_ARRAY.cbegin(), TYPE_ARRAY.cend());
    map.insert(MODIFIER_ARRAY.cbegin(), MODIFIER_ARRAY.cend());

    return map;
  }();

  export class Parser {
  private:
    std::vector<ir::Token> tokens;
    std::vector<char32_t> characters;
    std::vector<uint32_t> line_starts; // The token index where each line starts
    std::vector<uint32_t> string_starts; // The character index where each line starts
    
    ir::Symbols symbols;
    std::vector<uint32_t> limbs; // The data storage for big integers
    std::vector<ir::Instruction> instructions;
    text::cursor::Token cursor;
    std::vector<ir::Index> parents; // Stack of parent symbols for nested constructs

    // The indexes track the position within the module's various storage vectors 
    size_t widths_index = 0; // Increments after seeing a variable width token, such as `Spaces` or `Characters`
    size_t symbols_index = 0; // Increments after seeing a symbol token, such as `Identifier` or `Decimal`
    size_t lines_index = 0; // Increments after seeing a line break token, such as `LineFeed` or `CarriageReturnLineFeed`

    ir::Index NextSymbol();

    constexpr static bool IsTrivialToken(ir::Token token) {
      return TRIVIAL_SET.test(static_cast<uint8_t>(token));
    }

    constexpr static bool IsEscapeToken(ir::Token token) {
      return ESCAPE_SET.test(static_cast<uint8_t>(token));
    }

    constexpr static bool IsLiteralToken(ir::Token token) {
      return LITERAL_SET.test(static_cast<uint8_t>(token));
    }

    constexpr static bool IsBinaryToken(ir::Token token) {
      return BINARY_SET.test(static_cast<uint8_t>(token));
    }

    constexpr static bool IsUnaryPrefixToken(ir::Token token) {
      return UNARY_PREFIX_SET.test(static_cast<uint8_t>(token));
    }

    constexpr static bool IsModifierToken(ir::Token token) {
      return MODIFIER_SET.test(static_cast<uint8_t>(token));
    }

    constexpr static bool IsStatementToken(ir::Token token) {
      return STATEMENT_SET.test(static_cast<uint8_t>(token));
    }

    int Precedence(ir::Token token) const;
    void ConsumeCharactersToken();
    void ConsumeEscapeSequence();
    ir::Index ReportError(ir::Error error, ir::Index parent);
    ir::Index ParsePrimary(ir::Index parent);
    ir::Index ParseUnary(ir::Index parent);
    ir::Index ParseBinary(ir::Index parent, int min_precedence);

    void Push(ir::Index index) { parents.push_back(index); }
    void Pop(ir::Index index) {
      // TODO: Debug validation to ensure the index being popped is the one on top of the parents
      parents.pop_back();
    }
  public:
    Parser(input::Lexer& lexer);

    virtual ~Parser() = default;

    void Skip();
    void Advance(size_t count = 1);
    bool Match(ir::Token token);
    bool Check(ir::Token token);
    ir::Token Peek();

    void Instruct(ir::Opcode opcode, ir::Index res = ir::Index{}, ir::Index lhs = ir::Index{}, ir::Index rhs = ir::Index{});

    ir::Index Create(ir::symbol::Type type, ir::Index parent);
    ir::Index Create(ir::symbol::Type type, ir::Index parent, uint64_t value);
    ir::Index Create(ir::symbol::Type type, ir::Index parent, double value);
    ir::Index Create(ir::symbol::Type type, ir::Index parent, char32_t value);
    ir::Index Create(ir::symbol::Type type, ir::Index parent, bool value);

    ir::Index Parser::Expect(ir::Token token, ir::Error error, ir::Index parent);

    void None();
    void Spaces();
    void Tabs();
    void NewLine();
    void Comment();
    void MultiLineComment();
    void Trivial();

    bool WhiteSpace();
    bool WS();

    // Literals
    ir::Index Undefined(ir::Index parent);
    ir::Index Null(ir::Index parent);
    ir::Index Boolean(ir::Index parent);
    ir::Index Character(ir::Index parent);
    ir::Index Number(ir::Index parent);
    ir::Index String(ir::Index parent);
    ir::Index Array(ir::Index parent);
    ir::Index Enum(ir::Index parent);
    ir::Index Tuple(ir::Index parent);
    ir::Index Object(ir::Index parent);
    ir::Index TemplateString(ir::Index parent);
    ir::Index Function(ir::Index parent);
    ir::Index Value(ir::Index parent);

    ir::Index Identifier(ir::Index parent);
    ir::Index Destructuring(ir::Index parent);
    ir::Index Error(ir::Index parent);

    ir::Index Expression(ir::Index parent);
    ir::Index ObjectDeclaration(ir::Index parent);
    ir::Index Declaration(ir::Index parent);

    // Statements
    ir::Index ImportStatement(ir::Index parent);
    ir::Index Permissions(ir::Index parent);
    ir::Index RegisterStatement(ir::Index parent);
    ir::Index BreakStatement(ir::Index parent);
    ir::Index ContinueStatement(ir::Index parent);
    ir::Index ReturnStatement(ir::Index parent);
    ir::Index DoStatement(ir::Index parent);
    ir::Index WhileStatement(ir::Index parent);
    ir::Index RepeatStatement(ir::Index parent);
    ir::Index ForStatement(ir::Index parent);
    ir::Index ElseIfStatement(ir::Index parent);
    ir::Index ElseStatement(ir::Index parent);
    ir::Index IfStatement(ir::Index parent);
    ir::Index IsStatement(ir::Index parent);
    ir::Index HasStatement(ir::Index parent);
    ir::Index FromStatement(ir::Index parent);
    ir::Index DefaultStatement(ir::Index parent);
    ir::Index WhenStatement(ir::Index parent);
    ir::Index DeclarationStatement(ir::Index parent);
    ir::Index ExpressionStatement(ir::Index parent);
    ir::Index Statement(ir::Index parent);
    ir::Index Scope(ir::Index parent);
    ir::Index Condition(ir::Index parent);
  };
};