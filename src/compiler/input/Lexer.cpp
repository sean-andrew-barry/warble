import <array>;
import <limits>;
import <string>;
import <vector>;

import compiler.input.Lexer;
import compiler.ir.Error;
import compiler.text.cursor.String;
import compiler.text.Unicode;

namespace compiler::input {
  namespace {
    constexpr std::array<ir::Token, 16> DIGIT_TOKENS{
      ir::Token::Digits0, ir::Token::Digits1, ir::Token::Digits2, ir::Token::Digits3,
      ir::Token::Digits4, ir::Token::Digits5, ir::Token::Digits6, ir::Token::Digits7,
      ir::Token::Digits8, ir::Token::Digits9, ir::Token::DigitsA, ir::Token::DigitsB,
      ir::Token::DigitsC, ir::Token::DigitsD, ir::Token::DigitsE, ir::Token::DigitsF,
    };

    constexpr ir::Token DigitToken(uint8_t value) {
      return DIGIT_TOKENS[value & 0xF];
    }
  } // namespace

  Lexer::Position Lexer::Start() {
    return Position{cursor.Current(), tokens.size(), data.size(), errors.size()};
  }

  void Lexer::Rollback(const Lexer::Position& position) {
    furthest = std::max(furthest, cursor.cbegin());
    cursor.Retreat(position.cursor);
    tokens.resize(position.token);
    data.resize(position.data);
    errors.resize(position.error);
  }

  bool Lexer::Emit(ir::Token token) { tokens.push_back(token); return true; }

  // Simple QoL function to emit a token after matching a char/string and consume whitespace
  bool Lexer::Match(const char c, ir::Token token) { return cursor.Match(c) && Emit(token) && (WS() || true); }
  bool Lexer::Match(const std::string_view s, ir::Token token) { return cursor.Match(s) && Emit(token) && (WS() || true); }

  bool Lexer::EmitCharactersNibbles(uint32_t count) {
    if (count == 0) return false;
    uint32_t v = count;
    uint8_t nibbles[8];
    int n = 0;
    while (v > 0) { nibbles[n++] = static_cast<uint8_t>(v & 0xF); v >>= 4; }
    for (int i = n - 1; i >= 0; --i) {
      switch (nibbles[i]) {
        case 0x0: Emit(ir::Token::Characters0); break;
        case 0x1: Emit(ir::Token::Characters1); break;
        case 0x2: Emit(ir::Token::Characters2); break;
        case 0x3: Emit(ir::Token::Characters3); break;
        case 0x4: Emit(ir::Token::Characters4); break;
        case 0x5: Emit(ir::Token::Characters5); break;
        case 0x6: Emit(ir::Token::Characters6); break;
        case 0x7: Emit(ir::Token::Characters7); break;
        case 0x8: Emit(ir::Token::Characters8); break;
        case 0x9: Emit(ir::Token::Characters9); break;
        case 0xA: Emit(ir::Token::CharactersA); break;
        case 0xB: Emit(ir::Token::CharactersB); break;
        case 0xC: Emit(ir::Token::CharactersC); break;
        case 0xD: Emit(ir::Token::CharactersD); break;
        case 0xE: Emit(ir::Token::CharactersE); break;
        case 0xF: Emit(ir::Token::CharactersF); break;
      }
    }
    return true;
  }

  bool Lexer::EmitSpacesNibbles(uint32_t count) {
    if (count == 0) return false;
    uint32_t v = count;
    uint8_t nibbles[8];
    int n = 0;
    while (v > 0) { nibbles[n++] = static_cast<uint8_t>(v & 0xF); v >>= 4; }
    for (int i = n - 1; i >= 0; --i) {
      switch (nibbles[i]) {
        case 0x0: Emit(ir::Token::Spaces0); break;
        case 0x1: Emit(ir::Token::Spaces1); break;
        case 0x2: Emit(ir::Token::Spaces2); break;
        case 0x3: Emit(ir::Token::Spaces3); break;
        case 0x4: Emit(ir::Token::Spaces4); break;
        case 0x5: Emit(ir::Token::Spaces5); break;
        case 0x6: Emit(ir::Token::Spaces6); break;
        case 0x7: Emit(ir::Token::Spaces7); break;
        case 0x8: Emit(ir::Token::Spaces8); break;
        case 0x9: Emit(ir::Token::Spaces9); break;
        case 0xA: Emit(ir::Token::SpacesA); break;
        case 0xB: Emit(ir::Token::SpacesB); break;
        case 0xC: Emit(ir::Token::SpacesC); break;
        case 0xD: Emit(ir::Token::SpacesD); break;
        case 0xE: Emit(ir::Token::SpacesE); break;
        case 0xF: Emit(ir::Token::SpacesF); break;
      }
    }
    return true;
  }

  bool Lexer::EmitTabsNibbles(uint32_t count) {
    if (count == 0) return false;
    uint32_t v = count;
    uint8_t nibbles[8];
    int n = 0;
    while (v > 0) { nibbles[n++] = static_cast<uint8_t>(v & 0xF); v >>= 4; }
    for (int i = n - 1; i >= 0; --i) {
      switch (nibbles[i]) {
        case 0x0: Emit(ir::Token::Tabs0); break;
        case 0x1: Emit(ir::Token::Tabs1); break;
        case 0x2: Emit(ir::Token::Tabs2); break;
        case 0x3: Emit(ir::Token::Tabs3); break;
        case 0x4: Emit(ir::Token::Tabs4); break;
        case 0x5: Emit(ir::Token::Tabs5); break;
        case 0x6: Emit(ir::Token::Tabs6); break;
        case 0x7: Emit(ir::Token::Tabs7); break;
        case 0x8: Emit(ir::Token::Tabs8); break;
        case 0x9: Emit(ir::Token::Tabs9); break;
        case 0xA: Emit(ir::Token::TabsA); break;
        case 0xB: Emit(ir::Token::TabsB); break;
        case 0xC: Emit(ir::Token::TabsC); break;
        case 0xD: Emit(ir::Token::TabsD); break;
        case 0xE: Emit(ir::Token::TabsE); break;
        case 0xF: Emit(ir::Token::TabsF); break;
      }
    }
    return true;
  }

  bool Lexer::Error(ir::Error error) {
    Emit(ir::Token::Error);
    errors.push_back(error);
    return true;
  }

  bool Lexer::EmitAndAdvance(ir::Token token, size_t count = 1) {
    cursor.Advance(count);
    return Emit(token) && (WS() || true);
  }

  bool Lexer::Keyword(const std::string_view text) {
    if (!cursor.Check(text)) return false; // Compare the text to the content
    if (IsAlNum(text.size())) return false; // The next character cannot be an alphanumeric character with a keyword

    cursor.Advance(text.size());

    return true;
  }

  // Simple QoL overload to emit a token after matching a keyword and consume whitespace
  bool Lexer::Keyword(const std::string_view text, ir::Token token) {
    return Keyword(text) && Emit(token) && (WS() || true);
  }

  Lexer::Lexer(std::string&& source)
    : source{std::move(source)}, cursor{this->source}, furthest{cursor.cbegin()} {
  }

  bool Lexer::WhiteSpace() {
    switch (cursor.Peek()) {
      case '\v': return EmitAndAdvance(ir::Token::VerticalTab, 1);
      case '\f': return EmitAndAdvance(ir::Token::FormFeed, 1);
      case ' ': {
        uint32_t count = 0;
        while (!cursor.Done() && cursor.Peek() == ' ') { cursor.Advance(1); ++count; }
        return EmitSpacesNibbles(count);
      }
      case '\t': {
        uint32_t count = 0;
        while (!cursor.Done() && cursor.Peek() == '\t') { cursor.Advance(1); ++count; }
        return EmitTabsNibbles(count);
      }
      case '\n': {
        // Emit one LineFeed token per newline as we consume
        do {
          cursor.Advance(1);
          Emit(ir::Token::LineFeed);
        } while (!cursor.Done() && cursor.Peek() == '\n');
        return true;
      }
      case '\r': {
        // Emit CRLF or CR tokens as we consume
        do {
          cursor.Advance(1); // consume CR
          if (!cursor.Done() && cursor.Peek() == '\n') {
            cursor.Advance(1); // consume LF
            Emit(ir::Token::CarriageReturnLineFeed);
          } else {
            Emit(ir::Token::CarriageReturn);
          }
        } while (!cursor.Done() && cursor.Peek() == '\r');
        return true;
      }
      case '/': {
        switch (cursor.Peek(1)) {
          case '/': {
            cursor.Advance(2); // consume '//'
            Emit(ir::Token::CommentOpen);

            uint32_t run_count = 0;
            while (!cursor.Done() && !IsBreak()) {
              // capture literally (no escape parsing)
              data.push_back(static_cast<uint32_t>(cursor.CodePoint()));
              ++run_count;
            }
            EmitCharactersNibbles(run_count);
            return Emit(ir::Token::CommentClose);
          }
          case '*': {
            cursor.Advance(2); // consume '/*'
            Emit(ir::Token::MultiLineCommentOpen);

            uint32_t run_count = 0;
            while (!cursor.Done()) {
              // Check for closing '*/'
              if (cursor.Peek() == '*' && cursor.Peek(1) == '/') {
                EmitCharactersNibbles(run_count);
                run_count = 0;
                cursor.Advance(2);
                return Emit(ir::Token::MultiLineCommentClose);
              }

              // Handle line breaks as explicit tokens that terminate runs
              auto iter = cursor.cbegin();
              char32_t cp = cursor.CodePoint();

              if (cp == U'\n' || cp == U'\r' || cp == U'\u2028' || cp == U'\u2029') {
                EmitCharactersNibbles(run_count);
                run_count = 0;

                if (cp == U'\r') {
                  if (cursor.Peek() == '\n') {
                    cursor.Advance(1);
                    Emit(ir::Token::CarriageReturnLineFeed);
                    data.push_back(static_cast<uint32_t>(U'\r'));
                    data.push_back(static_cast<uint32_t>(U'\n'));
                  } else {
                    Emit(ir::Token::CarriageReturn);
                    data.push_back(static_cast<uint32_t>(U'\r'));
                  }
                } else if (cp == U'\n') { Emit(ir::Token::LineFeed); data.push_back(static_cast<uint32_t>(U'\n')); }
                else if (cp == U'\u2028') { Emit(ir::Token::LineSeparator); data.push_back(static_cast<uint32_t>(U'\u2028')); }
                else { Emit(ir::Token::ParagraphSeparator); data.push_back(static_cast<uint32_t>(U'\u2029')); }
                continue;
              }

              // Regular comment character (no escape processing)
              data.push_back(static_cast<uint32_t>(cp));
              ++run_count;
            }

            // EOF before closing '*/'
            EmitCharactersNibbles(run_count);
            return Error(ir::Error::MultiLineCommentExpectedClosingAsteriskSlash);
          }
          default: return false;
        }
      }
      default: {
        return HandleNonASCIIWhitespace();
      }
    }
  }

  bool Lexer::HandleNonASCIIWhitespace() {
    if (IsASCII()) return false;

    auto iter = cursor.cbegin();
    auto code_point = cursor.CodePoint(); // Consumes the full code point

    // LS/PS
    if (code_point == 0x2028) { return Emit(ir::Token::LineSeparator); }
    if (code_point == 0x2029) { return Emit(ir::Token::ParagraphSeparator); }

    // Other Unicode whitespace is not consumed here in the new design (no generic Spaces token).
    // Defer to other lexing paths by undoing and returning false.

    // Not whitespace; undo
    cursor.Retreat(iter);
    return false;
  }

  bool Lexer::WS() {
    bool matched = false;
    
    while (IsPossibleSpaceStart() && WhiteSpace()) {
      matched = true;
    }

    return matched;
  }

  bool Lexer::Null() { return Keyword("null", ir::Token::Null); }
  bool Lexer::Undefined() { return Keyword("undefined", ir::Token::Undefined); }
  bool Lexer::True() { return Keyword("true", ir::Token::True); }
  bool Lexer::False() { return Keyword("false", ir::Token::False); }
  bool Lexer::This() { return Keyword("this", ir::Token::This); }
  bool Lexer::That() { return Keyword("that", ir::Token::That); }
  bool Lexer::Import() { return Keyword("import", ir::Token::Import); }
  bool Lexer::Export() { return Keyword("export", ir::Token::Export); }
  bool Lexer::Register() { return Keyword("register", ir::Token::Register); }
  bool Lexer::From() { return Keyword("from", ir::Token::From); }
  bool Lexer::With() { return Keyword("with", ir::Token::With); }
  bool Lexer::Has() { return Keyword("has", ir::Token::Has); }
  bool Lexer::If() { return Keyword("if", ir::Token::If); }
  bool Lexer::Else() { return Keyword("else", ir::Token::Else); }
  bool Lexer::Do() { return Keyword("do", ir::Token::Do); }
  bool Lexer::While() { return Keyword("while", ir::Token::While); }
  bool Lexer::Repeat() { return Keyword("repeat", ir::Token::Repeat); }
  bool Lexer::Is() { return Keyword("is", ir::Token::Is); }
  bool Lexer::In() { return Keyword("in", ir::Token::In); }
  bool Lexer::For() { return Keyword("for", ir::Token::For); }
  bool Lexer::As() { return Keyword("as", ir::Token::As); }
  bool Lexer::Default() { return Keyword("default", ir::Token::Default); }
  bool Lexer::Auto() { return Keyword("auto", ir::Token::Auto); }
  bool Lexer::Void() { return Keyword("void", ir::Token::Void); }
  bool Lexer::When() { return Keyword("when", ir::Token::When); }
  bool Lexer::Await() { return Keyword("await", ir::Token::Await); }
  bool Lexer::Async() { return Keyword("async", ir::Token::Async); }
  bool Lexer::Compiler() { return Keyword("compiler", ir::Token::Compiler); }
  bool Lexer::Break() { return Keyword("break", ir::Token::Break); }
  bool Lexer::Continue() { return Keyword("continue", ir::Token::Continue); }
  bool Lexer::Return() { return Keyword("return", ir::Token::Return); }
  bool Lexer::Case() { return Keyword("case", ir::Token::Case); }
  bool Lexer::Yield() { return Keyword("yield", ir::Token::Yield); }
  bool Lexer::Let() { return Keyword("let", ir::Token::Let); }
  bool Lexer::Const() { return Keyword("const", ir::Token::Const); }
  bool Lexer::Mut() { return Keyword("mut", ir::Token::Mut); }

  bool Lexer::CaptureOpen() { return Match('[', ir::Token::CaptureOpen); }
  bool Lexer::CaptureClose() { return Match(']', ir::Token::CaptureClose); }
  bool Lexer::ParameterOpen() { return Match('(', ir::Token::ParameterOpen); }
  bool Lexer::ParameterClose() { return Match(')', ir::Token::ParameterClose); }
  bool Lexer::ScopeOpen() { return Match('{', ir::Token::ScopeOpen); }
  bool Lexer::ScopeClose() { return Match('}', ir::Token::ScopeClose); }
  bool Lexer::TupleOpen() { return Match('(', ir::Token::TupleOpen); }
  bool Lexer::TupleClose() { return Match(')', ir::Token::TupleClose); }
  bool Lexer::ArrayOpen() { return Match('[', ir::Token::ArrayOpen); }
  bool Lexer::ArrayClose() { return Match(']', ir::Token::ArrayClose); }
  bool Lexer::ObjectOpen() { return Match('{', ir::Token::ObjectOpen); }
  bool Lexer::ObjectClose() { return Match('}', ir::Token::ObjectClose); }
  bool Lexer::EnumOpen() { return Match('<', ir::Token::EnumOpen); }
  bool Lexer::EnumClose() { return Match('>', ir::Token::EnumClose); }

  bool Lexer::CharOpen() { return Match('\'', ir::Token::CharOpen); }
  bool Lexer::CharClose() { return Match('\'', ir::Token::CharClose); }
  bool Lexer::StringOpen() { return Match('"', ir::Token::StringOpen); }
  bool Lexer::StringClose() { return Match('"', ir::Token::StringClose); }
  bool Lexer::TemplateStringOpen() { return Match('`', ir::Token::TemplateStringOpen); }
  bool Lexer::TemplateStringClose() { return Match('`', ir::Token::TemplateStringClose); }
  bool Lexer::TemplateStringExpressionOpen() { return Match('{', ir::Token::TemplateStringExpressionOpen); }
  bool Lexer::TemplateStringExpressionClose() { return Match('}', ir::Token::TemplateStringExpressionClose); }
  bool Lexer::ConditionOpen() { return Match('(', ir::Token::ConditionOpen); }
  bool Lexer::ConditionClose() { return Match(')', ir::Token::ConditionClose); }
  bool Lexer::Pipeline() { return Match("->", ir::Token::Pipeline); }
  bool Lexer::Arrow() { return Match("=>", ir::Token::Arrow); }
  bool Lexer::Wildcard() { return Match('*', ir::Token::Wildcard); }
  bool Lexer::Comma() { return Match(',', ir::Token::Comma); }
  bool Lexer::Semicolon() { return Match(';', ir::Token::Semicolon); }
  bool Lexer::Slash() { return Match('/', ir::Token::Slash); }
  bool Lexer::TypeStart() { return Match(':', ir::Token::TypeStart); }
  bool Lexer::CommentOpen() { return Match("//", ir::Token::CommentOpen); }
  bool Lexer::CommentClose() { return Emit(ir::Token::CommentClose) && (WS() || true); }
  bool Lexer::MultiLineCommentOpen() { return Match("/*", ir::Token::MultiLineCommentOpen); }
  bool Lexer::MultiLineCommentClose() { return Match("*/", ir::Token::MultiLineCommentClose); }

  // Unary operators
  bool Lexer::Reference() { return Match('&', ir::Token::Reference); }
  bool Lexer::MutableReference() { return Match('*', ir::Token::MutableReference); }
  bool Lexer::Symbol() { return Match('$', ir::Token::Symbol); }
  bool Lexer::Copy() { return Match('@', ir::Token::Copy); }
  bool Lexer::Counted() { return Match('#', ir::Token::Counted); }
  bool Lexer::Positive() { return Match('+', ir::Token::Positive); }
  bool Lexer::Negative() { return Match('-', ir::Token::Negative); }
  bool Lexer::Increment() { return Match("++", ir::Token::Increment); }
  bool Lexer::Decrement() { return Match("--", ir::Token::Decrement); }
  bool Lexer::Not() { return Match('!', ir::Token::Not); }
  bool Lexer::Spread() { return Match("...", ir::Token::Spread); }
  bool Lexer::Move() { return Match('=', ir::Token::Move); }
  bool Lexer::BitwiseNot() { return Match('~', ir::Token::BitwiseNot); }

  bool Lexer::UnaryPrefixOperatorHelper() {
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

  bool Lexer::UnaryPrefixOperator() { return cursor.IsUnaryPrefixStart() && UnaryPrefixOperatorHelper(); }
  bool Lexer::UnaryPostfixOperator() { return cursor.IsUnaryPostfixStart() && UnaryPostfixOperatorHelper(); }

  bool Lexer::TrueLiteral() { return True(); }
  bool Lexer::FalseLiteral() { return False(); }
  bool Lexer::ThisLiteral() { return This(); }
  bool Lexer::ThatLiteral() { return That(); }
  bool Lexer::NullLiteral() { return Null(); }
  bool Lexer::UndefinedLiteral() { return Undefined(); }

  bool Lexer::Escape() {
    if (cursor.Done() || !cursor.Match('\\')) {
      return false; // not an escape start
    }

    if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

    const char c = cursor.Peek();
    auto hex_value = [](char ch) -> int {
      if (ch >= '0' && ch <= '9') return ch - '0';
      if (ch >= 'a' && ch <= 'f') return 10 + (ch - 'a');
      if (ch >= 'A' && ch <= 'F') return 10 + (ch - 'A');
      return -1;
    };

    auto emit_ascii = [&](char32_t cp){
      Emit(ir::Token::EscapeASCII);
      Push(cp);
      return true;
    };

    switch (c) {
      case 'n': cursor.Advance(1); return emit_ascii(U'\n');
      case 't': cursor.Advance(1); return emit_ascii(U'\t');
      case 'b': cursor.Advance(1); return emit_ascii(U'\b');
      case 'r': cursor.Advance(1); return emit_ascii(U'\r');
      case 'f': cursor.Advance(1); return emit_ascii(U'\f');
      case 'v': cursor.Advance(1); return emit_ascii(U'\v');
      case '\\': cursor.Advance(1); return emit_ascii(U'\\');
      case '"': cursor.Advance(1); return emit_ascii(U'"');
      case '\'': cursor.Advance(1); return emit_ascii(U'\'');
      case 'x': {
        // \xHH: exactly two hex digits
        cursor.Advance(1);
        if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

        uint32_t value = 0;
        for (int i = 0; i < 2; ++i) {
          if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

          int nib = hex_value(cursor.Peek());
          if (nib < 0) return Error(ir::Error::EscapeSequenceExpectedTwoHexDigits);

          value = (value << 4) | static_cast<uint32_t>(nib);
          cursor.Advance(1);
        }
        
        Emit(ir::Token::EscapeHex);
        Push(value);

        return true;
      }
      case 'u': {
        cursor.Advance(1);

        if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

        if (cursor.Peek() == '{') {
          cursor.Advance(1); // consume '{'
          uint32_t value = 0;
          size_t digits = 0;
          while (!cursor.Done()) {
            int nib = hex_value(cursor.Peek());
            if (nib < 0) break;
            if (digits == 6) break; // enforce max 6
            value = (value << 4) | static_cast<uint32_t>(nib);
            ++digits;
            cursor.Advance(1);
          }

          if (digits == 0) {
            return Error(ir::Error::EscapeSequenceExpectedOneOrMoreHexDigits);
          }

          // If next is still hex, we exceeded 6 digits
          if (!cursor.Done() && hex_value(cursor.Peek()) >= 0) {
            return Error(ir::Error::EscapeSequenceExpectedSixOrFewerHexDigits);
          }

          if (cursor.Done() || cursor.Peek() != '}') {
            return Error(ir::Error::EscapeSequenceExpectedClosingBrace);
          }

          cursor.Advance(1); // consume '}'

          if (value > 0x10FFFFu) {
            return Error(ir::Error::UnicodeCodePointOutOfRange);
          }

          if (value >= 0xD800u && value <= 0xDFFFu) {
            return Error(ir::Error::UnicodeSurrogateCodePointIsNotAValidScalarValue);
          }

          switch (digits) {
            case 1: Emit(ir::Token::EscapeUnicodeBraced1); break;
            case 2: Emit(ir::Token::EscapeUnicodeBraced2); break;
            case 3: Emit(ir::Token::EscapeUnicodeBraced3); break;
            case 4: Emit(ir::Token::EscapeUnicodeBraced4); break;
            case 5: Emit(ir::Token::EscapeUnicodeBraced5); break;
            case 6: Emit(ir::Token::EscapeUnicodeBraced6); break;
            default: break;
          }

          Push(value);
          return true;
        } else {
          // \uXXXX short form
          uint32_t value = 0;
          for (int i = 0; i < 4; ++i) {
            if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

            int nib = hex_value(cursor.Peek());
            if (nib < 0) return Error(ir::Error::EscapeSequenceExpectedFourHexDigits);

            value = (value << 4) | static_cast<uint32_t>(nib);
            cursor.Advance(1);
          }

          if (value >= 0xD800u && value <= 0xDFFFu) {
            return Error(ir::Error::UnicodeSurrogateCodePointIsNotAValidScalarValue);
          }

          Emit(ir::Token::EscapeUnicode);
          Push(value);
          return true;
        }
      }
      default: {
        // Default: treat as escaped ASCII character
        cursor.Advance(1);
        return emit_ascii(static_cast<unsigned char>(c));
      }
    }
  }

  bool Lexer::CharacterLiteral() {
    if (!CharOpen()) return false;

    if (cursor.Peek() == '\\') {
      if (!Escape()) {
        // Escape() already reported an error; for character literals, standardize message
        return Error(ir::Error::CharacterLiteralExpectedEscapeAfterBackslash);
      }
    } else if (cursor.Peek() == '\'') {
      return Error(ir::Error::CharacterLiteralExpectedContent);
    } else {
      if (IsBreak()) return Error(ir::Error::CharacterLiteralUnexpectedLineBreak);
      // Consume one raw code point and emit length token Characters1
      auto cp = cursor.CodePoint();
      Emit(ir::Token::Characters1);
      Push(cp);
    }

    if (!CharClose()) return Error(ir::Error::CharacterLiteralExpectedClosingSingleQuote);

    WS(); // Consume trailing whitespace
    return true;
  }

  bool Lexer::StringLiteral() {
    if (!StringOpen()) return false;

    uint32_t run_count = 0;
    while (true) {
      if (cursor.Done()) return Error(ir::Error::StringLiteralExpectedClosingDoubleQuote);
      if (IsBreak()) return Error(ir::Error::StringLiteralUnexpectedLineBreak);
      if (cursor.Peek() == '"') break;

      if (cursor.Peek() == '\\') {
        // flush current run before the escape
        EmitCharactersNibbles(run_count);
        run_count = 0;
        if (!Escape()) return Error(ir::Error::StringLiteralExpectedEscapeAfterBackslash);
        continue; // do not include escape in run; it implicitly represents Characters1
      }

      auto cp = cursor.CodePoint();
      Push(cp);
      ++run_count;
    }

    // flush trailing run
    EmitCharactersNibbles(run_count);

    if (!StringClose()) return Error(ir::Error::StringLiteralExpectedClosingDoubleQuote);

    WS(); // Consume trailing whitespace
    return true;
  }
  
  /****
   * Core idea
   * - The lexer is intentionally dumb about numbers. It does not try to interpret
   *   literals (no value folding, no float parsing, no signs consumed). It only
   *   streams what it sees into a reconstructable token sequence.
   * - All integer magnitude is emitted as hexadecimal nibbles using ir::Token::Digits0…DigitsF.
   *   These tokens represent the numeric value of a contiguous digit run, encoded as hex,
   *   and emitted in big-endian nibble order.
   *
   * Reconstructability and separators
   * - Source must be exactly reconstructable from tokens. That means every width-affecting
   *   character is preserved as its own token rather than being “interpreted away”.
   * - Underscores are always emitted as ir::Token::Underscore and terminate the current
   *   digit run. After an underscore, if more digits follow, that begins a new run.
   * - Leading zeros are preserved by emitting one ir::Token::Digits0 per zero at the
   *   front of each run. Because a 0 nibble has no effect on the decoded value, this
   *   preserves width without changing semantics.
   */

  // HexLiteral(): Parse 0x/0X-prefixed hex literals.
  // - Emits HexStart, then one Digits* token per hex digit (0-9, A-F), case-insensitive.
  // - Underscores are preserved as Underscore and terminate runs (but hex is already nibble-aligned).
  // - Accumulates the magnitude in base-16 and appends little-endian 32-bit limbs to `data`.
  // - Example: "0x00FF" → HexStart, Digits0, Digits0, DigitsF, DigitsF
  bool Lexer::HexLiteral() {
    cursor.Advance(2);
    Emit(ir::Token::HexStart);

    temp_le.clear();
    bool magnitude_started = false;

    auto accumulate_digit = [&](uint32_t digit) {
      if (!magnitude_started) {
        if (digit == 0) {
          return;
        }
        magnitude_started = true;
        temp_le.push_back(0);
      }

      uint64_t carry = digit;
      for (size_t i = 0; i < temp_le.size(); ++i) {
        uint64_t total = (static_cast<uint64_t>(temp_le[i]) << 4u) + carry;
        temp_le[i] = static_cast<uint32_t>(total & 0xFFFFFFFFu);
        carry = total >> 32u;
      }

      if (carry != 0) {
        temp_le.push_back(static_cast<uint32_t>(carry));
      }
    };

    while (!cursor.Done()) {
      char c = cursor.Peek();
      if (c == '_') {
        cursor.Advance();
        Emit(ir::Token::Underscore);
        continue;
      }

      uint32_t digit = 0;
      if (c >= '0' && c <= '9') {
        digit = static_cast<uint32_t>(c - '0');
      } else if (c >= 'a' && c <= 'f') {
        digit = static_cast<uint32_t>(10 + (c - 'a'));
      } else if (c >= 'A' && c <= 'F') {
        digit = static_cast<uint32_t>(10 + (c - 'A'));
      } else {
        break;
      }

      cursor.Advance();
      Emit(DigitToken(static_cast<uint8_t>(digit)));
      accumulate_digit(digit);
    }

    while (!temp_le.empty() && temp_le.back() == 0) {
      temp_le.pop_back();
    }

    if (!temp_le.empty()) {
      data.reserve(data.size() + temp_le.size());
      data.insert(data.end(), temp_le.begin(), temp_le.end());
    }

    temp_le.clear();

    WS(); // Consume trailing whitespace
    return true;
  }

  // OctalLiteral(): Parse 0o/0O-prefixed octal literals as a single pass bitstream.
  // - Emits OctalStart.
  // - Streams each octal digit's 3 bits MSB→LSB into a nibble accumulator; when 4 bits are filled,
  //   emits the corresponding Digits* token.
  // - Accumulates the magnitude in base-8 and appends the resulting limbs to `data`.
  // - A leading '0' when the accumulator is empty emits Digits0 to preserve width.
  // - An underscore flushes any partial nibble, is emitted as Underscore, and digit streaming continues.
  // - Stops at first non-octal/non-underscore.
  // - Example: "0o7"    → OctalStart, Digits7
  // - Example: "0o0077" → OctalStart, Digits0, Digits0, Digits3, DigitsF
  // - Example: "0o1234" → OctalStart, Digits2, Digits9, DigitsC
  bool Lexer::OctalLiteral() {
    cursor.Advance(2);
    Emit(ir::Token::OctalStart);

    uint8_t value = 0;
    uint8_t bits = 0; // bits collected toward current hex nibble

    temp_le.clear();
    bool magnitude_started = false;

    auto accumulate_digit = [&](uint32_t digit) {
      if (!magnitude_started) {
        if (digit == 0) {
          return;
        }
        magnitude_started = true;
        temp_le.push_back(0);
      }

      uint64_t carry = digit;
      for (size_t i = 0; i < temp_le.size(); ++i) {
        uint64_t total = static_cast<uint64_t>(temp_le[i]) * 8ull + carry;
        temp_le[i] = static_cast<uint32_t>(total & 0xFFFFFFFFu);
        carry = total >> 32u;
      }

      if (carry != 0) {
        temp_le.push_back(static_cast<uint32_t>(carry));
      }
    };

    auto flush_nibble = [&](){
      if (bits > 0) {
        Emit(DigitToken(value));
        value = 0;
        bits = 0;
      }
    };

    while (!cursor.Done()) {
      char c = cursor.Peek();
      if (c == '_') {
        flush_nibble();
        cursor.Advance();
        Emit(ir::Token::Underscore);
        continue;
      }
      if (!IsOctal()) break;

      uint8_t oct = static_cast<uint8_t>(c - '0');
      cursor.Advance();

      accumulate_digit(oct);

      if (bits == 0 && oct == 0) {
        Emit(ir::Token::Digits0);
        continue;
      }

      // push 3 bits MSB->LSB
      value = static_cast<uint8_t>((value << 1) | ((oct >> 2) & 1u));
      ++bits; if (bits == 4) { Emit(DigitToken(value)); value = 0; bits = 0; }

      value = static_cast<uint8_t>((value << 1) | ((oct >> 1) & 1u));
      ++bits; if (bits == 4) { Emit(DigitToken(value)); value = 0; bits = 0; }

      value = static_cast<uint8_t>((value << 1) | (oct & 1u));
      ++bits; if (bits == 4) { Emit(DigitToken(value)); value = 0; bits = 0; }
    }

    flush_nibble();

    while (!temp_le.empty() && temp_le.back() == 0) {
      temp_le.pop_back();
    }

    if (!temp_le.empty()) {
      data.reserve(data.size() + temp_le.size());
      data.insert(data.end(), temp_le.begin(), temp_le.end());
    }

    temp_le.clear();

    WS(); // Consume trailing whitespace
    return true;
  }

  // BinaryLiteral(): Parse 0b/0B-prefixed binary literals as a single pass bitstream.
  // - Emits BinaryStart.
  // - Packs bits into a 4-bit accumulator; when full, emits a Digits* token.
  // - Accumulates the magnitude in base-2 and appends the resulting limbs to `data`.
  // - A leading '0' when the accumulator is empty emits Digits0 to preserve width.
  // - Underscore flushes partial nibble and is emitted as Underscore.
  // - Stops at first non-binary/non-underscore.
  // - Example: "0b1111"      → BinaryStart, DigitsF
  // - Example: "0b0111_0011" → BinaryStart, Digits0, Digits7, Underscore, Digits0, Digits0, Digits3
  bool Lexer::BinaryLiteral() {
    cursor.Advance(2);
    Emit(ir::Token::BinaryStart);

    uint8_t value = 0;
    uint8_t bits = 0; // bits collected toward current hex nibble

    temp_le.clear();
    bool magnitude_started = false;

    auto accumulate_digit = [&](uint32_t digit) {
      if (!magnitude_started) {
        if (digit == 0) {
          return;
        }
        magnitude_started = true;
        temp_le.push_back(0);
      }

      uint64_t carry = digit;
      for (size_t i = 0; i < temp_le.size(); ++i) {
        uint64_t total = static_cast<uint64_t>(temp_le[i]) * 2ull + carry;
        temp_le[i] = static_cast<uint32_t>(total & 0xFFFFFFFFu);
        carry = total >> 32u;
      }

      if (carry != 0) {
        temp_le.push_back(static_cast<uint32_t>(carry));
      }
    };

    auto flush_nibble = [&](){
      if (bits > 0) {
        Emit(DigitToken(value));
        value = 0;
        bits = 0;
      }
    };

    while (!cursor.Done()) {
      char c = cursor.Peek();
      if (c == '_') {
        flush_nibble();
        cursor.Advance();
        Emit(ir::Token::Underscore);
        continue;
      }
      if (c != '0' && c != '1') break;

      cursor.Advance();

      uint32_t digit = static_cast<uint32_t>(c - '0');
      accumulate_digit(digit);

      if (bits == 0 && c == '0') {
        Emit(ir::Token::Digits0);
        continue;
      }

      value = static_cast<uint8_t>((value << 1) | static_cast<uint8_t>(c - '0'));
      ++bits;
      if (bits == 4) {
        Emit(DigitToken(value));
        value = 0;
        bits = 0;
      }
    }

    flush_nibble();

    while (!temp_le.empty() && temp_le.back() == 0) {
      temp_le.pop_back();
    }

    if (!temp_le.empty()) {
      data.reserve(data.size() + temp_le.size());
      data.insert(data.end(), temp_le.begin(), temp_le.end());
    }

    temp_le.clear();

    WS(); // Consume trailing whitespace
    return true;
  }

  // DecimalLiteral(): Parse base-10 literals, preserving textual fidelity while emitting mantissa limbs.
  // - Streams digits into nibble tokens (Digits0..DigitsF) to reconstruct the literal losslessly.
  // - Accumulates the mantissa in base-2^32 little-endian limbs within temp_le.
  // - Emits ir::Token::Mantissa once per stored limb and appends the limb to `data`.
  // - If a dot or exponent modifier is present, appends a 64-bit signed exponent as the final two limbs.
  // - Example: "100"    → Digits6, Digits4
  // - Example: "1_100"  → Digits1, Underscore, Digits6, Digits4
  // - Example: "42.7"   → Digits2, DigitsA, Dot, Digits7
  // - Example: "42.007" → Digits2, DigitsA, Dot, Digits0, Digits0, Digits7
  bool Lexer::DecimalLiteral() {
    bool consumed = false;
    temp_le.clear();

    enum class Stage { Integer, Fraction, Exponent };
    Stage stage = Stage::Integer;

    bool mantissa_nonzero = false;
    size_t fractional_digits = 0;

    bool seen_fraction = false;
    bool seen_exponent = false;
    bool exponent_negative = false;
    uint64_t exponent_abs = 0;
    bool exponent_overflow = false;

    auto multiply_add_digit = [&](uint32_t digit) {
      if (!mantissa_nonzero) {
        if (digit == 0) {
          return;
        }
        mantissa_nonzero = true;
        temp_le.push_back(0);
      }

      if (!mantissa_nonzero) {
        return;
      }

      uint64_t carry = digit;
      for (size_t i = 0; i < temp_le.size(); ++i) {
        uint64_t value = static_cast<uint64_t>(temp_le[i]) * 10ull + carry;
        temp_le[i] = static_cast<uint32_t>(value & 0xFFFFFFFFu);
        carry = value >> 32u;
      }

      if (carry != 0) {
        temp_le.push_back(static_cast<uint32_t>(carry));
      }
    };

    auto emit_run_tokens = [&](std::string_view run) {
      if (run.empty()) return;

      size_t i = 0;
      while (i < run.size() && run[i] == '0') {
        Emit(ir::Token::Digits0);
        ++i;
      }

      if (i == run.size()) {
        return;
      }

      hex_le.clear();
      hex_le.push_back(0);
      for (; i < run.size(); ++i) {
        uint32_t carry = static_cast<uint32_t>(run[i] - '0');
        for (size_t k = 0; k < hex_le.size(); ++k) {
          uint32_t total = static_cast<uint32_t>(hex_le[k]) * 10u + carry;
          hex_le[k] = static_cast<uint8_t>(total & 0xFu);
          carry = total >> 4;
        }
        while (carry > 0) {
          hex_le.push_back(static_cast<uint8_t>(carry & 0xFu));
          carry >>= 4;
        }
      }

      while (hex_le.size() > 1 && hex_le.back() == 0) {
        hex_le.pop_back();
      }

      for (auto it = hex_le.rbegin(); it != hex_le.rend(); ++it) {
        Emit(DigitToken(*it));
      }
    };

    auto process_mantissa_run = [&](std::string_view run, bool fractional) {
      for (char ch : run) {
        uint32_t digit = static_cast<uint32_t>(ch - '0');
        if (fractional) {
          fractional_digits += 1;
        }
        multiply_add_digit(digit);
      }
    };

    auto process_exponent_run = [&](std::string_view run) {
      for (char ch : run) {
        uint32_t digit = static_cast<uint32_t>(ch - '0');
        if (!exponent_overflow) {
          if (exponent_abs > (std::numeric_limits<uint64_t>::max() - digit) / 10ull) {
            exponent_overflow = true;
          } else {
            exponent_abs = exponent_abs * 10ull + digit;
          }
        }
      }
    };

    while (!cursor.Done()) {
      if (!IsDigit()) break;

      auto run_begin = cursor.cbegin();
      while (!cursor.Done() && IsDigit()) cursor.Advance();
      auto run_end = cursor.cbegin();

      if (run_begin != run_end) {
        std::string_view run{&*run_begin, static_cast<size_t>(std::distance(run_begin, run_end))};

        emit_run_tokens(run);

        switch (stage) {
          case Stage::Integer: process_mantissa_run(run, false); break;
          case Stage::Fraction: process_mantissa_run(run, true); break;
          case Stage::Exponent: process_exponent_run(run); break;
        }

        consumed = true;
      }

      if (cursor.Done()) break;
      char c = cursor.Peek();

      if (c == '_') {
        cursor.Advance();
        Emit(ir::Token::Underscore);
        continue;
      }

      if (stage != Stage::Exponent && c == '.') {
        seen_fraction = true;
        stage = Stage::Fraction;
        cursor.Advance();
        Emit(ir::Token::Dot);
        continue;
      }

      if (c == 'e' || c == 'E') {
        seen_exponent = true;
        stage = Stage::Exponent;
        cursor.Advance();
        Emit(ir::Token::Exponent);
        if (!cursor.Done()) {
          if (cursor.Peek() == '+') {
            cursor.Advance();
            Emit(ir::Token::Plus);
          } else if (cursor.Peek() == '-') {
            cursor.Advance();
            Emit(ir::Token::Minus);
            exponent_negative = true;
          }
        }
        continue;
      }
      break;
    }

    if (!consumed) {
      temp_le.clear();
      return false;
    }

    while (!temp_le.empty() && temp_le.back() == 0) {
      temp_le.pop_back();
    }

    uint32_t mantissa_limbs = static_cast<uint32_t>(temp_le.size());
    bool need_exponent_limbs = seen_fraction || seen_exponent;

    uint32_t additional = mantissa_limbs + (need_exponent_limbs ? 2u : 0u);
    if (additional > 0) {
      data.reserve(data.size() + additional);
    }

    for (uint32_t limb_index = 0; limb_index < mantissa_limbs; ++limb_index) {
      Emit(ir::Token::Mantissa);
      data.push_back(temp_le[limb_index]);
    }

    if (need_exponent_limbs) {
      bool overflow = exponent_overflow;
      bool overflow_negative = exponent_negative;
      uint64_t limit = exponent_negative
        ? static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1ull
        : static_cast<uint64_t>(std::numeric_limits<int64_t>::max());

      if (!overflow && exponent_abs > limit) {
        overflow = true;
        overflow_negative = exponent_negative;
      }

      int64_t exponent_total = 0;
      if (!overflow) {
        if (exponent_negative) {
          if (exponent_abs == limit) {
            exponent_total = std::numeric_limits<int64_t>::min();
          } else {
            exponent_total = -static_cast<int64_t>(exponent_abs);
          }
        } else {
          exponent_total = static_cast<int64_t>(exponent_abs);
        }

        if (fractional_digits > static_cast<size_t>(std::numeric_limits<int64_t>::max())) {
          overflow = true;
          overflow_negative = true;
        } else if (fractional_digits > 0) {
          int64_t fractional_shift = static_cast<int64_t>(fractional_digits);
          if (exponent_total < std::numeric_limits<int64_t>::min() + fractional_shift) {
            overflow = true;
            overflow_negative = true;
          } else {
            exponent_total -= fractional_shift;
          }
        }
      }

      if (overflow) {
        // TODO: Surface an overflow error during decimal lexing.
        exponent_total = overflow_negative
          ? std::numeric_limits<int64_t>::min()
          : std::numeric_limits<int64_t>::max();
      }

      uint64_t exponent_bits = static_cast<uint64_t>(exponent_total);
      data.push_back(static_cast<uint32_t>(exponent_bits & 0xFFFFFFFFu));
      data.push_back(static_cast<uint32_t>(exponent_bits >> 32u));
    }

    temp_le.clear();

    WS(); // Consume trailing whitespace
    return true;
  }

  // NumberLiteral(): Dispatch entry for numeric literals.
  // - Recognizes number starts as digits only (no leading '+'/'-'; those are unary operators).
  // - If the run begins with 0x/0X, 0o/0O, or 0b/0B, dispatch to HexLiteral/OctalLiteral/BinaryLiteral respectively.
  // - Otherwise, parse as DecimalLiteral().
  bool Lexer::NumberLiteral() {
    if (!cursor.IsNumberStart()) {
      return false;
    }

    if (cursor.Peek() == '0') {
      switch (cursor.Peek(1)) {
        case 'x':
        case 'X': return HexLiteral();
        case 'o':
        case 'O': return OctalLiteral();
        case 'b':
        case 'B': return BinaryLiteral();
        default: break;
      }
    }

    return DecimalLiteral();
  }

  // Identifier parsing (redesigned): same structure as String content without open/close markers.
  // Emits runs of Characters* nibble tokens for contiguous non-escape code points and Escape* tokens
  // for escapes. Appends every consumed code point to `data`.
  bool Lexer::Identifier() {
    // Must begin at a possible identifier start (ASCII quick check, escape, or non-ASCII)
    if (!IsPossibleIdentifierStart()) return false;

    bool first = true;
    bool consumed_any = false;
    uint32_t run_count = 0;

    while (!cursor.Done()) {
      if (cursor.Peek() == '\\') {
        // Flush any current run, then consume escape.
        if (run_count > 0) { EmitCharactersNibbles(run_count); run_count = 0; }
        if (!Escape()) return false; // escape already reports its own errors
        consumed_any = true;
        first = false;
        continue;
      }

      // Stop on whitespace or line break
      if (IsSpace() || IsBreak()) break;

      if (!IsASCII()) {
        // Validate Unicode code point using Unicode tables.
        auto it = cursor.cbegin();
        char32_t cp = cursor.CodePoint(); // consumes
        bool ok = first
          ? compiler::text::Unicode::IsIdentifierStart(cp) || cp == U'_'
          : compiler::text::Unicode::IsIdentifierContinue(cp) || cp == U'_';
        if (!ok) {
          // Not a valid identifier character; undo and stop.
          cursor.Retreat(it);
          break;
        }
        Push(cp);
        ++run_count;
        consumed_any = true;
        first = false;
        continue;
      }

      // ASCII branch: use fast bitsets for start/continue
      if (first) {
        if (!IsIdentStart()) break;
      } else {
        if (!IsIdent()) break;
      }

      char32_t cp = cursor.CodePoint(); // ASCII -> 1 byte
      Push(cp);
      ++run_count;
      consumed_any = true;
      first = false;
    }

    if (run_count > 0) EmitCharactersNibbles(run_count);

    WS(); // Consume trailing whitespace
    return consumed_any;
  }

  bool Lexer::IdentifierOrArrowFunction() {
    size_t start = tokens.size(); // Save the position before matching Identifier

    if (!Identifier()) {
      return false;
    }

    // Check for inline function arrow '=>'
    if (!Arrow()) return true;

    if (Expression()) {
      // A special zero width marker to tell the parser the identifier is an arrow function parameter
      tokens.insert(tokens.begin() + start, ir::Token::ArrowHead);

      return true;
    } else {
      return Error(ir::Error::ArrowFunctionExpectedExpression);
    }
  }

  // Template string: like String content but allows line breaks and embedded expressions.
  // - Escapes, line breaks, and expression boundaries terminate the Characters* run.
  // - Emits explicit newline tokens (LineFeed, CarriageReturnLineFeed, CarriageReturn, LineSeparator, ParagraphSeparator).
  // - Appends every code point (including newlines) to `data`.
  // - Uses error codes (no string diagnostics).
  bool Lexer::TemplateStringLiteral() {
    if (!TemplateStringOpen()) return false;

    uint32_t run_count = 0;
    while (!cursor.Done()) {
      // Closing backtick
      if (cursor.Peek() == '`') {
        EmitCharactersNibbles(run_count);
        run_count = 0;
        TemplateStringClose();

        WS(); // Consume trailing whitespace
        return true;
      }

      // Embedded expression
      if (cursor.Peek() == '{') {
        EmitCharactersNibbles(run_count);
        run_count = 0;
        if (!TemplateStringExpressionOpen()) return Error(ir::Error::TemplateStringLiteralExpectedExpression);
        if (!Expression()) return Error(ir::Error::TemplateStringLiteralExpectedExpression);
        if (!TemplateStringExpressionClose()) return Error(ir::Error::TemplateStringLiteralExpectedClosingBrace);
        continue;
      }

      // Escape sequence
      if (cursor.Peek() == '\\') {
        EmitCharactersNibbles(run_count);
        run_count = 0;
        if (!Escape()) return false;
        continue;
      }

      // Decode one code point to determine if it's a line break
      char32_t cp = cursor.CodePoint();
      if (cp == U'\n' || cp == U'\r' || cp == U'\u2028' || cp == U'\u2029') {
        // Line breaks inside template strings are explicit tokens and terminate runs
        EmitCharactersNibbles(run_count);
        run_count = 0;

        if (cp == U'\r') {
          if (cursor.Peek() == '\n') {
            cursor.Advance(1); // consume LF to make CRLF
            Emit(ir::Token::CarriageReturnLineFeed);
            data.push_back(static_cast<uint32_t>(U'\r'));
            data.push_back(static_cast<uint32_t>(U'\n'));
          } else {
            Emit(ir::Token::CarriageReturn);
            data.push_back(static_cast<uint32_t>(U'\r'));
          }
        } else if (cp == U'\n') {
          Emit(ir::Token::LineFeed);
          data.push_back(static_cast<uint32_t>(U'\n'));
        } else if (cp == U'\u2028') { // Line Separator
          Emit(ir::Token::LineSeparator);
          data.push_back(static_cast<uint32_t>(U'\u2028'));
        } else { // U'\u2029' Paragraph Separator
          Emit(ir::Token::ParagraphSeparator);
          data.push_back(static_cast<uint32_t>(U'\u2029'));
        }
        continue;
      }

      // Regular code point: accumulate into current run
      data.push_back(static_cast<uint32_t>(cp));
      ++run_count;
    }

    // Reached EOF without a closing backtick
    return Error(ir::Error::TemplateStringLiteralExpectedClosingBacktick);
  }

  bool Lexer::BinaryOperator(bool in_enum, bool in_type) {
    if (!IsBinaryStart()) return false;

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
          case '=': {
            switch (cursor.Peek(2)) {
              case '=': return EmitAndAdvance(ir::Token::AssertEqual, 3);
              default:  return EmitAndAdvance(ir::Token::Equal, 2);
            }
          }
          default: {
            // If inside a type annotation, '=' is not a binary operator,
            // it marks the end of the type annotation and start of the initializer.
            if (in_type) return false;

            return EmitAndAdvance(ir::Token::Assign, 1);
          }
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
        // If we're inside an enum we cannot match anything starting with '>' because
        // that would conflict with the closing '>' of the enum declaration.
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
          case '>': return EmitAndAdvance(ir::Token::Pipeline, 2);
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

  bool Lexer::SelectorOperator() {
    if (!IsBinaryStart()) return false;

    switch (cursor.Peek()) {
      case '?': {
        switch (cursor.Peek(1)) {
          case '.': return EmitAndAdvance(ir::Token::OptionalMemberAccess, 2);
          case ':': return EmitAndAdvance(ir::Token::MemberAccessStaticOptional, 2);
          default:  return false;
        }
      }
      case '.': {
        return EmitAndAdvance(ir::Token::MemberAccess, 1);
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

  bool Lexer::ParameterDeclarationList() { return ZeroOrMore([&]{ return ParameterDeclaration(); }, [&]{ return Comma(); }); }

  bool Lexer::Parameters() { return ParameterOpen() && ParameterDeclarationList() && ParameterClose(); }
  bool Lexer::Captures() { return CaptureOpen() && ParameterDeclarationList() && CaptureClose(); }

  bool Lexer::ArrowFunction() {
    if (!Arrow()) return false;

    if (Expression()) {
      return true;
    } else {
      return Error(ir::Error::ArrowFunctionExpectedExpression);
    }
  }

  bool Lexer::Function() {
    return Try([&]{
      if (Parameters()) return (Captures() || true) && FunctionBody() || ArrowFunction();
      else return Captures() && (Parameters() || true) && FunctionBody() || ArrowFunction();
    });
  }

  bool Lexer::EnumLiteral() {
    if (!EnumOpen()) return false;

    // Parse zero or more selectors separated by commas; trailing comma allowed.
    ZeroOrMore([&]{ return Selector(); }, [&]{ return Comma(); });

    if (!EnumClose()) return Error(ir::Error::EnumExpectedClosingAngleBracket);
    return true;
  }

  bool Lexer::ObjectLiteral() {
    if (!ObjectOpen()) return false;

    // Parse zero or more declarations separated by commas; trailing comma allowed.
    ZeroOrMore([&]{ return Declaration(); }, [&]{ return Comma(); });

    if (!ObjectClose()) return Error(ir::Error::ObjectExpectedClosingCurlyBrace);
    return true;
  }

  bool Lexer::ArrayLiteral() {
    if (!ArrayOpen()) return false;

    // Parse zero or more expressions separated by commas; trailing comma allowed.
    ZeroOrMore([&]{ return Expression(); }, [&]{ return Comma(); });

    if (!ArrayClose()) return Error(ir::Error::ArrayExpectedClosingBracket);
    return true;
  }

  bool Lexer::TupleLiteral() {
    if (!TupleOpen()) return false;

    // Parse zero or more expressions separated by commas; trailing comma allowed.
    ZeroOrMore([&]{ return Expression(); }, [&]{ return Comma(); });

    if (!TupleClose()) return Error(ir::Error::TupleExpectedClosingParenthesis);
    return true;
  }

  bool Lexer::CallablePrefixLiteralHelper() {
    switch (cursor.Peek()) {
      case '\'': return CharacterLiteral();
      case '"': return StringLiteral();
      case '`': return TemplateStringLiteral();
      default: return Number();
    }
  }

  bool Lexer::CallablePostfixLiteralHelper() {
    switch (cursor.Peek()) {
      case '\'': return CharacterLiteral();
      case '"': return StringLiteral();
      case '`': return TemplateStringLiteral();
      case '(': return ParameterFunction() || TupleLiteral();
      case '[': return CaptureFunction() || ArrayLiteral();
      case '{': return ObjectLiteral();
      case '<': return EnumLiteral();
      default: return false;
    }
  }

  bool Lexer::CallablePrefixLiteral() { return cursor.IsCallablePrefixStart() && CallablePrefixLiteralHelper(); }
  bool Lexer::CallablePostfixLiteral() { return cursor.IsCallablePostfixStart() && CallablePostfixLiteralHelper(); }

  bool Lexer::EscapePercent() {
    if (!cursor.Match('%')) return false;

    Emit(ir::Token::EscapePercent);

    // Expect two hex digits
    char high = cursor.Peek();
    char low = cursor.Peek(1);
    if (!IsHexDigit(high) || !IsHexDigit(low)) {
      return Error(ir::Error::EscapeInvalidPercentEncoding);
    }

    cursor.Advance(2);

    uint8_t byte = static_cast<uint8_t>((HexValue(high) << 4) | HexValue(low));
    Push(static_cast<uint32_t>(byte));

    return true;
  }

  bool Lexer::EscapeURL() {
    switch (cursor.Peek()) {
      case '%': return EscapePercent();
      case '\\': return Escape();
      default: return false;
    }
  }

  bool Lexer::Scheme() {
    return Try([&]{
      if (!IsAlpha()) return false; // must begin with a letter
      Emit(ir::Token::Scheme);

      if (!CaptureCharacters([&]{ return IsURLScheme(); }, [&]{ return EscapeURL(); })) return false; // require at least one protocol char

      if (!cursor.Match(':')) return false; // This is mandatory

      return true;
    });
  }

  bool Lexer::Userinfo() {
    return Try([&]{
      // Username must start with an authority character
      // TODO: Should probably allow escapes as the first character too
      if (!IsURLAuthority()) return false;

      // Emit Username token
      Emit(ir::Token::Username);

      // Capture username characters (allow escapes)
      CaptureCharacters([&]{ return IsURLAuthority(); }, [&]{ return EscapeURL(); });

      if (cursor.Match(':')) {
        // Password is present; emit Password token and capture until '@'
        Emit(ir::Token::Password);

        // Require at least one password character
        if (!CaptureCharacters([&]{ return IsURLAuthority(); }, [&]{ return EscapeURL(); })) return false;

        if (!cursor.Match('@')) return false; // Must terminate with '@'
        return true;
      } else if (cursor.Match('@')) {
        // No password present, username terminated by '@'
        return true;
      }

      // Neither ':' nor '@' found: rollback
      return false;
    });
  }

  bool Lexer::Port() {
    return Try([&]{
      if (!cursor.Match(':')) return false;
      Emit(ir::Token::Port);

      // Capture digits; allow escapes though unlikely
      // require at least one digit
      return CaptureCharacters([&]{ return IsDigit(); }, [&]{ return EscapeURL(); }); 
    });
  }

  bool Lexer::Authority() {
    if (!cursor.Match("//")) return false;
    Emit(ir::Token::Authority);

    // Optional userinfo: emits Username and optional Password tokens
    Userinfo();

    if (!Host()) return Error(ir::Error::SpecifierExpectedHost);

    Port(); // optional, but only after host
    return true;
  }

  bool Lexer::Hostname() {
    return Try([&]{
      // Hostname must start with alnum
      if (!IsAlNum()) return false;
      Emit(ir::Token::Hostname);
      // Capture hostname run: alnum, '-', '.'
      CaptureCharacters([&]{
        char c = cursor.Peek();
        return IsAlNum() || c == '-' || c == '.'; }, [&]{ return EscapeURL(); });
      return true;
    });
  }

  bool Lexer::IPv4() {
    return Try([&]{
      if (!IsDigit()) return false;
      Emit(ir::Token::IPv4);
      bool saw_dot = false;
      bool last_was_dot = false;

      // Capture digits and dots; require at least one dot, and cannot end with dot
      if (!CaptureCharacters([&]{
        char c = cursor.Peek();
        if (c == '.') { saw_dot = true; last_was_dot = true; return true; }
        if (IsDigit()) { last_was_dot = false; return true; }
        return false; }, [&]{ return EscapeURL(); })) return false;

      if (!saw_dot) return false;
      if (last_was_dot) return false;
      return true;
    });
  }

  bool Lexer::IPv6() {
    return Try([&]{
      if (!cursor.Match('[')) return false;

      Emit(ir::Token::IPv6);
      // Require at least one interior char (hex, ':', '.')
      if (!CaptureCharacters([&]{
        char c = cursor.Peek();
        return IsHex(c) || c == ':' || c == '.'; }, [&]{ return EscapeURL(); })) return false;

      if (!cursor.Match(']')) return false; // closing bracket not emitted
      return true;
    });
  }

  bool Lexer::Host() {
    return IPv6() || IPv4() || Hostname();
  }

  bool Lexer::PathSegments() {
    while (!cursor.Done() && Slash()) {
      // Capture sequences of `IsURLSegment` characters
      CaptureCharacters([&]{ return IsURLSegment(); }, [&]{ return EscapeURL(); });
    }

    return true; // path can be empty
  }

  bool Lexer::PathAbsolute() {
    if (!Slash()) return false;

    if (!CaptureCharacters([&]{ return IsURLSegment(); }, [&]{ return EscapeURL(); })) {
      return true; // Stop early if we have no segment
    }

    return PathSegments();
  }

  bool Lexer::PathRootless() {
    if (!CaptureCharacters([&]{ return IsURLSegment(); }, [&]{ return EscapeURL(); })) {
      return false; // Stop early if we have no segment
    }

    return PathSegments();
  }

  bool Lexer::PathNoScheme() {
    if (!CaptureCharacters([&]{ return IsURLSegmentNC(); }, [&]{ return EscapeURL(); })) {
      return false; // Stop early if we have no segment
    }

    return PathSegments();
  }

  bool Lexer::Query() {
    return Try([&]{
      if (!cursor.Match('?')) return false;
      // key[=value] (& key[=value])*
      // First key
      Emit(ir::Token::QueryKey);
      CaptureCharacters([&]{ return IsURLSearch(); }, [&]{ return EscapeURL(); });

      if (cursor.Match('=')) {
        Emit(ir::Token::QueryValue);
        CaptureCharacters([&]{ return IsURLSearch(); }, [&]{ return EscapeURL(); });
      }

      // Additional pairs
      while (cursor.Match('&')) {
        Emit(ir::Token::QueryKey);
        CaptureCharacters([&]{ return IsURLSearch(); }, [&]{ return EscapeURL(); });

        if (cursor.Match('=')) {
          Emit(ir::Token::QueryValue);
          CaptureCharacters([&]{ return IsURLSearch(); }, [&]{ return EscapeURL(); });
        }
      }

      return true;
    });
  }

  bool Lexer::Fragment() {
    if (!cursor.Match('#')) return false;

    Emit(ir::Token::Fragment);

    CaptureCharacters([&]{ return IsURLFragment(); }, [&]{ return EscapeURL(); });
    return true; // Fragment characters are optional
  }

  bool Lexer::Specifier() {
    if (!StringOpen()) return false;

    if (Scheme()) {
      // hier-part
      if (cursor.Check("//")) {
        if (!Authority()) return Error(ir::Error::SpecifierExpectedAuthority);
        if (!PathSegments()) return Error(ir::Error::SpecifierBadPathAfterAuthority);
      } else if (cursor.Check("/")) {
        // Could be absolute (starts with '/') or empty (just "/?" isn't empty).
        if (!PathAbsolute()) return Error(ir::Error::SpecifierBadAbsolutePath);
      } else {
        // rootless or empty
        if (!PathRootless()) return Error(ir::Error::SpecifierBadRootlessOrEmpty);
      }
    } else {
      // relative-part
      if (cursor.Check("//")) {
        if (!Authority()) return Error(ir::Error::SpecifierExpectedAuthority);
        if (!PathSegments()) return Error(ir::Error::SpecifierBadPathAfterAuthority);
      } else if (cursor.Check("/")) {
        if (!PathAbsolute()) return Error(ir::Error::SpecifierBadAbsolutePath);
      } else {
        // noscheme or empty (empty = zero pchars)
        if (!PathNoScheme()) return false; // not a specifier if nothing matched
      }
    }

    // Optional tail
    Query();    // consumes leading '?' if present; no error if absent
    Fragment(); // consumes leading '#' if present; no error if absent

    if (!StringClose()) return Error(ir::Error::SpecifierLiteralExpectedClosingDoubleQuote);

    WS(); // Consume trailing whitespace
    return true;
  }

  bool Lexer::TermShortcut() {
    switch (cursor.Peek()) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': return NumberLiteral();
      case 't': return TrueLiteral() || ThisLiteral() || ThatLiteral();
      case 'f': return FalseLiteral();
      case 'n': return NullLiteral();
      case 'u': return UndefinedLiteral();
      case '\'': return CharacterLiteral();
      case '"': return StringLiteral();
      case '`': return TemplateStringLiteral();
      case '(': return ParameterFunction() || TupleLiteral();
      case '[': return CaptureFunction() || ArrayLiteral();
      case '<': return EnumLiteral();
      case '{': return ObjectLiteral();
      default: return false;
    }
  }

  bool Lexer::Term() { return (cursor.IsValueStart() && TermShortcut()) || Identifier(); }

  bool Lexer::Selector() {
    if (UnaryPrefixOperator()) {
      if (!Selector()) {
        return Error(ir::Error::UnaryPrefixOperatorExpectedSelector);
      } else {
        return true;
      }
    }

    if (!Term()) return false;

    while (CallablePostfixLiteral()) {}

    if (SelectorOperator()) {
      if (!Selector()) {
        return Error(ir::Error::SelectorOperatorExpectedSelector);
      }
    }

    return true;
  }

  bool Lexer::Expression(bool in_enum, bool in_type) {
    if (UnaryPrefixOperator()) {
      if (!Expression(in_enum, in_type)) {
        return Error(ir::Error::UnaryPrefixOperatorExpectedExpression);
      } else {
        return true;
      }
    }

    if (!Term()) return false;

    while (CallablePostfixLiteral()) {}

    if (BinaryOperator(in_enum, in_type)) {
      if (!Expression(in_enum, in_type)) {
        return Error(ir::Error::BinaryOperatorExpectedExpression);
      }
    }

    return true;
  }

  bool Lexer::Declaration() {
    if (Let() || Const() || Mut()) {
      // One of these is optional in a plain declaration, only required in a declaration statement
    }

    if (!Identifier()) return Error(ir::Error::DeclarationExpectedIdentifier);

    if (TypeStart()) {
      if (!Expression(false, true)) {
        return Error(ir::Error::DeclarationExpectedTypeExpressionAfterColon);
      }
    }

    if (Match('=')) {
      Emit(ir::Token::Assign);
      WS();

      if (!Expression()) return Error(ir::Error::DeclarationExpectedInitializerExpressionAfterEquals);
    }

    return true;
  }

  bool Lexer::DeclarationStatement() {
    if (!(Let() || Const() || Mut())) return false;

    if (!Identifier()) return Error(ir::Error::DeclarationStatementExpectedIdentifier);

    if (TypeStart()) {
      if (!Expression(false, true)) {
        return Error(ir::Error::DeclarationStatementExpectedTypeExpressionAfterColon);
      }
    }

    if (Match('=')) {
      Emit(ir::Token::Assign);
      WS();

      if (!Expression()) return Error(ir::Error::DeclarationStatementExpectedInitializerExpressionAfterEquals);
    }

    if (!Semicolon()) return Error(ir::Error::DeclarationStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::ImportStatement() {
    if (!Import()) return false;

    Async();

    if (!Identifier()) return Error(ir::Error::ImportExpectedIdentifier);

    if (!From()) return Error(ir::Error::ImportExpectedFromKeyword);

    if (!Specifier()) return Error(ir::Error::ImportExpectedSpecifier);

    if (In()) {
      if (!Identifier()) return Error(ir::Error::ImportExpectedIdentifierAfterIn);
    }

    if (!Semicolon()) return Error(ir::Error::ImportExpectedSemicolon);
    return true;
  }

  bool Lexer::RegisterStatement() {
    if (!Register()) return false;

    if (!Identifier()) return Error(ir::Error::RegisterExpectedIdentifier);

    if (!From()) return Error(ir::Error::RegisterExpectedFromKeyword);

    if (!Specifier()) return Error(ir::Error::RegisterExpectedSpecifier);

    if (With()) {
      if (!Identifier()) return Error(ir::Error::RegisterExpectedIdentifierAfterWith);

      while (Comma()) {
        if (cursor.Peek() == ';') break; // trailing comma permitted
        if (!Identifier()) return Error(ir::Error::RegisterExpectedIdentifierAfterWith);
      }
    }

    if (!Semicolon()) return Error(ir::Error::RegisterExpectedSemicolon);
    return true;
  }

  bool Lexer::BreakStatement() {
    if (!Break()) return false;

    while (!parser.Done() && Break()) {} // Warble permits stacking `break` keywords

    if (!Semicolon()) return Error(ir::Error::BreakStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::ContinueStatement() {
    if (!Continue()) return false;

    while (!parser.Done() && Continue()) {} // Warble permits stacking `continue` keywords

    if (!Semicolon()) return Error(ir::Error::ContinueStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::ReturnStatement() {
    if (!Return()) return false;
    Expression(); // optional

    if (!Semicolon()) return Error(ir::Error::ReturnStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::YieldStatement() {
    if (!Yield()) return false;
    Expression(); // optional

    if (!Semicolon()) return Error(ir::Error::YieldStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::DoStatement() {
    if (!Do()) return false;

    if (!Block()) return Error(ir::Error::DoExpectedScopeBlock);

    return true;
  }

  bool Lexer::IfStatement() {
    if (!If()) return false;

    if (!Condition()) return Error(ir::Error::IfStatementExpectedCondition);

    if (!Block()) {
      bool matched = false;

      while (!parser.Done() && (IsPattern() || HasPattern() || FromPattern())) {
        matched = true;
      }

      if (!matched) return Error(ir::Error::IfStatementExpectedScopeBlockOrPatternArms);
    }

    if (Else()) {
      if (!IfStatement()) {
        if (!Block()) return Error(ir::Error::ElseStatementExpectedScopeBlock);
      }
    }

    return true;
  }

  bool Lexer::IsPattern() {
    if (!Is()) return false;

    if (!Condition()) return Error(ir::Error::IsPatternExpectedCondition);
    if (!Block()) return Error(ir::Error::IsPatternExpectedScopeBlock);

    return true;
  }

  bool Lexer::HasPattern() {
    if (!Has()) return false;

    if (!Condition()) return Error(ir::Error::HasPatternExpectedCondition);
    if (!Block()) return Error(ir::Error::HasPatternExpectedScopeBlock);

    return true;
  }

  bool Lexer::FromPattern() {
    if (!From()) return false;

    if (!Condition()) return Error(ir::Error::FromPatternExpectedCondition);
    if (!Block()) return Error(ir::Error::FromPatternExpectedScopeBlock);

    return true;
  }

  bool Lexer::WhileStatement() {
    if (!While()) return false;

    if (!Condition()) return Error(ir::Error::WhileExpectedCondition);
    if (!Block()) return Error(ir::Error::WhileExpectedScopeBlock);

    return true;
  }

  bool Lexer::RepeatStatement() {
    if (!Repeat()) return false;

    if (!Block()) return Error(ir::Error::RepeatExpectedScopeBlock);

    if (While()) {
      if (!Condition()) return Error(ir::Error::RepeatExpectedWhileCondition);
    }

    return true;
  }

  bool Lexer::ForStatement() {
    if (!For()) return false;

    if (!ConditionOpen()) return Error(ir::Error::ForExpectedConditionOpen);

    if (!Declaration()) return Error(ir::Error::ForExpectedBinding);

    if (!In()) return Error(ir::Error::ForExpectedInKeyword);

    if (!Expression()) return Error(ir::Error::ForExpectedExpression);

    if (!ConditionClose()) return Error(ir::Error::ForExpectedConditionClose);

    if (!Block()) return Error(ir::Error::ForExpectedScopeBlock);

    return true;
  }

  bool Lexer::StatementShortcut() {
    switch (cursor.Peek()) {
      case 'b': return BreakStatement();
      case 'c': return ContinueStatement();
      case 'd': return DoStatement();
      case 'f': return ForStatement();
      case 'i': return IfStatement() || ImportStatement();
      case 'r': return ReturnStatement() || RegisterStatement() || RepeatStatement();
      case 'w': return WhileStatement();
      case 'y': return YieldStatement();
      case ';': return Semicolon();
      default: return false;
    }
  }

  bool Lexer::Statement() { return (cursor.IsStatementStart() && StatementShortcut()) || Declaration() || Expression(); }

  bool Lexer::Condition() {
    if (!ConditionOpen()) return false;

    if (!Expression()) return Error(ir::Error::ConditionExpectedExpression);

    if (!ConditionClose()) return Error(ir::Error::ConditionExpectedClosingParenthesis);

    return true;
  }

  bool Lexer::Block() {
    if (!ScopeOpen()) return false;

    StatementList(); // Zero or more statements

    if (!ScopeClose()) return Error(ir::Error::BlockExpectedClosingCurlyBrace);

    return true;
  }

  bool Lexer::StatementList() {
    while (!cursor.Done()) {
      if (!Statement()) break;
    }

    return true;
  }

  bool Lexer::FunctionStatementList() { return StatementList(); }
};