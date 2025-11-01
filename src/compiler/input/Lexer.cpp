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

  // Optional white space
  bool Lexer::OWS() { WS(); return true; }

  bool Lexer::Null() { return Keyword("null") && Emit(ir::Token::Null) && OWS(); }
  bool Lexer::Undefined() { return Keyword("undefined") && Emit(ir::Token::Undefined) && OWS(); }
  bool Lexer::True() { return Keyword("true") && Emit(ir::Token::True) && OWS(); }
  bool Lexer::False() { return Keyword("false") && Emit(ir::Token::False) && OWS(); }
  bool Lexer::This() { return Keyword("this") && Emit(ir::Token::This) && OWS(); }
  bool Lexer::That() { return Keyword("that") && Emit(ir::Token::That) && OWS(); }
  bool Lexer::Import() { return Keyword("import") && Emit(ir::Token::Import) && OWS(); }
  bool Lexer::Export() { return Keyword("export") && Emit(ir::Token::Export) && OWS(); }
  bool Lexer::Register() { return Keyword("register") && Emit(ir::Token::Register) && OWS(); }
  bool Lexer::From() { return Keyword("from") && Emit(ir::Token::From) && OWS(); }
  bool Lexer::With() { return Keyword("with") && Emit(ir::Token::With) && OWS(); }
  bool Lexer::Has() { return Keyword("has") && Emit(ir::Token::Has) && OWS(); }
  bool Lexer::If() { return Keyword("if") && Emit(ir::Token::If) && OWS(); }
  bool Lexer::Else() { return Keyword("else") && Emit(ir::Token::Else) && OWS(); }
  bool Lexer::Do() { return Keyword("do") && Emit(ir::Token::Do) && OWS(); }
  bool Lexer::While() { return Keyword("while") && Emit(ir::Token::While) && OWS(); }
  bool Lexer::Repeat() { return Keyword("repeat") && Emit(ir::Token::Repeat) && OWS(); }
  bool Lexer::Is() { return Keyword("is") && Emit(ir::Token::Is) && OWS(); }
  bool Lexer::In() { return Keyword("in") && Emit(ir::Token::In) && OWS(); }
  bool Lexer::For() { return Keyword("for") && Emit(ir::Token::For) && OWS(); }
  bool Lexer::As() { return Keyword("as") && Emit(ir::Token::As) && OWS(); }
  bool Lexer::Default() { return Keyword("default") && Emit(ir::Token::Default) && OWS(); }
  bool Lexer::Auto() { return Keyword("auto") && Emit(ir::Token::Auto) && OWS(); }
  bool Lexer::Void() { return Keyword("void") && Emit(ir::Token::Void) && OWS(); }
  bool Lexer::When() { return Keyword("when") && Emit(ir::Token::When) && OWS(); }
  bool Lexer::Await() { return Keyword("await") && Emit(ir::Token::Await) && OWS(); }
  bool Lexer::Compiler() { return Keyword("compiler") && Emit(ir::Token::Compiler) && OWS(); }
  bool Lexer::Break() { return Keyword("break") && Emit(ir::Token::Break) && OWS(); }
  bool Lexer::Continue() { return Keyword("continue") && Emit(ir::Token::Continue) && OWS(); }
  bool Lexer::Return() { return Keyword("return") && Emit(ir::Token::Return) && OWS(); }
  bool Lexer::Case() { return Keyword("case") && Emit(ir::Token::Case) && OWS(); }
  bool Lexer::Yield() { return Keyword("yield") && Emit(ir::Token::Yield) && OWS(); }
  bool Lexer::Let() { return Keyword("let") && Emit(ir::Token::Let) && OWS(); }

  bool Lexer::CaptureOpen() { return cursor.Match('[') && Emit(ir::Token::CaptureOpen) && OWS(); }
  bool Lexer::CaptureClose() { return cursor.Match(']') && Emit(ir::Token::CaptureClose) && OWS(); }
  bool Lexer::ParameterOpen() { return cursor.Match('(') && Emit(ir::Token::ParameterOpen) && OWS(); }
  bool Lexer::ParameterClose() { return cursor.Match(')') && Emit(ir::Token::ParameterClose) && OWS(); }
  bool Lexer::ScopeOpen() { return cursor.Match('{') && Emit(ir::Token::ScopeOpen) && OWS(); }
  bool Lexer::ScopeClose() { return cursor.Match('}') && Emit(ir::Token::ScopeClose) && OWS(); }
  bool Lexer::TupleOpen() { return cursor.Match('(') && Emit(ir::Token::TupleOpen) && OWS(); }
  bool Lexer::TupleClose() { return cursor.Match(')') && Emit(ir::Token::TupleClose) && OWS(); }
  bool Lexer::ArrayOpen() { return cursor.Match('[') && Emit(ir::Token::ArrayOpen) && OWS(); }
  bool Lexer::ArrayClose() { return cursor.Match(']') && Emit(ir::Token::ArrayClose) && OWS(); }
  bool Lexer::ObjectOpen() { return cursor.Match('{') && Emit(ir::Token::ObjectOpen) && OWS(); }
  bool Lexer::ObjectClose() { return cursor.Match('}') && Emit(ir::Token::ObjectClose) && OWS(); }
  bool Lexer::EnumOpen() { return cursor.Match('<') && Emit(ir::Token::EnumOpen) && OWS(); }
  bool Lexer::EnumClose() { return cursor.Match('>') && Emit(ir::Token::EnumClose) && OWS(); }

  bool Lexer::CharOpen() { return cursor.Match('\'') && Emit(ir::Token::CharOpen) && OWS(); }
  bool Lexer::CharClose() { return cursor.Match('\'') && Emit(ir::Token::CharClose) && OWS(); }
  bool Lexer::StringOpen() { return cursor.Match('"') && Emit(ir::Token::StringOpen) && OWS(); }
  bool Lexer::StringClose() { return cursor.Match('"') && Emit(ir::Token::StringClose) && OWS(); }
  bool Lexer::TemplateStringOpen() { return cursor.Match('`') && Emit(ir::Token::TemplateStringOpen) && OWS(); }
  bool Lexer::TemplateStringClose() { return cursor.Match('`') && Emit(ir::Token::TemplateStringClose) && OWS(); }
  bool Lexer::TemplateStringExpressionOpen() { return cursor.Match('{') && Emit(ir::Token::TemplateStringExpressionOpen) && OWS(); }
  bool Lexer::TemplateStringExpressionClose() { return cursor.Match('}') && Emit(ir::Token::TemplateStringExpressionClose) && OWS(); }
  bool Lexer::ConditionOpen() { return cursor.Match('(') && Emit(ir::Token::ConditionOpen) && OWS(); }
  bool Lexer::ConditionClose() { return cursor.Match(')') && Emit(ir::Token::ConditionClose) && OWS(); }
  bool Lexer::Pipeline() { return cursor.Match("->") && Emit(ir::Token::Pipeline) && OWS(); }
  bool Lexer::Arrow() { return cursor.Match("=>") && Emit(ir::Token::Arrow) && OWS(); }
  bool Lexer::Wildcard() { return cursor.Match('*') && Emit(ir::Token::Wildcard) && OWS(); }
  bool Lexer::Comma() { return cursor.Match(',') && Emit(ir::Token::Comma) && OWS(); }
  bool Lexer::Semicolon() { return cursor.Match(';') && Emit(ir::Token::Semicolon) && OWS(); }
  bool Lexer::CommentOpen() { return cursor.Match("//") && Emit(ir::Token::CommentOpen) && OWS(); }
  bool Lexer::CommentClose() { return Emit(ir::Token::CommentClose) && OWS(); }
  bool Lexer::MultiLineCommentOpen() { return cursor.Match("/*") && Emit(ir::Token::MultiLineCommentOpen) && OWS(); }
  bool Lexer::MultiLineCommentClose() { return cursor.Match("*/") && Emit(ir::Token::MultiLineCommentClose) && OWS(); }

  bool Lexer::OptionalSemicolon() { Semicolon(); return true; }

  // Unary operators
  bool Lexer::Reference() { return cursor.Match('&') && Emit(ir::Token::Reference) && OWS(); }
  bool Lexer::MutableReference() { return cursor.Match('*') && Emit(ir::Token::MutableReference) && OWS(); }
  bool Lexer::Symbol() { return cursor.Match('$') && Emit(ir::Token::Symbol) && OWS(); }
  bool Lexer::Copy() { return cursor.Match('@') && Emit(ir::Token::Copy) && OWS(); }
  bool Lexer::Counted() { return cursor.Match('#') && Emit(ir::Token::Counted) && OWS(); }
  bool Lexer::Positive() { return cursor.Match('+') && Emit(ir::Token::Positive) && OWS(); }
  bool Lexer::Negative() { return cursor.Match('-') && Emit(ir::Token::Negative) && OWS(); }
  bool Lexer::Increment() { return cursor.Match("++") && Emit(ir::Token::Increment) && OWS(); }
  bool Lexer::Decrement() { return cursor.Match("--") && Emit(ir::Token::Decrement) && OWS(); }
  bool Lexer::Not() { return cursor.Match('!') && Emit(ir::Token::Not) && OWS(); }
  bool Lexer::Spread() { return cursor.Match("...") && Emit(ir::Token::Spread) && OWS(); }
  bool Lexer::Move() { return cursor.Match('=') && Emit(ir::Token::Move) && OWS(); }
  bool Lexer::BitwiseNot() { return cursor.Match('~') && Emit(ir::Token::BitwiseNot) && OWS(); }

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
      if (!IsBacktracked()) data.push_back(static_cast<uint32_t>(cp));
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
        if (!IsBacktracked()) data.push_back(value);
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

          if (!IsBacktracked()) data.push_back(value);
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
          if (!IsBacktracked()) data.push_back(value);
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
      if (!IsBacktracked()) data.push_back(static_cast<uint32_t>(cp));
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
      if (!IsBacktracked()) data.push_back(static_cast<uint32_t>(cp));
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
  // - Accumulates the magnitude in base-16 and appends little-endian 32-bit limbs to `data`.
  // - Example: "0x00FF" → HexStart, Digits0, Digits0, DigitsF, DigitsF
  bool Lexer::Hex() {
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

    return true;
  }

  // Octal(): Parse 0o/0O-prefixed octal literals as a single pass bitstream.
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
  bool Lexer::Octal() {
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

    return true;
  }

  // Binary(): Parse 0b/0B-prefixed binary literals as a single pass bitstream.
  // - Emits BinaryStart.
  // - Packs bits into a 4-bit accumulator; when full, emits a Digits* token.
  // - Accumulates the magnitude in base-2 and appends the resulting limbs to `data`.
  // - A leading '0' when the accumulator is empty emits Digits0 to preserve width.
  // - Underscore flushes partial nibble and is emitted as Underscore.
  // - Stops at first non-binary/non-underscore.
  // - Example: "0b1111"      → BinaryStart, DigitsF
  // - Example: "0b0111_0011" → BinaryStart, Digits0, Digits7, Underscore, Digits0, Digits0, Digits3
  bool Lexer::Binary() {
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

    return true;
  }

  // Decimal(): Parse base-10 literals, preserving textual fidelity while emitting mantissa limbs.
  // - Streams digits into nibble tokens (Digits0..DigitsF) to reconstruct the literal losslessly.
  // - Accumulates the mantissa in base-2^32 little-endian limbs within temp_le.
  // - Emits ir::Token::Mantissa once per stored limb and appends the limb to `data`.
  // - If a dot or exponent modifier is present, appends a 64-bit signed exponent as the final two limbs.
  // - Example: "100"    → Digits6, Digits4
  // - Example: "1_100"  → Digits1, Underscore, Digits6, Digits4
  // - Example: "42.7"   → Digits2, DigitsA, Dot, Digits7
  // - Example: "42.007" → Digits2, DigitsA, Dot, Digits0, Digits0, Digits7
  bool Lexer::Decimal() {
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
    return true;
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
  // for escapes. Appends every consumed code point to `data`.
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
        if (!IsBacktracked()) data.push_back(static_cast<uint32_t>(cp));
        ++run_count;
        continue;
      }

      if (IsIdent()) {
        // ASCII identifier continue ([A-Za-z0-9_])
        auto cp = cursor.CodePoint();
        if (!IsBacktracked()) data.push_back(static_cast<uint32_t>(cp));
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

      return OWS();
    } else {
      return Error(ir::Error::ArrowFunctionExpectedExpression);
    }
  }

  // Template string: like String content but allows line breaks and embedded expressions.
  // - Escapes, line breaks, and expression boundaries terminate the Characters* run.
  // - Emits explicit newline tokens (LineFeed, CarriageReturnLineFeed, CarriageReturn, LineSeparator, ParagraphSeparator).
  // - Appends every code point (including newlines) to `data`.
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

  bool Lexer::BinaryOperator(bool in_enum) { return IsBinaryStart() && BinaryOperatorHelper(in_enum) && OWS(); }

  bool Lexer::ParameterDeclaration() {
    int modifiers = Modifiers();

    if (Identifier()) {
      if (cursor.Peek() == ',' || cursor.Peek() == ')') return true;
    } else {
      if (modifiers > 0) {
        return Error(ir::Error::DeclarationExpectedIdentifierAfterModifiers);
      }

      return false;
    }

    return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
  }

  bool Lexer::ParameterDeclarationList() { return ZeroOrMore([&]{ return ParameterDeclaration(); }, [&]{ return Comma(); }); }

  bool Lexer::Parameters() { return ParameterOpen() && ParameterDeclarationList() && ParameterClose(); }
  bool Lexer::Captures() { return CaptureOpen() && ParameterDeclarationList() && CaptureClose(); }

  bool Lexer::ArrowFunction() {
    if (!Arrow()) return false;

    if (Expression()) {
      return OWS();
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
      case 't': return True() || This() || That();
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
        return Error(ir::Error::DeclarationExplicitTypeCannotBeUsedWithPostfixUnaryOperators);
      }

      if (!Expression()) {
        return Error(ir::Error::DeclarationExpectedTypeExpressionAfterColon);
      }

      if (Assign()) {
        if (Expression()) {
          return true;
        }
        else {
          return Error(ir::Error::DeclarationExpectedExpressionAfterAssignmentOperator);
        }
      }
      else {
        return true;
      }
    }

    if (CallablePostfixLiteral()) {
      if (operators > 0) {
        return Error(ir::Error::DeclarationCannotSkipAssignmentOperatorWithPostfixUnaryOperators);
      }

      return true;
    }
    else if (Assign()) {
      if (Expression()) {
        return true;
      }
      else {
        return Error(ir::Error::DeclarationExpectedExpressionAfterAssignmentOperator);
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
        return Error(ir::Error::DeclarationExpectedIdentifierAfterModifiers);
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