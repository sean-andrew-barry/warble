import compiler.text.Builder;

import compiler.ir.Token;
import compiler.ir.Error;
import compiler.ir.Opcode;
import compiler.ir.Index;
import compiler.ir.Instruction;
import compiler.input.Lexer;
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

  void Builder::Append(compiler::ir::Index index) {
    Append(index.Row());
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

  void Builder::Append(compiler::ir::Instruction& instruction) { Append(std::as_const(instruction)); }
  void Builder::Append(compiler::input::Lexer& lexer) { Append(std::as_const(lexer)); }

  void Builder::Append(const compiler::input::Lexer& lexer) {
    const auto& tokens = lexer.Tokens();
    const auto& characters = lexer.Characters();
    // const auto& limbs = lexer.Limbs();
    const auto& errors = lexer.Errors();

    size_t token_index = 0;
    size_t character_index = 0;
    // size_t limb_index = 0;
    size_t error_index = 0;

    while (token_index < tokens.size()) {
      auto token = tokens[token_index];

      if (IsCharactersToken(token)) {
        uint32_t run_length = 0;
        while (token_index < tokens.size() && IsCharactersToken(tokens[token_index])) {
          // Append(tokens[token_index]);
          // Append(", ");
          run_length = (run_length << 4) | CharactersDigit(tokens[token_index]);
          ++token_index;
        }

        Append("Characters");
        Append(run_length);
        Append('(');
        Append('"');
        for (size_t count = 0; count < run_length; ++count) {
          if (character_index < characters.size()) {
            Append(characters[character_index++]);
          } else {
            Append("<missing character>");
          }
        }
        Append('"');
        Append(')');
      } else if (IsEscapeToken(token)) {
        Append(tokens[token_index++]);
        Append('(');
        Append(characters[character_index++]);
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
