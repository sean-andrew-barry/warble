import <array>;
import <limits>;
import <string>;
import <vector>;

import compiler.input.Lexer;
import compiler.ir.Error;
import compiler.ir.Token;
import compiler.text.cursor.String;
import compiler.text.Unicode;
import compiler.text.Convert;
import compiler.utility.Print;

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
    return Position{cursor.cbegin(), tokens.size(), errors.size()};
  }

  void Lexer::Rollback(const Lexer::Position& position) {
    auto current = cursor.cbegin();
    if (current > position.cursor) {
      furthest = std::max(furthest, current);
      has_backtracked = true;
    }

    cursor.Retreat(position.cursor);
    tokens.resize(position.token);
    errors.resize(position.error);
  }

  void Lexer::PushCharacter(char32_t cp) {
    if (IsBacktracked()) return; // Don't add character data when backtracked
    data.push_back(static_cast<uint32_t>(cp));
  }

  void Lexer::PushLimb(uint32_t word) {
    if (IsBacktracked()) return; // Don't add limb data when backtracked
    data.push_back(word);
  }

  bool Lexer::Emit(ir::Token token) { tokens.push_back(token); return true; }

  // Simple QoL function to emit a token after matching a char/string
  bool Lexer::Match(const char c, ir::Token token) { return cursor.Match(c) && Emit(token); }
  bool Lexer::Match(const std::string_view s, ir::Token token) { return cursor.Match(s) && Emit(token); }
  // Match with whitespace consumption
  bool Lexer::MatchWS(const char c, ir::Token token) { return Match(c, token) && (WS() || true); }
  bool Lexer::MatchWS(const std::string_view s, ir::Token token) { return Match(s, token) && (WS() || true); }

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

  bool Lexer::EmitDigitsNibbles(uint32_t count) {
    if (count == 0) return false;
    uint32_t v = count;
    uint8_t nibbles[8];
    int n = 0;
    while (v > 0) { nibbles[n++] = static_cast<uint8_t>(v & 0xF); v >>= 4; }
    for (int i = n - 1; i >= 0; --i) {
      Emit(DigitToken(nibbles[i]));
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
    return false;
  }

  bool Lexer::EmitAndAdvance(ir::Token token, size_t count) {
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
    : source{std::move(source)}, cursor{this->source}, furthest{cursor.cbegin()}, has_backtracked{false}
  {
    const size_t n = source.size();
    tokens.reserve(n * 2 / 5);    // 0.4n
    data.reserve(n * 7 / 16);     // 0.4375n
    WS();
  }

  bool Lexer::WhiteSpace() {
    // utility::Print("Lexer::WhiteSpace at:", cursor.Peek());

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
              PushCharacter(cursor.CodePoint());
              ++run_count;
            }
            EmitCharactersNibbles(run_count);
            Emit(ir::Token::CommentClose);
            return true;
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
                Emit(ir::Token::MultiLineCommentClose);
                return true;
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
                    Emit(ir::Token::Characters2); // Tell the parser we're saving two characters
                    PushCharacter(U'\r');
                    PushCharacter(U'\n');
                  } else {
                    Emit(ir::Token::CarriageReturn);
                    Emit(ir::Token::Characters1); // Tell the parser we're saving one character
                    PushCharacter(U'\r');
                  }
                } else if (cp == U'\n') {
                  Emit(ir::Token::LineFeed);
                  Emit(ir::Token::Characters1);
                  PushCharacter(U'\n');
                } else if (cp == U'\u2028') {
                  Emit(ir::Token::LineSeparator);
                  Emit(ir::Token::Characters1);
                  PushCharacter(U'\u2028');
                } else {
                  Emit(ir::Token::ParagraphSeparator);
                  Emit(ir::Token::Characters1);
                  PushCharacter(U'\u2029');
                }
                continue;
              }

              // Regular comment character (no escape processing)
              PushCharacter(cp);
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
    if (code_point == 0x2028) {
      Emit(ir::Token::LineSeparator);
      return true;
    }

    if (code_point == 0x2029) {
      Emit(ir::Token::ParagraphSeparator);
      return true;
    }

    // Other Unicode whitespace is not consumed here in the new design (no generic Spaces token).
    // Defer to other lexing paths by undoing and returning false.

    // Not whitespace; undo
    cursor.Retreat(iter);
    return false;
  }

  bool Lexer::WS() {
    bool matched = false;
    
    while (!cursor.Done() && IsPossibleSpaceStart() && WhiteSpace()) {
      matched = true;
    }

    return matched;
  }

  bool Lexer::Null() { return Keyword("null", ir::Token::Null); }
  bool Lexer::Undefined() { return Keyword("undefined", ir::Token::Undefined); }
  bool Lexer::Readonly() { return Keyword("readonly", ir::Token::Readonly); }
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
  bool Lexer::Try() { return Keyword("try", ir::Token::Try); }
  bool Lexer::Else() { return Keyword("else", ir::Token::Else); }
  bool Lexer::Do() { return Keyword("do", ir::Token::Do); }
  bool Lexer::While() { return Keyword("while", ir::Token::While); }
  bool Lexer::Loop() { return Keyword("loop", ir::Token::Loop); }
  bool Lexer::Repeat() { return Keyword("repeat", ir::Token::Repeat); }
  bool Lexer::Is() { return Keyword("is", ir::Token::Is); }
  bool Lexer::In() { return Keyword("in", ir::Token::In); }
  bool Lexer::As() { return Keyword("as", ir::Token::As); }
  bool Lexer::For() { return Keyword("for", ir::Token::For); }
  bool Lexer::Default() { return Keyword("default", ir::Token::Default); }
  bool Lexer::Auto() { return Keyword("auto", ir::Token::Auto); }
  bool Lexer::When() { return Keyword("when", ir::Token::When); }
  bool Lexer::Comtime() { return Keyword("comtime", ir::Token::Comtime); }
  bool Lexer::Runtime() { return Keyword("runtime", ir::Token::Runtime); }
  bool Lexer::Await() { return Keyword("await", ir::Token::Await); }
  bool Lexer::Assume() { return Keyword("assume", ir::Token::Assume); }
  bool Lexer::Expect() { return Keyword("expect", ir::Token::Expect); }
  bool Lexer::Async() { return Keyword("async", ir::Token::Async); }
  bool Lexer::Compiler() { return Keyword("compiler", ir::Token::Compiler); }
  bool Lexer::Break() { return Keyword("break", ir::Token::Break); }
  bool Lexer::Continue() { return Keyword("continue", ir::Token::Continue); }
  bool Lexer::Return() { return Keyword("return", ir::Token::Return); }
  bool Lexer::Panic() { return Keyword("panic", ir::Token::Panic); }
  bool Lexer::Yield() { return Keyword("yield", ir::Token::Yield); }
  bool Lexer::Pass() { return Keyword("pass", ir::Token::Pass); }
  bool Lexer::Fail() { return Keyword("fail", ir::Token::Fail); }
  bool Lexer::Let() { return Keyword("let", ir::Token::Let); }
  bool Lexer::Const() { return Keyword("const", ir::Token::Const); }
  bool Lexer::Mut() { return Keyword("mut", ir::Token::Mut); }
  bool Lexer::Public() { return Keyword("public", ir::Token::Public); }
  bool Lexer::Protected() { return Keyword("protected", ir::Token::Protected); }
  bool Lexer::Private() { return Keyword("private", ir::Token::Private); }

  bool Lexer::CaptureOpen() { return MatchWS('[', ir::Token::CaptureOpen); }
  bool Lexer::CaptureClose() { return MatchWS(']', ir::Token::CaptureClose); }
  bool Lexer::ParameterOpen() { return MatchWS('(', ir::Token::ParameterOpen); }
  bool Lexer::ParameterClose() { return MatchWS(')', ir::Token::ParameterClose); }
  bool Lexer::ScopeOpen() { return MatchWS('{', ir::Token::ScopeOpen); }
  bool Lexer::ScopeClose() { return MatchWS('}', ir::Token::ScopeClose); }
  bool Lexer::TupleOpen() { return MatchWS('(', ir::Token::TupleOpen); }
  bool Lexer::TupleClose() { return MatchWS(')', ir::Token::TupleClose); }
  bool Lexer::ArrayOpen() { return MatchWS('[', ir::Token::ArrayOpen); }
  bool Lexer::ArrayClose() { return MatchWS(']', ir::Token::ArrayClose); }
  bool Lexer::ObjectOpen() { return MatchWS('{', ir::Token::ObjectOpen); }
  bool Lexer::ObjectClose() { return MatchWS('}', ir::Token::ObjectClose); }
  bool Lexer::EnumOpen() { return MatchWS('<', ir::Token::EnumOpen); }
  bool Lexer::EnumClose() { return MatchWS('>', ir::Token::EnumClose); }

  bool Lexer::CharacterOpen() { return Match('\'', ir::Token::CharacterOpen); }
  bool Lexer::CharacterClose() { return MatchWS('\'', ir::Token::CharacterClose); }
  bool Lexer::StringOpen() { return Match('"', ir::Token::StringOpen); }
  bool Lexer::StringClose() { return MatchWS('"', ir::Token::StringClose); }
  bool Lexer::TemplateStringOpen() { return Match('`', ir::Token::TemplateStringOpen); }
  bool Lexer::TemplateStringClose() { return MatchWS('`', ir::Token::TemplateStringClose); }
  bool Lexer::TemplateStringExpressionOpen() { return MatchWS('{', ir::Token::TemplateStringExpressionOpen); }
  bool Lexer::TemplateStringExpressionClose() { return Match('}', ir::Token::TemplateStringExpressionClose); }
  bool Lexer::ConditionOpen() { return MatchWS('(', ir::Token::ConditionOpen); }
  bool Lexer::ConditionClose() { return MatchWS(')', ir::Token::ConditionClose); }
  bool Lexer::Pipeline() { return MatchWS("->", ir::Token::Pipeline); }
  bool Lexer::Arrow() { return MatchWS("=>", ir::Token::Arrow); }
  bool Lexer::Wildcard() { return MatchWS('*', ir::Token::Wildcard); }
  bool Lexer::Comma() { return MatchWS(',', ir::Token::Comma); }
  bool Lexer::Semicolon() { return MatchWS(';', ir::Token::Semicolon); }
  bool Lexer::Path() { return Match('/', ir::Token::Path); }
  bool Lexer::TypeStart() { return MatchWS(':', ir::Token::TypeStart); }
  bool Lexer::Initialize() { return MatchWS('=', ir::Token::Initialize); }
  bool Lexer::CommentOpen() { return Match("//", ir::Token::CommentOpen); }
  bool Lexer::CommentClose() { return Emit(ir::Token::CommentClose) && (WS() || true); } // Consume WS?
  bool Lexer::MultiLineCommentOpen() { return Match("/*", ir::Token::MultiLineCommentOpen); }
  bool Lexer::MultiLineCommentClose() { return MatchWS("*/", ir::Token::MultiLineCommentClose); }

  // Unary prefix operators
  bool Lexer::ReferenceOf() { return MatchWS('&', ir::Token::ReferenceOf); }
  bool Lexer::MutableReferenceOf() { return MatchWS('*', ir::Token::MutableReferenceOf); }
  bool Lexer::SymbolOf() { return MatchWS('$', ir::Token::SymbolOf); }
  bool Lexer::CopyOf() { return MatchWS('@', ir::Token::CopyOf); }
  bool Lexer::LengthOf() { return MatchWS('#', ir::Token::LengthOf); }
  bool Lexer::Positive() { return MatchWS('+', ir::Token::Positive); }
  bool Lexer::Negative() { return MatchWS('-', ir::Token::Negative); }
  bool Lexer::Increment() { return MatchWS("++", ir::Token::Increment); }
  bool Lexer::Decrement() { return MatchWS("--", ir::Token::Decrement); }
  bool Lexer::Not() { return MatchWS('!', ir::Token::Not); }
  bool Lexer::Spread() { return MatchWS("...", ir::Token::Spread); }
  bool Lexer::BitwiseNot() { return MatchWS('~', ir::Token::BitwiseNot); }

  // Unary postfix operators
  bool Lexer::Guard() { return MatchWS('?', ir::Token::Guard); }

  bool Lexer::UnaryPrefixOperatorHelper() {
    switch (cursor.Peek()) {
      case '+': return Increment() || Positive();
      case '-': return Decrement() || Negative();
      case '*': return MutableReferenceOf();
      case '&': return ReferenceOf();
      case '$': return SymbolOf();
      case '#': return LengthOf();
      case '@': return CopyOf();
      case '!': return Not();
      case '.': return Spread();
      case '~': return BitwiseNot();
      case 'a': return Await() || Assume();
      case 'e': return Expect();
      case 'r': return Runtime();
      case 'c': return Comtime();
      default: return false;
    }
  }

  bool Lexer::UnaryPostfixOperatorHelper() {
    switch (cursor.Peek()) {
      case '?': return Guard();

      // Strictly speaking, postfix calls are classified as binary operations with an implicit operator
      // But syntactically it's better to just treat them as postfix unary operators
      case '\'': return CharacterLiteral();
      case '"': return StringLiteral();
      case '(': return ParameterFunctionLiteral() || TupleLiteral();
      case '[': return CaptureFunctionLiteral() || ArrayLiteral();
      case '{': return ObjectLiteral();
      case '<': return EnumLiteral();
      default: return false;
    }
  }

  bool Lexer::ModifierHelper() {
    switch (cursor.Peek()) {
      // case 'l': return Let();
      case 'c': return Const();
      case 'm': return Mut();
      case 'p': return Private() || Public() || Protected();
      default: return false;
    }
  }

  bool Lexer::UnaryPrefixOperator() { return IsUnaryPrefixStart() && UnaryPrefixOperatorHelper(); }
  bool Lexer::UnaryPostfixOperator() { return IsUnaryPostfixStart() && UnaryPostfixOperatorHelper(); }
  bool Lexer::Modifier() { return IsModifierStart() && ModifierHelper(); }
  bool Lexer::Modifiers() { return OneOrMore([&](){ return Modifier(); }); }

  bool Lexer::TrueLiteral() { return True(); }
  bool Lexer::FalseLiteral() { return False(); }
  bool Lexer::ThisLiteral() { return This(); }
  bool Lexer::ThatLiteral() { return That(); }
  bool Lexer::NullLiteral() { return Null(); }
  bool Lexer::ReadonlyLiteral() { return Readonly(); }
  bool Lexer::UndefinedLiteral() { return Undefined(); }

  bool Lexer::Escape() {
    if (cursor.Done() || !cursor.Match('\\')) {
      return false; // not an escape start
    }

    if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

    const char c = cursor.Peek();

    auto emit_ascii = [&](char32_t cp){
      Emit(ir::Token::EscapeASCII);
      PushCharacter(cp);
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
      case '{': cursor.Advance(1); return emit_ascii(U'{');
      case '}': cursor.Advance(1); return emit_ascii(U'}');
      case 'x': {
        // \xHH: exactly two hex digits
        cursor.Advance(1);
        if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

        uint32_t value = 0;
        for (int i = 0; i < 2; ++i) {
          if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

          int nib = compiler::text::Convert::ToHex(cursor.Peek());
          if (nib < 0) return Error(ir::Error::EscapeSequenceExpectedTwoHexDigits);

          value = (value << 4) | static_cast<uint32_t>(nib);
          cursor.Advance(1);
        }
        
        Emit(ir::Token::EscapeHex);
        PushCharacter(static_cast<char32_t>(value));

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
            int nib = compiler::text::Convert::ToHex(cursor.Peek());
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
          if (!cursor.Done() && compiler::text::Convert::ToHex(cursor.Peek()) >= 0) {
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

          PushCharacter(static_cast<char32_t>(value));
          return true;
        } else {
          // \uXXXX short form
          uint32_t value = 0;
          for (int i = 0; i < 4; ++i) {
            if (cursor.Done()) return Error(ir::Error::EscapeSequenceUnexpectedEndOfInput);

            int nib = compiler::text::Convert::ToHex(cursor.Peek());
            if (nib < 0) return Error(ir::Error::EscapeSequenceExpectedFourHexDigits);

            value = (value << 4) | static_cast<uint32_t>(nib);
            cursor.Advance(1);
          }

          if (value >= 0xD800u && value <= 0xDFFFu) {
            return Error(ir::Error::UnicodeSurrogateCodePointIsNotAValidScalarValue);
          }

          Emit(ir::Token::EscapeUnicode);
          PushCharacter(static_cast<char32_t>(value));
          return true;
        }
      }
      default: {
        cursor.Advance(1); // Still consume it
        PushCharacter(static_cast<unsigned char>(c)); // I *think* we still want to capture it as part of the string/identifier
        return Error(ir::Error::EscapeSequenceUnknown);
      }
    }
  }

  bool Lexer::CharacterLiteral() {
    if (!CharacterOpen()) return false;

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
      PushCharacter(cp);
    }

    if (!CharacterClose()) return Error(ir::Error::CharacterLiteralExpectedClosingSingleQuote);

    WS(); // Consume trailing whitespace
    return true;
  }

  bool Lexer::StringLiteral() {
    const size_t open_token = tokens.size(); // Save this in case it's a template string
    if (!StringOpen()) return false;

    uint32_t run_count = 0;
    while (true) {
      if (cursor.Done()) return Error(ir::Error::StringLiteralExpectedClosingDoubleQuote);
      if (cursor.Peek() == '"') break;

      if (cursor.Peek() == '\\') {
        // flush current run before the escape
        EmitCharactersNibbles(run_count);
        run_count = 0;
        if (!Escape()) return Error(ir::Error::StringLiteralExpectedEscapeAfterBackslash);
        continue; // do not include escape in run; it implicitly represents Characters1
      }

      // Embedded expression
      if (cursor.Peek() == '{') {
        tokens[open_token] = ir::Token::TemplateStringOpen; // convert to template string
        EmitCharactersNibbles(run_count);
        run_count = 0;
        if (!TemplateStringExpressionOpen()) return Error(ir::Error::TemplateStringLiteralExpectedExpression);
        if (!Expression()) return Error(ir::Error::TemplateStringLiteralExpectedExpression);
        if (!TemplateStringExpressionClose()) return Error(ir::Error::TemplateStringLiteralExpectedClosingBrace);
        continue;
      }

      char32_t cp = cursor.CodePoint();
      if (cp == U'\n' || cp == U'\r' || cp == U'\u2028' || cp == U'\u2029') {
        // Line breaks inside template strings are explicit tokens and terminate runs
        EmitCharactersNibbles(run_count);
        run_count = 0;

        if (cp == U'\r') {
          if (cursor.Peek() == '\n') {
            cursor.Advance(1); // consume LF to make CRLF
            Emit(ir::Token::CarriageReturnLineFeed);
            PushCharacter(U'\r');
            PushCharacter(U'\n');
          } else {
            Emit(ir::Token::CarriageReturn);
            PushCharacter(U'\r');
          }
        } else if (cp == U'\n') {
          Emit(ir::Token::LineFeed);
          PushCharacter(U'\n');
        } else if (cp == U'\u2028') { // Line Separator
          Emit(ir::Token::LineSeparator);
          PushCharacter(U'\u2028');
        } else { // U'\u2029' Paragraph Separator
          Emit(ir::Token::ParagraphSeparator);
          PushCharacter(U'\u2029');
        }
        continue;
      }

      PushCharacter(cp);
      ++run_count;
    }

    // flush trailing run
    EmitCharactersNibbles(run_count);

    if (!StringClose()) return Error(ir::Error::StringLiteralExpectedClosingDoubleQuote);
    if (tokens[open_token] == ir::Token::TemplateStringOpen) {
      tokens[tokens.size() - 1] = ir::Token::TemplateStringClose; // convert close token
    }

    WS(); // Consume trailing whitespace
    return true;
  }
  
  /****
   * Integer literals: value + format-mask strategy
   *
   * - The lexer fully parses integer literals into a BigInt value stored in the `data`
   *   side-table (little-endian uint32_t limbs).
   * - The token stream preserves literal spelling losslessly (except hex digit case)
   *   by emitting a *format mask* instead of digit-value tokens.
   *
   * Format mask
   * - Each contiguous run of digit characters in source is represented by its run length.
   * - Run length is encoded as big-endian base-16 nibbles using Digits0..DigitsF tokens.
   * - Underscores in the source are emitted as Underscore tokens at the exact positions.
   * - Base is communicated by an optional prefix token: HexStart / OctalStart / BinaryStart.
   *   No prefix implies decimal.
   *
   * Limb metadata
   * - The lexer appends the literal's limbs to `data`.
   * - The lexer emits: Limbs, then a Digits* run encoding
   *   the total limb count (including the first limb) as big-endian hex nibbles.
   * - If Limbs is absent, the limb count is implicitly 1.
   * - Floating-point literals include the two exponent limbs in the limb count.
   */

  namespace {
    void BigIntMulAddLE(std::vector<uint32_t>& le, uint32_t base, uint32_t digit) {
      uint64_t carry = digit;
      for (size_t i = 0; i < le.size(); ++i) {
        uint64_t total = static_cast<uint64_t>(le[i]) * static_cast<uint64_t>(base) + carry;
        le[i] = static_cast<uint32_t>(total & 0xFFFFFFFFu);
        carry = total >> 32u;
      }
      if (carry != 0) {
        le.push_back(static_cast<uint32_t>(carry));
      }
    }

    void TrimLE(std::vector<uint32_t>& le) {
      while (le.size() > 1 && le.back() == 0u) {
        le.pop_back();
      }
    }
  } // namespace

  bool Lexer::HexLiteral() {
    cursor.Advance(2);
    Emit(ir::Token::HexStart);

    temp_le.clear();
    temp_le.push_back(0u);

    uint32_t run_length = 0;
    size_t digits_seen = 0;
    bool leading_separator = false;
    bool last_was_separator = false;
    bool invalid_digit = false;

    while (!cursor.Done()) {
      char c = cursor.Peek();
      if (c == '_') {
        if (digits_seen == 0) {
          leading_separator = true;
        }
        if (run_length > 0) {
          EmitDigitsNibbles(run_length);
          run_length = 0;
        }
        cursor.Advance();
        Emit(ir::Token::Underscore);
        last_was_separator = true;
        continue;
      }

      if (IsAlNum() && !IsHex()) {
        invalid_digit = true;
        break;
      }

      if (!IsHex()) {
        break;
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
      ++run_length;
      ++digits_seen;
      last_was_separator = false;
      BigIntMulAddLE(temp_le, 16u, digit);
    }

    if (run_length > 0) {
      EmitDigitsNibbles(run_length);
    }

    if (invalid_digit) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralInvalidHexDigit);
      EmitDigitsNibbles(Repeat([&]{ return IsHex() || IsDigit(); }));
      WS();
      return false;
    }

    if (digits_seen == 0) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralExpectedDigit);
      EmitDigitsNibbles(Repeat([&]{ return IsHex() || IsDigit(); }));
      WS();
      return false;
    }

    if (leading_separator || last_was_separator) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralSeparatorCannotLeadOrTrail);
      EmitDigitsNibbles(Repeat([&]{ return IsHex() || IsDigit(); }));
      WS();
      return false;
    }

    TrimLE(temp_le);

    const size_t limb_count = temp_le.size();
    if (limb_count > 0) {
      Emit(ir::Token::Limbs);
      EmitDigitsNibbles(static_cast<uint32_t>(limb_count));
    }
    for (size_t i = 0; i < limb_count; ++i) {
      PushLimb(temp_le[i]);
    }

    temp_le.clear();

    Identifier() || WS(); // Consume a postfix identifier or trailing whitespace
    return true;
  }

  bool Lexer::OctalLiteral() {
    cursor.Advance(2);
    Emit(ir::Token::OctalStart);

    temp_le.clear();
    temp_le.push_back(0u);

    uint32_t run_length = 0;
    size_t digits_seen = 0;
    bool leading_separator = false;
    bool last_was_separator = false;
    bool invalid_digit = false;

    while (!cursor.Done()) {
      char c = cursor.Peek();
      if (c == '_') {
        if (digits_seen == 0) {
          leading_separator = true;
        }
        if (run_length > 0) {
          EmitDigitsNibbles(run_length);
          run_length = 0;
        }
        cursor.Advance();
        Emit(ir::Token::Underscore);
        last_was_separator = true;
        continue;
      }

      if (IsDigit() && !IsOctal()) {
        invalid_digit = true;
        break;
      }

      if (!IsOctal()) break;

      uint32_t digit = static_cast<uint32_t>(c - '0');
      cursor.Advance();
      ++run_length;
      ++digits_seen;
      last_was_separator = false;
      BigIntMulAddLE(temp_le, 8u, digit);
    }

    if (run_length > 0) {
      EmitDigitsNibbles(run_length);
    }

    if (invalid_digit) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralInvalidOctalDigit);
      EmitDigitsNibbles(Repeat([&]{ return IsDigit(); }));
      WS();
      return false;
    }

    if (digits_seen == 0) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralExpectedDigit);
      EmitDigitsNibbles(Repeat([&]{ return IsDigit(); }));
      WS();
      return false;
    }

    if (leading_separator || last_was_separator) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralSeparatorCannotLeadOrTrail);
      EmitDigitsNibbles(Repeat([&]{ return IsDigit(); }));
      WS();
      return false;
    }

    TrimLE(temp_le);

    const size_t limb_count = temp_le.size();
    if (limb_count > 0) {
      Emit(ir::Token::Limbs);
      EmitDigitsNibbles(static_cast<uint32_t>(limb_count));
    }
    for (size_t i = 0; i < limb_count; ++i) {
      PushLimb(temp_le[i]);
    }

    temp_le.clear();

    Identifier() || WS(); // Consume a postfix identifier or trailing whitespace
    return true;
  }

  bool Lexer::BinaryLiteral() {
    cursor.Advance(2);
    Emit(ir::Token::BinaryStart);

    temp_le.clear();
    temp_le.push_back(0u);

    uint32_t run_length = 0;
    size_t digits_seen = 0;
    bool leading_separator = false;
    bool last_was_separator = false;
    bool invalid_digit = false;

    while (!cursor.Done()) {
      char c = cursor.Peek();
      if (c == '_') {
        if (digits_seen == 0) {
          leading_separator = true;
        }
        if (run_length > 0) {
          EmitDigitsNibbles(run_length);
          run_length = 0;
        }
        cursor.Advance();
        Emit(ir::Token::Underscore);
        last_was_separator = true;
        continue;
      }

      if (IsDigit() && !IsBinary()) {
        invalid_digit = true;
        break;
      }

      if (!IsBinary()) break;
      uint32_t digit = static_cast<uint32_t>(c - '0');
      cursor.Advance();
      ++run_length;
      ++digits_seen;
      last_was_separator = false;
      BigIntMulAddLE(temp_le, 2u, digit);
    }

    if (run_length > 0) {
      EmitDigitsNibbles(run_length);
    }

    if (invalid_digit) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralInvalidBinaryDigit);
      EmitDigitsNibbles(Repeat([&]{ return IsDigit(); }));
      WS();
      return false;
    }

    if (digits_seen == 0) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralExpectedDigit);
      EmitDigitsNibbles(Repeat([&]{ return IsDigit(); }));
      WS();
      return false;
    }

    if (leading_separator || last_was_separator) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralSeparatorCannotLeadOrTrail);
      EmitDigitsNibbles(Repeat([&]{ return IsDigit(); }));
      WS();
      return false;
    }

    TrimLE(temp_le);

    const size_t limb_count = temp_le.size();
    if (limb_count > 0) {
      Emit(ir::Token::Limbs);
      EmitDigitsNibbles(static_cast<uint32_t>(limb_count));
    }
    for (size_t i = 0; i < limb_count; ++i) {
      PushLimb(temp_le[i]);
    }

    temp_le.clear();

    Identifier() || WS(); // Consume a postfix identifier or trailing whitespace
    return true;
  }

  bool Lexer::DecimalLiteral() {
    // Decimal literals must begin with a digit. If we start with an underscore, this isn't
    // a decimal literal at all (it may be an identifier), so do not consume anything.
    if (!cursor.Done() && cursor.Peek() == '_') {
      return false;
    }

    bool consumed = false;

    enum class Stage { Integer, Fraction, Exponent };
    Stage stage = Stage::Integer;

    temp_le.clear();
    temp_le.push_back(0u); // every numeric literal has at least one limb

    size_t fractional_digits = 0;

    bool seen_fraction = false;
    bool seen_exponent = false;
    bool exponent_negative = false;
    uint64_t exponent_abs = 0;
    bool exponent_overflow = false;

    bool last_was_separator = false;

    while (!cursor.Done()) {
      char c = cursor.Peek();

      if (IsDigit()) {
        uint32_t run_length = 0;

        while (!cursor.Done() && IsDigit()) {
          uint32_t digit = static_cast<uint32_t>(cursor.Peek() - '0');
          cursor.Advance();
          ++run_length;

          switch (stage) {
            case Stage::Integer:
              BigIntMulAddLE(temp_le, 10u, digit);
              break;
            case Stage::Fraction:
              BigIntMulAddLE(temp_le, 10u, digit);
              fractional_digits += 1;
              break;
            case Stage::Exponent:
              if (!exponent_overflow) {
                if (exponent_abs > (std::numeric_limits<uint64_t>::max() - digit) / 10ull) {
                  exponent_overflow = true;
                } else {
                  exponent_abs = exponent_abs * 10ull + digit;
                }
              }
              break;
          }
        }

        EmitDigitsNibbles(run_length);
        consumed = true;
        last_was_separator = false;
        continue;
      }

      if (c == '_') {
        if (!consumed) {
          // Not a decimal literal (identifiers can start with '_').
          temp_le.clear();
          return false;
        }

        cursor.Advance();
        Emit(ir::Token::Underscore);
        last_was_separator = true;
        continue;
      }

      if (stage != Stage::Exponent && c == '.') {
        // A decimal point must be followed by a digit or an exponent marker; otherwise,
        // leave the dot for other lexing paths (e.g., range or member access operators).
        char next = cursor.Peek(1);
        if (!IsDigit(next) && next != 'e' && next != 'E') break;
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
            Emit(ir::Token::Positive);
          } else if (cursor.Peek() == '-') {
            cursor.Advance();
            Emit(ir::Token::Negative);
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

    // Once we've consumed digits, trailing separators are illegal.
    if (last_was_separator) {
      temp_le.clear();
      Error(ir::Error::NumericLiteralSeparatorCannotLeadOrTrail);
      EmitDigitsNibbles(Repeat([&]{ return IsDigit(); }));
      WS();
      return false;
    }

    TrimLE(temp_le);

    const size_t mantissa_limb_count = temp_le.size();

    bool need_exponent_limbs = seen_fraction || seen_exponent;
    const size_t total_limb_count = mantissa_limb_count + (need_exponent_limbs ? 2 : 0);
    if (total_limb_count > 0) {
      Emit(ir::Token::Limbs);
      EmitDigitsNibbles(static_cast<uint32_t>(total_limb_count));
    }
    for (size_t i = 0; i < mantissa_limb_count; ++i) {
      PushLimb(temp_le[i]);
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
      PushLimb(static_cast<uint32_t>(exponent_bits & 0xFFFFFFFFu));
      PushLimb(static_cast<uint32_t>(exponent_bits >> 32u));
    }

    temp_le.clear();

    Identifier() || WS(); // Consume a postfix identifier or trailing whitespace
    return true;
  }

  // NumberLiteral(): Dispatch entry for numeric literals.
  // - Recognizes number starts as digits only (no leading '+'/'-'; those are unary operators).
  // - If the run begins with 0x/0X, 0o/0O, or 0b/0B, dispatch to HexLiteral/OctalLiteral/BinaryLiteral respectively.
  // - Otherwise, parse as DecimalLiteral().
  bool Lexer::NumberLiteral() {
    if (!IsNumberStart()) {
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
  // for escapes. Appends every consumed code point to `characters`.
  bool Lexer::Identifier() {
    // Must begin at a possible identifier start (ASCII quick check, escape, or non-ASCII)
    if (!IsPossibleIdentifierStart()) return false;

    auto is_identifier_code_point = [&](char32_t cp, bool is_first) {
      if (is_first) {
        return compiler::text::Unicode::IsIdentifierStart(cp) || cp == U'_';
      }
      return compiler::text::Unicode::IsIdentifierContinue(cp);
    };

    bool first = true;
    bool consumed_any = false;
    uint32_t run_count = 0;

    while (!cursor.Done()) {
      if (cursor.Peek() == '\\') {
        // Flush any current run, then consume escape.
        if (run_count > 0) { EmitCharactersNibbles(run_count); run_count = 0; }
        if (!Escape()) return false; // escape already reports its own errors

        // Escapes are legal in identifiers only if they decode to a valid start/continue code point.
        if (data.empty()) {
          return Error(ir::Error::DeclarationExpectedIdentifier);
        }

        char32_t escaped_cp = static_cast<char32_t>(data.back());
        if (!is_identifier_code_point(escaped_cp, first)) {
          return Error(ir::Error::DeclarationExpectedIdentifier);
        }

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
        bool ok = is_identifier_code_point(cp, first);
        if (!ok) {
          // Not a valid identifier character; undo and stop.
          cursor.Retreat(it);
          break;
        }
        PushCharacter(cp);
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
      PushCharacter(cp);
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

    if (FunctionHeader()) {
      if (!Arrow()) return Error(ir::Error::ArrowFunctionExpectedArrowAfterFunctionHeader);
    } else {
      // Check for inline function arrow '=>'
      if (!Arrow()) return true;
    }

    if (Expression()) {
      // A special zero width marker to tell the parser the identifier is an arrow function parameter
      tokens.insert(tokens.begin() + start, ir::Token::ArrowHead);

      return true;
    } else {
      return Error(ir::Error::ArrowFunctionExpectedExpression);
    }
  }

  bool Lexer::BinaryOperator(bool in_enum, bool in_type) {
    if (!IsBinaryStart()) return false;

    switch (cursor.Peek()) {
      case '!': {
        switch (cursor.Peek(1)) {
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
      case '&': {
        switch (cursor.Peek(1)) {
          case '&': {
            switch (cursor.Peek(2)) {
              case '=': return EmitAndAdvance(ir::Token::AssignAnd, 3);
              default:  return EmitAndAdvance(ir::Token::And, 2);
            }
          }
          case '=': return EmitAndAdvance(ir::Token::BitwiseAssignAnd, 2);
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
          case '.': return EmitAndAdvance(ir::Token::Range, 2);
          default: return EmitAndAdvance(ir::Token::MemberReference, 1);
        }
      }
      case ':': {
        switch (cursor.Peek(1)) {
          case ':': return EmitAndAdvance(ir::Token::StaticMemberReference, 2);
          default: return EmitAndAdvance(ir::Token::MutableMemberReference, 1);
        }
      }
      case 't': {
        return Keyword("to", ir::Token::To);
      }
      default: return false;
    }
  }

  bool Lexer::FunctionHeader() { return Const(); } // Currently just 'const' modifier
  bool Lexer::FunctionBody() { return (FunctionHeader() || true) && (ArrowFunction() || Block()); }
  bool Lexer::ParameterFunctionLiteral() { return Try([&]{ return ParameterList() && FunctionBody(); }); }
  bool Lexer::CaptureFunctionLiteral() { return Try([&]{ return CaptureList() && (ParameterList() || true) && FunctionBody(); }); }

  bool Lexer::EnumLiteral() {
    if (!EnumOpen()) return false;

    // Parse one or more expressions separated by commas; trailing comma allowed.
    bool matched = OneOrMore([&]{ return Expression(true, false); }, [&]{ return Comma(); });

    if (cursor.Peek() != '>') {
      if (cursor.Peek() == ',') {
        Error(ir::Error::EnumExpectedExpression);
        Comma();
      } else if (matched) {
        Error(ir::Error::EnumExpectedCommaOrClosingAngleBracket);
      } else {
        Error(ir::Error::EnumExpectedExpression);
      }
      
      // Tolerant scan: harvest exprs and commas until we hit '>' or ';' or something else.
      ZeroOrMore([&]{ return Expression(true, false) || Comma(); });
    }

    if (!EnumClose()) return Error(ir::Error::EnumExpectedClosingAngleBracket);
    return true;
  }

  bool Lexer::ObjectLiteral() {
    if (!ObjectOpen()) return false;

    auto declaration = [&]{
      if (Let()) {
        return StatementModeDeclaration();
      } else {
        return PropertyModeDeclaration();
      }
    };

    // Parse one or more declarations separated by commas; trailing comma allowed.
    bool matched = OneOrMore(
      declaration,
      [&]{ return Comma(); }
    );

    if (cursor.Peek() != '}') {
      if (cursor.Peek() == ',') {
        Error(ir::Error::ObjectExpectedDeclaration);
        Comma();
      } else if (matched) {
        Error(ir::Error::ObjectExpectedCommaOrClosingCurlyBrace);
      } else {
        Error(ir::Error::ObjectExpectedDeclaration);
      }

      // Tolerant scan: harvest declarations and commas until we hit '}' or ';' or something else.
      ZeroOrMore([&]{ return declaration() || Comma(); });
    }

    if (!ObjectClose()) return Error(ir::Error::ObjectExpectedClosingCurlyBrace);
    return true;
  }

  bool Lexer::ArrayLiteral() {
    if (!ArrayOpen()) return false;

    bool matched = false;

    if (Expression()) {
      if (Semicolon()) {
        if (!Expression()) return Error(ir::Error::ArrayExpectedCountExpression);
        matched = true;
      } else if (Comma()) {
        // Parse one or more expressions separated by commas; trailing comma allowed.
        matched = OneOrMore([&]{ return Expression(); }, [&]{ return Comma(); });
      }
    }

    if (cursor.Peek() != ']') {
      if (cursor.Peek() == ',') {
        Error(ir::Error::ArrayExpectedExpression);
        Comma();
      } else if (matched) {
        Error(ir::Error::ArrayExpectedCommaOrClosingBracket);
      } else {
        Error(ir::Error::ArrayExpectedExpression);
      }

      // Tolerant scan: harvest exprs and commas until we hit ']' or ';' or something else.
      ZeroOrMore([&]{ return Expression() || Comma(); });
    }

    if (!ArrayClose()) return Error(ir::Error::ArrayExpectedClosingBracket);
    return true;
  }

  bool Lexer::TupleLiteral() {
    if (!TupleOpen()) return false;

    // Parse one or more expressions separated by commas; trailing comma allowed.
    bool matched = OneOrMore([&]{ return Expression(); }, [&]{ return Comma(); });

    if (cursor.Peek() != ')') {
      if (cursor.Peek() == ',') {
        Error(ir::Error::TupleExpectedExpression);
        Comma();
      } else if (matched) {
        // We parsed an element, so at this point we expected ',' or ')'.
        Error(ir::Error::TupleExpectedCommaOrClosingParenthesis);
      } else {
        // We couldn't even parse the first element.
        Error(ir::Error::TupleExpectedExpression);
      }

      // Tolerant scan: harvest exprs and commas until we hit ')' or ';' or something else.
      ZeroOrMore([&]{ return Expression() || Comma(); });
    }

    if (!TupleClose()) return Error(ir::Error::TupleExpectedClosingParenthesis);
    return true;
  }

  bool Lexer::EscapePercent() {
    if (!cursor.Match('%')) return false;

    Emit(ir::Token::EscapePercent);

    // Expect two hex digits
    char high = cursor.Peek();
    char low = cursor.Peek(1);
    if (!IsHex(high) || !IsHex(low)) {
      return Error(ir::Error::EscapeInvalidPercentEncoding);
    }

    cursor.Advance(2);

    uint8_t byte = static_cast<uint8_t>((compiler::text::Convert::ToHex(high) << 4) | compiler::text::Convert::ToHex(low));
    PushCharacter(static_cast<char32_t>(byte));

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
    Host();
    // if (!Host()) return Error(ir::Error::SpecifierExpectedHost);

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
    while (!cursor.Done() && Path()) {
      // Capture sequences of `IsURLSegment` characters
      CaptureCharacters([&]{ return IsURLSegment(); }, [&]{ return EscapeURL(); });
    }

    return true; // path can be empty
  }

  bool Lexer::PathAbsolute() {
    if (!Path()) return false;

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

  bool Lexer::OperandShortcut() {
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
      case 'a': return Auto();
      case 'n': return NullLiteral();
      case 'u': return UndefinedLiteral();
      case '\'': return CharacterLiteral();
      case '"': return StringLiteral();
      case '(': return ParameterFunctionLiteral() || TupleLiteral();
      case '[': return CaptureFunctionLiteral() || ArrayLiteral();
      case '<': return EnumLiteral();
      case '{': return ObjectLiteral();
      case 'd': return DoExpression();
      case 'i': return IfExpression();
      case 't': return TrueLiteral() || ThisLiteral() || ThatLiteral() ||TryExpression();
      case 'l': return LoopExpression();
      case 'w': return WhileExpression();
      case 'r': return RepeatExpression() || ReadonlyLiteral();
      case 'f': return FalseLiteral() || ForExpression();
      default: return false;
    }
  }

  bool Lexer::Operand() { return (IsOperandStart() && OperandShortcut()) || IdentifierOrArrowFunction(); }

  bool Lexer::Expression(bool in_enum, bool in_type) {
    if (UnaryPrefixOperator()) {
      if (!Expression(in_enum, in_type)) {
        return Error(ir::Error::UnaryPrefixOperatorExpectedExpression);
      } else {
        return true;
      }
    }

    if (!Operand()) {
      return false;
    }

    while (UnaryPostfixOperator()) {}

    if (BinaryOperator(in_enum, in_type)) {
      if (!Expression(in_enum, in_type)) {
        Error(ir::Error::BinaryOperatorExpectedExpression);

        // Attempt to recover from double binary operators
        ZeroOrMore([&]{ return BinaryOperator(in_enum, in_type) || Expression(in_enum, in_type); });

        return false;
      }
    }

    return true;
  }

  bool Lexer::DefaultExpression() {
    if (!Default()) return false;

    // `default` considers the parentheses mandatory for clarity.
    if (!Condition()) {
      return Error(ir::Error::DefaultExpressionExpectedCondition);
    }

    return true;
  }

  bool Lexer::DoExpression() {
    if (!DoStatement()) return false;
    DefaultExpression();

    return true;
  }

  bool Lexer::IfExpression() {
    if (!IfStatement()) return false;
    DefaultExpression();

    return true;
  }

  bool Lexer::TryExpression() {
    if (!TryStatement()) return false;
    DefaultExpression();

    return true;
  }

  bool Lexer::LoopExpression() {
    if (!LoopStatement()) return false;
    DefaultExpression();

    return true;
  }

  bool Lexer::WhileExpression() {
    if (!WhileStatement()) return false;
    DefaultExpression();

    return true;
  }

  bool Lexer::RepeatExpression() {
    if (!RepeatStatement()) return false;
    DefaultExpression();

    return true;
  }

  bool Lexer::ForExpression() {
    if (!ForStatement()) return false;
    DefaultExpression();

    return true;
  }

  bool Lexer::ExpressionStatement() {
    if (!Expression()) {
      return Semicolon(); // allow empty expression statements
    }

    if (!Semicolon()) return Error(ir::Error::ExpressionStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::StatementModeDeclaration() {
    // If there was a `let` it will have already been handled by the caller
    ZeroOrMore([&]{ return Modifier(); });

    if (!Destructure() && !Identifier()) {
      return Error(ir::Error::DeclarationExpectedIdentifier);
    }

    TypeExpression(); // Optional

    if (!InitializerExpression()) {
      return Error(ir::Error::DeclarationExpectedInitializer);
    }

    return true;
  }

  bool Lexer::ParameterModeDeclaration() {
    Let(); // Optional, but if present must be first

    ZeroOrMore([&]{ return Modifier(); });

    if (Spread() || ReferenceOf() || MutableReferenceOf() || CopyOf() || SymbolOf()) {
      if (Identifier()) {
        TypeExpression(); // Optional
      }

      return true; // Rest parameters can be captured with just `...name`
    }

    if (!Destructure() && !Identifier()) {
      return false;
    }

    TypeExpression(); // Optional
    InitializerExpression(); // TODO: Should I allow defaults?

    return true;
  }

  bool Lexer::PropertyModeDeclaration() {
    if (Let()) {
      // Property mode can swap to statement mode with the `let` keyword
      return StatementModeDeclaration();
    }

    ZeroOrMore([&]{ return Modifier(); });

    if (Identifier() || EnumLiteral()) {
      TypeExpression(); // Optional

      if (!InitializerExpression()) {
        return Error(ir::Error::DeclarationExpectedInitializer);
      }

      return true;
    }

    return PrefixedExpression();
  }

  bool Lexer::CaptureModeDeclaration() {
    if (Let()) {
      // Capture mode can swap to statement mode with the `let` keyword
      return StatementModeDeclaration();
    }

    ZeroOrMore([&]{ return Modifier(); });

    if (ThisLiteral() || ThatLiteral()) {
      return true; // `this` and `that` are allowed capture names without type or initializer
    }

    if (Identifier()) {
      TypeExpression(); // Optional

      return true;
    }

    return PrefixedExpression();
  }

  bool Lexer::LoopModeDeclaration() {
    Let(); // Optional, but if present must be first

    ZeroOrMore([&]{ return Modifier(); }); // Optional

    if (!Identifier()) return false;
    TypeExpression(); // Optional

    return true;
  }

  bool Lexer::DestructureTuple() {
    if (!TupleOpen()) return false;

    // Parse one or more expressions separated by commas; trailing comma allowed.
    ZeroOrMore([&]{ return DestructureTuple() || (Spread() && Identifier()) || Identifier(); }, [&]{ return Comma(); });

    if (!TupleClose()) return Error(ir::Error::TupleExpectedClosingParenthesis);
    return true;
  }

  bool Lexer::DestructureArray() {
    if (!ArrayOpen()) return false;

    // Parse one or more expressions separated by commas; trailing comma allowed.
    ZeroOrMore([&]{ return DestructureArray() || (Spread() && Identifier()) || Identifier(); }, [&]{ return Comma(); });

    if (!ArrayClose()) return Error(ir::Error::ArrayExpectedClosingBracket);
    return true;
  }

  bool Lexer::DestructureEnum() {
    if (!EnumOpen()) return false;

    // Parse one or more expressions separated by commas; trailing comma allowed.
    ZeroOrMore([&]{ return DestructureEnum() || (Spread() && Identifier()) || Identifier(); }, [&]{ return Comma(); });

    if (!EnumClose()) return Error(ir::Error::EnumExpectedClosingAngleBracket);
    return true;
  }

  bool Lexer::DestructureObject() {
    if (!ObjectOpen()) return false;

    // Parse one or more expressions separated by commas; trailing comma allowed.
    ZeroOrMore([&]{
      if (Identifier()) {
        if (As()) {
          return DestructureObject() || Identifier();
        }

        return true;
      } else if (Spread()) {
        return Identifier(); // Spread must be followed by an identifier to capture into
      }
      
      return false;
    }, [&]{ return Comma(); });

    if (!ObjectClose()) return Error(ir::Error::ObjectExpectedClosingCurlyBrace);
    return true;
  }

  bool Lexer::Destructure() {
    switch (cursor.Peek()) {
      case '{': return DestructureObject();
      case '[': return DestructureArray();
      case '(': return DestructureTuple();
      case '<': return DestructureEnum();
    }
    
    return false;
  }

  bool Lexer::PrefixedCopyOf() {
    return (CopyOf() && Expression()) || Error(ir::Error::CopyOfExpectedExpression);
  }

  bool Lexer::PrefixedReferenceOf() {
    return (ReferenceOf() && Expression()) || Error(ir::Error::ReferenceOfExpectedExpression);
  }

  bool Lexer::PrefixedMutableReferenceOf() {
    return (MutableReferenceOf() && Expression()) || Error(ir::Error::MutableReferenceOfExpectedExpression);
  }

  bool Lexer::PrefixedSymbolOf() {
    return (SymbolOf() && Expression()) || Error(ir::Error::SymbolOfExpectedExpression);
  }
  
  bool Lexer::PrefixedSpread() {
    return (Spread() && Expression()) || Error(ir::Error::SpreadExpectedExpression);
  }

  bool Lexer::PrefixedExpression() {
    switch (cursor.Peek()) {
      case '@': return PrefixedCopyOf();
      case '&': return PrefixedReferenceOf();
      case '*': return PrefixedMutableReferenceOf();
      case '$': return PrefixedSymbolOf();
      case '.': return PrefixedSpread();
    }
    
    return false;
  }

  bool Lexer::TypeExpression() {
    if (!TypeStart()) return false;
    
    if (!Expression(false, true)) {
      return Error(ir::Error::TypeExpectedExpression);
    }

    return true;
  }

  bool Lexer::InitializerExpression() {
    if (!Initialize()) return false;
    
    if (!Expression()) {
      return Error(ir::Error::InitializerExpectedExpression);
    }

    return true;
  }

  bool Lexer::ParameterList() {
    if (!ParameterOpen()) return false;

    ZeroOrMore([&]{ return ParameterModeDeclaration(); }, [&]{ return Comma(); });

    if (!ParameterClose()) return Error(ir::Error::ParameterListExpectedClosingParenthesis);

    return true;
  }

  bool Lexer::CaptureList() {
    if (!CaptureOpen()) return false;

    ZeroOrMore([&]{ return CaptureModeDeclaration(); }, [&]{ return Comma(); });

    if (!CaptureClose()) return Error(ir::Error::CaptureListExpectedClosingBracket);

    return true;
  }

  bool Lexer::ArrowFunction() {
    if (!Arrow()) return false;

    if (!Expression()) return Error(ir::Error::ArrowFunctionExpectedExpression);

    return true;
  }

  bool Lexer::ImportStatement() {
    if (!Import()) return false;

    Async();

    if (!Identifier() && !DestructureObject()) return Error(ir::Error::ImportExpectedIdentifier);

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

    while (!cursor.Done() && Break()) {} // Warble permits stacking `break` keywords

    if (!Semicolon()) return Error(ir::Error::BreakStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::ContinueStatement() {
    if (!Continue()) return false;

    while (!cursor.Done() && Continue()) {} // Warble permits stacking `continue` keywords

    if (!Semicolon()) return Error(ir::Error::ContinueStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::ReturnStatement() {
    if (!Return()) return false;

    Async(); // Optional, but must come first if present
    Pass() || Fail(); // Optional
    Expression(); // Optional

    if (!Semicolon()) return Error(ir::Error::ReturnStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::YieldStatement() {
    if (!Yield()) return false;

    Async(); // Optional, but must come first if present
    Pass() || Fail(); // Optional
    Expression(); // Optional

    if (!Semicolon()) return Error(ir::Error::YieldStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::PanicStatement() {
    if (!Panic()) return false;
    Expression(); // Optional

    if (!Semicolon()) return Error(ir::Error::PanicStatementExpectedSemicolon);

    return true;
  }

  bool Lexer::DoStatement() {
    if (!Do()) return false;

    if (!Block()) return Error(ir::Error::DoExpectedScopeBlock);

    return true;
  }

  bool Lexer::LoopStatement() {
    if (!Loop()) return false;

    if (!Block()) return Error(ir::Error::LoopExpectedScopeBlock);

    return true;
  }

  bool Lexer::ElseStatement() {
    if (!Else()) return false;

    if (!IfStatement() && !TryStatement()) {
      if (!Block()) return Error(ir::Error::ElseStatementExpectedScopeBlock);
    }

    return true;
  }

  bool Lexer::IfStatement() {
    if (!If()) return false;

    if (!Condition()) return Error(ir::Error::IfStatementExpectedCondition);

    if (!Block()) {
      bool matched = false;

      // Match any number of patterns
      while (!cursor.Done() && Pattern()) {
        matched = true;
      }

      if (!matched) return Error(ir::Error::IfStatementExpectedScopeBlockOrPatternArms);
    }

    ElseStatement(); // Optional

    return true;
  }

  bool Lexer::TryStatement() {
    if (!Try()) return false;

    Condition(); // Optional

    if (!Block()) return Error(ir::Error::TryStatementExpectedScopeBlock);

    ElseStatement(); // Optional

    return true;
  }

  bool Lexer::IsPattern() {
    if (!Is()) return false;

    if (!Condition()) return Error(ir::Error::IsPatternExpectedCondition);
    if (!Block() && !Pattern()) return Error(ir::Error::IsPatternExpectedScopeBlock);

    return true;
  }

  bool Lexer::HasPattern() {
    if (!Has()) return false;

    if (!Condition()) return Error(ir::Error::HasPatternExpectedCondition);
    if (!Block() && !Pattern()) return Error(ir::Error::HasPatternExpectedScopeBlock);

    return true;
  }

  bool Lexer::FromPattern() {
    if (!From()) return false;

    if (!Condition()) return Error(ir::Error::FromPatternExpectedCondition);
    if (!Block() && !Pattern()) return Error(ir::Error::FromPatternExpectedScopeBlock);

    return true;
  }

  bool Lexer::Pattern() {
    switch (cursor.Peek()) {
      case 'i': return IsPattern();
      case 'h': return HasPattern();
      case 'f': return FromPattern();
      default: return false;
    }
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

    if (!While()) return Error(ir::Error::RepeatExpectedWhileKeyword);

    if (!Condition()) return Error(ir::Error::RepeatExpectedWhileCondition);

    return true;
  }

  bool Lexer::ForStatement() {
    if (!For()) return false;

    if (!LoopModeDeclaration()) return Error(ir::Error::ForExpectedBinding);

    if (!In()) return Error(ir::Error::ForExpectedInKeyword);

    if (!Condition()) return Error(ir::Error::ForExpectedCondition);

    if (!Block()) return Error(ir::Error::ForExpectedScopeBlock);

    return true;
  }

  bool Lexer::DeclarationOrExpressionStatement() {
    // A declaration statement MUST have leading modifier keyword.
    if (!Modifiers()) {
      // So if it doesn't, it has to be an expression statement.
      return ExpressionStatement();
    }
    
    if (!StatementModeDeclaration()) {
      return Error(ir::Error::InvalidDeclarationStatement);
    }

    if (!Semicolon()) return Error(ir::Error::DeclarationExpectedSemicolon);
    
    return true;
  }

  bool Lexer::LetStatement() {
    if (!Let()) return false;

    if (!StatementModeDeclaration()) {
      return Error(ir::Error::InvalidDeclarationStatement);
    }

    if (!Semicolon()) return Error(ir::Error::DeclarationExpectedSemicolon);
    
    return true;
  }

  bool Lexer::StatementShortcut() {
    switch (cursor.Peek()) {
      case 'b': return BreakStatement();
      case 'c': return ContinueStatement();
      case 'd': return DoStatement();
      case 'f': return ForStatement();
      case 'i': return IfStatement() || ImportStatement();
      case 'l': return LetStatement() || LoopStatement();
      case 'p': return PanicStatement();
      case 't': return TryStatement();
      case 'r': return ReturnStatement() || RegisterStatement() || RepeatStatement();
      case 'w': return WhileStatement();
      case 'y': return YieldStatement();
      case ';': return Semicolon(); // This is more performant than checking for empty expression statement
      default: return false;
    }
  }

  bool Lexer::Statement() {
    return (IsStatementStart() && StatementShortcut()) || DeclarationOrExpressionStatement();
  }

  bool Lexer::Condition() {
    if (!ConditionOpen()) return false;

    if (!Expression()) return Error(ir::Error::ConditionExpectedExpression);

    if (!ConditionClose()) return Error(ir::Error::ConditionExpectedClosingParenthesis);

    return true;
  }

  bool Lexer::Block() {
    if (!ScopeOpen()) return false;

    ZeroOrMore([&]{ return Statement(); });

    if (!ScopeClose()) return Error(ir::Error::BlockExpectedClosingCurlyBrace);

    return true;
  }

  bool Lexer::StatementList() {
    while (!cursor.Done()) {
      auto start = cursor.cbegin();
      if (!Statement() && !cursor.Done()) {

        switch (cursor.Peek()) {
          case ',':
            Error(ir::Error::StatementFoundUnexpectedComma);
            cursor.Advance();
            break;
          case ';':
            Error(ir::Error::StatementFoundUnexpectedSemicolon);
            cursor.Advance();
            break;
          case '.':
            Error(ir::Error::StatementFoundUnexpectedDot);
            cursor.Advance();
            break;
          case ')':
            Error(ir::Error::StatementFoundUnexpectedClosingParenthesis);
            cursor.Advance();
            break;
          case '}':
            Error(ir::Error::StatementFoundUnexpectedClosingCurlyBrace);
            cursor.Advance();
            break;
          case ']':
            Error(ir::Error::StatementFoundUnexpectedClosingBracket);
            cursor.Advance();
            break;
          default:
            utility::Print("Lexer::StatementList failed to match a statement at an unknown character: ", cursor.Peek());
            if (!Expression()) {
              Error(ir::Error::StatementFoundUnexpectedCharacter);
              cursor.Advance();
            }
            break;
        }

        // utility::Print("Lexer::StatementList failed to match a statement: ", cursor.Peek());
        // return false;
      }

      // A temporary safeguard against infinite loops, this will be removed later
      if (start == cursor.cbegin()) {
        // No progress made
        utility::Print("Lexer::StatementList failed to progress: ", cursor.Peek());
        break;
      }
    }

    return true;
  }
}
