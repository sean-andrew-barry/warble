import compiler.text.Builder;

import compiler.ir.Token;
import compiler.ir.Error;
import compiler.ir.Opcode;
import compiler.ir.Symbol;
import compiler.ir.Instruction;
import compiler.ir.Symbols;
import compiler.ir.symbol.Kind;
import compiler.input.Lexer;
import compiler.input.Parser;
import compiler.text.Unicode;

import <string>;
import <string_view>;
import <algorithm>;
import <format>;
import <optional>;
import <vector>;
import <initializer_list>;

namespace {
  using compiler::ir::Token;

  constexpr bool IsCharactersToken(Token token) {
    return token >= Token::Characters0 && token <= Token::CharactersF;
  }

  constexpr uint32_t CharactersDigit(Token token) {
    return static_cast<uint32_t>(token) - static_cast<uint32_t>(Token::Characters0);
  }

  constexpr bool IsDigitsToken(Token token) {
    return token >= Token::Digits0 && token <= Token::DigitsF;
  }

  constexpr uint32_t DigitValue(Token token) {
    return static_cast<uint32_t>(token) - static_cast<uint32_t>(Token::Digits0);
  }

  constexpr char HexChar(uint32_t value) {
    return value < 10 ? static_cast<char>('0' + value) : static_cast<char>('A' + (value - 10));
  }

  bool IsEscapeToken(Token token) {
    switch (token) {
      case Token::EscapeASCII:
      case Token::EscapeHex:
      case Token::EscapeUnicode:
      case Token::EscapeUnicodeBraced1:
      case Token::EscapeUnicodeBraced2:
      case Token::EscapeUnicodeBraced3:
      case Token::EscapeUnicodeBraced4:
      case Token::EscapeUnicodeBraced5:
      case Token::EscapeUnicodeBraced6:
      // case Token::EscapePercent:
        return true;
      default:
        return false;
    }
  }

  std::string SanitizeUtf8(std::string_view text) {
    std::string sanitized;
    sanitized.reserve(text.size());

    for (unsigned char ch : text) {
      switch (ch) {
        case '\\': sanitized += "\\\\"; break;
        case '"': sanitized += "\\\""; break;
        case '\n': sanitized += "\\n"; break;
        case '\r': sanitized += "\\r"; break;
        case '\t': sanitized += "\\t"; break;
        case '\v': sanitized += "\\v"; break;
        case '\f': sanitized += "\\f"; break;
        default:
          if (ch < 0x20) {
            sanitized += "\\x";
            sanitized.push_back(HexChar((ch >> 4) & 0xF));
            sanitized.push_back(HexChar(ch & 0xF));
          } else {
            sanitized.push_back(static_cast<char>(ch));
          }
          break;
      }
    }

    return sanitized;
  }
}

namespace compiler::text {
  void Builder::Append(compiler::ir::Token t) {
    Append(compiler::ir::ToString(t));
  }

  void Builder::Append(compiler::ir::Error e) {
    Append(compiler::ir::ToString(e));
  }

  void Builder::Append(compiler::ir::Opcode opcode) {
    Append(compiler::ir::ToString(opcode));
  }

  void Builder::Append(compiler::ir::Symbol index) {
    Append(index.Row());
  }

  void Builder::Append(compiler::ir::symbol::Kind kind) {
    Append(compiler::ir::symbol::ToString(kind));
  }

  void Builder::Append(const compiler::ir::Instruction& instruction) {
    Append(instruction.Opcode());
    output += '(';
    Append(instruction.Result());
    output += ", ";
    Append(instruction.Primary());
    output += ", ";
    Append(instruction.Secondary());
    output += ')';
  }

  void Builder::Append(const compiler::ir::Symbols& symbols) {
    Line("Symbols(", symbols.Count(), ")");

    for (size_t row = 0; row < symbols.Count(); ++row) {
      const compiler::ir::Symbol i{static_cast<uint32_t>(row)};

      Append("  #");
      Append(i.Row());
      Append(' ');
      Append(symbols.Kind(i));
      Append(" parent=");
      Append(symbols.Parent(i));
      Append(" name=");
      Append(symbols.Name(i));
      Append(" flags=");
      Append(symbols.Flags(i));
      Append(" payload=(");
      Append(symbols.PayloadLow(i));
      Append(", ");
      Append(symbols.PayloadHigh(i));
      Append(")");
      Append(" token=");
      Append(symbols.Token(i));
      Append("\n");
    }
  }

  void Builder::Append(const compiler::input::Parser& parser) {
    Append("=== Symbols ===\n");
    Append(parser.Symbols());
    Append("=== Instructions ===\n");

    const auto& instructions = parser.Instructions();
    Append("Instructions(");
    Append(static_cast<uint64_t>(instructions.size()));
    Append(")\n");

    for (size_t n = 0; n < instructions.size(); ++n) {
      Append("  @");
      Append(static_cast<uint64_t>(n));
      Append(' ');
      Append(instructions[n]);
      Append("\n");
    }
  }

  void Builder::Append(compiler::ir::Instruction& instruction) { Append(std::as_const(instruction)); }
  void Builder::Append(compiler::input::Lexer& lexer) { Append(std::as_const(lexer)); }
  void Builder::Append(compiler::ir::Symbols& symbols) { Append(std::as_const(symbols)); }
  void Builder::Append(compiler::input::Parser& parser) { Append(std::as_const(parser)); }

  void Builder::Append(const compiler::input::Lexer& lexer) {
    const auto& tokens = lexer.Tokens();
    const auto& data = lexer.Data();
    const auto& errors = lexer.Errors();

    size_t token_index = 0;
    size_t data_index = 0;
    size_t error_index = 0;
    size_t string_depth = 0;
    size_t template_string_depth = 0;
    size_t template_expression_depth = 0;

    auto in_string_content = [&]() {
      return string_depth > 0 || (template_string_depth > 0 && template_expression_depth == 0);
    };

    auto print_char = [&]() {
      Append('(');
      if (data_index < data.size()) {
        Append(static_cast<char32_t>(data[data_index++]));
      } else {
        Append("<missing character>");
      }
      Append(')');
    };

    auto print_string = [&](const size_t length) {
      Append('(');
      Append('"');

      for (size_t count = 0; count < length; ++count) {
        if (data_index < data.size()) {
          Append(static_cast<char32_t>(data[data_index++]));
        } else {
          Append("<missing ");
          Append(length - count);
          Append(" characters>");
          break;
        }
      }

      Append('"');
      Append(')');
    };

    while (token_index < tokens.size()) {
      auto token = tokens[token_index];

      if (in_string_content()) {
        switch (token) {
          case compiler::ir::Token::EscapeASCII:
          case compiler::ir::Token::EscapeHex:
          case compiler::ir::Token::EscapeUnicode:
          case compiler::ir::Token::EscapeUnicodeBraced1:
          case compiler::ir::Token::EscapeUnicodeBraced2:
          case compiler::ir::Token::EscapeUnicodeBraced3:
          case compiler::ir::Token::EscapeUnicodeBraced4:
          case compiler::ir::Token::EscapeUnicodeBraced5:
          case compiler::ir::Token::EscapeUnicodeBraced6:
            Append(tokens[token_index++]);
            print_char();
            Append(", ");
            continue;
          case compiler::ir::Token::CarriageReturn:
          case compiler::ir::Token::LineFeed:
          case compiler::ir::Token::LineSeparator:
          case compiler::ir::Token::ParagraphSeparator:
          case compiler::ir::Token::VerticalTab:
          case compiler::ir::Token::FormFeed:
            ++data_index; // skip stored character
            Append(tokens[token_index++]);
            Append(", ");
            continue;
          case compiler::ir::Token::CarriageReturnLineFeed:
            data_index += 2; // skip stored characters
            Append(tokens[token_index++]);
            Append(", ");
            continue;
        }
      }

      if (IsCharactersToken(token)) {
        uint32_t run_length = 0;
        while (token_index < tokens.size() && IsCharactersToken(tokens[token_index])) {
          run_length = (run_length << 4) | CharactersDigit(tokens[token_index]);
          ++token_index;
        }

        Append("Characters");
        Append(run_length);
        Append('(');
        Append('"');
        for (size_t count = 0; count < run_length; ++count) {
          if (data_index < data.size()) {
            Append(static_cast<char32_t>(data[data_index++]));
          } else {
            Append("<missing ");
            Append(run_length - count);
            Append(" characters>");
            break;
          }
        }
        Append('"');
        Append(')');
      } else if (token == compiler::ir::Token::StringOpen) {
        ++string_depth;
        Append(tokens[token_index++]);
      } else if (token == compiler::ir::Token::StringClose) {
        --string_depth;
        Append(tokens[token_index++]);
      } else if (token == compiler::ir::Token::TemplateStringOpen) {
        ++template_string_depth;
        Append(tokens[token_index++]);
      } else if (token == compiler::ir::Token::TemplateStringClose) {
        if (template_string_depth > 0) --template_string_depth;
        Append(tokens[token_index++]);
      } else if (token == compiler::ir::Token::TemplateStringExpressionOpen) {
        if (template_string_depth > 0) ++template_expression_depth;
        Append(tokens[token_index++]);
      } else if (token == compiler::ir::Token::TemplateStringExpressionClose) {
        if (template_expression_depth > 0) --template_expression_depth;
        Append(tokens[token_index++]);
      } else if (IsEscapeToken(token)) {
        Append(tokens[token_index++]);
        print_char();
      } else if (token == compiler::ir::Token::Limbs) {
        // Read limb count encoded as big-endian hex nibbles in Digits0..DigitsF tokens.
        ++token_index; // consume Limbs

        uint32_t limb_count = 0;
        size_t digits_consumed = 0;
        while (token_index < tokens.size() && IsDigitsToken(tokens[token_index])) {
          limb_count = (limb_count << 4) | DigitValue(tokens[token_index]);
          ++token_index;
          ++digits_consumed;
        }

        Append("Limbs");
        Append(limb_count);
        Append('(');

        if (digits_consumed == 0) {
          Append("<missing Digits run>");
        } else {
          const size_t available = (data_index < data.size()) ? (data.size() - data_index) : 0;
          const size_t to_consume = std::min(static_cast<size_t>(limb_count), available);

          // Print all limbs (most-significant first).
          for (size_t i = 0; i < to_consume; ++i) {
            const uint32_t limb = data[data_index + (to_consume - 1 - i)];
            if (i != 0) Append(", ");
            Append(std::format("0x{:08X}", limb));
          }

          if (to_consume != static_cast<size_t>(limb_count)) {
            Append(" <missing limb data>");
          }

          data_index += to_consume;
        }

        Append(')');
      } else if (token == compiler::ir::Token::Error) {
        Append(tokens[token_index++]);
        Append('(');
        if (error_index < errors.size()) {
          Append(errors[error_index++]);
        } else {
          Append("<missing error>");
        }
        Append(')');
      } else {
        Append(tokens[token_index++]);
      }

      Append(", ");
    }
  }
}
