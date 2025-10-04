export module compiler.input.Lexer;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;
import <expected>;

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Error;
import compiler.ir.symbol.Type;
import compiler.text.cursor.String;
import compiler.text.Convert;

namespace compiler::input {
  consteval std::bitset<256> MakeBitset(auto fn) {
    std::bitset<256> bs;
    fn(bs);
    return bs;
  }

  // Bitsets for efficiently checking ASCII characters
  inline constexpr auto DIGIT = MakeBitset([](auto& bs){
    for (char c = '0'; c <= '9'; ++c) bs.set(c);
  });

  inline constexpr auto HEX = MakeBitset([](auto& bs){
    for (char c = '0'; c <= '9'; ++c) bs.set(c);
    for (char c = 'A'; c <= 'F'; ++c) bs.set(c);
    for (char c = 'a'; c <= 'f'; ++c) bs.set(c);
  });

  inline constexpr auto OCTAL = MakeBitset([](auto& bs){
    for (char c = '0'; c <= '7'; ++c) bs.set(c);
  });

  inline constexpr auto WHITESPACE = MakeBitset([](auto& bs){
    bs.set(' ');
    bs.set('\f'); // Form feed
    bs.set('\n'); // Line feed
    bs.set('\r'); // Carriage return
    bs.set('\t'); // Horizontal tab
    bs.set('\v'); // Vertical tab
  });

  inline constexpr auto PUNCT = MakeBitset([](auto& bs){
    for (char c : "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~") bs.set(c);
  });

  inline constexpr auto SLASH = MakeBitset([](auto& bs){
    bs.set('/');
    bs.set('\\');
  });

  inline constexpr auto UPPER = MakeBitset([](auto& bs){
    for (char c = 'A'; c <= 'Z'; ++c) bs.set(c);
  });

  inline constexpr auto LOWER = MakeBitset([](auto& bs){
    for (char c = 'a'; c <= 'z'; ++c) bs.set(c);
  });

  inline constexpr auto ALPHA = UPPER | LOWER;
  inline constexpr auto ALNUM = ALPHA | DIGIT;

  inline constexpr auto IDENTIFIER_START = MakeBitset([](auto& bs){
    bs.set('_');
    bs.set('\\'); // Escape sequences are legal in identifiers
  }) | ALPHA;

  inline constexpr auto IDENTIFIER = IDENTIFIER_START | DIGIT;

  inline constexpr auto VALUE_START = MakeBitset([](auto& bs){
    for (char c : "tfnu'\"`+-([{<.") bs.set(c);
  }) | DIGIT;

  inline constexpr auto STATEMENT_START = MakeBitset([](auto& bs){
    for (char c : "bcdefimrw") bs.set(c);
  });

  inline constexpr auto UNARY_PREFIX_START = MakeBitset([](auto& bs){
    for (char c : "+-*&$#@!?^=.~aneu") bs.set(c);
  });

  inline constexpr auto UNARY_POSTFIX_START = MakeBitset([](auto& bs){
    for (char c : "*&$?@#+-!.") bs.set(c);
  });

  inline constexpr auto CALLABLE_PREFIX_START = MakeBitset([](auto& bs){
    for (char c : "'\"`") bs.set(c);
  });

  inline constexpr auto CALLABLE_POSTFIX_START = MakeBitset([](auto& bs){
    for (char c : "'\"`-([{<") bs.set(c);
  });

  inline constexpr auto BINARY_START = MakeBitset([](auto& bs){
    for (char c : "!=<>+-*/%^?&|oawu.:") bs.set(c);
  });

  inline constexpr auto MODIFIER_START = MakeBitset([](auto& bs){
    for (char c : "aceimpslr") bs.set(c);
  });

  inline constexpr auto NUMBER_START = MakeBitset([](auto& bs){
    for (char c : "+-") bs.set(c);
  }) | DIGIT;

  inline constexpr auto NOT_ASCII = MakeBitset([](auto& bs){
    // Set bits for lead bytes of UTF-8 encoded non-ASCII characters (128-255) to 1
    for (int i = 128; i < 256; ++i) {
      bs.set(i);
    }
  });

  inline constexpr auto POSSIBLE_SPACE_START = MakeBitset([](auto& bs){
    bs.set('/'); // Comments are treated as whitespace for this bitset
  }) | WHITESPACE | NOT_ASCII;

  inline constexpr auto URL_PROTOCOL = MakeBitset([](auto& bs){
    // These cannot be the starting character
    for (char c : "+.-") bs.set(c);
  }) | ALNUM;

  inline constexpr auto URL_AUTHORITY = MakeBitset([](auto& bs){
    // These cannot be the starting character
    for (char c : ".-_~&") bs.set(c);
  }) | ALNUM;

  inline constexpr auto URL_HOST = MakeBitset([](auto& bs){
    // These cannot be the starting character
    for (char c : ".-") bs.set(c);
  }) | ALNUM;

  inline constexpr auto URL_PATHNAME = MakeBitset([](auto& bs){
    // Unreserved characters
    for (char c : ".-_~") bs.set(c);

    // Reserved characters
    for (char c : ":@&=+$,") bs.set(c);

    // Sub-delimiters
    for (char c : "!\'()*;%") bs.set(c);
  }) | ALNUM;

  inline constexpr auto URL_SEARCH = MakeBitset([](auto& bs){
    // Unreserved characters
    for (char c : ".-_~") bs.set(c);

    // Reserved characters
    for (char c : "!$\'()*,;:@%") bs.set(c);
  }) | ALNUM;

  inline constexpr auto URL_HASH = MakeBitset([](auto& bs){
    // Unreserved characters
    for (char c : ".-_~") bs.set(c);

    // Reserved characters
    for (char c : "!$&\'()*+,;=@/?%") bs.set(c);
  }) | ALNUM;

  inline constexpr auto FILE_PATH = MakeBitset([](auto& bs){
    for (char c : "-_!#$%&'()[]{}+@~^. ") bs.set(c);
  }) | ALNUM;

  inline constexpr auto POSSIBLE_IDENTIFIER_START = IDENTIFIER_START | NOT_ASCII;

  export class Lexer {
  private:
  protected:
    program::Module& mod;
    text::cursor::String cursor;
    std::string::const_iterator furthest; // Highest source iterator reached to suppress duplicate side-effects after rollback.

    struct Position {
      std::string::const_iterator cursor;
      size_t token;
    };

    // Capture the current cursor and the current token count so speculative
    // lexing can roll back only tokens produced after this point.
    // NOTE: We intentionally record size(), not size()-1. Using size()-1 would
    // erase one pre-existing token on rollback and underflow when empty.
    Position Lexer::Start() {
      return Position{cursor.Current(), mod.Tokens().size()};
    }

    void Lexer::Rollback(const Position& position) {
      furthest = std::max(furthest, cursor.cbegin());
      cursor.Retreat(position.cursor);
      mod.Tokens().erase(mod.Tokens().begin() + position.token, mod.Tokens().end());
    }

    constexpr decltype(auto) Peek() const { return cursor.Peek(); }
    constexpr decltype(auto) Peek(size_t n) const { return cursor.Peek(n); }

    bool Lexer::Emit(ir::Token token) { mod.AddToken(token); return true; }

    bool Lexer::EmitRepeated(ir::Token token, uint32_t count) {
      if (count == 0) return false;
      while (count > 255) {
        mod.AddToken(token, static_cast<uint8_t>(255));
        count -= 255;
      }
      mod.AddToken(token, static_cast<uint8_t>(count));
      return true;
    }

    bool Lexer::EmitRepeated(ir::Token token, std::string::const_iterator begin) {
      return EmitRepeated(token, static_cast<uint32_t>(std::distance(begin, cursor.cbegin())));
    }

    bool Lexer::EmitStringSymbol(ir::Token token, size_t start) {
      Emit(token); // Marks that a symbol is found here

      // Do not create the symbol when backtracked, it was already created
      if (furthest > cursor.cbegin()) return false;

      ir::Symbol sym = mod.AddSymbol(ir::symbol::Type::String);
      sym.Value(static_cast<uint32_t>(start), static_cast<uint32_t>(mod.Characters().size() - start));
      return true;
    }

    bool Lexer::Error(ir::Error error) {
      Emit(ir::Token::Error); // Marks that an error symbol is found here

      // TODO: Do we create error symbols when backtracked? I'm not sure.
      // For now, we do create them even when backtracked.

      ir::Symbol sym = mod.AddSymbol(ir::symbol::Type::Error);
      sym.Value(static_cast<uint64_t>(error)); // Store the error code
      return true;
    }

    bool Lexer::EmitAndAdvance(ir::Token token, size_t count = 1) {
      cursor.Advance(count);
      return Emit(token);
    }

    bool Lexer::Keyword(const std::string_view text) {
      if (!cursor.Check(text)) return false; // Compare the text to the content
      if (IsAlNum(text.size())) return false; // The next character cannot be an alphanumeric character with a keyword

      cursor.Advance(text.size());

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

    // Repeat helpers for speculative lexing.
    // ZeroOrMore(fn): repeatedly applies fn until it fails; rollback last failed probe.
    bool ZeroOrMore(const auto& fn) {
      for (;;) {
        auto s = Start();
        if (!fn()) { Rollback(s); break; }
      }
      return true; // Always succeeds (matches empty sequence)
    }

    // ZeroOrMore(item, sep): matches item (sep item)* without a trailing sep.
    bool ZeroOrMore(const auto& item, const auto& sep) {
      for (;;) {
        auto s = Start();
        if (!item()) { Rollback(s); break; }
        if (!sep()) break; // No rollback: separator absence is normal termination.
      }
      return true;
    }

    // OneOrMore(fn): require at least one match, then reuse ZeroOrMore.
    bool OneOrMore(const auto& fn) {
      auto first = Start();
      if (!fn()) { Rollback(first); return false; }
      return ZeroOrMore(fn);
    }

    // OneOrMore(item, sep): require one item then repeat (sep item)*.
    bool OneOrMore(const auto& item, const auto& sep) {
      auto first = Start();
      if (!item()) { Rollback(first); return false; }

      for (;;) {
        auto s = Start();
        // Probe separator; if absent we're done.
        if (!sep()) { Rollback(s); break; }
        // After a separator there must be another item; if not, rollback separator.
        if (!item()) { Rollback(s); break; }
      }
      return true;
    }

    bool Line(ir::Token t) {
      // Make sure we don't double record lines when backtracked
      if (cursor.cbegin() > furthest) {
        mod.AddLine(cursor.cbegin());
        furthest = cursor.cbegin();
      }

      return Emit(t);
    }
  public:
    Lexer(program::Module& mod, const std::string_view source)
      : mod{mod}, cursor{source}, furthest{cursor.cbegin()} {
      mod.AddLine(cursor.cbegin()); // Start with the first line
    }

    virtual ~Lexer() = default;

    bool WhiteSpace() {
      switch (cursor.Peek()) {
        case '\v': return EmitAndAdvance(ir::Token::VerticalTab, 1);
        case '\f': return EmitAndAdvance(ir::Token::FormFeed, 1);
        case ' ' : return EmitRepeated(ir::Token::Spaces, cursor.Consume(' '));
        case '\t': return EmitRepeated(ir::Token::Tabs, cursor.Consume('\t'));
        case '\n': return EmitRepeated(ir::Token::NewlinesLF, cursor.Consume('\n'));
        case '\r': {
          auto count = Count("\r\n");
          if (count > 0) {
            cursor.Advance(count * 2);
            return EmitRepeated(ir::Token::NewlinesCRLF, count);
          }

          return EmitRepeated(ir::Token::NewlinesCR, cursor.Consume('\r'));
        }
        case '/': {
          switch (cursor.Peek(1)) {
            case '/': {
              cursor.Advance(2); // Consume the opening `'//'`
              Emit(ir::Token::CommentOpen);

              auto begin = cursor.cbegin();
              auto start = mod.Characters().size();
              
              while (true) {
                if (cursor.Done() || IsBreak()) {
                  EmitStringSymbol(ir::Token::Comment, start); // Create the symbol
                  EmitRepeated(ir::Token::Characters, begin); // Record the UTF-8 width of the comment

                  return Emit(ir::Token::CommentClose); // Special zero width end marker
                }

                // Don't capture characters when backtracked
                if (furthest > cursor.cbegin()) {
                  cursor.Advance(1);
                } else {
                  mod.AddCharacter(cursor.CodePoint());
                }
              }
            }
            case '*': {
              cursor.Advance(2); // Consume the opening `'\*'`
              Emit(ir::Token::MultiLineCommentOpen);

              auto begin = cursor.cbegin();
              auto start = mod.Characters().size();

              while (!cursor.Done()) {
                if (cursor.Peek() == '*' && cursor.Peek(1) == '/') 
                  cursor.Advance(2); // Consume the closing `'*/'`

                  EmitStringSymbol(ir::Token::MultiLineComment, start);
                  EmitRepeated(ir::Token::Characters, begin); // Record the UTF-8 width of the comment

                  return Emit(ir::Token::MultiLineCommentClose);
                }

                // Don't capture characters when backtracked
                if (furthest > cursor.cbegin()) {
                  cursor.Advance(1);
                } else {
                  auto cp = cursor.CodePoint();
                  mod.AddCharacter(cp);

                  switch (cp) {
                    case U'\u2028':
                    case U'\u2029':
                    case U'\n': {
                      mod.AddLine(cursor.cbegin());
                      break;
                    }
                    case U'\r': {
                      if (cursor.Peek() == '\n') {
                        cursor.Advance(1); // Consume the LF as part of the CRLF
                        mod.AddCharacter(U'\n'); // No need to call `CodePoint`, we already know what it is
                        mod.AddLine(cursor.cbegin());
                      } else {
                        mod.AddLine(cursor.cbegin()); // A lone CR is still a newline
                      }
                      break;
                    }
                  }
                }
              }

              return Error(ir::Error::MultiLineCommentExpectedClosingAsteriskSlash);
            }
            default: return false; // Wasn't a comment
          }
        }
        default: {
          if (!IsASCII()) {
            auto iter = cursor.cbegin();
            auto code_point = cursor.CodePoint(); // This consumes the full code point automatically

            // Check for a line or paragraph separator
            if (code_point == 0x2028) {
              mod.AddLine(cursor.cbegin());
              return Emit(ir::Token::NewlineLS);
            } else if (code_point == 0x2029) {
              mod.AddLine(cursor.cbegin());
              return Emit(ir::Token::NewlinePS);
            } else if (Unicode::IsWhiteSpace(code_point)) {
              return Emit(ir::Token::Spaces, 1);
            } else {
              cursor.Retreat(iter); // Undo the consumption of the code point
              return false; // Wasn't whitespace
            }
          }

          return false; // Wasn't unicode
        }
      }
    }

    bool WS() {
      bool matched = false;
      
      while (IsPossibleSpaceStart() && WhiteSpace()) {
        matched = true;
      }

      return matched;
    }

    // Optional white space
    bool OWS() { WS(); return true; }

    bool Null() { return Keyword("null") && Emit(ir::Token::Null); }
    bool Undefined() { return Keyword("undefined") && Emit(ir::Token::Undefined); }
    bool True() { return Keyword("true") && Emit(ir::Token::True); }
    bool False() { return Keyword("false") && Emit(ir::Token::False); }
    bool This() { return Keyword("this") && Emit(ir::Token::This); }
    bool Import() { return Keyword("import") && Emit(ir::Token::Import); }
    bool Export() { return Keyword("export") && Emit(ir::Token::Export); }
    bool Register() { return Keyword("register") && Emit(ir::Token::Register); }
    bool From() { return Keyword("from") && Emit(ir::Token::From); }
    bool With() { return Keyword("with") && Emit(ir::Token::With); }
    bool Has() { return Keyword("has") && Emit(ir::Token::Has); }
    bool If() { return Keyword("if") && Emit(ir::Token::If); }
    bool Else() { return Keyword("else") && Emit(ir::Token::Else); }
    bool Do() { return Keyword("do") && Emit(ir::Token::Do); }
    bool While() { return Keyword("while") && Emit(ir::Token::While); }
    bool Repeat() { return Keyword("repeat") && Emit(ir::Token::Repeat); }
    bool Is() { return Keyword("is") && Emit(ir::Token::Is); }
    bool In() { return Keyword("in") && Emit(ir::Token::In); }
    bool For() { return Keyword("for") && Emit(ir::Token::For); }
    bool As() { return Keyword("as") && Emit(ir::Token::As); }
    bool Default() { return Keyword("default") && Emit(ir::Token::Default); }
    bool Auto() { return Keyword("auto") && Emit(ir::Token::Auto); }
    bool Void() { return Keyword("void") && Emit(ir::Token::Void); }
    bool Match() { return Keyword("match") && Emit(ir::Token::Match); }
    bool Await() { return Keyword("await") && Emit(ir::Token::Await); }
    bool Compiler() { return Keyword("compiler") && Emit(ir::Token::Compiler); }
    bool Break() { return Keyword("break") && Emit(ir::Token::Break); }
    bool Continue() { return Keyword("continue") && Emit(ir::Token::Continue); }
    bool Return() { return Keyword("return") && Emit(ir::Token::Return); }
    bool Case() { return Keyword("case") && Emit(ir::Token::Case); }
    bool Yield() { return Keyword("yield") && Emit(ir::Token::Yield); }
    bool Let() { return Keyword("let") && Emit(ir::Token::Let); }

    bool CaptureOpen() { return cursor.Match('[') && Emit(ir::Token::CaptureOpen); }
    bool CaptureClose() { return cursor.Match(']') && Emit(ir::Token::CaptureClose); }
    bool ParameterOpen() { return cursor.Match('(') && Emit(ir::Token::ParameterOpen); }
    bool ParameterClose() { return cursor.Match(')') && Emit(ir::Token::ParameterClose); }
    bool ScopeOpen() { return cursor.Match('{') && Emit(ir::Token::ScopeOpen); }
    bool ScopeClose() { return cursor.Match('}') && Emit(ir::Token::ScopeClose); }
    bool TupleOpen() { return cursor.Match('(') && Emit(ir::Token::TupleOpen); }
    bool TupleClose() { return cursor.Match(')') && Emit(ir::Token::TupleClose); }
    bool ArrayOpen() { return cursor.Match('[') && Emit(ir::Token::ArrayOpen); }
    bool ArrayClose() { return cursor.Match(']') && Emit(ir::Token::ArrayClose); }
    bool ObjectOpen() { return cursor.Match('{') && Emit(ir::Token::ObjectOpen); }
    bool ObjectClose() { return cursor.Match('}') && Emit(ir::Token::ObjectClose); }
    bool EnumOpen() { return cursor.Match('<') && Emit(ir::Token::EnumOpen); }
    bool EnumClose() { return cursor.Match('>') && Emit(ir::Token::EnumClose); }

    bool CharOpen() { return cursor.Match('\'') && Emit(ir::Token::CharOpen); }
    bool CharClose() { return cursor.Match('\'') && Emit(ir::Token::CharClose); }
    bool StringOpen() { return cursor.Match('"') && Emit(ir::Token::StringOpen); }
    bool StringClose() { return cursor.Match('"') && Emit(ir::Token::StringClose); }
    bool TemplateStringOpen() { return cursor.Match('`') && Emit(ir::Token::TemplateStringOpen); }
    bool TemplateStringClose() { return cursor.Match('`') && Emit(ir::Token::TemplateStringClose); }
    bool TemplateStringExpressionOpen() { return cursor.Match('{') && Emit(ir::Token::TemplateStringExpressionOpen); }
    bool TemplateStringExpressionClose() { return cursor.Match('}') && Emit(ir::Token::TemplateStringExpressionClose); }
    bool ConditionOpen() { return cursor.Match('(') && Emit(ir::Token::ConditionOpen); }
    bool ConditionClose() { return cursor.Match(')') && Emit(ir::Token::ConditionClose); }
    bool Call() { return cursor.Match("->") && Emit(ir::Token::Call); }
    bool Wildcard() { return cursor.Match('*') && Emit(ir::Token::Wildcard); }
    bool Comma() { return cursor.Match(',') && Emit(ir::Token::Comma); }
    bool Semicolon() { return cursor.Match(';') && Emit(ir::Token::Semicolon); }
    bool CommentOpen() { return cursor.Match("//") && Emit(ir::Token::CommentOpen); }
    bool CommentClose() { return Emit(ir::Token::CommentClose); }
    bool MultiLineCommentOpen() { return cursor.Match("/*") && Emit(ir::Token::MultiLineCommentOpen); }
    bool MultiLineCommentClose() { return cursor.Match("*/") && Emit(ir::Token::MultiLineCommentClose); }
    bool DestructuredArrayOpen() { return cursor.Match('[') && Emit(ir::Token::DestructuredArrayOpen); }
    bool DestructuredArrayClose() { return cursor.Match(']') && Emit(ir::Token::DestructuredArrayClose); }
    bool DestructuredTupleOpen() { return cursor.Match('(') && Emit(ir::Token::DestructuredTupleOpen); }
    bool DestructuredTupleClose() { return cursor.Match(')') && Emit(ir::Token::DestructuredTupleClose); }
    bool DestructuredObjectOpen() { return cursor.Match('{') && Emit(ir::Token::DestructuredObjectOpen); }
    bool DestructuredObjectClose() { return cursor.Match('}') && Emit(ir::Token::DestructuredObjectClose); }
    bool DestructuredEnumOpen() { return cursor.Match('<') && Emit(ir::Token::DestructuredEnumOpen); }
    bool DestructuredEnumClose() { return cursor.Match('>') && Emit(ir::Token::DestructuredEnumClose); }

    bool OptionalSemicolon() { Semicolon(); return true; }

    // Unary operators
    bool Reference() { return cursor.Match('&') && Emit(ir::Token::Reference); }
    bool MutableReference() { return cursor.Match('*') && Emit(ir::Token::MutableReference); }
    bool Symbol() { return cursor.Match('$') && Emit(ir::Token::Symbol); }
    bool Copy() { return cursor.Match('@') && Emit(ir::Token::Copy); }
    bool Counted() { return cursor.Match('#') && Emit(ir::Token::Counted); }
    bool Positive() { return cursor.Match('+') && Emit(ir::Token::Positive); }
    bool Negative() { return cursor.Match('-') && Emit(ir::Token::Negative); }
    bool Increment() { return cursor.Match("++") && Emit(ir::Token::Increment); }
    bool Decrement() { return cursor.Match("--") && Emit(ir::Token::Decrement); }
    bool Not() { return cursor.Match('!') && Emit(ir::Token::Not); }
    bool Spread() { return cursor.Match("...") && Emit(ir::Token::Spread); }
    bool Move() { return cursor.Match('=') && Emit(ir::Token::Move); }
    bool BitwiseNot() { return cursor.Match('~') && Emit(ir::Token::BitwiseNot); }

    bool UnaryPrefixOperatorHelper() {
      switch (cursor.Peek()) {
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
        case 'e': return Expect();
        default: return false;
      }
    }

    inline bool UnaryPrefixOperator() { return cursor.IsUnaryPrefixStart() && UnaryPrefixOperatorHelper(); }
    inline bool UnaryPostfixOperator() { return cursor.IsUnaryPostfixStart() && UnaryPostfixOperatorHelper(); }

    // Generic escape sequence parser.
    //  - Consumes an escape sequence beginning with '\\'
    //  - Emits escape-related tokens (e.g., EscapeUnicodeShort, EscapeHexCode, etc.)
    //  - Emits an ir::Token::Characters with the UTF-8 width of the resulting single code point
    //  - Returns the decoded UTF-32 code point as std::expected<char32_t, ir::Error>
    // Responsibilities of callers:
    //  - If building a string-like buffer, append the returned code point to mod.Characters() only when not backtracked
    //  - Do not emit an additional Characters token for this code point (Escape already recorded its width)
    // Error policy:
    //  - No errors are reported here beyond returning the specific ir::Error; callers may additionally report context-specific errors
    std::expected<char32_t, ir::Error> Escape() {
      if (!cursor.Match('\\')) return std::unexpected(ir::Error::EscapeSequenceUnexpectedEndOfInput);

      // Unicode escape or code point
      if (cursor.Match('u')) {
        // Unicode code point escape \u{XXXXX...}
        if (cursor.Match('{')) {
          Emit(ir::Token::EscapeUnicodeCodePointStart); // Represents the `\u{` part
          auto begin_hex = cursor.cbegin();

          while (true) {
            if (cursor.Done()) {
              return std::unexpected(ir::Error::EscapeSequenceUnexpectedEndOfInput);
            }
            if (!IsHex()) break;
            cursor.Advance();
          }

          auto end_hex = cursor.cbegin();
          uint32_t width = static_cast<uint32_t>(std::distance(begin_hex, end_hex));
          if (width == 0) {
            return std::unexpected(ir::Error::EscapeSequenceExpectedAtLeastOneHexDigit);
          } else if (width > 6) {
            return std::unexpected(ir::Error::EscapeSequenceExpectedSixOrFewerHexDigits);
          } else {
            EmitRepeated(ir::Token::Characters, width); // Record the UTF-8 width of the codepoint digits
          }

          auto converted = text::Unicode::HexStringToCodePoint(std::string_view{begin_hex, end_hex});
          if (!converted) {
            return std::unexpected(converted.error());
          }

          if (cursor.Match('}')) {
            Emit(ir::Token::EscapeUnicodeCodePointEnd);
            return converted.value();
          } else {
            return std::unexpected(ir::Error::EscapeSequenceExpectedClosingBrace);
          }
        } else { // Unicode escape \uXXXX
          auto begin_hex = cursor.cbegin();
          for (int i = 0; i < 4; ++i) {
            if (cursor.Done()) return std::unexpected(ir::Error::EscapeSequenceUnexpectedEndOfInput);
            if (!IsHex()) return std::unexpected(ir::Error::EscapeSequenceExpectedFourHexDigits);
            cursor.Advance();
          }

          auto converted = text::Unicode::HexStringToCodePoint(std::string_view{begin_hex, cursor.cbegin()});
          if (!converted) return std::unexpected(converted.error());

          Emit(ir::Token::EscapeUnicodeShort);
          return converted.value();
        }
      } else if (cursor.Peek() == 'x') { // Hexadecimal escape \xXX
        cursor.Advance();
        auto begin_hex = cursor.cbegin();
        for (int i = 0; i < 2; ++i) {
          if (cursor.Done()) {
            return std::unexpected(ir::Error::EscapeSequenceUnexpectedEndOfInput);
          }
          if (!IsHex()) {
            return std::unexpected(ir::Error::EscapeSequenceExpectedTwoHexDigits);
          }
          cursor.Advance();
        }

        auto converted = text::Unicode::HexStringToCodePoint(std::string_view{begin_hex, cursor.cbegin()});
        if (!converted) {
          return std::unexpected(converted.error());
        }

        Emit(ir::Token::EscapeHexCode);
        return converted.value();
      } else { // Single character escape
        auto c = cursor.Peek();
        cursor.Advance();

        switch (c) {
          case 'n': Emit(ir::Token::EscapeNewline); return U'\n';
          case 't': Emit(ir::Token::EscapeTab); return U'\t';
          case 'b': Emit(ir::Token::EscapeBackspace); return U'\b';
          case 'r': Emit(ir::Token::EscapeReturn); return U'\r';
          case 'f': Emit(ir::Token::EscapeFormFeed); return U'\f';
          default:  Emit(ir::Token::EscapeCharacter); return static_cast<unsigned char>(c); // TODO: What if it isn't ASCII?
        }
      }
    }

    bool Char() {
      if (!CharOpen()) return false;

      bool backtracked = furthest > cursor.cbegin();
      char32_t cp = 0;

      if (cursor.Peek() == '\\') {
        auto esc = Escape();
        if (!esc) {
          Error(esc.error());
          // For character literals we standardize on this specific error, regardless of underlying escape issue
          return Error(ir::Error::CharacterLiteralExpectedEscapeAfterBackslash);
        }

        cp = esc.value();
        Emit(ir::Token::Character); // Always emit token (tokens are rolled back, symbols are not)

        if (!backtracked) {
          ir::Symbol sym = mod.AddSymbol(ir::symbol::Type::Character);
          sym.Value(static_cast<uint64_t>(cp));
        }
      } else if (cursor.Peek() == '\'') {
        return Error(ir::Error::CharacterLiteralExpectedContent);
      } else {
        auto begin = cursor.cbegin();

        cp = cursor.CodePoint(); // Consume raw code point from source
        Emit(ir::Token::Character); // Mark symbol location
        if (!backtracked) {
          ir::Symbol sym = mod.AddSymbol(ir::symbol::Type::Character);
          sym.Value(static_cast<uint64_t>(cp));
        }

        EmitRepeated(ir::Token::Characters, begin); // Record the UTF-8 width of the character
      }

      if (!CharClose()) return Error(ir::Error::CharacterLiteralExpectedClosingSingleQuote);
      return OWS();
    }

    bool String() {
      if (!StringOpen()) return false;

      auto begin = cursor.cbegin();
      auto start = mod.Characters().size();

      while (true) {
        if (cursor.Done()) return Error(ir::Error::StringLiteralExpectedClosingDoubleQuote);
        if (IsBreak()) return Error(ir::Error::StringLiteralUnexpectedLineBreak);
        if (cursor.Peek() == '"') break; // End of string

        if (cursor.Peek() == '\\') {
          // Record width for the plain chunk preceding the escape
          EmitRepeated(ir::Token::Characters, begin);

          auto esc = Escape();
          if (!esc) return Error(esc.error());

          // Append resulting code point unless backtracked
          if (!(furthest > cursor.cbegin())) {
            mod.AddCharacter(esc.value());
          }

          begin = cursor.cbegin(); // start a new chunk after the escape
          continue;
        }

        // Don't capture characters when backtracked
        if (furthest > cursor.cbegin()) {
          cursor.Advance(1);
        } else {
          mod.AddCharacter(cursor.CodePoint());
        }
      }

      EmitRepeated(ir::Token::Characters, begin); // Record the UTF-8 width of the string

      Emit(ir::Token::Character); // Mark that a symbol is found here

      if (mod.Characters().size() > start) {
        ir::Symbol sym = mod.AddSymbol(ir::symbol::Type::String);
        sym.Value(static_cast<uint32_t>(start), static_cast<uint32_t>(mod.Characters().size() - start));
      }

      if (!StringClose()) return Error(ir::Error::StringLiteralExpectedClosingDoubleQuote);
      return OWS();
    }

    // Numeric literal helpers (Hex/Octal/Binary and general Number) adapted from lexical lexer
    bool Hex() {
      // assume leading 0x has already been consumed by caller or caller will advance; here consume 2
      cursor.Advance(2);
      uint32_t width = 2;

      while (!cursor.Done()) {
        if (cursor.Peek() == '_') { cursor.Advance(); ++width; Emit(ir::Token::Underscore); continue; }
        if (!IsHex()) break;
        cursor.Advance(); ++width;
      }

      Emit(ir::Token::Hex);
      return true;
    }

    bool Octal() {
      cursor.Advance(2);
      uint32_t width = 2;

      while (!cursor.Done()) {
        if (cursor.Peek() == '_') { cursor.Advance(); ++width; continue; }
        if (!IsOctal()) break;
        cursor.Advance(); ++width;
      }

      Emit(ir::Token::Octal);
      return true;
    }

    bool Binary() {
      cursor.Advance(2);
      uint32_t width = 2;

      while (!cursor.Done()) {
        if (cursor.Peek() == '_') { cursor.Advance(); ++width; continue; }
        if (!IsBinary()) break;
        cursor.Advance(); ++width;
      }

      Emit(ir::Token::Binary);
      return true;
    }

    bool NumberHelper() {
      auto start_iter = cursor.cbegin();
      bool negative = false;

      if (cursor.Peek() == '-') { cursor.Advance(); }
      else if (cursor.Peek() == '+') { cursor.Advance(); }

      if (cursor.Peek() == '0') {
        switch (cursor.Peek(1)) {
          case 'x': return Hex();
          case 'o': return Octal();
          case 'b': return Binary();
        }
      }

      bool decimal_point_encountered = false;
      bool e_encountered = false;
      bool exponent_sign_encountered = false;
      bool digit_encountered = false;

      while (!cursor.Done()) {
        char c = cursor.Peek();

        if (c == '_' && digit_encountered) { cursor.Advance(); continue; }
        else if (c == '.') {
          if (decimal_point_encountered || e_encountered) break;
          decimal_point_encountered = true;
          if (!IsDigit(cursor.Peek(1))) break; // trailing decimal not allowed
        } else if (c == 'E' || c == 'e') {
          if (e_encountered) break;
          e_encountered = true;
          decimal_point_encountered = true;
        } else if ((c == '+' || c == '-') && e_encountered && !exponent_sign_encountered) {
          exponent_sign_encountered = true;
        } else if (IsDigit(c)) {
          digit_encountered = true;
        } else {
          break;
        }

        cursor.Advance();
      }

      size_t size = static_cast<size_t>(std::distance(start_iter, cursor.cbegin()));
      if (size == 0 || !digit_encountered) {
        if (size > 0) cursor.Retreat(start_iter);
        return false;
      }

      if (decimal_point_encountered) Emit(ir::Token::Decimal);
      else Emit(ir::Token::Integer);

      return true;
    }

    inline bool Number() { return cursor.IsNumberStart() && NumberHelper(); }

    // Identifier parsing: supports escapes in identifiers and unicode code points.
    bool IdentifierHelper() {
      auto start = cursor.cbegin();
      // First character must be identifier start (including '\\' for escapes)
      if (cursor.Peek() == '\\') {
        auto esc = Escape();
        if (!esc) return false; // propagate failure; a later pass may produce a diagnostic
      } else if (!IsIdentStart()) {
        return false;
      } else {
        cursor.Advance();
      }

      // Subsequent characters may be identifier continue characters
      while (!cursor.Done()) {
        if (cursor.Peek() == '\\') {
          auto esc = Escape();
          if (!esc) break; // allow escape inside identifier; stop if malformed
        } else if (IsIdent()) {
          cursor.Advance();
        } else {
          break;
        }
      }

      // Extract the text of the identifier
      std::string_view text{start, cursor.cbegin()};
      if (text.empty()) return false;

      // Check for keywords first and emit corresponding tokens
      if (text == "as") return Emit(ir::Token::As);
      if (text == "auto") return Emit(ir::Token::Auto);
      if (text == "await") return Emit(ir::Token::Await);
      if (text == "break") return Emit(ir::Token::Break);
      if (text == "case") return Emit(ir::Token::Case);
      if (text == "compiler") return Emit(ir::Token::Compiler);
      if (text == "continue") return Emit(ir::Token::Continue);
      if (text == "default") return Emit(ir::Token::Default);
      if (text == "do") return Emit(ir::Token::Do);
      if (text == "else") return Emit(ir::Token::Else);
      if (text == "export") return Emit(ir::Token::Export);
      if (text == "false") return Emit(ir::Token::False);
      if (text == "for") return Emit(ir::Token::For);
      if (text == "from") return Emit(ir::Token::From);
      if (text == "has") return Emit(ir::Token::Has);
      if (text == "if") return Emit(ir::Token::If);
      if (text == "import") return Emit(ir::Token::Import);
      if (text == "in") return Emit(ir::Token::In);
      if (text == "is") return Emit(ir::Token::Is);
      if (text == "let") return Emit(ir::Token::Let);
      if (text == "match") return Emit(ir::Token::Match);
      if (text == "null") return Emit(ir::Token::Null);
      if (text == "register") return Emit(ir::Token::Register);
      if (text == "repeat") return Emit(ir::Token::Repeat);
      if (text == "return") return Emit(ir::Token::Return);
      if (text == "this") return Emit(ir::Token::This);
      if (text == "true") return Emit(ir::Token::True);
      if (text == "undefined") return Emit(ir::Token::Undefined);
      if (text == "void") return Emit(ir::Token::Void);
      if (text == "while") return Emit(ir::Token::While);
      if (text == "with") return Emit(ir::Token::With);
      if (text == "yield") return Emit(ir::Token::Yield);

      // Not a keyword: emit an Identifier token for the consumed range
      return Emit(ir::Token::Identifier);
    }

    bool Identifier() { return cursor.IsPossibleIdentifierStart() && IdentifierHelper(); }

    // Full template string parser: stores characters in module, emits Characters tokens and Symbols,
    // and handles interpolation via TemplateStringExpressionOpen/Close.
    bool TemplateString() {
      if (!TemplateStringOpen()) return false;

      auto start_index = mod.Characters().size();
      size_t chunk_width = 0;

      while (!cursor.Done()) {
        if (cursor.Peek() == '`') {
          // Flush any remaining characters as a symbol + token
          if (chunk_width > 0) {
            Emit(ir::Token::Characters);
            ir::Symbol sym = mod.AddSymbol(ir::symbol::Type::String);
            sym.Value(static_cast<uint32_t>(start_index), static_cast<uint32_t>(mod.Characters().size() - start_index));
            // Reset for next potential chunk (though closing)
            start_index = mod.Characters().size();
            chunk_width = 0;
          }

          TemplateStringClose();
          return true;
        }

        if (cursor.Peek() == '{') {
          // Flush current chunk before entering expression
          if (chunk_width > 0) {
            Emit(ir::Token::Characters);
            ir::Symbol sym = mod.AddSymbol(ir::symbol::Type::String);
            sym.Value(static_cast<uint32_t>(start_index), static_cast<uint32_t>(mod.Characters().size() - start_index));
            start_index = mod.Characters().size();
            chunk_width = 0;
          }

          if (!TemplateStringExpressionOpen()) return Error("Template string literal didn't find the opening '{'");

          // Parse embedded expression
          if (!Expression()) return Error("Template string literal expected an expression after '{'");
          if (!TemplateStringExpressionClose()) return Error("Template string literal expected a closing '}' after expression");

          // Continue with next chunk
          continue;
        }

        if (cursor.Peek() == '\\') {
          // Flush current chunk before handling escape
          if (chunk_width > 0) {
            Emit(ir::Token::Characters);
            ir::Symbol sym = mod.AddSymbol(ir::symbol::Type::String);
            sym.Value(static_cast<uint32_t>(start_index), static_cast<uint32_t>(mod.Characters().size() - start_index));
            start_index = mod.Characters().size();
            chunk_width = 0;
          }

          if (!Escape()) return Error("Template string literal expected an escape sequence after '\\'");
          continue;
        }

        // Consume a Unicode code point and append to module characters
        auto iter = cursor.cbegin();
        auto cp = cursor.CodePoint(); // consumes code point
        mod.AddCharacter(cp);
        ++chunk_width;

        // Track new lines similar to other places
        switch (cp) {
          case U'\u2028':
          case U'\u2029':
          case U'\n': {
            mod.AddLine(cursor.cbegin());
            break;
          }
          case U'\r': {
            if (cursor.Peek() == '\n') {
              cursor.Advance(1);
              mod.AddCharacter(U'\n');
              mod.AddLine(cursor.cbegin());
            } else {
              mod.AddLine(cursor.cbegin());
            }
            break;
          }
        }
      }

      return Error("Unexpected end of file in template string literal");
    }

    bool BinaryOperatorHelper(bool in_enum = false) {
      switch (cursor.Peek()) {
        case '!': {
          switch (cursor.Peek(1)) {
            case '!': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssignTruthyAnd, 3);
                default:  return EmitAndAdvance(ir::Token::Wrap, 2);
              }
            }
            case '=': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssertNotEqual, 3);
                default:  return EmitAndAdvance(ir::Token::NotEqual, 2);
              }
            }
            default: return false;
          }
        }
        case '=': {
          switch (cursor.Peek(1)) {
            case '>': return EmitAndAdvance(ir::Token::InlineFunctionArrow, 2);
            case '=': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssertEqual, 3);
                default:  return EmitAndAdvance(ir::Token::Equal, 2);
              }
            }
            default: return EmitAndAdvance(ir::Token::Assign, 1);
          }
        }
        case '<': {
          switch (cursor.Peek(1)) {
            case '<': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::BitwiseAssignLeftShift, 3);
                case '<': return EmitAndAdvance(ir::Token::BitwiseTripleLeftShift, 3);
                default:  return EmitAndAdvance(ir::Token::BitwiseLeftShift, 2);
              }
            }
            case '=': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssertLesserOrEqual, 3);
                default:  return EmitAndAdvance(ir::Token::LesserOrEqual, 2);
              }
            }
            default: return EmitAndAdvance(ir::Token::Lesser, 1);
          }
        }
        case '>': {
          if (in_enum) return false;
          switch (cursor.Peek(1)) {
            case '>': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::BitwiseAssignRightShift, 3);
                case '>': return EmitAndAdvance(ir::Token::BitwiseTripleRightShift, 3);
                default:  return EmitAndAdvance(ir::Token::BitwiseRightShift, 2);
              }
            }
            case '=': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssertGreaterOrEqual, 3);
                default:  return EmitAndAdvance(ir::Token::GreaterOrEqual, 2);
              }
            }
            default: return EmitAndAdvance(ir::Token::Greater, 1);
          }
        }
        case '+': {
          switch (cursor.Peek(1)) {
            case '=': return EmitAndAdvance(ir::Token::AssignAdd, 2);
            default:  return EmitAndAdvance(ir::Token::Add, 1);
          }
        }
        case '-': {
          switch (cursor.Peek(1)) {
            case '=': return EmitAndAdvance(ir::Token::AssignSubtract, 2);
            default:  return EmitAndAdvance(ir::Token::Subtract, 1);
          }
        }
        case '*': {
          switch (cursor.Peek(1)) {
            case '*': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssignExponent, 3);
                default:  return EmitAndAdvance(ir::Token::Exponent, 2);
              }
            }
            case '=': return EmitAndAdvance(ir::Token::AssignMultiply, 2);
            default:  return EmitAndAdvance(ir::Token::Multiply, 1);
          }
        }
        case '/': {
          switch (cursor.Peek(1)) {
            case '=': return EmitAndAdvance(ir::Token::AssignDivide, 2);
            default:  return EmitAndAdvance(ir::Token::Divide, 1);
          }
        }
        case '%': {
          switch (cursor.Peek(1)) {
            case '=': return EmitAndAdvance(ir::Token::AssignModulo, 2);
            default:  return EmitAndAdvance(ir::Token::Modulo, 1);
          }
        }
        case '^': {
          switch (cursor.Peek(1)) {
            case '=': return EmitAndAdvance(ir::Token::BitwiseAssignXor, 2);
            default:  return EmitAndAdvance(ir::Token::BitwiseXor, 1);
          }
        }
        case '?': {
          switch (cursor.Peek(1)) {
            case '.': return EmitAndAdvance(ir::Token::OptionalMemberAccess, 2);
            case ':': return EmitAndAdvance(ir::Token::MemberAccessStaticOptional, 2);
            case '?': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssignTruthyOr, 3);
                default:  return EmitAndAdvance(ir::Token::Unwrap, 2);
              }
            }
            case '=': return EmitAndAdvance(ir::Token::AssignOptional, 2);
            default:  return false;
          }
        }
        case '&': {
          switch (cursor.Peek(1)) {
            case '&': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssignAnd, 3);
                default:  return EmitAndAdvance(ir::Token::And, 2);
              }
            }
            default: return EmitAndAdvance(ir::Token::BitwiseAnd, 1);
          }
        }
        case '|': {
          switch (cursor.Peek(1)) {
            case '|': {
              switch (cursor.Peek(2)) {
                case '=': return EmitAndAdvance(ir::Token::AssignOr, 3);
                default:  return EmitAndAdvance(ir::Token::Or, 2);
              }
            }
            case '=': return EmitAndAdvance(ir::Token::BitwiseAssignOr, 2);
            default:  return EmitAndAdvance(ir::Token::BitwiseOr, 1);
          }
        }
        case 'w': return With();
        case '.': {
          switch (cursor.Peek(1)) {
            case '.': {
              switch (cursor.Peek(2)) {
                case '.': return EmitAndAdvance(ir::Token::InclusiveRange, 3);
                default:  return EmitAndAdvance(ir::Token::ExclusiveRange, 2);
              }
            }
            default: return EmitAndAdvance(ir::Token::MemberAccess, 1);
          }
        }
        case ':': {
          switch (cursor.Peek(1)) {
            case ':': return EmitAndAdvance(ir::Token::MemberAccessStatic, 2);
            default:  return false;
          }
        }
        default: return false;
      }
    }

    inline bool BinaryOperator(bool in_enum = false) { return cursor.IsBinaryStart() && BinaryOperatorHelper(in_enum); }

    bool ParameterDeclaration() {
      int modifiers = Modifiers();

      if (Identifier()) {
        if (cursor.Peek() == ',' || cursor.Peek() == ')') return true;
      } else {
        if (modifiers > 0) {
          return Error("Declaration expected an identifier after the modifiers");
        }

        return false;
      }

      return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
    }

    bool ParameterDeclarationList() { return ZeroOrMore([&]{ return ParameterDeclaration(); }, [&]{ return Comma(); }); }

    bool Parameters() { return ParameterOpen() && ParameterDeclarationList() && ParameterClose(); }

    bool Function() {
      return Try([&]{
        if (Parameters()) return (Captures() || true) && FunctionBody() || ArrowFunction();
        else return Captures() && (Parameters() || true) && FunctionBody() || ArrowFunction();
      });
    }

    // High-level parsing helpers (ported from lexical::Lexer)
    bool IdentifiedExpression() { return IsIdent() && Identifier() && ContinueExpression(); }

    bool CallablePrefixLiteralHelper() {
      switch (cursor.Peek()) {
        case '\'': return Char();
        case '"': return String();
        case '`': return TemplateString();
        default: return Number();
      }
    }

    bool CallablePostfixLiteralHelper() {
      switch (cursor.Peek()) {
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

    inline bool CallablePrefixLiteral() { return cursor.IsCallablePrefixStart() && CallablePrefixLiteralHelper(); }
    inline bool CallablePostfixLiteral() { return cursor.IsCallablePostfixStart() && CallablePostfixLiteralHelper(); }

    bool ValueShortcut() {
      switch (cursor.Peek()) {
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
        case '<': return TemplateString() || Enum();
        case '{': return Object();
        default: return false;
      }
    }

    bool Value() { return (cursor.IsValueStart() && ValueShortcut()) || Identifier(); }

    bool InitialValue() {
      if (cursor.IsValueStart() && ValueShortcut()) {
        // maybe_declaration = false; // not tracked in new lexer
      }

      return Identifier();
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
      while (!cursor.Done()) {
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
      while (!cursor.Done()) {
        if (BinaryOperator(true)) {
          while (UnaryPrefixOperator()) {}

          if (!Value()) return false;
        } else if (!CallablePostfixLiteral()) {
          break; // No more operators or values
        }
      }

      return true;
    }

    bool Expression() {
      while (UnaryPrefixOperator()) {}

      if (!Value()) {
        return false;
      }

      return ContinueExpression();
    }

    bool EnumExpression() {
      while (UnaryPrefixOperator()) {}

      if (!Value()) {
        return false;
      }

      return ContinueEnumExpression();
    }

    bool DeclarationValue() {
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
      int modifiers = Modifiers();

      if (UnaryStartedExpression()) return true;

      if (Identifier()) {
        if (cursor.Peek() == ',') return true;
      }
      else {
        if (modifiers > 0) {
          return Error("Declaration expected an identifier after the modifiers");
        }

        return false;
      }

      return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
    }

    bool StatementShortcut() {
      switch (cursor.Peek()) {
        case 'b': return Break();
        case 'c': return Continue() || Case();
        case 'd': return Do() || Default();
        case 'e': return Export();
        case 'f': return For();
        case 'i': return If() || Import() || Is();
        case 'm': return Match();
        case 'r': return Return() || Repeat() || Register();
        case 'w': return While();
        case 'y': return Yield();
        default: return false;
      }
    }

    bool Statement() { return (cursor.IsStatementStart() && StatementShortcut()) || Declaration() || Expression() && (Semicolon() || true); }

    bool StatementList() {
      while (!cursor.Done()) {
        if (!Statement()) break;
      }

      return true;
    }

    bool FunctionStatementList() { return StatementList(); }

    constexpr bool IsBreak() const {
      switch (cursor.Peek()) {
        case '\n': return true;
        case '\r': return true;
        default: {
          // Quick test for LS or PS first two bytes: E2 80 xx
          if (static_cast<unsigned char>(cursor.Peek()) == 0xE2 && static_cast<unsigned char>(cursor.Peek(1)) == 0x80) {
            switch (static_cast<unsigned char>(cursor.Peek(2))) {
              case 0xA8: return true; // LS
              case 0xA9: return true; // PS
            }
          }

          return false;
        }
      }
    }

    constexpr bool IsASCII() const { return IsASCII(cursor.Peek()); }
    constexpr bool IsSpace() const { return IsSpace(cursor.Peek()); }
    constexpr bool IsAlpha() const { return IsAlpha(cursor.Peek()); }
    constexpr bool IsAlNum() const { return IsAlNum(cursor.Peek()); }
    constexpr bool IsUpper() const { return IsUpper(cursor.Peek()); }
    constexpr bool IsLower() const { return IsLower(cursor.Peek()); }
    constexpr bool IsDigit() const { return IsDigit(cursor.Peek()); }
    constexpr bool IsPunct() const { return IsPunct(cursor.Peek()); }
    constexpr bool IsSlash() const { return IsSlash(cursor.Peek()); }
    constexpr bool IsOctal() const { return IsOctal(cursor.Peek()); }
    constexpr bool IsHex  () const { return IsHex(cursor.Peek()); }
    constexpr bool IsIdent() const { return IsIdent(cursor.Peek()); }
    constexpr bool IsBinary() const { return IsBinary(cursor.Peek()); }
    constexpr bool IsURLProtocol() const { return IsURLProtocol(cursor.Peek()); }
    constexpr bool IsURLHost() const { return IsURLHost(cursor.Peek()); }
    constexpr bool IsURLPathname() const { return IsURLPathname(cursor.Peek()); }
    constexpr bool IsURLSearch() const { return IsURLSearch(cursor.Peek()); }
    constexpr bool IsURLHash() const { return IsURLHash(cursor.Peek()); }
    constexpr bool IsURLAuthority() const { return IsURLAuthority(cursor.Peek()); }
    constexpr bool IsFilePath() const { return IsFilePath(cursor.Peek()); }
    constexpr bool IsBinaryStart() const { return IsBinaryStart(cursor.Peek()); }
    constexpr bool IsNumberStart() const { return IsNumberStart(cursor.Peek()); }
    constexpr bool IsIdentStart() const { return IsIdentStart(cursor.Peek()); }
    constexpr bool IsValueStart() const { return IsValueStart(cursor.Peek()); }
    constexpr bool IsStatementStart() const { return IsStatementStart(cursor.Peek()); }
    constexpr bool IsUnaryPrefixStart() const { return IsUnaryPrefixStart(cursor.Peek()); }
    constexpr bool IsUnaryPostfixStart() const { return IsUnaryPostfixStart(cursor.Peek()); }
    constexpr bool IsCallablePrefixStart() const { return IsCallablePrefixStart(cursor.Peek()); }
    constexpr bool IsCallablePostfixStart() const { return IsCallablePostfixStart(cursor.Peek()); }
    constexpr bool IsModifierStart() const { return IsModifierStart(cursor.Peek()); }
    constexpr bool IsPossibleSpaceStart() const { return IsPossibleSpaceStart(cursor.Peek()); }
    constexpr bool IsPossibleIdentifierStart() const { return IsPossibleIdentifierStart(cursor.Peek()); }

    constexpr bool IsBreak(size_t i) const {
      switch (cursor.Peek(i)) {
        case '\n': return true;
        case '\r': return true;
        default: {
          // Quick test for LS or PS first two bytes: E2 80 xx
          if (static_cast<unsigned char>(cursor.Peek(i)) == 0xE2 && static_cast<unsigned char>(cursor.Peek(i + 1)) == 0x80) {
            switch (static_cast<unsigned char>(cursor.Peek(i + 2))) {
              case 0xA8: return true; // LS
              case 0xA9: return true; // PS
            }
          }

          return false;
        }
      }
    }

    constexpr bool IsASCII(size_t i) const { return IsASCII(cursor.Peek(i)); }
    constexpr bool IsSpace(size_t i) const { return IsSpace(cursor.Peek(i)); }
    constexpr bool IsAlpha(size_t i) const { return IsAlpha(cursor.Peek(i)); }
    constexpr bool IsAlNum(size_t i) const { return IsAlNum(cursor.Peek(i)); }
    constexpr bool IsUpper(size_t i) const { return IsUpper(cursor.Peek(i)); }
    constexpr bool IsLower(size_t i) const { return IsLower(cursor.Peek(i)); }
    constexpr bool IsDigit(size_t i) const { return IsDigit(cursor.Peek(i)); }
    constexpr bool IsPunct(size_t i) const { return IsPunct(cursor.Peek(i)); }
    constexpr bool IsSlash(size_t i) const { return IsSlash(cursor.Peek(i)); }
    constexpr bool IsOctal(size_t i) const { return IsOctal(cursor.Peek(i)); }
    constexpr bool IsHex  (size_t i) const { return IsHex(cursor.Peek(i)); }
    constexpr bool IsIdent(size_t i) const { return IsIdent(cursor.Peek(i)); }
    constexpr bool IsBinary(size_t i) const { return IsBinary(cursor.Peek(i)); }
    constexpr bool IsBinaryStart(size_t i) const { return IsBinaryStart(cursor.Peek(i)); }
    constexpr bool IsNumberStart(size_t i) const { return IsNumberStart(cursor.Peek(i)); }
    constexpr bool IsURLProtocol(size_t i) const { return IsURLProtocol(cursor.Peek(i)); }
    constexpr bool IsURLHost(size_t i) const { return IsURLHost(cursor.Peek(i)); }
    constexpr bool IsURLPathname(size_t i) const { return IsURLPathname(cursor.Peek(i)); }
    constexpr bool IsURLSearch(size_t i) const { return IsURLSearch(cursor.Peek(i)); }
    constexpr bool IsURLHash(size_t i) const { return IsURLHash(cursor.Peek(i)); }
    constexpr bool IsURLAuthority(size_t i) const { return IsURLAuthority(cursor.Peek(i)); }
    constexpr bool IsFilePath(size_t i) const { return IsFilePath(cursor.Peek(i)); }
    constexpr bool IsIdentStart(size_t i) const { return IsIdentStart(cursor.Peek(i)); }
    constexpr bool IsValueStart(size_t i) const { return IsValueStart(cursor.Peek(i)); }
    constexpr bool IsStatementStart(size_t i) const { return IsStatementStart(cursor.Peek(i)); }
    constexpr bool IsUnaryPrefixStart(size_t i) const { return IsUnaryPrefixStart(cursor.Peek(i)); }
    constexpr bool IsUnaryPostfixStart(size_t i) const { return IsUnaryPostfixStart(cursor.Peek(i)); }
    constexpr bool IsCallablePrefixStart(size_t i) const { return IsCallablePrefixStart(cursor.Peek(i)); }
    constexpr bool IsCallablePostfixStart(size_t i) const { return IsCallablePostfixStart(cursor.Peek(i)); }
    constexpr bool IsModifierStart(size_t i) const { return IsModifierStart(cursor.Peek(i)); }
    constexpr bool IsPossibleSpaceStart(size_t i) const { return IsPossibleSpaceStart(cursor.Peek(i)); }
    constexpr bool IsPossibleIdentifierStart(size_t i) const { return IsPossibleIdentifierStart(cursor.Peek(i)); }

    constexpr bool IsASCII(const char c) const { return static_cast<uint8_t>(c) < 0x80; }
    constexpr bool IsSpace(const char c) const { return WHITESPACE[static_cast<uint8_t>(c)]; }
    constexpr bool IsAlpha(const char c) const { return ALPHA[static_cast<uint8_t>(c)]; }
    constexpr bool IsAlNum(const char c) const { return ALNUM[static_cast<uint8_t>(c)]; }
    constexpr bool IsUpper(const char c) const { return UPPER[static_cast<uint8_t>(c)]; }
    constexpr bool IsLower(const char c) const { return LOWER[static_cast<uint8_t>(c)]; }
    constexpr bool IsDigit(const char c) const { return DIGIT[static_cast<uint8_t>(c)]; }
    constexpr bool IsPunct(const char c) const { return PUNCT[static_cast<uint8_t>(c)]; }
    constexpr bool IsSlash(const char c) const { return SLASH[static_cast<uint8_t>(c)]; }
    constexpr bool IsOctal(const char c) const { return OCTAL[static_cast<uint8_t>(c)]; }
    constexpr bool IsHex  (const char c) const { return HEX  [static_cast<uint8_t>(c)]; }
    constexpr bool IsIdent(const char c) const { return IDENTIFIER[static_cast<uint8_t>(c)]; }
    constexpr bool IsBinary(const char c) const { return c == '0'|| c == '1'; }
    constexpr bool IsIdentStart(const char c) const { return IDENTIFIER_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsValueStart(const char c) const { return VALUE_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsStatementStart(const char c) const { return STATEMENT_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsUnaryPrefixStart(const char c) const { return UNARY_PREFIX_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsUnaryPostfixStart(const char c) const { return UNARY_POSTFIX_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsCallablePrefixStart(const char c) const { return CALLABLE_PREFIX_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsCallablePostfixStart(const char c) const { return CALLABLE_POSTFIX_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsBinaryStart(const char c) const { return BINARY_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsModifierStart(const char c) const { return MODIFIER_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsNumberStart(const char c) const { return NUMBER_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsPossibleSpaceStart(const char c) const { return POSSIBLE_SPACE_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsPossibleIdentifierStart(const char c) const { return POSSIBLE_IDENTIFIER_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLProtocol(const char c) const { return URL_PROTOCOL[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLHost(const char c) const { return URL_HOST[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLPathname(const char c) const { return URL_PATHNAME[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLSearch(const char c) const { return URL_SEARCH[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLHash(const char c) const { return URL_HASH[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLAuthority(const char c) const { return URL_AUTHORITY[static_cast<uint8_t>(c)]; }
    constexpr bool IsFilePath(const char c) const { return FILE_PATH[static_cast<uint8_t>(c)]; }

    // TODO: Explore these alternatives for performance
    // TODO: Also explore a byte LUT
    // constexpr bool IsUpper(const char c) const { return static_cast<unsigned char>(c) - 'A' <= ('Z' - 'A'); }
    // constexpr bool IsLower(const char c) const { return static_cast<unsigned char>(c) - 'a' <= ('z' - 'a'); }
    // constexpr bool IsDigit(const char c) const { return static_cast<unsigned char>(c) - '0' <= 9; }
    // constexpr bool IsAlpha(const char c) const {
    //   unsigned char uc = static_cast<unsigned char>(c);
    //   return (uc - 'A' <= ('Z' - 'A')) || (uc - 'a' <= ('z' - 'a'));
    // }
    // constexpr bool IsAlNum(const char c) const {
    //   unsigned char uc = static_cast<unsigned char>(c);
    //   return (uc - '0' <= 9) || (uc - 'A' <= ('Z' - 'A')) || (uc - 'a' <= ('z' - 'a'));
    // }
    // constexpr bool IsOctal(const char c) const { return static_cast<unsigned char>(c) - '0' <= 7; }
    // constexpr bool IsNumberStart(const char c) const {
    //   unsigned char uc = static_cast<unsigned char>(c);
    //   return (uc - '0' <= 9) || uc == '+' || uc == '-';
    // }
  };
};