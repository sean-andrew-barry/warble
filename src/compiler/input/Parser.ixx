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
import compiler.ir.Functions;
import compiler.ir.Error;
import compiler.ir.Instruction;
import compiler.ir.Token;
import compiler.ir.Opcode;
import compiler.ir.symbol.Flag;
import compiler.text.cursor.Token;

namespace compiler::input {
  export class Parser {
  private:
    std::vector<ir::Token> tokens;
    std::vector<uint32_t> data;
    size_t data_index = 0; // Current data index
    std::vector<uint32_t> lines; // The token index where each line starts
    
    ir::Symbols symbols;
    ir::Functions functions;
    ir::Symbol undefined; // The undefined symbol index
    ir::Symbol unresolved; // The unresolved symbol index
    ir::Symbol identity; // The identity symbol index
    std::vector<ir::Instruction> instructions;

    text::cursor::Token cursor;
    std::vector<ir::Symbol> declarations; // Stack of active named declarations
    std::vector<ir::Symbol> break_targets; // Stack of active loop exit labels
    std::vector<ir::Symbol> continue_targets; // Stack of active loop continue labels
    std::vector<ir::Symbol> topics; // Stack of active topics

    ir::Symbol NextSymbol();
    void SlideDrop(ir::Symbol symbol);
    ir::Symbol MakeTemporary(ir::Symbol parent, ir::Symbol lhs, ir::Symbol rhs);
    ir::Opcode UnaryPrefixOpcode(ir::Token token) const;
    ir::Opcode UnaryPostfixOpcode(ir::Token token) const;
    ir::Opcode BinaryOpcode(ir::Token token) const;

    void PushDeclarationScope(ir::Symbol scope);
    void PopDeclarationScope(ir::Symbol scope);
    void PushScopeLevelDeclaration(ir::Symbol parent, ir::Symbol declaration);

    ir::Symbol CanonicalIntegerLiteral(ir::Token start_token,
                     ir::Error expected_start,
                     ir::Error expected_limb_count,
                     ir::Error expected_limb_data,
                     ir::Symbol parent);

    // Consumes a run of `Characters*` and `Escape*` tokens and advances `data_index`.
    // Stops at the first non-Characters/non-Escape token (including line breaks).
    // Returns {length_consumed, ok} where ok is false if we ran out of `data`.
    std::pair<uint32_t, bool> ConsumeTextRun();

    constexpr static bool IsTrivialToken(ir::Token token) {
      switch (token) {
        case ir::Token::VerticalTab:
        case ir::Token::FormFeed:
        case ir::Token::None:

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
        case ir::Token::SpacesF:

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
        case ir::Token::TabsF:

        case ir::Token::LineFeed:
        case ir::Token::CarriageReturnLineFeed:
        case ir::Token::CarriageReturn:
        case ir::Token::LineSeparator:
        case ir::Token::ParagraphSeparator:

        case ir::Token::CommentOpen:
        case ir::Token::MultiLineCommentOpen:
          return true;
        default:
          return false;
      }
    }

    constexpr static bool IsEscapeToken(ir::Token token) {
      switch (token) {
        case ir::Token::EscapeASCII:
        case ir::Token::EscapeHex:
        case ir::Token::EscapeUnicode:
        case ir::Token::EscapeUnicodeBraced1:
        case ir::Token::EscapeUnicodeBraced2:
        case ir::Token::EscapeUnicodeBraced3:
        case ir::Token::EscapeUnicodeBraced4:
        case ir::Token::EscapeUnicodeBraced5:
        case ir::Token::EscapeUnicodeBraced6:
          return true;
        default:
          return false;
      }
    }

    constexpr static bool IsCharactersToken(ir::Token token) {
      return token >= ir::Token::Characters0 && token <= ir::Token::CharactersF;
    }

    constexpr static bool IsDigitsToken(ir::Token token) {
      return token >= ir::Token::Digits0 && token <= ir::Token::DigitsF;
    }

    constexpr static bool IsLiteralToken(ir::Token token) {
      switch (token) {
        case ir::Token::ArrayOpen:
        case ir::Token::EnumOpen:
        case ir::Token::ObjectOpen:
        case ir::Token::TupleOpen:
        case ir::Token::CharacterOpen:
        case ir::Token::CaptureOpen:
        case ir::Token::ParameterOpen:
        case ir::Token::TemplateStringOpen:
        case ir::Token::StringOpen:
        case ir::Token::Undefined:
        case ir::Token::Null:
        case ir::Token::Readonly:
        case ir::Token::True:
        case ir::Token::False:
          return true;
        default:
          return false;
      }
    }

    constexpr static bool IsBinaryToken(ir::Token token) {
      switch (token) {
        case ir::Token::Add:
        case ir::Token::Subtract:
        case ir::Token::Multiply:
        case ir::Token::Divide:
        case ir::Token::Modulo:
        case ir::Token::Range:
        case ir::Token::MemberReference:
        case ir::Token::MutableMemberReference:
        case ir::Token::StaticMemberReference:
        case ir::Token::And:
        case ir::Token::Or:
        case ir::Token::Pipeline:
        case ir::Token::Equal:
        case ir::Token::NotEqual:
        case ir::Token::GreaterOrEqual:
        case ir::Token::Greater:
        case ir::Token::LesserOrEqual:
        case ir::Token::Lesser:
        case ir::Token::AssertEqual:
        case ir::Token::AssertGreaterOrEqual:
        case ir::Token::AssertLesserOrEqual:
        case ir::Token::AssertNotEqual:
        case ir::Token::BitwiseLeftShift:
        case ir::Token::BitwiseRightShift:
        case ir::Token::TripleLeftShift:
        case ir::Token::TripleRightShift:
        case ir::Token::BitwiseTripleLeftShift:
        case ir::Token::BitwiseTripleRightShift:
        case ir::Token::BitwiseAnd:
        case ir::Token::BitwiseXor:
        case ir::Token::BitwiseOr:
        case ir::Token::To:
        case ir::Token::Arrow:

        case ir::Token::Assign:
        case ir::Token::AssignAdd:
        case ir::Token::AssignSubtract:
        case ir::Token::AssignMultiply:
        case ir::Token::AssignDivide:
        case ir::Token::AssignModulo:
        case ir::Token::AssignAnd:
        case ir::Token::AssignOr:
        case ir::Token::BitwiseAssignLeftShift:
        case ir::Token::BitwiseAssignRightShift:
        case ir::Token::BitwiseAssignTripleLeftShift:
        case ir::Token::BitwiseAssignTripleRightShift:
        case ir::Token::BitwiseAssignAnd:
        case ir::Token::BitwiseAssignXor:
        case ir::Token::BitwiseAssignOr:
          return true;
        default:
          return false;
      }
    }

    constexpr static bool IsUnaryPrefixToken(ir::Token token) {
      switch (token) {
        case ir::Token::Await:
        case ir::Token::Expect:
        case ir::Token::CopyOf:
        case ir::Token::LengthOf:
        case ir::Token::SymbolOf:
        case ir::Token::ReferenceOf:
        case ir::Token::MutableReferenceOf:
        case ir::Token::Spread:
        case ir::Token::Decrement:
        case ir::Token::Increment:
        case ir::Token::Negative:
        case ir::Token::Positive:
        case ir::Token::Not:
        case ir::Token::BitwiseNot:
          return true;
        default:
          return false;
      }
    }

    constexpr static bool IsModifierToken(ir::Token token) {
      switch (token) {
        case ir::Token::Const:
        case ir::Token::Let:
        case ir::Token::Mut:
        case ir::Token::Private:
        case ir::Token::Protected:
        case ir::Token::Public:
          return true;
        default:
          return false;
      }
    }

    constexpr static bool IsStatementToken(ir::Token token) {
      switch (token) {
        case ir::Token::Do:
        case ir::Token::For:
        case ir::Token::Loop:
        case ir::Token::While:
        case ir::Token::Repeat:
        case ir::Token::Break:
        case ir::Token::Continue:
        case ir::Token::Return:
        case ir::Token::Yield:
        case ir::Token::Panic:
        case ir::Token::If:
          return true;
        default:
          return false;
      }
    }

    int Precedence(ir::Token token) const;
    bool IsRightAssociative(ir::Token token) const;
    void ConsumeCharactersToken();
    void ConsumeEscapeSequence();
    bool Match(ir::Token token, ir::Symbol parent);
    bool Expect(ir::Token token, ir::Symbol parent);
    ir::Symbol ReportError(ir::Error error, ir::Symbol parent);
    ir::Symbol Lookup(ir::Symbol parent);
    ir::Symbol ParsePrimary(ir::Symbol parent);
    ir::Symbol ParseUnaryPrefix(ir::Symbol parent);
    ir::Symbol ParseUnaryPostfix(ir::Symbol parent);
    ir::Symbol ParseBinary(ir::Symbol parent, int min_precedence);

    ir::Symbol Create(ir::symbol::Kind type, ir::Symbol parent);
    ir::Symbol Copy(ir::Symbol source, ir::Symbol parent);
    ir::Symbol Open(ir::symbol::Kind type, ir::Symbol parent);
    ir::Symbol Close(ir::Symbol index);
  public:
    Parser(std::vector<ir::Token>&& tokens, std::vector<uint32_t>&& data);

    virtual ~Parser() = default;

    const ir::Symbols& Symbols() const { return symbols; }
    ir::Symbols& Symbols() { return symbols; }
    const std::vector<ir::Instruction>& Instructions() const { return instructions; }
    std::vector<ir::Instruction>& Instructions() { return instructions; }

    // Debug-friendly entrypoint: parses as many top-level statements as possible.
    ir::Symbol Parse();

    uint32_t Instruct(ir::Opcode opcode, ir::Symbol res = ir::Symbol{}, ir::Symbol lhs = ir::Symbol{}, ir::Symbol rhs = ir::Symbol{});

    ir::Symbol Expect(ir::Token token, ir::Error error, ir::Symbol parent);

    bool None(ir::Symbol parent);
    bool Spaces(ir::Symbol parent);
    bool Tabs(ir::Symbol parent);
    bool NewLine(ir::Symbol parent);
    bool Comment(ir::Symbol parent);
    bool MultiLineComment(ir::Symbol parent);
    bool WhiteSpaceHelper(ir::Symbol parent);
    bool WhiteSpace(ir::Symbol parent);

    // Literals
    ir::Symbol UndefinedLiteral(ir::Symbol parent);
    ir::Symbol NullLiteral(ir::Symbol parent);
    ir::Symbol ThisLiteral(ir::Symbol parent);
    ir::Symbol ThatLiteral(ir::Symbol parent);
    ir::Symbol BooleanLiteral(ir::Symbol parent);
    ir::Symbol CharacterLiteral(ir::Symbol parent);
    ir::Symbol HexLiteral(ir::Symbol parent);
    ir::Symbol OctalLiteral(ir::Symbol parent);
    ir::Symbol BinaryLiteral(ir::Symbol parent);
    ir::Symbol DecimalLiteral(ir::Symbol parent);
    ir::Symbol StringLiteral(ir::Symbol parent);
    ir::Symbol ArrayLiteral(ir::Symbol parent);
    ir::Symbol EnumLiteral(ir::Symbol parent);
    ir::Symbol TupleLiteral(ir::Symbol parent);
    ir::Symbol ObjectLiteral(ir::Symbol parent);
    ir::Symbol TemplateStringLiteral(ir::Symbol parent);
    ir::Symbol FunctionLiteral(ir::Symbol parent);
    ir::Symbol Value(ir::Symbol parent);

    ir::Symbol Identifier(ir::Symbol parent);
    ir::Symbol Destructuring(ir::Symbol parent);
    ir::Symbol Error(ir::Symbol parent);

    ir::Symbol Expression(ir::Symbol parent);
    ir::Symbol ObjectDeclaration(ir::Symbol parent);
    ir::symbol::Flag Modifiers(ir::Symbol parent);
    ir::Symbol Declaration(ir::Symbol parent);

    // Statements
    ir::Symbol ImportStatement(ir::Symbol parent);
    ir::Symbol Permissions(ir::Symbol parent);
    ir::Symbol RegisterStatement(ir::Symbol parent);
    ir::Symbol BreakStatement(ir::Symbol parent);
    ir::Symbol ContinueStatement(ir::Symbol parent);
    ir::Symbol ReturnStatement(ir::Symbol parent);
    ir::Symbol YieldStatement(ir::Symbol parent);
    ir::Symbol PanicStatement(ir::Symbol parent);
    ir::Symbol DoStatement(ir::Symbol parent);
    ir::Symbol WhileStatement(ir::Symbol parent);
    ir::Symbol RepeatStatement(ir::Symbol parent);
    ir::Symbol ForStatement(ir::Symbol parent);
    ir::Symbol ElseStatement(ir::Symbol parent);
    ir::Symbol IfStatement(ir::Symbol parent);
    ir::Symbol IsStatement(ir::Symbol parent, ir::Symbol subject, ir::Symbol when_end);
    ir::Symbol HasStatement(ir::Symbol parent, ir::Symbol subject, ir::Symbol when_end);
    ir::Symbol FromStatement(ir::Symbol parent, ir::Symbol subject, ir::Symbol when_end);
    ir::Symbol DefaultStatement(ir::Symbol parent, ir::Symbol when_end);
    ir::Symbol WhenStatement(ir::Symbol parent);
    ir::Symbol DeclarationStatement(ir::Symbol parent);
    ir::Symbol ExpressionStatement(ir::Symbol parent);
    ir::Symbol Statement(ir::Symbol parent);
    ir::Symbol Scope(ir::Symbol parent);
    ir::Symbol Condition(ir::Symbol parent);
    ir::Symbol ScopeOrInlineStatement(ir::Symbol parent);
  };
};
