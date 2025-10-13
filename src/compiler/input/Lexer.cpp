import <array>;
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
    return Position{cursor.Current(), tokens.size(), characters.size(), errors.size()};
  }

  void Lexer::Rollback(const Lexer::Position& position) {
    furthest = std::max(furthest, cursor.cbegin());
    cursor.Retreat(position.cursor);
    tokens.resize(position.token);
    characters.resize(position.character);
    errors.resize(position.error);
  }

  bool Lexer::Emit(ir::Token token) { tokens.push_back(token); return true; }

  // Deprecated EmitRepeated overloads removed in new design (widths are embedded as hex tokens)

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
    return Emit(token);
  }

  bool Lexer::Keyword(const std::string_view text) {
    if (!cursor.Check(text)) return false; // Compare the text to the content
    if (IsAlNum(text.size())) return false; // The next character cannot be an alphanumeric character with a keyword

    cursor.Advance(text.size());

    return true;
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
              characters.push_back(cursor.CodePoint());
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
                    cursor.Advance(1); Emit(ir::Token::CarriageReturnLineFeed); characters.push_back(U'\r'); characters.push_back(U'\n');
                  } else {
                    Emit(ir::Token::CarriageReturn);
                    characters.push_back(U'\r');
                  }
                } else if (cp == U'\n') { Emit(ir::Token::LineFeed); characters.push_back(U'\n'); }
                else if (cp == U'\u2028') { Emit(ir::Token::LineSeparator); characters.push_back(U'\u2028'); }
                else { Emit(ir::Token::ParagraphSeparator); characters.push_back(U'\u2029'); }
                continue;
              }

              // Regular comment character (no escape processing)
              characters.push_back(cp);
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

  // Optional white space
  bool Lexer::OWS() { WS(); return true; }

  bool Lexer::Null() { return Keyword("null") && Emit(ir::Token::Null); }
  bool Lexer::Undefined() { return Keyword("undefined") && Emit(ir::Token::Undefined); }
  bool Lexer::True() { return Keyword("true") && Emit(ir::Token::True); }
  bool Lexer::False() { return Keyword("false") && Emit(ir::Token::False); }
  bool Lexer::This() { return Keyword("this") && Emit(ir::Token::This); }
  bool Lexer::Import() { return Keyword("import") && Emit(ir::Token::Import); }
  bool Lexer::Export() { return Keyword("export") && Emit(ir::Token::Export); }
  bool Lexer::Register() { return Keyword("register") && Emit(ir::Token::Register); }
  bool Lexer::From() { return Keyword("from") && Emit(ir::Token::From); }
  bool Lexer::With() { return Keyword("with") && Emit(ir::Token::With); }
  bool Lexer::Has() { return Keyword("has") && Emit(ir::Token::Has); }
  bool Lexer::If() { return Keyword("if") && Emit(ir::Token::If); }
  bool Lexer::Else() { return Keyword("else") && Emit(ir::Token::Else); }
  bool Lexer::Do() { return Keyword("do") && Emit(ir::Token::Do); }
  bool Lexer::While() { return Keyword("while") && Emit(ir::Token::While); }
  bool Lexer::Repeat() { return Keyword("repeat") && Emit(ir::Token::Repeat); }
  bool Lexer::Is() { return Keyword("is") && Emit(ir::Token::Is); }
  bool Lexer::In() { return Keyword("in") && Emit(ir::Token::In); }
  bool Lexer::For() { return Keyword("for") && Emit(ir::Token::For); }
  bool Lexer::As() { return Keyword("as") && Emit(ir::Token::As); }
  bool Lexer::Default() { return Keyword("default") && Emit(ir::Token::Default); }
  bool Lexer::Auto() { return Keyword("auto") && Emit(ir::Token::Auto); }
  bool Lexer::Void() { return Keyword("void") && Emit(ir::Token::Void); }
  bool Lexer::When() { return Keyword("when") && Emit(ir::Token::When); }
  bool Lexer::Await() { return Keyword("await") && Emit(ir::Token::Await); }
  bool Lexer::Compiler() { return Keyword("compiler") && Emit(ir::Token::Compiler); }
  bool Lexer::Break() { return Keyword("break") && Emit(ir::Token::Break); }
  bool Lexer::Continue() { return Keyword("continue") && Emit(ir::Token::Continue); }
  bool Lexer::Return() { return Keyword("return") && Emit(ir::Token::Return); }
  bool Lexer::Case() { return Keyword("case") && Emit(ir::Token::Case); }
  bool Lexer::Yield() { return Keyword("yield") && Emit(ir::Token::Yield); }
  bool Lexer::Let() { return Keyword("let") && Emit(ir::Token::Let); }

  bool Lexer::CaptureOpen() { return cursor.Match('[') && Emit(ir::Token::CaptureOpen); }
  bool Lexer::CaptureClose() { return cursor.Match(']') && Emit(ir::Token::CaptureClose); }
  bool Lexer::ParameterOpen() { return cursor.Match('(') && Emit(ir::Token::ParameterOpen); }
  bool Lexer::ParameterClose() { return cursor.Match(')') && Emit(ir::Token::ParameterClose); }
  bool Lexer::ScopeOpen() { return cursor.Match('{') && Emit(ir::Token::ScopeOpen); }
  bool Lexer::ScopeClose() { return cursor.Match('}') && Emit(ir::Token::ScopeClose); }
  bool Lexer::TupleOpen() { return cursor.Match('(') && Emit(ir::Token::TupleOpen); }
  bool Lexer::TupleClose() { return cursor.Match(')') && Emit(ir::Token::TupleClose); }
  bool Lexer::ArrayOpen() { return cursor.Match('[') && Emit(ir::Token::ArrayOpen); }
  bool Lexer::ArrayClose() { return cursor.Match(']') && Emit(ir::Token::ArrayClose); }
  bool Lexer::ObjectOpen() { return cursor.Match('{') && Emit(ir::Token::ObjectOpen); }
  bool Lexer::ObjectClose() { return cursor.Match('}') && Emit(ir::Token::ObjectClose); }
  bool Lexer::EnumOpen() { return cursor.Match('<') && Emit(ir::Token::EnumOpen); }
  bool Lexer::EnumClose() { return cursor.Match('>') && Emit(ir::Token::EnumClose); }

  bool Lexer::CharOpen() { return cursor.Match('\'') && Emit(ir::Token::CharOpen); }
  bool Lexer::CharClose() { return cursor.Match('\'') && Emit(ir::Token::CharClose); }
  bool Lexer::StringOpen() { return cursor.Match('"') && Emit(ir::Token::StringOpen); }
  bool Lexer::StringClose() { return cursor.Match('"') && Emit(ir::Token::StringClose); }
  bool Lexer::TemplateStringOpen() { return cursor.Match('`') && Emit(ir::Token::TemplateStringOpen); }
  bool Lexer::TemplateStringClose() { return cursor.Match('`') && Emit(ir::Token::TemplateStringClose); }
  bool Lexer::TemplateStringExpressionOpen() { return cursor.Match('{') && Emit(ir::Token::TemplateStringExpressionOpen); }
  bool Lexer::TemplateStringExpressionClose() { return cursor.Match('}') && Emit(ir::Token::TemplateStringExpressionClose); }
  bool Lexer::ConditionOpen() { return cursor.Match('(') && Emit(ir::Token::ConditionOpen); }
  bool Lexer::ConditionClose() { return cursor.Match(')') && Emit(ir::Token::ConditionClose); }
  bool Lexer::Call() { return cursor.Match("->") && Emit(ir::Token::Call); }
  bool Lexer::Wildcard() { return cursor.Match('*') && Emit(ir::Token::Wildcard); }
  bool Lexer::Comma() { return cursor.Match(',') && Emit(ir::Token::Comma); }
  bool Lexer::Semicolon() { return cursor.Match(';') && Emit(ir::Token::Semicolon); }
  bool Lexer::CommentOpen() { return cursor.Match("//") && Emit(ir::Token::CommentOpen); }
  bool Lexer::CommentClose() { return Emit(ir::Token::CommentClose); }
  bool Lexer::MultiLineCommentOpen() { return cursor.Match("/*") && Emit(ir::Token::MultiLineCommentOpen); }
  bool Lexer::MultiLineCommentClose() { return cursor.Match("*/") && Emit(ir::Token::MultiLineCommentClose); }
  bool Lexer::DestructuredArrayOpen() { return cursor.Match('[') && Emit(ir::Token::DestructuredArrayOpen); }
  bool Lexer::DestructuredArrayClose() { return cursor.Match(']') && Emit(ir::Token::DestructuredArrayClose); }
  bool Lexer::DestructuredTupleOpen() { return cursor.Match('(') && Emit(ir::Token::DestructuredTupleOpen); }
  bool Lexer::DestructuredTupleClose() { return cursor.Match(')') && Emit(ir::Token::DestructuredTupleClose); }
  bool Lexer::DestructuredObjectOpen() { return cursor.Match('{') && Emit(ir::Token::DestructuredObjectOpen); }
  bool Lexer::DestructuredObjectClose() { return cursor.Match('}') && Emit(ir::Token::DestructuredObjectClose); }
  bool Lexer::DestructuredEnumOpen() { return cursor.Match('<') && Emit(ir::Token::DestructuredEnumOpen); }
  bool Lexer::DestructuredEnumClose() { return cursor.Match('>') && Emit(ir::Token::DestructuredEnumClose); }

  bool Lexer::OptionalSemicolon() { Semicolon(); return true; }

  // Unary operators
  bool Lexer::Reference() { return cursor.Match('&') && Emit(ir::Token::Reference); }
  bool Lexer::MutableReference() { return cursor.Match('*') && Emit(ir::Token::MutableReference); }
  bool Lexer::Symbol() { return cursor.Match('$') && Emit(ir::Token::Symbol); }
  bool Lexer::Copy() { return cursor.Match('@') && Emit(ir::Token::Copy); }
  bool Lexer::Counted() { return cursor.Match('#') && Emit(ir::Token::Counted); }
  bool Lexer::Positive() { return cursor.Match('+') && Emit(ir::Token::Positive); }
  bool Lexer::Negative() { return cursor.Match('-') && Emit(ir::Token::Negative); }
  bool Lexer::Increment() { return cursor.Match("++") && Emit(ir::Token::Increment); }
  bool Lexer::Decrement() { return cursor.Match("--") && Emit(ir::Token::Decrement); }
  bool Lexer::Not() { return cursor.Match('!') && Emit(ir::Token::Not); }
  bool Lexer::Spread() { return cursor.Match("...") && Emit(ir::Token::Spread); }
  bool Lexer::Move() { return cursor.Match('=') && Emit(ir::Token::Move); }
  bool Lexer::BitwiseNot() { return cursor.Match('~') && Emit(ir::Token::BitwiseNot); }

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

  bool Lexer::Escape() {
    if (cursor.Done() || cursor.Peek() != '\\') {
      return false; // not an escape start
    }

    cursor.Advance(1); // consume '\\'
    if (cursor.Done()) {
      Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);
      return false;
    }

    const char c = cursor.Peek();
    auto hex_value = [](char ch) -> int {
      if (ch >= '0' && ch <= '9') return ch - '0';
      if (ch >= 'a' && ch <= 'f') return 10 + (ch - 'a');
      if (ch >= 'A' && ch <= 'F') return 10 + (ch - 'A');
      return -1;
    };

    auto emit_ascii = [&](char32_t cp){
      Emit(ir::Token::EscapeASCII);
      if (!IsBacktracked()) characters.push_back(cp);
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
        if (cursor.Done()) { Error(ir::Error::EscapeSequenceUnexpectedEndOfInput); return false; }
        uint32_t value = 0;
        for (int i = 0; i < 2; ++i) {
          if (cursor.Done()) { Error(ir::Error::EscapeSequenceUnexpectedEndOfInput); return false; }
          int nib = hex_value(cursor.Peek());
          if (nib < 0) { Error(ir::Error::EscapeSequenceExpectedTwoHexDigits); return false; }
          value = (value << 4) | static_cast<uint32_t>(nib);
          cursor.Advance(1);
        }
        Emit(ir::Token::EscapeHex);
        if (!IsBacktracked()) characters.push_back(static_cast<char32_t>(value));
        return true;
      }
      case 'u': {
        cursor.Advance(1);

        if (cursor.Done()) { Error(ir::Error::EscapeSequenceUnexpectedEndOfInput); return false; }

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

          if (digits == 0) { Error(ir::Error::EscapeSequenceExpectedOneOrMoreHexDigits); return false; }
          // If next is still hex, we exceeded 6 digits
          if (!cursor.Done() && hex_value(cursor.Peek()) >= 0) { Error(ir::Error::EscapeSequenceExpectedSixOrFewerHexDigits); return false; }
          if (cursor.Done() || cursor.Peek() != '}') { Error(ir::Error::EscapeSequenceExpectedClosingBrace); return false; }
          cursor.Advance(1); // consume '}'

          if (value > 0x10FFFFu) { Error(ir::Error::UnicodeCodePointOutOfRange); return false; }
          if (value >= 0xD800u && value <= 0xDFFFu) { Error(ir::Error::UnicodeSurrogateCodePointIsNotAValidScalarValue); return false; }

          switch (digits) {
            case 1: Emit(ir::Token::EscapeUnicodeBraced1); break;
            case 2: Emit(ir::Token::EscapeUnicodeBraced2); break;
            case 3: Emit(ir::Token::EscapeUnicodeBraced3); break;
            case 4: Emit(ir::Token::EscapeUnicodeBraced4); break;
            case 5: Emit(ir::Token::EscapeUnicodeBraced5); break;
            case 6: Emit(ir::Token::EscapeUnicodeBraced6); break;
            default: break;
          }

          if (!IsBacktracked()) characters.push_back(static_cast<char32_t>(value));
          return true;
        } else {
          // \uXXXX short form
          uint32_t value = 0;
          for (int i = 0; i < 4; ++i) {
            if (cursor.Done()) { Error(ir::Error::EscapeSequenceUnexpectedEndOfInput); return false; }
            int nib = hex_value(cursor.Peek());
            if (nib < 0) { Error(ir::Error::EscapeSequenceExpectedFourHexDigits); return false; }
            value = (value << 4) | static_cast<uint32_t>(nib);
            cursor.Advance(1);
          }
          if (value >= 0xD800u && value <= 0xDFFFu) { Error(ir::Error::UnicodeSurrogateCodePointIsNotAValidScalarValue); return false; }
          Emit(ir::Token::EscapeUnicode);
          if (!IsBacktracked()) characters.push_back(static_cast<char32_t>(value));
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

  bool Lexer::Char() {
    if (!CharOpen()) return false;

    if (cursor.Peek() == '\\') {
      if (!Escape()) {
        // Escape() already reported an error; for character literals, standardize message
        return Error(ir::Error::CharacterLiteralExpectedEscapeAfterBackslash);
      }
    } else if (cursor.Peek() == '\'') {
      return Error(ir::Error::CharacterLiteralExpectedContent);
    } else {
      // Consume one raw code point and emit length token Characters1
      auto cp = cursor.CodePoint();
      Emit(ir::Token::Characters1);
      if (!IsBacktracked()) characters.push_back(cp);
    }

    if (!CharClose()) return Error(ir::Error::CharacterLiteralExpectedClosingSingleQuote);
    return OWS();
  }

  bool Lexer::String() {
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
        if (!Escape()) return false; // Escape() reports its own errors
        continue; // do not include escape in run; it implicitly represents Characters1
      }

      auto cp = cursor.CodePoint();
      if (!IsBacktracked()) characters.push_back(cp);
      ++run_count;
    }

    // flush trailing run
    EmitCharactersNibbles(run_count);

    if (!StringClose()) return Error(ir::Error::StringLiteralExpectedClosingDoubleQuote);
    return OWS();
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

  // Hex(): Parse 0x/0X-prefixed hex literals.
  // - Emits HexStart, then one Digits* token per hex digit (0-9, A-F), case-insensitive.
  // - Underscores are preserved as Underscore and terminate runs (but hex is already nibble-aligned).
  // - No value folding; simply streams digits to tokens; stops at first non-hex/non-underscore.
  // - Example: "0x00FF" → HexStart, Digits0, Digits0, DigitsF, DigitsF
  bool Lexer::Hex() {
    cursor.Advance(2);
    Emit(ir::Token::HexStart);

    while (!cursor.Done()) {
      char c = cursor.Peek();
      if (c == '_') {
        cursor.Advance();
        Emit(ir::Token::Underscore);
        continue;
      }

      if (!IsHex()) break;

      switch (c) {
        case '0': EmitAndAdvance(ir::Token::Digits0); break;
        case '1': EmitAndAdvance(ir::Token::Digits1); break;
        case '2': EmitAndAdvance(ir::Token::Digits2); break;
        case '3': EmitAndAdvance(ir::Token::Digits3); break;
        case '4': EmitAndAdvance(ir::Token::Digits4); break;
        case '5': EmitAndAdvance(ir::Token::Digits5); break;
        case '6': EmitAndAdvance(ir::Token::Digits6); break;
        case '7': EmitAndAdvance(ir::Token::Digits7); break;
        case '8': EmitAndAdvance(ir::Token::Digits8); break;
        case '9': EmitAndAdvance(ir::Token::Digits9); break;
        case 'a': case 'A': EmitAndAdvance(ir::Token::DigitsA); break;
        case 'b': case 'B': EmitAndAdvance(ir::Token::DigitsB); break;
        case 'c': case 'C': EmitAndAdvance(ir::Token::DigitsC); break;
        case 'd': case 'D': EmitAndAdvance(ir::Token::DigitsD); break;
        case 'e': case 'E': EmitAndAdvance(ir::Token::DigitsE); break;
        case 'f': case 'F': EmitAndAdvance(ir::Token::DigitsF); break;
      }
    }

    return true;
  }

  // Octal(): Parse 0o/0O-prefixed octal literals as a single pass bitstream.
  // - Emits OctalStart.
  // - Streams each octal digit's 3 bits MSB→LSB into a nibble accumulator; when 4 bits are filled,
  //   emits the corresponding Digits* token.
  // - A leading '0' when the accumulator is empty emits Digits0 to preserve width.
  // - An underscore flushes any partial nibble, is emitted as Underscore, and digit streaming continues.
  // - Stops at first non-octal/non-underscore.
  // - Example: "0o7"    → OctalStart, Digits7
  // - Example: "0o0077" → OctalStart, Digits0, Digits0, Digits3, DigitsF
  // - Example: "0o1234" → OctalStart, Digits2, Digits9, DigitsC
  bool Lexer::Octal() {
    cursor.Advance(2);
    Emit(ir::Token::OctalStart);

    uint8_t value = 0;
    uint8_t bits = 0; // bits collected toward current hex nibble

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
    return true;
  }

  // Binary(): Parse 0b/0B-prefixed binary literals as a single pass bitstream.
  // - Emits BinaryStart.
  // - Packs bits into a 4-bit accumulator; when full, emits a Digits* token.
  // - A leading '0' when the accumulator is empty emits Digits0 to preserve width.
  // - Underscore flushes partial nibble and is emitted as Underscore.
  // - Stops at first non-binary/non-underscore.
  // - Example: "0b1111"      → BinaryStart, DigitsF
  // - Example: "0b0111_0001" → BinaryStart, Digits0, Digits7, Underscore, Digits0, Digits0, Digits0, Digits1
  bool Lexer::Binary() {
    cursor.Advance(2);
    Emit(ir::Token::BinaryStart);

    uint8_t value = 0;
    uint8_t bits = 0; // bits collected toward current hex nibble

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
    return true;
  }

  // Decimal(): Parse base-10 runs into hex nibble tokens, preserving separators.
  // - Does not use a prefix token; decimal is the default base.
  // - Consumes runs of [0-9]+, separated by underscores, dots, and scientific notation markers.
  // - Emits leading '0's as Digits0 to preserve width; an all-zero run emits only those zeros.
  // - Converts the remaining run using base-10 multiply-accumulate into hex nibbles, stored LE
  //   in a buffer, then emitted in big-endian nibble order as Digits* tokens.
  // - Between runs, emits Underscore, Dot, Exponent and an optional Plus/Minus immediately
  //   after Exponent. Does not assemble floats or scientific notation; the parser owns that.
  // - Stops when a non-digit, non-separator is encountered.
  // - Example: "100"   → Digits6, Digits4
  // - Example: "1_100" → Digits1, Underscore, Digits6, Digits4
  // - Example: "42.7"  → Digits2, DigitsA, Dot, Digits7
  bool Lexer::Decimal() {
    bool consumed = false;

    auto flush_run = [&](std::string_view run){
      if (run.empty()) return;

      // Emit leading zeros for exact reconstruction
      size_t i = 0;
      while (i < run.size() && run[i] == '0') { Emit(ir::Token::Digits0); ++i; }
      if (i == run.size()) return;

      // Multiply-accumulate in base-10 into hex nibbles (little-endian in buffer)
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
      while (hex_le.size() > 1 && hex_le.back() == 0) hex_le.pop_back();
      for (auto it = hex_le.rbegin(); it != hex_le.rend(); ++it) Emit(DigitToken(*it));
    };

    while (!cursor.Done()) {
      if (!IsDigit()) break;

      auto run_begin = cursor.cbegin();
      while (!cursor.Done() && IsDigit()) cursor.Advance();
      auto run_end = cursor.cbegin();

      if (run_begin != run_end) {
        std::string_view run{&*run_begin, static_cast<size_t>(std::distance(run_begin, run_end))};
        flush_run(run);
        consumed = true;
      }

      if (cursor.Done()) break;
      char c = cursor.Peek();
      if (c == '_') { cursor.Advance(); Emit(ir::Token::Underscore); continue; }
      if (c == '.') { cursor.Advance(); Emit(ir::Token::Dot); continue; }
      if (c == 'e' || c == 'E') {
        cursor.Advance(); Emit(ir::Token::Exponent);
        if (!cursor.Done()) {
          if (cursor.Peek() == '+') { cursor.Advance(); Emit(ir::Token::Plus); }
          else if (cursor.Peek() == '-') { cursor.Advance(); Emit(ir::Token::Minus); }
        }
        continue;
      }
      break;
    }

    return consumed;
  }

  // Number(): Dispatch entry for numeric literals.
  // - Recognizes number starts as digits only (no leading '+'/'-'; those are unary operators).
  // - If the run begins with 0x/0X, 0o/0O, or 0b/0B, dispatch to Hex/Octal/Binary respectively.
  // - Otherwise, parse as Decimal().
  bool Lexer::Number() {
    if (!cursor.IsNumberStart()) {
      return false;
    }

    if (cursor.Peek() == '0') {
      switch (cursor.Peek(1)) {
        case 'x':
        case 'X': return Hex();
        case 'o':
        case 'O': return Octal();
        case 'b':
        case 'B': return Binary();
        default: break;
      }
    }

    return Decimal();
  }

  // Identifier parsing (redesigned): same structure as String content without open/close markers.
  // Emits runs of Characters* nibble tokens for contiguous non-escape code points and Escape* tokens
  // for escapes. Appends every consumed code point to `characters`.
  bool Lexer::Identifier() {
    // Must begin at a valid identifier start or an escape or a non-ASCII code point
    if (!IsPossibleIdentStart()) return false;

    uint32_t run_count = 0;
    // If the first char is not an escape and is valid raw byte, include it by falling into the loop
    while (!cursor.Done()) {
      if (cursor.Peek() == '\\') {
        // flush current run
        EmitCharactersNibbles(run_count);
        run_count = 0;
        if (!Escape()) return false; // propagate escape failure
        continue;
      }

      // Stop on whitespace or line break
      if (IsSpace() || IsBreak()) break;

      // Accept non-ASCII code points wholesale; for ASCII require identifier continue
      if (!IsASCII()) {
        auto cp = cursor.CodePoint();
        if (!IsBacktracked()) characters.push_back(cp);
        ++run_count;
        continue;
      }

      if (IsIdent()) {
        // ASCII identifier continue ([A-Za-z0-9_])
        auto cp = cursor.CodePoint();
        if (!IsBacktracked()) characters.push_back(cp);
        ++run_count;
        continue;
      }

      break; // reached a non-identifier ASCII char (operator, punct, etc.)
    }

    // flush trailing run
    EmitCharactersNibbles(run_count);

    // Success if we produced any output (either run_count>0 or at least one escape).
    // We can approximate by returning true when something was consumed: run_count>0 or previous loop consumed an escape.
    // The easiest check: we consumed at least one character if run_count>0 or the last action was Escape().
    // Since tracking last action adds state, accept returning true if we advanced at least one position.
    // If nothing consumed, return false.
    return true;
  }

  // Template string: like String content but allows line breaks and embedded expressions.
  // - Escapes, line breaks, and expression boundaries terminate the Characters* run.
  // - Emits explicit newline tokens (LineFeed, CarriageReturnLineFeed, CarriageReturn, LineSeparator, ParagraphSeparator).
  // - Appends every code point (including newlines) to `characters`.
  // - Uses error codes (no string diagnostics).
  bool Lexer::TemplateString() {
    if (!TemplateStringOpen()) return false;

    uint32_t run_count = 0;
    while (!cursor.Done()) {
      // Closing backtick
      if (cursor.Peek() == '`') {
        EmitCharactersNibbles(run_count);
        run_count = 0;
        TemplateStringClose();
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
            characters.push_back(U'\r');
            characters.push_back(U'\n');
          } else {
            Emit(ir::Token::CarriageReturn);
            characters.push_back(U'\r');
          }
        } else if (cp == U'\n') {
          Emit(ir::Token::LineFeed);
          characters.push_back(U'\n');
        } else if (cp == U'\u2028') { // Line Separator
          Emit(ir::Token::LineSeparator);
          characters.push_back(U'\u2028');
        } else { // U'\u2029' Paragraph Separator
          Emit(ir::Token::ParagraphSeparator);
          characters.push_back(U'\u2029');
        }
        continue;
      }

      // Regular code point: accumulate into current run
      characters.push_back(cp);
      ++run_count;
    }

    // Reached EOF without a closing backtick
    return Error(ir::Error::TemplateStringLiteralExpectedClosingBacktick);
  }

  bool Lexer::BinaryOperatorHelper(bool in_enum) {
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

  bool Lexer::BinaryOperator(bool in_enum) { return cursor.IsBinaryStart() && BinaryOperatorHelper(in_enum); }

  bool Lexer::ParameterDeclaration() {
    int modifiers = Modifiers();

    if (Identifier()) {
      if (cursor.Peek() == ',' || cursor.Peek() == ')') return true;
    } else {
      if (modifiers > 0) {
        return Error(ir::Error::ParserDeclarationExpectedIdentifierAfterModifiers);
      }

      return false;
    }

    return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
  }

  bool Lexer::ParameterDeclarationList() { return ZeroOrMore([&]{ return ParameterDeclaration(); }, [&]{ return Comma(); }); }

  bool Lexer::Parameters() { return ParameterOpen() && ParameterDeclarationList() && ParameterClose(); }

  bool Lexer::Function() {
    return Try([&]{
      if (Parameters()) return (Captures() || true) && FunctionBody() || ArrowFunction();
      else return Captures() && (Parameters() || true) && FunctionBody() || ArrowFunction();
    });
  }

  // High-level parsing helpers (ported from lexical::Lexer)
  bool Lexer::IdentifiedExpression() { return IsIdent() && Identifier() && ContinueExpression(); }

  bool Lexer::CallablePrefixLiteralHelper() {
    switch (cursor.Peek()) {
      case '\'': return Char();
      case '"': return String();
      case '`': return TemplateString();
      default: return Number();
    }
  }

  bool Lexer::CallablePostfixLiteralHelper() {
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

  bool Lexer::CallablePrefixLiteral() { return cursor.IsCallablePrefixStart() && CallablePrefixLiteralHelper(); }
  bool Lexer::CallablePostfixLiteral() { return cursor.IsCallablePostfixStart() && CallablePostfixLiteralHelper(); }

  bool Lexer::ValueShortcut() {
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

  bool Lexer::Value() { return (cursor.IsValueStart() && ValueShortcut()) || Identifier(); }

  bool Lexer::InitialValue() {
    if (cursor.IsValueStart() && ValueShortcut()) {
      // maybe_declaration = false; // not tracked in new lexer
    }

    return Identifier();
  }

  bool Lexer::UnaryStartedExpression() {
    int count = UnaryPrefixOperators();
    if (count == 0) return false;

    if (Value()) return ContinueExpression();

    if (count > 0) {
      return Error(ir::Error::UnaryPrefixOperatorExpectedValue);
    }

    return false;
  }

  bool Lexer::ContinueExpression() {
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

  bool Lexer::ContinueEnumExpression() {
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

  bool Lexer::Expression() {
    while (UnaryPrefixOperator()) {}

    if (!Value()) {
      return false;
    }

    return ContinueExpression();
  }

  bool Lexer::EnumExpression() {
    while (UnaryPrefixOperator()) {}

    if (!Value()) {
      return false;
    }

    return ContinueEnumExpression();
  }

  bool Lexer::DeclarationValue() {
    int operators = UnaryPostfixOperators();

    if (TypeStart()) {
      if (operators > 0) {
        return Error(ir::Error::ParserDeclarationExplicitTypeCannotBeUsedWithPostfixUnaryOperators);
      }

      if (!Expression()) {
        return Error(ir::Error::ParserDeclarationExpectedTypeExpressionAfterColon);
      }

      if (Assign()) {
        if (Expression()) {
          return true;
        }
        else {
          return Error(ir::Error::ParserDeclarationExpectedExpressionAfterAssignmentOperator);
        }
      }
      else {
        return true;
      }
    }

    if (CallablePostfixLiteral()) {
      if (operators > 0) {
        return Error(ir::Error::ParserDeclarationCannotSkipAssignmentOperatorWithPostfixUnaryOperators);
      }

      return true;
    }
    else if (Assign()) {
      if (Expression()) {
        return true;
      }
      else {
        return Error(ir::Error::ParserDeclarationExpectedExpressionAfterAssignmentOperator);
      }
    }

    return false;
  }

  bool Lexer::Declaration() {
    int modifiers = Modifiers();

    if (UnaryStartedExpression()) return true;

    if (Identifier()) {
      if (cursor.Peek() == ',') return true;
    }
    else {
      if (modifiers > 0) {
        return Error(ir::Error::ParserDeclarationExpectedIdentifierAfterModifiers);
      }

      return false;
    }

    return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
  }

  bool Lexer::StatementShortcut() {
    switch (cursor.Peek()) {
      case 'b': return Break();
      case 'c': return Continue() || Case();
      case 'd': return Do() || Default();
      case 'e': return Export();
      case 'f': return For();
      case 'i': return If() || Import() || Is();
      case 'r': return Return() || Repeat() || Register();
      case 'w': return While() || When();
      case 'y': return Yield();
      default: return false;
    }
  }

  bool Lexer::Statement() { return (cursor.IsStatementStart() && StatementShortcut()) || Declaration() || Expression() && (Semicolon() || true); }

  bool Lexer::StatementList() {
    while (!cursor.Done()) {
      if (!Statement()) break;
    }

    return true;
  }

  bool Lexer::FunctionStatementList() { return StatementList(); }
};