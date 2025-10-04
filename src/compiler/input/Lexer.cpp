import compiler.input.Lexer;
import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Error;
import compiler.ir.symbol.Type;
import compiler.text.cursor.String;
import compiler.text.Convert;

namespace compiler::input {
  Lexer::Position Lexer::Start() {
    return Position{cursor.Current(), mod.Tokens().size()};
  }

  void Lexer::Rollback(const Lexer::Position& position) {
    furthest = std::max(furthest, cursor.cbegin());
    cursor.Retreat(position.cursor);
    mod.Tokens().erase(mod.Tokens().begin() + position.token, mod.Tokens().end());
  }

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

  bool Lexer::Line(ir::Token t) {
    // Make sure we don't double record lines when backtracked
    if (cursor.cbegin() > furthest) {
      mod.AddLine(cursor.cbegin());
      furthest = cursor.cbegin();
    }

    return Emit(t);
  }

  bool Lexer::WhiteSpace() {
    switch (cursor.Peek()) {
      case '\v': return EmitAndAdvance(ir::Token::VerticalTab, 1);
      case '\f': return EmitAndAdvance(ir::Token::FormFeed, 1);
      case ' ' : return EmitRepeated(ir::Token::Spaces, cursor.Consume(' '));
      case '\t': return EmitRepeated(ir::Token::Tabs, cursor.Consume('\t'));
      case '\n': {
        uint32_t count = 0;
        do {
          cursor.Advance(1);
          mod.AddLine(cursor.cbegin());
          count += 1;
        } while (!cursor.Done() && cursor.Peek() == '\n');

        return EmitRepeated(ir::Token::LineFeeds, count);
      }
      case '\r': {
        if (cursor.Peek(1) == '\n') {
          uint32_t count = 1;
          cursor.Advance(2);
          mod.AddLine(cursor.cbegin());

          // It's a CRLF sequence
          while (!cursor.Done() && cursor.Match("\r\n")) {
            mod.AddLine(cursor.cbegin());
            count += 1;
          }

          return EmitRepeated(ir::Token::CarriageReturnLineFeeds, count);
        } else {
          uint32_t count = 0;
          // It's a series of lone CR characters
          do {
            cursor.Advance(1);
            mod.AddLine(cursor.cbegin());
            count += 1;
          } while (!cursor.Done() && cursor.Match("\r"));

          return EmitRepeated(ir::Token::CarriageReturns, count);
        }
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
              if (cursor.Peek() == '*' && cursor.Peek(1) == '/') {
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
            return Line(ir::Token::LineSeparators);
          } else if (code_point == 0x2029) {
            return Line(ir::Token::ParagraphSeparators);
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
  bool Lexer::Match() { return Keyword("match") && Emit(ir::Token::Match); }
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

  std::expected<char32_t, ir::Error> Lexer::Escape() {
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

  bool Lexer::Char() {
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

  bool Lexer::String() {
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
        if (furthest <= cursor.cbegin()) {
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
  bool Lexer::Hex() {
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

  bool Lexer::Octal() {
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

  bool Lexer::Binary() {
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

  bool Lexer::NumberHelper() {
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

  bool Lexer::Number() { return cursor.IsNumberStart() && NumberHelper(); }

  // Identifier parsing: supports escapes in identifiers and unicode code points.
  bool Lexer::IdentifierHelper() {
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

  bool Lexer::Identifier() { return cursor.IsPossibleIdentifierStart() && IdentifierHelper(); }

  // Full template string parser: stores characters in module, emits Characters tokens and Symbols,
  // and handles interpolation via TemplateStringExpressionOpen/Close.
  bool Lexer::TemplateString() {
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
        return Error("Declaration expected an identifier after the modifiers");
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
      return Error("Expected a value after the unary prefix operator");
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

  bool Lexer::Declaration() {
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

  bool Lexer::StatementShortcut() {
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

  bool Lexer::Statement() { return (cursor.IsStatementStart() && StatementShortcut()) || Declaration() || Expression() && (Semicolon() || true); }

  bool Lexer::StatementList() {
    while (!cursor.Done()) {
      if (!Statement()) break;
    }

    return true;
  }

  bool Lexer::FunctionStatementList() { return StatementList(); }
};