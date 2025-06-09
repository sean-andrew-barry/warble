export module lexical.lexer;

import ir.context._module;

import node.scope.context._module;
import lexical.source;
import lexical.tokens;
import lexical.cursor.string;
import lexical.cursor.code_points;
import lexical.cursor.token;
import utility.print;
import lexical.unicode;
import <vector>;
import <string>;
import <string_view>;
import <type_traits>;
import <algorithm>;
import <charconv>;
import <stdexcept>;
import <utility>;

namespace lexical {
  export class Lexer {
  private:
    node::scope::context::Module& mod;
    lexical::Source& source;
    lexical::cursor::String cursor;
    // std::string_view stored;
    bool maybe_declaration = false;
    std::vector<char32_t> saved;
  public:
    constexpr bool Done() const { return cursor.Done(); }
    constexpr bool Valid() const { return cursor.Valid(); }
  private:
    struct Position {
      std::string::const_iterator start;
      size_t size;
    };

    constexpr void Advance(size_t n = 1) { cursor.Advance(n); }
    constexpr void Retreat(size_t n = 1) { cursor.Retreat(n); }
    constexpr void Retreat(std::string::const_iterator iter) { cursor.Retreat(iter); }
    constexpr decltype(auto) Peek() const { return cursor.Peek(); }
    constexpr decltype(auto) Peek(size_t n) const { return cursor.Peek(n); }
    constexpr bool Check(const char c) const { return cursor.Check(c); }
    constexpr bool Check(const std::string_view view) const { return cursor.Check(view); }
    constexpr bool Match(const char c) { return cursor.Match(c); }
    constexpr bool Match(const std::string_view view) { return cursor.Match(view); }
    constexpr bool Keyword(const std::string_view view) { return cursor.Keyword(view); }
    constexpr decltype(auto) cbegin() const { return cursor.cbegin(); }
    constexpr decltype(auto) cend() const { return cursor.cend(); }
    decltype(auto) CodePoint() { return cursor.CodePoint(); }
    Position Start() const { return Position{cursor.cbegin(), source.Size()}; }

    lexical::Tokens Last(size_t i = 0) {
      const auto& tokens = source.Tokens();

      while (tokens.size() > i) {
        const auto& token = tokens.at(tokens.size() - 1 - i++);
        if (!lexical::cursor::Token::IsTrivial(token.Type())) return token.Type();
      }

      return lexical::Tokens::NONE;
    }

    void Rollback(Position position) {
      cursor.Retreat(position.start);
      source.Rollback(position.size);
    }

    void Save(lexical::Tokens type, uint32_t width, uint32_t offset = 0) {
      source.Push(type, width, offset);

      if (!lexical::cursor::Token::IsTrivial(type)) {
        utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(source.Read()));
        // utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(std::string{cbegin() - width, cbegin()}));
      }
    }

    void Store(lexical::Tokens type, uint32_t width, uint32_t offset = 0) {
      return Save(type, width, offset);
      // stored = std::string_view{cbegin(), width};
      // source.Push(type, width, offset);

      // if (!lexical::cursor::Token::IsTrivial(type)) {
      //   utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(source.Read()));
      //   // utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(std::string{cbegin() - width, cbegin()}));
      // }
    }

    bool Skip(lexical::Tokens type, size_t width) {
      Advance(width);
      Save(type, static_cast<uint32_t>(width));
      WS();
      return true;
    }

    bool Try(const auto& fn) {
      auto start = Start();

      if (fn()) {
        return true;
      } else {
        Rollback(start);
        return false;
      }
    }

    template<std::invocable Func>
    bool Run(Func fn) {
      if constexpr (std::same_as<std::invoke_result_t<Func>, void>) {
        fn();
        return true;
      } else if constexpr (std::same_as<std::invoke_result_t<Func>, bool>) {
        return fn();
      } else {
        static_assert(std::disjunction_v<std::is_same<std::invoke_result_t<Func>, void>, 
                                        std::is_same<std::invoke_result_t<Func>, bool>>, 
                                        "Func must return either void or bool.");
      }
    }

    bool ZeroOrMore(const auto& fn) {
      if (Done()) return false;

      while (true) {
        auto start = Start();

        if (!fn()) {
          Rollback(start);
          break;
        }
      }

      return true;
    }

    bool ZeroOrMore(const auto& fn1, const auto& fn2) {
      if (Done()) return false;

      while (true) {
        auto start = Start();

        if (!fn1()) {
          Rollback(start);
          break;
        } else if (!fn2()) {
          break; // Break without rolling back
        }
      }

      return true;
    }

    bool OneOrMore(const auto& fn1, const auto& fn2) {
      if (Done()) return false;

      auto start = Start();

      // Make sure it matches at least once
      if (fn1()) {
        if (!fn2()) {
          return true;
        }
      }
      else {
        Rollback(start);
        return false;
      }

      while (true) {
        auto start = Start();

        if (!fn1()) {
          Rollback(start);
          break;
        } else if (!fn2()) {
          break; // Break without rolling back
        }
      }

      return true;
    }

    bool OneOrMore(const auto& fn) {
      if (Done()) return false;

      auto start = Start();

      // Make sure it matches at least once
      if (!fn()) {
        Rollback(start);
        return false;
      }

      while (true) {
        auto start = Start();

        if (!fn()) {
          Rollback(start);
          break;
        }
      }

      return true;
    }

    bool Test(lexical::Tokens type, const std::string_view value) {
      if (!Match(value)) return false;

      Save(type, static_cast<uint32_t>(value.size()));
      WS();
      return true;
    }

    bool Test(lexical::Tokens type, const char value) {
      if (!Match(value)) return false;

      Save(type, 1u);
      WS();

      return true;
    }

    bool Test(lexical::Tokens type, const auto value, const std::string_view expected) {
      if (Test(type, value)) {
        return true;
      } else {
        return Error(type, expected);
      }
    }

    bool Error(lexical::Tokens type, const std::string_view expected) {
      // Print("ERROR: ", expected, " token ", token);

      // Display();

      // recovery = true;
      // Save(lexical::Tokens::ERROR, expected);
      // utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(source.Read()));
      utility::Debug("ERROR:", expected, "token", lexical::Token{type});
      Save(lexical::Tokens::ERROR, 0);
      WS();
      return false;
    }

    bool Error(const std::string_view expected) {
      // Print("ERROR: ", expected);

      // Display();

      // recovery = true;
      // Save(lexical::Tokens::ERROR, expected);
      utility::Debug("ERROR:", expected);
      Save(lexical::Tokens::ERROR, 0);
      WS();
      return false;
    }

    bool Keyword(lexical::Tokens type, const std::string_view value) {
      if (!Keyword(value)) return false;

      Save(type, static_cast<uint32_t>(value.size()));
      WS();
      return true;
    }

    bool Keyword(lexical::Tokens type, const std::string_view value, const std::string_view expected) {
      if (Keyword(type, value)) {
        return true;
      } else {
        return Error(type, expected);
      }
    }

    bool Lexer::Spaces(uint32_t count) {
      switch (count) {
        case 0: return false;
        case 1: return Emit(Tokens::SPACE1);
        case 2: return Emit(Tokens::SPACE2);
        case 3: return Emit(Tokens::SPACE3);
        case 4: return Emit(Tokens::SPACE4);
        case 5: return Emit(Tokens::SPACE5);
        case 6: return Emit(Tokens::SPACE6);
        case 7: return Emit(Tokens::SPACE7);
        case 8: return Emit(Tokens::SPACE8);
        case 9: return Emit(Tokens::SPACE8, Tokens::SPACE1);
        case 10: return Emit(Tokens::SPACE8, Tokens::SPACE2);
        case 11: return Emit(Tokens::SPACE8, Tokens::SPACE3);
        case 12: return Emit(Tokens::SPACE8, Tokens::SPACE4);
        case 13: return Emit(Tokens::SPACE8, Tokens::SPACE5);
        case 14: return Emit(Tokens::SPACE8, Tokens::SPACE6);
        case 15: return Emit(Tokens::SPACE8, Tokens::SPACE7);
        case 16: return Emit(Tokens::SPACE16);
        default: return Emit(Tokens::SPACE16) && Spaces(count - 16);
      }
    }

    bool Lexer::Tabs(uint32_t count) {
      switch (count) {
        case 0: return false;
        case 1: return Emit(Tokens::TAB1);
        case 2: return Emit(Tokens::TAB2);
        case 3: return Emit(Tokens::TAB3);
        case 4: return Emit(Tokens::TAB4);
        case 5: return Emit(Tokens::TAB5);
        case 6: return Emit(Tokens::TAB6);
        case 7: return Emit(Tokens::TAB7);
        case 8: return Emit(Tokens::TAB8);
        case 9: return Emit(Tokens::TAB8, Tokens::TAB1);
        case 10: return Emit(Tokens::TAB8, Tokens::TAB2);
        case 11: return Emit(Tokens::TAB8, Tokens::TAB3);
        case 12: return Emit(Tokens::TAB8, Tokens::TAB4);
        case 13: return Emit(Tokens::TAB8, Tokens::TAB5);
        case 14: return Emit(Tokens::TAB8, Tokens::TAB6);
        case 15: return Emit(Tokens::TAB8, Tokens::TAB7);
        case 16: return Emit(Tokens::TAB16);
        default: return Emit(Tokens::TAB16) && Tabs(count - 16);
      }
    }

    bool Lexer::LFs(uint32_t count) {
      switch (count) {
        case 0: return false;
        case 1: return Emit(Tokens::NEWLINE_LF1);
        case 2: return Emit(Tokens::NEWLINE_LF2);
        case 3: return Emit(Tokens::NEWLINE_LF3);
        case 4: return Emit(Tokens::NEWLINE_LF4);
        case 5: return Emit(Tokens::NEWLINE_LF5);
        case 6: return Emit(Tokens::NEWLINE_LF6);
        case 7: return Emit(Tokens::NEWLINE_LF7);
        case 8: return Emit(Tokens::NEWLINE_LF8);
        case 9: return Emit(Tokens::NEWLINE_LF8, Tokens::NEWLINE_LF1);
        case 10: return Emit(Tokens::NEWLINE_LF8, Tokens::NEWLINE_LF2);
        case 11: return Emit(Tokens::NEWLINE_LF8, Tokens::NEWLINE_LF3);
        case 12: return Emit(Tokens::NEWLINE_LF8, Tokens::NEWLINE_LF4);
        case 13: return Emit(Tokens::NEWLINE_LF8, Tokens::NEWLINE_LF5);
        case 14: return Emit(Tokens::NEWLINE_LF8, Tokens::NEWLINE_LF6);
        case 15: return Emit(Tokens::NEWLINE_LF8, Tokens::NEWLINE_LF7);
        case 16: return Emit(Tokens::NEWLINE_LF16);
        default: return Emit(Tokens::NEWLINE_LF16) && LFs(count - 16);
      }
    }

    bool Lexer::CRLFs(uint32_t count) {
      switch (count) {
        case 0: return false;
        case 1: return Emit(Tokens::NEWLINE_CRLF1);
        case 2: return Emit(Tokens::NEWLINE_CRLF2);
        case 3: return Emit(Tokens::NEWLINE_CRLF3);
        case 4: return Emit(Tokens::NEWLINE_CRLF4);
        case 5: return Emit(Tokens::NEWLINE_CRLF5);
        case 6: return Emit(Tokens::NEWLINE_CRLF6);
        case 7: return Emit(Tokens::NEWLINE_CRLF7);
        case 8: return Emit(Tokens::NEWLINE_CRLF8);
        case 9: return Emit(Tokens::NEWLINE_CRLF8, Tokens::NEWLINE_CRLF1);
        case 10: return Emit(Tokens::NEWLINE_CRLF8, Tokens::NEWLINE_CRLF2);
        case 11: return Emit(Tokens::NEWLINE_CRLF8, Tokens::NEWLINE_CRLF3);
        case 12: return Emit(Tokens::NEWLINE_CRLF8, Tokens::NEWLINE_CRLF4);
        case 13: return Emit(Tokens::NEWLINE_CRLF8, Tokens::NEWLINE_CRLF5);
        case 14: return Emit(Tokens::NEWLINE_CRLF8, Tokens::NEWLINE_CRLF6);
        case 15: return Emit(Tokens::NEWLINE_CRLF8, Tokens::NEWLINE_CRLF7);
        case 16: return Emit(Tokens::NEWLINE_CRLF16);
        default: return Emit(Tokens::NEWLINE_CRLF16) && CRLFs(count - 16);
      }
    }

    bool Lexer::Chars(uint32_t count) {
      switch (count) {
        case 0: return false;
        case 1: return Emit(Tokens::CHAR1);
        case 2: return Emit(Tokens::CHAR1, Tokens::CHAR1);
        case 3: return Emit(Tokens::CHAR3);
        case 4: return Emit(Tokens::CHAR3, Tokens::CHAR1);
        case 5: return Emit(Tokens::CHAR5);
        case 6: return Emit(Tokens::CHAR5, Tokens::CHAR1);
        case 7: return Emit(Tokens::CHAR7);
        case 8: return Emit(Tokens::CHAR8);
        case 9: return Emit(Tokens::CHAR8, Tokens::CHAR1);
        case 10: return Emit(Tokens::CHAR7, Tokens::CHAR3);
        case 11: return Emit(Tokens::CHAR8, Tokens::CHAR3);
        case 12: return Emit(Tokens::CHAR7, Tokens::CHAR5);
        case 13: return Emit(Tokens::CHAR8, Tokens::CHAR5);
        case 14: return Emit(Tokens::CHAR7, Tokens::CHAR7);
        case 15: return Emit(Tokens::CHAR8, Tokens::CHAR7);
        case 16: return Emit(Tokens::CHAR16);
        default: return Emit(Tokens::CHAR16) && Chars(count - 16);
      }
    }

    bool Lexer::Digits(uint32_t count) {
      switch (count) {
        case 0: return false;
        case 1: return Emit(Tokens::DIGIT1);
        case 2: return Emit(Tokens::DIGIT1, Tokens::DIGIT1);
        case 3: return Emit(Tokens::DIGIT3);
        case 4: return Emit(Tokens::DIGIT3, Tokens::DIGIT1);
        case 5: return Emit(Tokens::DIGIT5);
        case 6: return Emit(Tokens::DIGIT5, Tokens::DIGIT1);
        case 7: return Emit(Tokens::DIGIT7);
        case 8: return Emit(Tokens::DIGIT8);
        case 9: return Emit(Tokens::DIGIT8, Tokens::DIGIT1);
        case 10: return Emit(Tokens::DIGIT7, Tokens::DIGIT3);
        case 11: return Emit(Tokens::DIGIT8, Tokens::DIGIT3);
        case 12: return Emit(Tokens::DIGIT7, Tokens::DIGIT5);
        case 13: return Emit(Tokens::DIGIT8, Tokens::DIGIT5);
        case 14: return Emit(Tokens::DIGIT7, Tokens::DIGIT7);
        case 15: return Emit(Tokens::DIGIT8, Tokens::DIGIT7);
        case 16: return Emit(Tokens::DIGIT16);
        default: return Emit(Tokens::DIGIT16) && Digits(count - 16);
      }
    }

    bool Lexer::Identifiers(uint32_t count) {
      switch (count) {
        case 0: return false;
        case 1: return Emit(Tokens::IDENTIFIER1);
        case 2: return Emit(Tokens::IDENTIFIER1, Tokens::IDENTIFIER1);
        case 3: return Emit(Tokens::IDENTIFIER3);
        case 4: return Emit(Tokens::IDENTIFIER3, Tokens::IDENTIFIER1);
        case 5: return Emit(Tokens::IDENTIFIER5);
        case 6: return Emit(Tokens::IDENTIFIER5, Tokens::IDENTIFIER1);
        case 7: return Emit(Tokens::IDENTIFIER7);
        case 8: return Emit(Tokens::IDENTIFIER8);
        case 9: return Emit(Tokens::IDENTIFIER8, Tokens::IDENTIFIER1);
        case 10: return Emit(Tokens::IDENTIFIER7, Tokens::IDENTIFIER3);
        case 11: return Emit(Tokens::IDENTIFIER8, Tokens::IDENTIFIER3);
        case 12: return Emit(Tokens::IDENTIFIER7, Tokens::IDENTIFIER5);
        case 13: return Emit(Tokens::IDENTIFIER8, Tokens::IDENTIFIER5);
        case 14: return Emit(Tokens::IDENTIFIER7, Tokens::IDENTIFIER7);
        case 15: return Emit(Tokens::IDENTIFIER8, Tokens::IDENTIFIER7);
        case 16: return Emit(Tokens::IDENTIFIER16);
        default: return Emit(Tokens::IDENTIFIER16) && Identifiers(count - 16);
      }
    }
  public:
    Lexer(node::scope::context::Module& mod, lexical::Source& source)
      : mod{mod}, source{source}, cursor{source.Text()}
    {
      WS(); // Consume any leading white space
    }

    bool WhiteSpace() {
      switch (Peek()) {
        case ' ': return Spaces(Consume(' '));
        case '\n': return LFs(Consume('\n'));
        case '\t': return Tabs(Consume('\t'));
        case '\v': {
          Advance();
          return Emit(lexical::Tokens::VERTICAL_TAB);
        }
        case '\f': {
          Advance();
          return Emit(lexical::Tokens::FORM_FEED);
        }
        case '\r': {
          auto count = Count("\r\n");
          if (count > 0) {
            Advance(count * 2);
            return CRLFs(count);
          }

          // If it wasn't a sequence of CRLFs then just emit a CR cause we don't have a compact form for that
          Advance(1);
          return Emit(lexical::Tokens::NEWLINE_CR);
        }
        case '/': {
          switch (Peek(1)) {
            case '/': {
              Advance(2);
              Emit(lexical::Tokens::COMMENT_OPEN);

              uint32_t count = 0;
              while (true) {
                if (Done()) {
                  // Reached the end of the source
                  return Characters(count);
                } else if (cursor.IsBreak()) {
                  Characters(count);
                  return Emit(lexical::Tokens::COMMENT_CLOSE);
                }

                Advance();
                ++count;
              }
            }
            case '*': {
              Advance(2); // Consume the opening `'\*'`
              Emit(lexical::Tokens::MULTI_LINE_COMMENT_OPEN);

              uint32_t count = 0;
              while (Valid()) {
                if (Peek() == '*' && Peek(1) == '/') {
                  // Reached the end of the source
                  Characters(count);
                  Advance(2); // Consume the closing `'*/'`
                  return Emit(MULTI_LINE_COMMENT_CLOSE);
                }

                Advance();
                ++count;
              }

              return Error("Expected to find a closing `*/` to end the multi-line comment.");
            }
            default: return false; // Wasn't a comment
          }
        }
        default: {
          if (!cursor.IsASCII()) {
            auto iter = cbegin();
            auto code_point = CodePoint(); // This consumes the full code point automatically

            // Check for a line or paragraph separator
            if (code_point == 0x2028) {
              code_point_offset += 2;
              return Emit(lexical::Tokens::NEWLINE_LS);
            } else if (code_point == 0x2029) {
              code_point_offset += 2;
              return Emit(lexical::Tokens::NEWLINE_PS);
            } else if (Unicode::IsWhiteSpace(code_point)) {
              uint32_t length = static_cast<uint32_t>(std::distance(iter, cbegin()));
              code_point_offset += length - 1;
              return Emit(lexical::Tokens::SPACE1);
            } else {
              Retreat(iter); // Undo the consumption of the code point
              return false; // Wasn't whitespace
            }
          }

          return false; // Wasn't unicode
        }
      }
    }

    bool WS() {
      bool matched = false;
      
      while (cursor.IsPossibleSpaceStart() && WhiteSpace()) {
        matched = true;
      }

      return matched;
    }

    // Optional white space
    bool OWS() { WS(); return true; }

    // Keywords
    bool Null() { return Keyword(lexical::Tokens::_NULL, "null"); }
    bool Undefined() { return Keyword(lexical::Tokens::UNDEFINED, "undefined"); }
    bool True() { return Keyword(lexical::Tokens::TRUE, "true"); }
    bool False() { return Keyword(lexical::Tokens::FALSE, "false"); }
    bool This() { return Keyword(lexical::Tokens::THIS, "this"); }
    bool Import() { return Keyword(lexical::Tokens::IMPORT, "import"); }
    bool Export() { return Keyword(lexical::Tokens::EXPORT, "export"); }
    bool Register() { return Keyword(lexical::Tokens::REGISTER, "register"); }
    bool From() { return Keyword(lexical::Tokens::FROM, "from"); }
    bool With() { return Keyword(lexical::Tokens::WITH, "with"); }
    bool Has() { return Keyword(lexical::Tokens::HAS, "has"); }
    bool If() { return Keyword(lexical::Tokens::IF, "if"); }
    bool Else() { return Keyword(lexical::Tokens::ELSE, "else"); }
    bool Do() { return Keyword(lexical::Tokens::DO, "do"); }
    bool While() { return Keyword(lexical::Tokens::WHILE, "while"); }
    bool Repeat() { return Keyword(lexical::Tokens::REPEAT, "repeat"); }
    bool Is() { return Keyword(lexical::Tokens::IS, "is"); }
    bool In() { return Keyword(lexical::Tokens::IN, "in"); }
    bool For() { return Keyword(lexical::Tokens::FOR, "for"); }
    bool As() { return Keyword(lexical::Tokens::AS, "as"); }
    bool Default() { return Keyword(lexical::Tokens::DEFAULT, "default"); }
    bool Auto() { return Keyword(lexical::Tokens::AUTO, "auto"); }
    bool Void() { return Keyword(lexical::Tokens::VOID, "void"); }
    bool Match() { return Keyword(lexical::Tokens::MATCH, "match"); }
    bool Await() { return Keyword(lexical::Tokens::AWAIT, "await"); }
    bool Compiler() { return Keyword(lexical::Tokens::COMPILER, "compiler"); }
    bool Break() { return Keyword(lexical::Tokens::BREAK, "break"); }
    bool Continue() { return Keyword(lexical::Tokens::CONTINUE, "continue"); }
    bool Return() { return Keyword(lexical::Tokens::RETURN, "return"); }
    bool Case() { return Keyword(lexical::Tokens::CASE, "case"); }
    bool Yield() { return Keyword(lexical::Tokens::YIELD, "yield"); }
    bool Let() { return Keyword(lexical::Tokens::LET, "let"); }

    // Symbols
    bool TemplateOpen() { return Test(lexical::Tokens::TEMPLATE_OPEN, '<'); }
    bool TemplateClose() { return Test(lexical::Tokens::TEMPLATE_CLOSE, '>'); }
    bool CaptureOpen() { return Test(lexical::Tokens::CAPTURE_OPEN, '['); }
    bool CaptureClose() { return Test(lexical::Tokens::CAPTURE_CLOSE, ']'); }
    bool ParameterOpen() { return Test(lexical::Tokens::PARAMETER_OPEN, '('); }
    bool ParameterClose() { return Test(lexical::Tokens::PARAMETER_CLOSE, ')'); }
    bool TupleOpen() { return Test(lexical::Tokens::TUPLE_OPEN, '('); }
    bool TupleClose() { return Test(lexical::Tokens::TUPLE_CLOSE, ')'); }
    bool ScopeOpen() { return Test(lexical::Tokens::SCOPE_OPEN, '{'); }
    bool ScopeClose() { return Test(lexical::Tokens::SCOPE_CLOSE, '}'); }
    bool ObjectOpen() { return Test(lexical::Tokens::OBJECT_OPEN, '{'); }
    bool ObjectClose() { return Test(lexical::Tokens::OBJECT_CLOSE, '}'); }
    bool ArrayOpen() { return Test(lexical::Tokens::ARRAY_OPEN, '['); }
    bool ArrayClose() { return Test(lexical::Tokens::ARRAY_CLOSE, ']'); }
    bool EnumOpen() { return Test(lexical::Tokens::ENUM_OPEN, '<'); }
    bool EnumClose() { return Test(lexical::Tokens::ENUM_CLOSE, '>'); }
    bool ImportsOpen() { return Test(lexical::Tokens::IMPORTS_OPEN, '{'); }
    bool ImportsClose() { return Test(lexical::Tokens::IMPORTS_CLOSE, '}'); }
    bool CharOpen() { return Test(lexical::Tokens::CHAR_OPEN, '\''); }
    bool CharClose() { return Test(lexical::Tokens::CHAR_CLOSE, '\''); }
    bool StringOpen() { return Test(lexical::Tokens::STRING_OPEN, '"'); }
    bool StringClose() { return Test(lexical::Tokens::STRING_CLOSE, '"'); }
    bool TemplateStringOpen() { return Test(lexical::Tokens::TEMPLATE_STRING_OPEN, '`'); }
    bool TemplateStringClose() { return Test(lexical::Tokens::TEMPLATE_STRING_CLOSE, '`'); }
    bool TemplateStringExpressionOpen() { return Test(lexical::Tokens::TEMPLATE_STRING_EXPRESSION_OPEN, '{'); }
    bool TemplateStringExpressionClose() { return Test(lexical::Tokens::TEMPLATE_STRING_EXPRESSION_CLOSE, '}'); }
    bool ConditionOpen() { return Test(lexical::Tokens::CONDITION_OPEN, '('); }
    bool ConditionClose() { return Test(lexical::Tokens::CONDITION_CLOSE, ')'); }
    bool Call() { return Test(lexical::Tokens::CALL, "->"); }
    bool Wildcard() { return Test(lexical::Tokens::WILDCARD, '*'); }
    bool Comma() { return Test(lexical::Tokens::COMMA, ','); }
    bool Semicolon() { return Test(lexical::Tokens::SEMICOLON, ';'); }
    bool CommentOpen() { return Test(lexical::Tokens::COMMENT_OPEN, "//"); }
    bool CommentClose() { Save(lexical::Tokens::COMMENT_CLOSE, 0); return true; }
    bool MultiLineCommentOpen() { return Test(lexical::Tokens::MULTI_LINE_COMMENT_OPEN, "/*"); }
    bool MultiLineCommentClose() { return Test(lexical::Tokens::MULTI_LINE_COMMENT_CLOSE, "*/"); }
    bool DestructuredArrayOpen() { return Test(lexical::Tokens::DESTRUCTURED_ARRAY_OPEN, '['); }
    bool DestructuredArrayClose() { return Test(lexical::Tokens::DESTRUCTURED_ARRAY_CLOSE, ']'); }
    bool DestructuredTupleOpen() { return Test(lexical::Tokens::DESTRUCTURED_TUPLE_OPEN, '('); }
    bool DestructuredTupleClose() { return Test(lexical::Tokens::DESTRUCTURED_TUPLE_CLOSE, ')'); }
    bool DestructuredObjectOpen() { return Test(lexical::Tokens::DESTRUCTURED_OBJECT_OPEN, '{'); }
    bool DestructuredObjectClose() { return Test(lexical::Tokens::DESTRUCTURED_OBJECT_CLOSE, '}'); }
    bool DestructuredEnumOpen() { return Test(lexical::Tokens::DESTRUCTURED_ENUM_OPEN, '<'); }
    bool DestructuredEnumClose() { return Test(lexical::Tokens::DESTRUCTURED_ENUM_CLOSE, '>'); }

    bool OptionalSemicolon() { return Semicolon() || true; }

    // Unary operators
    bool Reference() { return Test(lexical::Tokens::REFERENCE, '&'); }
    bool MutableReference() { return Test(lexical::Tokens::MUTABLE_REFERENCE, '*'); }
    bool Symbol() { return Test(lexical::Tokens::SYMBOL, '$'); }
    bool Copy() { return Test(lexical::Tokens::COPY, '@'); }
    bool Counted() { return Test(lexical::Tokens::COUNTED, '#'); }
    bool Positive() { return Test(lexical::Tokens::POSITIVE, '+'); }
    bool Negative() { return Test(lexical::Tokens::NEGATIVE, '-'); }
    bool Increment() { return Test(lexical::Tokens::INCREMENT, "++"); }
    bool Decrement() { return Test(lexical::Tokens::DECREMENT, "--"); }
    bool Not() { return Test(lexical::Tokens::NOT, '!'); }
    bool Spread() { return Test(lexical::Tokens::SPREAD, "..."); }
    bool Move() { return Test(lexical::Tokens::MOVE, '='); }
    bool BitwiseNot() { return Test(lexical::Tokens::BITWISE_NOT, '~'); }

    bool UnaryPrefixOperatorHelper() {
      switch (Peek()) {
        case '+': return Increment() || Positive();
        case '-': return Decrement() || Negative();
        case '*': return MutableReference();
        case '&': return Reference();
        case '$': return Symbol();
        case '#': return Counted();
        case '@': return Copy();
        case '!': return Not();
        case '.': return Spread();
        case '=': return Move();
        case '~': return BitwiseNot();
        case 'a': return Await();
        default: return false;
      }
    }

    bool UnaryPostfixOperatorHelper() {
      switch (Peek()) {
        case '*': return MutableReference();
        case '&': return Reference();
        case '$': return Symbol();
        case '@': return Copy();
        case '#': return Counted();
        case '+': return Increment();
        case '-': return Decrement();
        case '!': return Not();
        case '.': return Spread();
        default: return false;
      }
    }

    inline bool UnaryPrefixOperator() { return cursor.IsUnaryPrefixStart() && UnaryPrefixOperatorHelper(); }
    inline bool UnaryPostfixOperator() { return cursor.IsUnaryPostfixStart() && UnaryPostfixOperatorHelper(); }

    // Binary operators
    bool Assign() { return Test(lexical::Tokens::ASSIGN, '='); }
    bool Equal() { return Test(lexical::Tokens::EQUAL, "=="); }
    bool AssertEqual() { return Test(lexical::Tokens::ASSERT_EQUAL, "==="); }
    bool NotEqual() { return Test(lexical::Tokens::NOT_EQUAL, "!="); }
    bool AssertNotEqual() { return Test(lexical::Tokens::ASSERT_NOT_EQUAL, "!=="); }
    bool Add() { return Test(lexical::Tokens::ADD, '+'); }
    bool AssignAdd() { return Test(lexical::Tokens::ASSIGN_ADD, "+="); }
    bool Subtract() { return Test(lexical::Tokens::SUBTRACT, '-'); }
    bool AssignSubtract() { return Test(lexical::Tokens::ASSIGN_SUBTRACT, "-="); }
    bool Multiply() { return Test(lexical::Tokens::MULTIPLY, '*'); }
    bool AssignMultiply() { return Test(lexical::Tokens::ASSIGN_MULTIPLY, "*="); }
    bool Exponent() { return Test(lexical::Tokens::EXPONENT, "**"); }
    bool AssignExponent() { return Test(lexical::Tokens::ASSIGN_EXPONENT, "**="); }
    bool Divide() { return Test(lexical::Tokens::DIVIDE, '/'); }
    bool AssignDivide() { return Test(lexical::Tokens::ASSIGN_DIVIDE, "/="); }
    bool Modulo() { return Test(lexical::Tokens::MODULO, '%'); }
    bool AssignModulo() { return Test(lexical::Tokens::ASSIGN_MODULO, "%="); }
    bool BitwiseXor() { return Test(lexical::Tokens::BITWISE_XOR, '^'); }
    bool BitwiseAssignXor() { return Test(lexical::Tokens::BITWISE_ASSIGN_XOR, "^="); }
    bool BitwiseOr() { return Test(lexical::Tokens::BITWISE_OR, '|'); }
    bool BitwiseAssignOr() { return Test(lexical::Tokens::BITWISE_ASSIGN_OR, "|="); }
    bool Wrap() { return Test(lexical::Tokens::WRAP, "!!"); }
    bool Unwrap() { return Test(lexical::Tokens::UNWRAP, "??"); }
    bool InlineFunctionArrow() { return Test(lexical::Tokens::INLINE_FUNCTION_ARROW, "=>"); }
    bool InclusiveRange() { return Test(lexical::Tokens::INCLUSIVE_RANGE, "..."); }
    bool ExclusiveRange() { return Test(lexical::Tokens::EXCLUSIVE_RANGE, ".."); }
    bool MemberAccess() { return Test(lexical::Tokens::MEMBER_ACCESS, '.'); }
    bool MemberAccessStatic() { return Test(lexical::Tokens::MEMBER_ACCESS_STATIC, ':'); }
    bool MemberAccessStaticOptional() { return Test(lexical::Tokens::MEMBER_ACCESS_STATIC_OPTIONAL, "?:"); }
    bool MemberAccessOptional() { return Test(lexical::Tokens::MEMBER_ACCESS_OPTIONAL, "?."); }
    bool Lesser() { return Test(lexical::Tokens::LESSER, '<'); }
    bool LesserOrEqual() { return Test(lexical::Tokens::LESSER_OR_EQUAL, "<="); }
    bool AssertLesserOrEqual() { return Test(lexical::Tokens::ASSERT_LESSER_OR_EQUAL, "<=="); }
    bool Greater() { return Test(lexical::Tokens::GREATER, '>'); }
    bool GreaterOrEqual() { return Test(lexical::Tokens::GREATER_OR_EQUAL, ">="); }
    bool AssertGreaterOrEqual() { return Test(lexical::Tokens::ASSERT_GREATER_OR_EQUAL, ">=="); }
    bool TripleLeftShift() { return Test(lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT, "<<<"); }
    bool BitwiseLeftShift() { return Test(lexical::Tokens::BITWISE_LEFT_SHIFT, "<<"); }
    bool BitwiseAssignLeftShift() { return Test(lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT, "<<="); }
    bool BitwiseTripleRightShift() { return Test(lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT, ">>>"); }
    bool BitwiseRightShift() { return Test(lexical::Tokens::BITWISE_RIGHT_SHIFT, ">>"); }
    bool BitwiseAssignRightShift() { return Test(lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT, ">>="); }
    bool And() { return Test(lexical::Tokens::AND, "&&"); }
    bool Or() { return Test(lexical::Tokens::OR, "||"); }
    bool TypeStart() { return Test(lexical::Tokens::TYPE_START, ':'); }

    bool BinaryOperatorHelper(bool in_enum = false) {
      switch (Peek()) {
        case '!': {
          switch (Peek(1)) {
            case '!': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_TRUTHY_AND, 3);
                default: return Skip(lexical::Tokens::WRAP, 2);
              }
            }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_NOT_EQUAL, 3);
                default: return Skip(lexical::Tokens::NOT_EQUAL, 2);
              }
            }
            default: return false;
          }
        }
        case '=': {
          switch (Peek(1)) {
            case '>': return Skip(lexical::Tokens::INLINE_FUNCTION_ARROW, 2);
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_EQUAL, 3);
                default: return Skip(lexical::Tokens::EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::ASSIGN, 1);
          }
        }
        case '<': {
          switch (Peek(1)) {
            case '<': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT, 3);
                case '<': return Skip(lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT, 3);
                default: return Skip(lexical::Tokens::BITWISE_LEFT_SHIFT, 2);
              }
            }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_LESSER_OR_EQUAL, 3);
                default: return Skip(lexical::Tokens::LESSER_OR_EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::LESSER, 1);
          }
        }
        case '>': {
          if (in_enum) return false;

          switch (Peek(1)) {
            case '>': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT, 3);
                case '>': return Skip(lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT, 3);
                default: return Skip(lexical::Tokens::BITWISE_RIGHT_SHIFT, 2);
              }
            }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_GREATER_OR_EQUAL, 3);
                default: return Skip(lexical::Tokens::GREATER_OR_EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::GREATER, 1);
          }
        }
        case '+': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_ADD, 2);
            default: return Skip(lexical::Tokens::ADD, 1);
          }
        }
        case '-': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_SUBTRACT, 2);
            default: return Skip(lexical::Tokens::SUBTRACT, 1);
          }
        }
        case '*': {
          switch (Peek(1)) {
            case '*': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_EXPONENT, 3);
                default: return Skip(lexical::Tokens::EXPONENT, 2);
              }
            }
            case '=': return Skip(lexical::Tokens::ASSIGN_MULTIPLY, 2);
            default: return Skip(lexical::Tokens::MULTIPLY, 1);
          }
        }
        case '/': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_DIVIDE, 2);
            default: return Skip(lexical::Tokens::DIVIDE, 1);
          }
        }
        case '%': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_MODULO, 2);
            default: return Skip(lexical::Tokens::MODULO, 1);
          }
        }
        case '^': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_XOR, 2);
            default: return Skip(lexical::Tokens::BITWISE_XOR, 1);
          }
        }
        case '?': {
          switch (Peek(1)) {
            case '.': return Skip(lexical::Tokens::OPTIONAL_MEMBER_ACCESS, 2);
            case ':': return Skip(lexical::Tokens::MEMBER_ACCESS_STATIC_OPTIONAL, 2);
            case '?': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_TRUTHY_OR, 3);
                default: return Skip(lexical::Tokens::UNWRAP, 2);
              }
            }
            case '=': return Skip(lexical::Tokens::ASSIGN_OPTIONAL, 2);
            default: return false;
          }
        }
        case '&': {
          switch (Peek(1)) {
            case '&': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_AND, 3);
                default: return Skip(lexical::Tokens::AND, 2);
              }
            }
            default: return Skip(lexical::Tokens::BITWISE_AND, 1);
          }
        }
        case '|': {
          switch (Peek(1)) {
            case '|': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_OR, 3);
                default: return Skip(lexical::Tokens::OR, 2);
              }
            }
            case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_OR, 2);
            default: return Skip(lexical::Tokens::BITWISE_OR, 1);
          }
        }
        case 'o': return Keyword(lexical::Tokens::OR, "or");
        case 'a': return Keyword(lexical::Tokens::AND, "and");
        case 'w': return With();
        case '.': {
          switch (Peek(1)) {
            case '.': {
              switch (Peek(2)) {
                case '.': return Skip(lexical::Tokens::INCLUSIVE_RANGE, 3);
                default: return Skip(lexical::Tokens::EXCLUSIVE_RANGE, 2);
              }
            }
            default: return Skip(lexical::Tokens::MEMBER_ACCESS, 1);
          }
        }
        case ':': {
          switch (Peek(1)) {
            case ':': return Skip(lexical::Tokens::MEMBER_ACCESS_STATIC, 2);
            default: return false;
          }
        }
      }

      return false;
    }

    inline bool BinaryOperator(bool in_enum = false) { return cursor.IsBinaryStart() && BinaryOperatorHelper(in_enum); }

    bool Escape() {
      if (Peek() != '\\') return false;

      Advance(); // Consume '\\'
      uint32_t width = 1;

      // Unicode escape or code point
      if (Peek() == 'u') {
        Advance();
        ++width;

        // Unicode code point escape \u{XXXXX...}
        if (Peek() == '{') {
          Advance();
          ++width;

          while (Valid() && Peek() != '}') {
            Advance(); // Consume characters within {}
            ++width;
          }

          if (Peek() == '}') {
            Advance();
            ++width;
          } else {
            return Error("Escape sequence expected a closing '}' brace");
          }

          Save(lexical::Tokens::ESCAPE_UNICODE_CODEPOINT, width);
        } else { // Unicode escape \uXXXX
          for (int i = 0; i < 4; ++i) {
            if (Done() || !cursor.IsHex()) {
              return Error("Escape sequence expected 4 hexadecimal digits");
            }

            Advance(); // Consume hexadecimal digit
            ++width;
          }

          Save(lexical::Tokens::ESCAPE_UNICODE_SHORT, width);
        }
      } else if (Peek() == 'x') { // Hexadecimal escape \xXX
        Advance();
        ++width;

        for (int i = 0; i < 2; ++i) {
          if (Done() || !cursor.IsHex()) {
            return Error("Escape sequence expected 2 hexadecimal digits");
          }

          Advance(); // Consume hexadecimal digit
          ++width;
        }

        Save(lexical::Tokens::ESCAPE_HEX_CODE, width);
      } else if (Valid()) { // Single character escape
        auto c = Peek();
        Advance(); // Consume the character following '\'
        ++width;

        if      (c == '\n') Save(lexical::Tokens::ESCAPE_NEWLINE, width);
        else if (c == '\t') Save(lexical::Tokens::ESCAPE_TAB, width);
        else if (c == '\b') Save(lexical::Tokens::ESCAPE_BACKSPACE, width);
        else if (c == '\r') Save(lexical::Tokens::ESCAPE_RETURN, width);
        else if (c == '\f') Save(lexical::Tokens::ESCAPE_FORM_FEED, width);
        else Save(lexical::Tokens::ESCAPE_LITERAL, width);
      } else {
        return Error("Unexpected end of content in escape sequence");
      }

      return true;
    }

    bool Text(const char end = '\0') {
      uint32_t width = 0;

      while (true) {
        if (Done()) {
          return Error("Unexpected end of content in text sequence");
        }

        auto c = Peek();
        
        if (c == end) {
          Store(lexical::Tokens::TEXT, width); // Save the content between the quotes as a TEXT token
          break;
        } else if (c == '\\') {
          Store(lexical::Tokens::TEXT, width); // Save the content between the quotes as a TEXT token

          if (Escape()) {
            return Text(end); // Match more text after the escape
          } else {
            return Error("Text sequence expected an escape sequence after the backslash '\\' character");
          }
        }

        Advance(); // Consume the character
        ++width;
      }

      return true;
    }

    bool ValueShortcut() {
      switch (Peek()) {
        case '-':
        case '+':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': return Number();
        case 't': return True();
        case 'f': return False();
        case 'n': return Null();
        case 'u': return Undefined();
        case '\'': return Char();
        case '"': return String();
        case '`': return TemplateString();
        case '(': return ParameterFunction() || Tuple();
        case '[': return CaptureFunction() || Array();
        case '<': return TemplateFunction() || Enum();
        case '{': return Object();
        default: return false;
      }
    }

    bool Value() {
      return (cursor.IsValueStart() && ValueShortcut()) || Identifier();
    }

    bool InitialValue() {
      if (cursor.IsValueStart() && ValueShortcut()) {
        maybe_declaration = false;
      }

      return Identifier();
    }

    bool Char() {
      return CharOpen()
          && Text('\'')
          && (CharClose() || Error("Unexpected end of input within character literal."))
          && (IdentifiedExpression() || OWS());
    }

    bool String() {
      return StringOpen()
          && Text('"')
          && (StringClose() || Error("Unexpected end of input within string literal."))
          && (IdentifiedExpression() || OWS());
    }

    bool TemplateString() {
      if (!TemplateStringOpen()) return false;

      uint32_t width = 0;

      while (true) {
        if (Done()) return Error("Unexpected end of file in template string literal");

        if (Peek() == '`') {
          if (width > 0) {
            Save(lexical::Tokens::TEXT, width); // Exclude the closing quote from saved range
          }

          return TemplateStringClose() && (IdentifiedExpression() || OWS());
        } else if (Peek() == '\\') {
          Save(lexical::Tokens::TEXT, width);
          
          if (!Escape()) return Error("Template string literal expected an escape sequence after '\\'");

          width = 0; // Update the width for the next TEXT chunk
        } else if (Peek() == '{') {
          if (width > 0) {
            Save(lexical::Tokens::TEXT, width);
          }
          
          if (!TemplateStringExpressionOpen()) {
            return Error("Template string literal didn't find the opening '{'");
          }

          if (!Expression()) return Error("Template string literal expected an expression after '{'");
          if (!TemplateStringExpressionClose()) return Error("Template string literal expected a closing '}' after expression");

          width = 0; // Update the width for the next TEXT chunk
        } else {
          Advance(); // Consume the characters inside the quotes
          ++width;
        }
      }

      throw std::runtime_error("Template string literal didn't hit the end of the file or a closing token");
    }

    bool Hex(bool negative) {
      // Skip the `0x` or `0X`
      Advance(2);
      uint32_t width = 2;
      uint64_t current = 0;

      while (Valid()) {
        if (Peek() == '_') {
          Advance();
          ++width;
          Emit(lexical::Tokens::UNDERSCORE);
          continue;
        } else if (!cursor.IsHex()) {
          break;
        }

        char digit = Peek();
        uint64_t numValue;

        // Convert hex digit to numerical value
        if (digit >= '0' && digit <= '9') {
          numValue = digit - '0';
        } else if (digit >= 'A' && digit <= 'F') {
          numValue = 10 + (digit - 'A');
        } else if (digit >= 'a' && digit <= 'f') {
          numValue = 10 + (digit - 'a');
        } else {
          // Handle invalid hex character
          return Error("Unexpected non-hex character while matching a hexadecimal literal");
        }

        // Shift current value left by 4 bits and add new digit
        current = (current << 4) | numValue;

        Advance(); // Move to the next character
        ++width;
      }

      if (negative) {
        Store(lexical::Tokens::HEX_INTEGER, width);
      } else {
        Store(lexical::Tokens::HEX_NUMBER, width);
      }

      return IdentifiedExpression() || OWS();
    }

    bool Octal(bool negative) {
      // Skip the `0o` or `0O` (assuming this is how octal is denoted)
      Advance(2);
      uint32_t width = 2;
      uint64_t current = 0;

      while (Valid()) {
        if (Peek() == '_') {
          Advance();
          ++width;
          continue;
        } else if (!cursor.IsOctal()) {
          break;
        }

        char digit = Peek();
        uint64_t numValue;

        // Convert octal digit to numerical value
        if (digit >= '0' && digit <= '7') {
          numValue = digit - '0';
        } else {
          // Handle invalid octal character
          return Error("Unexpected non-octal character while matching an octal literal");
        }

        // Shift current value left by 3 bits (for octal) and add new digit
        current = (current << 3) | numValue;

        Advance(); // Move to the next character
        ++width;
      }

      if (negative) {
        Store(lexical::Tokens::OCTAL_INTEGER, width);
      } else {
        Store(lexical::Tokens::OCTAL_NUMBER, width);
      }
      
      return IdentifiedExpression() || OWS();
    }

    bool Binary(bool negative) {
      // Skip the `0b` or `0B`
      Advance(2);
      uint32_t width = 2;
      std::bitset<64> bits;

      uint64_t index = 0;
      while (Valid() && index < 64) {
        if (Peek() == '_') {
          Advance();
          ++width;
          continue;
        } else if (!cursor.IsBinary()) {
          break;
        }

        if (Peek() == '1') {
          bits.set(index);
        }

        Advance();
        ++index;
        ++width;
      }

      if (index > 0) {
        // uint64_t current = bits.to_ullong();

        if (negative) {
          Store(lexical::Tokens::BINARY_INTEGER, width);
        } else {
          Store(lexical::Tokens::BINARY_NUMBER, width);
        }

        return IdentifiedExpression() || OWS();
      } else {
        Retreat(width);
        return false;
      }
    }

    bool NumberHelper() {
      auto start = Start();
      bool negative = false;

      if (Peek() == '-') {
        negative = true;
        Advance();
      } else if (Peek() == '+') {
        // A leading + is allowed for symmetry, but has no meaning as it's implicit
        Advance();
      }

      if (Peek() == '0') {
        // Peek the next character
        switch (Peek(1)) {
          case 'x':
          case 'X': return Hex(negative);
          case 'o':
          case 'O': return Octal(negative);
          case 'b':
          case 'B': return Binary(negative);
        }
      }

      bool decimal_point_encountered = false;
      bool e_encountered = false;
      bool exponent_sign_encountered = false;
      bool digit_encountered = false;

      while (Valid()) {
        char c = Peek();

        if (c == '_' && digit_encountered) {
          Advance();
          continue; // Allow _ as a separator
        } else if (c == '.') {
          if (decimal_point_encountered || e_encountered) break;
          decimal_point_encountered = true;

          // A trailing decimal is not allowed
          // TODO: How does this interact with scientific notation? IDK
          if (!cursor.IsDigit(Peek(1))) {
            break;
          }
        } else if (c == 'E' || c == 'e') {
          if (e_encountered) break;
          e_encountered = true;
          decimal_point_encountered = true; // Prevents further decimals
        } else if ((c == '+' || c == '-') && e_encountered && !exponent_sign_encountered) {
          exponent_sign_encountered = true;
        } else if (cursor.IsDigit(c)) {
          digit_encountered = true;
        } else {
          break;
        }

        Advance();
      }

      uint32_t size = static_cast<uint32_t>(std::distance(start.start, cursor.cbegin()));

      if (size == 0) {
        return false;
      } else if (!digit_encountered) {
        Retreat(size);
        return false;
      }

      // Decide how to store the value based on flags
      if (decimal_point_encountered) {
        Store(lexical::Tokens::DECIMAL, size);
      } else if (negative) {
        Store(lexical::Tokens::INTEGER, size);
      } else {
        Store(lexical::Tokens::NUMBER, size);
      }

      return IdentifiedExpression() || OWS();
    }

    inline bool Number() { return cursor.IsNumberStart() && NumberHelper(); }

    bool Parameters() { return ParameterOpen() && ParameterDeclarationList() && ParameterClose(); }
    bool Captures() { return CaptureOpen() && CaptureDeclarationList() && CaptureClose(); }
    bool FunctionBody() { return ScopeOpen() && FunctionStatementList() && ScopeClose(); }
    bool InlineFunctionBody() { return Expression(); }
    bool Array() { return ArrayOpen() && ExpressionList() && ArrayClose(); }
    bool Tuple() { return TupleOpen() && ExpressionList() && TupleClose(); }
    bool Object() { return ObjectOpen() && MemberDeclarationList() && ObjectClose(); }
    bool Enum() { return EnumOpen() && EnumExpressionList() && EnumClose(); }
    bool ArrowFunction() { return InlineFunctionArrow() && InlineFunctionBody(); }
    bool CaptureFunction() { return Try([&]{ return Captures() && (Parameters() || true) && FunctionBody() || ArrowFunction(); }); }
    bool ParameterFunction() { return Try([&]{ return Parameters() && (Captures() || true) && FunctionBody() || ArrowFunction(); }); }

    bool Function() {
      return Try([&]{
        if (Parameters()) return (Captures() || true) && FunctionBody() || ArrowFunction();
        else return Captures() && (Parameters() || true) && FunctionBody() || ArrowFunction();
      });
    }

    bool DestructuredArray() { return DestructuredArrayOpen() && DeclarationList() && DestructuredArrayClose(); }
    bool DestructuredTuple() { return DestructuredTupleOpen() && DeclarationList() && DestructuredTupleClose(); }
    bool DestructuredObject() { return DestructuredObjectOpen() && DeclarationList() && DestructuredObjectClose(); }
    bool DestructuredEnum() { return DestructuredEnumOpen() && DeclarationList() && DestructuredEnumClose(); }

    bool Destructured() {
      switch (Peek()) {
        case '(': return DestructuredTuple();
        case '{': return DestructuredObject();
        case '<': return DestructuredEnum();
        case '[': return DestructuredArray();
        default: return false;
      }
    }

    bool IdentifierList() { return ZeroOrMore([&]{ return Identifier(); }, [&]{ return Comma(); }); }
    bool CaptureDeclarationList() { return ZeroOrMore([&]{ return CaptureDeclaration(); }, [&]{ return Comma(); }); }
    bool DeclarationList() { return ZeroOrMore([&]{ return Declaration(); }, [&]{ return Comma(); }); }
    bool MemberDeclarationList() { return ZeroOrMore([&]{ return MemberDeclaration(); }, [&]{ return Comma(); }); }
    bool ExpressionList() { return ZeroOrMore([&]{ return Expression(); }, [&]{ return Comma(); }); }
    bool EnumExpressionList() { return ZeroOrMore([&]{ return EnumExpression(); }, [&]{ return Comma(); }); }
    bool ParameterDeclarationList() { return ZeroOrMore([&]{ return ParameterDeclaration(); }, [&]{ return Comma(); }); }

    bool CaptureDeclaration() {
      switch (Peek()) {
        case '=': {
          auto c = Peek(1);
          if (c == ',' || c == ']' || cursor.IsSpace(c)) {
            return Move();
          }
          break;
        }
        case '@': {
          auto c = Peek(1);
          if (c == ',' || c == ']' || cursor.IsSpace(c)) {
            return Copy();
          }
          break;
        }
        case '&': {
          auto c = Peek(1);
          if (c == ',' || c == ']' || cursor.IsSpace(c)) {
            return Reference();
          }
          break;
        }
      }

      return Declaration();
    }

    bool IdentifiedExpression() {
      return cursor.IsIdent() && Identifier() && ContinueExpression();
    }

    bool DeclarationExpression() {
      // Handle unary prefix operators at the beginning of an expression
      while (UnaryPrefixOperator()) {}

      auto start = cbegin();
      while (Value() && ContinueExpression()) {}

      return cbegin() > start;
    }

    bool Expression() {
      // maybe_declaration = true;

      // Handle unary prefix operators at the beginning of an expression
      while (UnaryPrefixOperator()) {
        // maybe_declaration = false;
      }

      // The next element should be a value
      if (!Value()) {
        return false;
      }

      return ContinueExpression();
    }

    bool EnumExpression() {
      // Handle unary prefix operators at the beginning of an expression
      while (UnaryPrefixOperator()) {
      }

      // The next element should be a value
      if (!Value()) {
        return false;
      }

      return ContinueEnumExpression();
    }

    bool UnaryStartedExpression() {
      int count = UnaryPrefixOperators();
      if (count == 0) return false;

      if (Value()) return ContinueExpression();

      if (count > 0) {
        return Error("Expected a value after the unary prefix operator");
      }

      return false;
    }

    bool ContinueExpression() {
      while (Valid()) {
        if (BinaryOperator()) {
          while (UnaryPrefixOperator()) {}

          if (!Value()) return false;
        } else if (!CallablePostfixLiteral()) {
          break; // No more operators or values
        }
      }

      return true;
    }

    bool ContinueEnumExpression() {
      while (Valid()) {
        if (BinaryOperator(true)) {
          while (UnaryPrefixOperator()) {}

          if (!Value()) return false;
        } else if (!CallablePostfixLiteral()) {
          break; // No more operators or values
        }
      }

      return true;
    }

    bool CallablePrefixLiteralHelper() {
      switch (Peek()) {
        case '\'': return Char();
        case '"': return String();
        case '`': return TemplateString();
        default: return Number();
      }
    }

    bool CallablePostfixLiteralHelper() {
      switch (Peek()) {
        case '\'': return Char();
        case '"': return String();
        case '`': return TemplateString();
        case '-': return ArrowFunction();
        case '(': return ParameterFunction() || Tuple();
        case '[': return CaptureFunction() || Array();
        case '{': return Object();
        case '<': return Enum();
        default: return false;
      }
    }

    bool ModifierHelper() {
      switch (Peek()) {
        case 'e': return Export();
        case 'l': return Let();
        default: return false;
      }
    }

    inline bool Modifier() { return cursor.IsModifierStart() && ModifierHelper(); }
    inline bool CallablePrefixLiteral() { return cursor.IsCallablePrefixStart() && CallablePrefixLiteralHelper(); }
    inline bool CallablePostfixLiteral() { return cursor.IsCallablePostfixStart() && CallablePostfixLiteralHelper(); }

    bool DeclarationValue() {
      // Any number of postfix unary operators are legal
      int operators = UnaryPostfixOperators();

      if (TypeStart()) {
        if (operators > 0) {
          return Error("Declaration cannot use an explicit type along with postfix unary operators");
        }

        if (!Expression()) {
          return Error("Declaration expected a type expression after ':'");
        }

        if (Assign()) {
          if (Expression()) {
            return true;
          }
          else {
            return Error("Declaration expected an expression after assignment '=' operator");
          }
        }
        else {
          return true;
        }
      }

      if (CallablePostfixLiteral()) {
        if (operators > 0) {
          return Error("Declaration cannot skip the '=' operator if it is using unary postfix operators");
        }

        return true;
      }
      else if (Assign()) {
        if (Expression()) {
          return true;
        }
        else {
          return Error("Declaration expected an expression after assignment '=' operator");
        }
      }

      return false;
    }

    bool Declaration() {
      // Consume any number of modifier keywords
      int modifiers = Modifiers();

      if (UnaryStartedExpression()) return true;

      if (Identifier()) {
        if (Peek() == ',') return true;
      }
      else {
        if (modifiers > 0) {
          return Error("Declaration expected an identifier after the modifiers");
        }

        return false;
      }

      return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
    }

    bool ParameterDeclaration() {
      int modifiers = Modifiers();

      if (Identifier()) {
        if (Peek() == ',') return true;
      }
      else {
        if (modifiers > 0) {
          return Error("Declaration expected an identifier after the modifiers");
        }

        return false;
      }

      return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
    }

    bool MemberDeclaration() {
      // utility::Debug("Parsing a member declaration list");

      // Consume any number of modifier keywords
      int modifiers = Modifiers();

      // Capture unary expressions like `@a`
      if (UnaryStartedExpression()) return true;

      if (ComputedProperty()) {
        Alias(); // Optional
      }
      else if (Identifier()) {
        if (Peek() == ',') return true;
      }
      else {
        if (modifiers > 0) {
          return Error("Declaration expected an identifier after the modifiers");
        }

        return false;
      }

      return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
    }

    bool Condition() {
      if (!ConditionOpen()) return false;
      if (!Expression()) return Error("Condition expected an expression after opening '('");
      
      return ConditionClose() || Error("Condition expected a closing ')'");
    }

    bool IdentifierHelper() {
      if (Done()) return false;

      auto start = cbegin();
      uint32_t offset = 0;

      // Check if the first character is a valid starting character for an identifier
      if (cursor.IsIdentStart()) {
        if (Peek() == '\\') {
          throw std::runtime_error("Haven't supported escape sequences in identifiers yet");
        } else {
          Advance(); // Consume the starting character
        }
      } else if (!cursor.IsASCII()) {
        auto code_point = cursor.CodePoint();

        if (Unicode::IsIdentifierStart(code_point)) {
          offset += static_cast<uint32_t>(std::distance(start, cbegin())) - 1;
        } else {
          Error("Found illegal unicode character " + std::to_string(code_point));
          return false; // Not a valid starting character
        }
      } else {
        return false;
      }

      // Consume remaining characters if they are alphanumeric or underscores
      while (Valid()) {
        if (cursor.IsIdent()) {
          Advance();
        } else if (!cursor.IsASCII()) {
          auto start = cbegin();
          auto code_point = cursor.CodePoint();

          if (Unicode::IsIdentifierContinue(code_point)) {
            offset += static_cast<uint32_t>(std::distance(start, cbegin())) - 1;
          } else {
            Error("Found illegal unicode character " + std::to_string(code_point));
            break;
          }
        } else {
          break;
        }
      }

      std::string_view text{start, cbegin()};

      auto type = lexical::cursor::Token::FindKeyword(text);
      if (!lexical::cursor::Token::IsIdentifiable(type)) {
        Retreat(start); // Reset
        return false;
      }

      Store(type, static_cast<uint32_t>(text.size()), offset);
      
      if (WS() && Peek() == 'a') {
        return (As() && (Identifier() || Error("Expected an identifier after the `as` keyword"))) || true; // Still matched an identifier even if it isn't an alias next
      } else {
        return true;
      }
    }

    bool Identifier() { return cursor.IsPossibleIdentifierStart() && IdentifierHelper(); }

    bool Alias() { return As() && Identifier(); }

    bool IdentifierExpression() {
      if (Identifier()) {
        if (As()) {
          return Identifier();
        }

        return true;
      }

      return false;
    }

    bool InlineScopeStatement() {
      return Statement()
          && Last() == lexical::Tokens::SEMICOLON; // Required
    }

    bool ScopeStatement() {
      return ScopeOpen()
          && StatementList()
          && (ScopeClose() || Error("Block statement expected a closing `}` brace"));
    }

    bool FlexibleScopeStatement() {
      return ScopeStatement() || InlineScopeStatement();
    }

    bool ForStatement() {
      if (!For()) return false;
      if (!ConditionOpen()) return Error("For statement expected an opening '(' after keyword 'for'");

      int modifiers = Modifiers();
      
      if (Destructured()) {

      }
      else if (Identifier()) {
        int operators = UnaryPostfixOperators();

        if (TypeStart()) {
          if (operators > 0) {
            return Error("For statement declaration cannot specify a type if it is using unary postfix operators");
          }

          if (!Expression()) {
            return Error("For statement declaration expected a type expression after ':'");
          }
        }
      }
      else {
        return Error("For statement declaration expected an identifier or destructuring syntax");
      }

      if (!In()) return Error("For statement expected the `in` keyword after the declaration");
      if (!Expression()) return Error("For statement expected an expression after the `in` keyword");
      if (!ConditionClose()) return Error("For statement expected a closing ')' after the condition");
      if (!FlexibleScopeStatement()) return Error("For statement expected a scope block or inline statement after the closing ')'");

      return OptionalSemicolon();
    }

    bool ElseStatement() {
      return Else() && (IfStatement() || FlexibleScopeStatement());
    }

    bool IfStatement() {
      return If()
          && Condition()
          && FlexibleScopeStatement()
          && (ElseStatement() || true)
          && OptionalSemicolon();
    }

    bool WhileStatement() {
      return While()
          && Condition()
          && FlexibleScopeStatement()
          && OptionalSemicolon();
    }

    bool DoStatement() {
      if (!Do()) return false;

      if (!ScopeStatement()) return Error("Do statement expected a scope after the 'do' keyword");

      if (While()) {
        return Error("Warble does not use a traditional `do ... while` loop, use `repeat ... while` instead.");
      }

      return OptionalSemicolon();
    }

    bool RepeatStatement() {
      if (!Repeat()) return false;

      if (!ScopeStatement()) return Error("Repeat statement expected a scope after the 'repeat' keyword");

      if (While()) {
        if (Condition()) {
          return Semicolon() || Error("The semicolon is mandatory after a condition in a 'repeat ... while' loop");
        }
        else {
          return OptionalSemicolon();
        }
      }
      else {
        return true; // The `while (condition)` portion is optional
      }
    }

    bool ImportStatementIdentifierList() {
      if (!ImportsOpen()) return false;

      if (!ZeroOrMore([this]{ return Declaration(); }, [this]{ return Comma(); }) || Done()) {
        return Error("Unexpected end of file while parsing an import statement identifier list");
      }

      return ImportsClose();
    }

    bool ImportWildcard() {
      // TODO: This will probably be deprecated
      if (!Wildcard()) return false;

      if (!As()) {
        return Error("Import statement expected an 'as' keyword after the wildcard '*'");
      }

      if (!Identifier()) {
        return Error("Import statement expected an identifier after the 'as' keyword in wildcard '*' import");
      }

      return true;
    }

    bool StringSpecifier() {
      if (!String()) return false;

      if (In()) {
        return String() || Error("String specifier expected a module name string literal after the 'in' keyword");
      }

      return true;
    }

    bool WildcardSpecifier() {
      if (!Wildcard()) return false;

      if (In()) {
        return String() || Error("Wildcard specifier expected a module name string literal after the 'in' keyword");
      }

      return true;
    }

    bool Specifier() { return WildcardSpecifier() || StringSpecifier(); }

    bool ImportStatement() {
      return Import()
          && (ImportWildcard() || ImportStatementIdentifierList() || Identifier())
          && (From() || Error("Import statement expected the from keyword"))
          && (StringSpecifier() || Error("Import statement expected a string literal as the specifier"))
          // && Run([this]{ mod.Import(std::move(Read(lexical::Tokens::TEXT))); return true; })
          && (Semicolon() || Error("Import statement expected an ending semicolon"));
    }

    bool RegisterStatement() {
      if (!Register()) return false;

      // There is no need to attempt to register the package early as the standard library will provide plenty to keep the workers busy.
      return (String() || Error("Register statement expected a string literal as the path"))
          && (As() || Error("Register statement expected the `as` keyword after the path"))
          && (String() || Error("Register statement expected a string literal as the specifier"))
          && (With() || Error("Register statement expected the `with` keyword after the specifier"))
          && Run([&]{
            return (ArrayOpen() || Error("Register statement expected an array literal of specifiers"))
                && Run([&]{
                  while (Peek() != ']') {
                    if (!Specifier()) break;
                    if (!Comma()) break;
                  }

                  return true;
                })
                && (ArrayClose() || Error("Register statement expected to find a closing `]` symbol"));
          })
          && (Semicolon() || Error("Register statement expected an ending semicolon"));
    }

    bool ExportStatement() {
      return Export()
          && Run([&]{
            if (Wildcard()) {
              return (From() || Error("Aggregate export statement expected the from keyword"))
                  && (String() || Error("Aggregate export expected a string literal as the specifier"))
                  ;
            } else {
              return Expression();
            }
          })
          && (Semicolon() || Error("Export statement expected a semicolon"));
    }

    bool _PropertyDeclaration() {
      while (Modifier()) {}

      if (!Destructured() && !Identifier()) {
        return Error("Property declaration statement expected destructuring enum syntax or an identifier after the modifiers");
      }

      Alias(); // Optional

      if (CallablePostfixLiteral()) {
        return Semicolon() || Error("Property declaration statement expected a semicolon");
      }

      if (TypeStart()) {
        if (!Expression()) {
          return Error("Property declaration statement expected a type expression after ':'");
        }
      }
      
      if (Peek() == '=') {
        if (Assign()) {
          if (Expression()) {
            return Semicolon() || Error("Property declaration statement expected a semicolon");
          }
          else {
            return Error("Property declaration statement expected an expression after initialization operator");
          }
        }
        else {
          return Error("Property declaration statement expected an initialization operator");
        }
      }
      else {
        return Error("Property declaration statement expected an initialization operator");
      }
    }

    bool ComputedProperty() {
      if (!ComputedPropertyOpen()) return false;

      int count = 0;
      while (Valid()) {
        if (!EnumExpression()) break;

        count += 1;
        if (!Comma()) break;
      }

      if (count == 0) {
        return Error("Declaration expected at least one expression after the opening '<' in the computed property name");
      }

      if (!ComputedPropertyClose()) {
        return Error("Declaration expected a closing '>' after the expression in the computed property name");
      }

      return true;
    }

    int Modifiers() {
      int count = 0;
      while (Modifier()) {
        count += 1;
      }

      return count;
    }

    int UnaryPostfixOperators() {
      int count = 0;
      while (UnaryPostfixOperator()) {
        count += 1;
      }

      return count;
    }

    int UnaryPrefixOperators() {
      int count = 0;
      while (UnaryPrefixOperator()) {
        count += 1;
      }

      return count;
    }

    bool DeclarationStatement() {
      if (Modifiers() == 0) return false;

      if (Destructured()) {
        if (Assign()) {
          if (Expression()) {
            return Semicolon() || Error("Declaration statement expected a semicolon");
          }
          else {
            return Error("Declaration statement expected an expression after assignment '=' operator");
          }
        }
        else {
          return Error("Declaration statement expected a right-hand side initialization value after the destructuring syntax");
        }
      }
      else if (Identifier()) {
        // Any number of postfix unary operators are legal
        int operators = UnaryPostfixOperators();

        if (TypeStart()) {
          if (operators > 0) {
            return Error("Declaration statement cannot specify a type if it is using unary postfix operators");
          }

          if (!Expression()) {
            return Error("Declaration statement expected a type expression after ':'");
          }

          if (Assign()) {
            if (Expression()) {
              return Semicolon() || Error("Declaration statement expected a semicolon");
            }
            else {
              return Error("Declaration statement expected an expression after assignment '=' operator");
            }
          }
          else {
            return Error("Declaration statement expected a right-hand side initialization value after the type");
          }
        }
        else if (CallablePostfixLiteral()) {
          if (operators > 0) {
            return Error("Declaration statement cannot skip the '=' operator if it is using unary postfix operators");
          }

          return Semicolon() || Error("Declaration statement expected a semicolon");
        }
        else if (Assign()) {
          if (Expression()) {
            return Semicolon() || Error("Declaration statement expected a semicolon");
          }
          else {
            return Error("Declaration statement expected an expression after assignment '=' operator");
          }
        }
        else {
          return Error("Declaration statement expected a right-hand side initialization value");
        }
      }
      else {
        return Error("Declaration statement expected either destructuring syntax or an identifier after the modifiers");
      }
    }

    bool ExpressionStatement() {
      return Expression()
          && (Semicolon() || Error("Expression statement expected a semicolon"));
    }

    bool ContinueStatement() {
      return OneOrMore([this]{ return Continue(); })
          && (Semicolon() || Error("Continue statement expected a semicolon"));
    }

    bool BreakStatement() { return OneOrMore([this]{ return Break(); }) && (Semicolon() || Error("Break statement expected a semicolon")); }

    bool YieldStatement() {
      if (!Yield()) return false;
      if (Semicolon()) return true; // Valid to just end right away

      return (Expression() || Error("Yield statement expected an expression or a semicolon"))
          && (Semicolon() || Error("Yield statement expected a semicolon"));
    }

    bool ReturnStatement() {
      if (!Return()) return false;
      if (Semicolon()) return true; // Valid to just end right away

      return (Expression() || Error("Return statement expected an expression or a semicolon"))
          && (Semicolon() || Error("Return statement expected a semicolon"));
    }

    bool CaseStatement() {
      if (!Case()) return false;
      if (Semicolon()) return true; // Valid to just end right away

      return (Expression() || Error("Case statement expected an expression or a semicolon"))
          && (Semicolon() || Error("Case statement expected a semicolon"));
    }

    bool MatchStatement() {
      return Match()
          && (Condition() || Error("Match statement expected a condition"))
          && (ScopeStatement() || Error("Match statement expected a scope block"))
          && OptionalSemicolon();
    }

    bool IsStatement() {
      return Is()
          && (Condition() || Error("Is statement expected a condition"))
          && (IsStatement() || (FlexibleScopeStatement() && OptionalSemicolon()));
    }

    bool DefaultStatement() {
      return Default()
          && (FlexibleScopeStatement() || Error("Default statement expected a block or inline statement"));
          // && (Semicolon() || Error("Default statement expected a semicolon"));
    }

    bool StatementShortcut() {
      switch (Peek()) {
        case 'b': return BreakStatement();
        case 'c': return ContinueStatement() || CaseStatement();
        case 'd': return DoStatement() || DefaultStatement();
        case 'e': return ExportStatement();
        case 'f': return ForStatement();
        case 'i': return IfStatement() || ImportStatement() || IsStatement();
        case 'm': return MatchStatement();
        case 'r': return ReturnStatement() || RepeatStatement() || RegisterStatement();
        case 'w': return WhileStatement();
        case 'y': return YieldStatement();
        default: return false;
      }
    }

    bool Statement() { return (cursor.IsStatementStart() && StatementShortcut()) || DeclarationStatement() || ExpressionStatement(); }
    
    bool StatementList() {
      // std::cout << "Starting StatementList" << std::endl;

      while (Valid()) {
        // // Check for either of these
        // if (!Compiletime()) {
        //   Runtime();
        // }

        if (!Statement()) {
          // if (recovery && !Recover()) {
          //   break;
          // }

          break;
        }
      }

      return true;
    }

    bool FunctionStatementList() { return StatementList(); }
  };
};