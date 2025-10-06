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

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Instruction;
import compiler.ir.Token;
import compiler.text.cursor.Token;

namespace compiler::input {
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
    ir::Token::Spaces,
    ir::Token::Tabs,
    ir::Token::LineFeeds,
    ir::Token::CarriageReturnLineFeeds,
    ir::Token::CarriageReturns,
    ir::Token::LineSeparators,
    ir::Token::ParagraphSeparators,
    ir::Token::VerticalTab,
    ir::Token::FormFeed,
    ir::Token::CommentOpen,
    ir::Token::CommentClose,
    ir::Token::MultiLineCommentOpen,
    ir::Token::MultiLineCommentClose,
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
    program::Module& mod;
    text::cursor::Token cursor;

    // The indexes track the position within the module's various storage vectors 
    size_t widths_index = 0; // Increments after seeing a variable width token, such as `Spaces` or `Characters`
    size_t symbols_index = 0; // Increments after seeing a symbol token, such as `Identifier` or `Decimal`
    size_t lines_index = 0; // Increments after seeing a line break token, such as `LineFeed` or `CarriageReturnLineFeed`
  public:
    virtual ~Parser() = default;

    void Skip();
    bool Match(ir::Token token);
    bool Expect(ir::Token token);

    void Instruct(ir::Opcode opcode, ir::Index res = ir::Index{}, ir::Index lhs = ir::Index{}, ir::Index rhs = ir::Index{});

    ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type);
    ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type, uint64_t value);
    ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type, double value);
    ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type, char32_t value);
    ir::Index Parser::Create(ir::Index parent, ir::symbol::Type type, bool value);

    // Literals
    ir::Index Parser::Undefined(ir::Index parent);
    ir::Index Parser::Null(ir::Index parent);
    ir::Index Parser::Boolean(ir::Index parent);
    ir::Index Parser::Character(ir::Index parent);
    ir::Index Parser::Integer(ir::Index parent);
    ir::Index Parser::Decimal(ir::Index parent);
    ir::Index Parser::String(ir::Index parent);
    ir::Index Parser::Array(ir::Index parent);
    ir::Index Parser::Enum(ir::Index parent);
    ir::Index Parser::Tuple(ir::Index parent);
    ir::Index Parser::Object(ir::Index parent);
    ir::Index Parser::TemplateString(ir::Index parent);
    ir::Index Parser::Function(ir::Index parent);

    ir::Index Parser::Identifier(ir::Index parent);
    ir::Index Parser::Destructuring(ir::Index parent);
    ir::Index Parser::Error(ir::Index parent);

    ir::Index Parser::Expression(ir::Index parent);
    ir::Index Parser::ObjectDeclaration(ir::Index parent);
    ir::Index Parser::Declaration(ir::Index parent);

    // Statements
    ir::Index Parser::ImportStatement(ir::Index parent);
    ir::Index Parser::Permissions(ir::Index parent);
    ir::Index Parser::RegisterStatement(ir::Index parent);
    ir::Index Parser::BreakStatement(ir::Index parent);
    ir::Index Parser::ContinueStatement(ir::Index parent);
    ir::Index Parser::ReturnStatement(ir::Index parent);
    ir::Index Parser::DoStatement(ir::Index parent);
    ir::Index Parser::WhileStatement(ir::Index parent);
    ir::Index Parser::RepeatStatement(ir::Index parent);
    ir::Index Parser::ForStatement(ir::Index parent);
    ir::Index Parser::ElseIfStatement(ir::Index parent);
    ir::Index Parser::ElseStatement(ir::Index parent);
    ir::Index Parser::IfStatement(ir::Index parent);
    ir::Index Parser::IsStatement(ir::Index parent);
    ir::Index Parser::HasStatement(ir::Index parent);
    ir::Index Parser::DefaultStatement(ir::Index parent);
    ir::Index Parser::MatchStatement(ir::Index parent);
    ir::Index Parser::DeclarationStatement(ir::Index parent);
    ir::Index Parser::ExpressionStatement(ir::Index parent);
    ir::Index Parser::Statement(ir::Index parent);
    ir::Index Parser::Scope(ir::Index parent);
    ir::Index Parser::Condition(ir::Index parent);
  };
};