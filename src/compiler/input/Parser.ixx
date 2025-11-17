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
import compiler.ir.Error;
import compiler.ir.Instruction;
import compiler.ir.Token;
import compiler.ir.Opcode;
import compiler.text.cursor.Token;

namespace compiler::input {
  export class Parser {
  private:
    std::vector<ir::Token> tokens;
    std::vector<uint32_t> data;
    uint32_t data_position = 0; // Current data index
    std::vector<uint32_t> lines; // The token index where each line starts
    
    ir::Symbols symbols;
    std::vector<uint32_t> limbs; // The data storage for big integers
    std::vector<ir::Instruction> instructions;

    text::cursor::Token cursor;
    std::vector<ir::Index> declarations; // Stack of active named declarations
    std::vector<ir::Index> break_targets; // Stack of active loop exit labels
    std::vector<ir::Index> continue_targets; // Stack of active loop continue labels
    std::vector<ir::Index> topics; // Stack of active topics

    ir::Index NextSymbol();
    void SlideDestruct(ir::Index symbol);
    ir::Index MakeTemporary(ir::Index parent, ir::Index lhs, ir::Index rhs);
    ir::Opcode UnaryOpcode(ir::Token token) const;
    ir::Opcode BinaryOpcode(ir::Token token) const;

    constexpr static bool IsTrivialToken(ir::Token token) {
      // return TRIVIAL_SET.test(static_cast<uint8_t>(token));
      return false; // TODO
    }

    constexpr static bool IsEscapeToken(ir::Token token) {
      // return ESCAPE_SET.test(static_cast<uint8_t>(token));
      return false; // TODO
    }

    constexpr static bool IsLiteralToken(ir::Token token) {
      // return LITERAL_SET.test(static_cast<uint8_t>(token));
      return false; // TODO
    }

    constexpr static bool IsBinaryToken(ir::Token token) {
      // return BINARY_SET.test(static_cast<uint8_t>(token));
      return false; // TODO
    }

    constexpr static bool IsUnaryPrefixToken(ir::Token token) {
      // return UNARY_PREFIX_SET.test(static_cast<uint8_t>(token));
      return false; // TODO
    }

    constexpr static bool IsModifierToken(ir::Token token) {
      // return MODIFIER_SET.test(static_cast<uint8_t>(token));
      return false; // TODO
    }

    constexpr static bool IsStatementToken(ir::Token token) {
      // return STATEMENT_SET.test(static_cast<uint8_t>(token));
      return false; // TODO
    }

    int Precedence(ir::Token token) const;
    void ConsumeCharactersToken();
    void ConsumeEscapeSequence();
    ir::Index ReportError(ir::Error error, ir::Index parent);
    ir::Index ParsePrimary(ir::Index parent);
    ir::Index ParseUnary(ir::Index parent);
    ir::Index ParseBinary(ir::Index parent, int min_precedence);

    ir::Index Open(ir::symbol::Kind type, ir::Index parent);
    ir::Index Close(ir::Index index);
  public:
    Parser(std::vector<ir::Token>&& tokens, std::vector<uint32_t>&& data);

    virtual ~Parser() = default;

    uint32_t Instruct(ir::Opcode opcode, ir::Index res = ir::Index{}, ir::Index lhs = ir::Index{}, ir::Index rhs = ir::Index{});

    ir::Index Create(ir::symbol::Kind type, ir::Index parent);

    ir::Index Expect(ir::Token token, ir::Error error, ir::Index parent);

    bool None(ir::Token token);
    bool Spaces(ir::Token token);
    bool Tabs(ir::Token token);
    bool NewLine(ir::Token token);
    bool Comment(ir::Token token);
    bool MultiLineComment(ir::Token token);
    bool WhiteSpace(ir::Token token);

    // Literals
    ir::Index Undefined(ir::Index parent);
    ir::Index Null(ir::Index parent);
    ir::Index Boolean(ir::Index parent);
    ir::Index Character(ir::Index parent);
    ir::Index Hex(ir::Index parent);
    ir::Index Octal(ir::Index parent);
    ir::Index Binary(ir::Index parent);
    ir::Index Decimal(ir::Index parent);
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
    ir::Index ElseStatement(ir::Index parent);
    ir::Index IfStatement(ir::Index parent);
    ir::Index IsStatement(ir::Index parent, ir::Index subject, ir::Index when_end);
    ir::Index HasStatement(ir::Index parent, ir::Index subject, ir::Index when_end);
    ir::Index FromStatement(ir::Index parent, ir::Index subject, ir::Index when_end);
    ir::Index DefaultStatement(ir::Index parent, ir::Index when_end);
    ir::Index WhenStatement(ir::Index parent);
    ir::Index DeclarationStatement(ir::Index parent);
    ir::Index ExpressionStatement(ir::Index parent);
    ir::Index Statement(ir::Index parent);
    ir::Index Scope(ir::Index parent);
    ir::Index Condition(ir::Index parent);
  };
};
