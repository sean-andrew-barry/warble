export module compiler.input.Lexer;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;
import <expected>;

import compiler.program.Module;
import compiler.ir.Error;
import compiler.ir.symbol.Type;
import compiler.text.cursor.String;

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
    for (char c : "bcdefirw") bs.set(c);
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
    std::string source;
    std::vector<ir::Token> tokens;
    std::vector<char32_t> characters;
    std::vector<uint32_t> line_starts; // The token index where each line starts
    std::vector<uint32_t> string_starts; // The character index where each line starts

    text::cursor::String cursor;
    std::string::const_iterator furthest; // Highest source iterator reached to suppress duplicate side-effects after rollback.
  protected:
    struct Position {
      std::string::const_iterator cursor;
      size_t token;
    };

    // Capture the current cursor and the current token count so speculative
    // lexing can roll back only tokens produced after this point.
    // NOTE: We intentionally record size(), not size()-1. Using size()-1 would
    // erase one pre-existing token on rollback and underflow when empty.
    Position Start();

    void Rollback(const Position& position);
    bool Emit(ir::Token token);
    bool EmitRepeated(ir::Token token, uint32_t count);
    bool EmitRepeated(ir::Token token, std::string::const_iterator begin);
    bool EmitStringSymbol(ir::Token token, size_t start);
    bool Error(ir::Error error);
    bool EmitAndAdvance(ir::Token token, size_t count = 1);
    bool Keyword(const std::string_view text);
    bool Line(ir::Token t);
    bool IsBacktracked() const { return furthest > cursor.cbegin(); }

    // ---- Tiny, hot-path helpers (inline) to simplify WhiteSpace without cost ----
    // Consume a run of '\n' and record line starts; returns the number consumed.
    inline uint32_t ConsumeLFs() {
      uint32_t count = 0;
      while (!cursor.Done() && cursor.Peek() == '\n') {
        cursor.Advance(1);
        mod.AddLine(cursor.cbegin());
        ++count;
      }
      return count;
    }

    // Consume a run of CRLF pairs and record line starts; returns the number of pairs consumed.
    inline uint32_t ConsumeCRLFs() {
      uint32_t count = 0;
      while (!cursor.Done() && cursor.Peek() == '\r' && cursor.Peek(1) == '\n') {
        cursor.Advance(2);
        mod.AddLine(cursor.cbegin());
        ++count;
      }
      return count;
    }

    // Consume a run of lone '\r' (not followed by '\n') and record line starts; returns the number consumed.
    inline uint32_t ConsumeCRs() {
      uint32_t count = 0;
      while (!cursor.Done() && cursor.Peek() == '\r' && cursor.Peek(1) != '\n') {
        cursor.Advance(1);
        mod.AddLine(cursor.cbegin());
        ++count;
      }
      return count;
    }

    // Handle a single non-ASCII whitespace code point. Returns true if whitespace was consumed and tokens emitted.
    bool HandleNonASCIIWhitespace();

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
  public:
    Lexer(program::Module& mod, const std::string_view source);

    virtual ~Lexer() = default;

    bool WhiteSpace();

    bool WS();

    // Optional white space
    bool OWS();

    bool Null();
    bool Undefined();
    bool True();
    bool False();
    bool This();
    bool Import();
    bool Export();
    bool Register();
    bool From();
    bool With();
    bool Has();
    bool If();
    bool Else();
    bool Do();
    bool While();
    bool Repeat();
    bool Is();
    bool In();
    bool For();
    bool As();
    bool Default();
    bool Auto();
    bool Void();
    bool When();
    bool Await();
    bool Compiler();
    bool Break();
    bool Continue();
    bool Return();
    bool Case();
    bool Yield();
    bool Let();

    bool CaptureOpen();
    bool CaptureClose();
    bool ParameterOpen();
    bool ParameterClose();
    bool ScopeOpen();
    bool ScopeClose();
    bool TupleOpen();
    bool TupleClose();
    bool ArrayOpen();
    bool ArrayClose();
    bool ObjectOpen();
    bool ObjectClose();
    bool EnumOpen();
    bool EnumClose();

    bool CharOpen();
    bool CharClose();
    bool StringOpen();
    bool StringClose();
    bool TemplateStringOpen();
    bool TemplateStringClose();
    bool TemplateStringExpressionOpen();
    bool TemplateStringExpressionClose();
    bool ConditionOpen();
    bool ConditionClose();
    bool Call();
    bool Wildcard();
    bool Comma();
    bool Semicolon();
    bool CommentOpen();
    bool CommentClose();
    bool MultiLineCommentOpen();
    bool MultiLineCommentClose();
    bool DestructuredArrayOpen();
    bool DestructuredArrayClose();
    bool DestructuredTupleOpen();
    bool DestructuredTupleClose();
    bool DestructuredObjectOpen();
    bool DestructuredObjectClose();
    bool DestructuredEnumOpen();
    bool DestructuredEnumClose();

    bool OptionalSemicolon();

    // Unary operators
    bool Reference();
    bool MutableReference();
    bool Symbol();
    bool Copy();
    bool Counted();
    bool Positive();
    bool Negative();
    bool Increment();
    bool Decrement();
    bool Not();
    bool Spread();
    bool Move();
    bool BitwiseNot();

    bool UnaryPrefixOperatorHelper();

    bool UnaryPrefixOperator();
    bool UnaryPostfixOperator();

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
    std::expected<char32_t, ir::Error> Escape();

    bool Char();
    bool String();
    bool Hex();
    bool Octal();
    bool Binary();
    bool NumberHelper();
    bool Number();
    bool IdentifierHelper();
    bool Identifier();
    bool TemplateString();
    bool BinaryOperatorHelper(bool in_enum = false);
    bool BinaryOperator(bool in_enum = false);
    bool ParameterDeclaration();
    bool ParameterDeclarationList();
    bool Parameters();
    bool Function();
    bool IdentifiedExpression();
    bool CallablePrefixLiteralHelper();
    bool CallablePostfixLiteralHelper();
    bool CallablePrefixLiteral();
    bool CallablePostfixLiteral();
    bool ValueShortcut();
    bool Value();
    bool InitialValue();
    bool UnaryStartedExpression();
    bool ContinueExpression();
    bool ContinueEnumExpression();
    bool Expression();
    bool EnumExpression();
    bool DeclarationValue();
    bool Declaration();
    bool StatementShortcut();
    bool Statement();
    bool StatementList();
    bool FunctionStatementList();

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