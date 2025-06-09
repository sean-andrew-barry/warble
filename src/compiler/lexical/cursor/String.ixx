export module lexical.cursor.string;

import lexical.cursor;
// import lexical.token;
import lexical.unicode;
import <string>;
import <string_view>;
import <bitset>;

namespace lexical::cursor {
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

  constexpr char END() { return '\0'; }

  export class String : public lexical::Cursor<std::string, END> {
  private:
  public:
    using Super::Super;
    using Super::Peek;

    char32_t PeekCodePoint() {
      char8_t ch = Super::Peek();
      size_t length = lexical::Unicode::GetCodePointLength(ch);

      if (length > 0 && Size() >= length) {
        char32_t code_point = lexical::Unicode::GetCodePoint(reinterpret_cast<const char8_t*>(&*cbegin()), length);

        return code_point;
      } else {
        return '\0';
      }
    }

    char32_t CodePoint() {
      char8_t ch = Super::Peek();
      size_t length = lexical::Unicode::GetCodePointLength(ch);

      if (length > 0 && Size() >= length) {
        char32_t code_point = lexical::Unicode::GetCodePoint(reinterpret_cast<const char8_t*>(&*cbegin()), length);
        Advance(length);

        return code_point;
      } else {
        return '\0';
      }
    }

    constexpr bool Fits(const std::string_view text) const {
      // If it doesn't go past the end, it can match
      return Size() >= text.size();
    }

    constexpr bool Check(const char c) const {
      return Super::Peek() == c;
    }

    constexpr bool Check(const std::string_view text) const {
      if (!Fits(text)) return false; // If it doesn't fit, it can't match

      auto temp = cbegin();
      for (size_t i = 0; i < text.size(); ++i, ++temp) {
        if (text[i] != *temp) {
          return false;
        }
      }

      return true; // All characters matched
    }

    constexpr bool Match(const char c) {
      auto iter = cbegin();
      if (iter != cend() && *iter == c) {
        Advance();
        return true;
      } else {
        return false;
      }
    }

    constexpr bool Match(const std::string_view text) {
      if (!Check(text)) return false; // Compare the text to the content

      Advance(text.size());

      return true;
    }

    constexpr std::string_view Capture(const std::string_view text) {
      if (!Check(text)) return std::string_view{cbegin(), cbegin()};

      std::string_view captured{cbegin(), cbegin() + text.size()};

      Advance(text.size());

      return captured;
    }

    constexpr bool Keyword(const std::string_view text) {
      if (!Check(text)) return false; // Compare the text to the content
      if (IsAlNum(*(cbegin() + text.size()))) return false; // The next character cannot be an alphanumeric character

      Advance(text.size());

      return true;
    }

    constexpr unsigned char PeekU8() const {
      return static_cast<unsigned char>(Super::Peek());
    }

    constexpr unsigned char PeekU8(size_t n) const {
      return static_cast<unsigned char>(Super::Peek(n));
    }

    constexpr bool IsBreak() const {
      switch (Super::Peek()) {
        case '\n': return true;
        case '\r': return true;
        default: {
          // Quick test for LS or PS first two bytes: E2 80 xx
          if (PeekU8() == 0xE2 && PeekU8(1) == 0x80) {
            switch (PeekU8(2)) {
              case 0xA8: return true; // LS
              case 0xA9: return true; // PS
            }
          }

          return false;
        }
      }
    }

    constexpr bool IsASCII() const { return IsASCII(Super::Peek()); }
    constexpr bool IsSpace() const { return IsSpace(Super::Peek()); }
    constexpr bool IsAlpha() const { return IsAlpha(Super::Peek()); }
    constexpr bool IsAlNum() const { return IsAlNum(Super::Peek()); }
    constexpr bool IsUpper() const { return IsUpper(Super::Peek()); }
    constexpr bool IsLower() const { return IsLower(Super::Peek()); }
    constexpr bool IsDigit() const { return IsDigit(Super::Peek()); }
    constexpr bool IsPunct() const { return IsPunct(Super::Peek()); }
    constexpr bool IsSlash() const { return IsSlash(Super::Peek()); }
    constexpr bool IsOctal() const { return IsOctal(Super::Peek()); }
    constexpr bool IsHex  () const { return IsHex(Super::Peek()); }
    constexpr bool IsIdent() const { return IsIdent(Super::Peek()); }
    constexpr bool IsBinary() const { return IsBinary(Super::Peek()); }
    constexpr bool IsIdentStart() const { return IsIdentStart(Super::Peek()); }
    constexpr bool IsValueStart() const { return IsValueStart(Super::Peek()); }
    constexpr bool IsStatementStart() const { return IsStatementStart(Super::Peek()); }
    constexpr bool IsUnaryPrefixStart() const { return IsUnaryPrefixStart(Super::Peek()); }
    constexpr bool IsUnaryPostfixStart() const { return IsUnaryPostfixStart(Super::Peek()); }
    constexpr bool IsCallablePrefixStart() const { return IsCallablePrefixStart(Super::Peek()); }
    constexpr bool IsCallablePostfixStart() const { return IsCallablePostfixStart(Super::Peek()); }
    constexpr bool IsBinaryStart() const { return IsBinaryStart(Super::Peek()); }
    constexpr bool IsModifierStart() const { return IsModifierStart(Super::Peek()); }
    constexpr bool IsNumberStart() const { return IsNumberStart(Super::Peek()); }
    constexpr bool IsPossibleSpaceStart() const { return IsPossibleSpaceStart(Super::Peek()); }
    constexpr bool IsPossibleIdentifierStart() const { return IsPossibleIdentifierStart(Super::Peek()); }
    constexpr bool IsURLProtocol() const { return IsURLProtocol(Super::Peek()); }
    constexpr bool IsURLHost() const { return IsURLHost(Super::Peek()); }
    constexpr bool IsURLPathname() const { return IsURLPathname(Super::Peek()); }
    constexpr bool IsURLSearch() const { return IsURLSearch(Super::Peek()); }
    constexpr bool IsURLHash() const { return IsURLHash(Super::Peek()); }
    constexpr bool IsURLAuthority() const { return IsURLAuthority(Super::Peek()); }
    constexpr bool IsFilePath() const { return IsFilePath(Super::Peek()); }

    // constexpr bool IsBreak(size_t i) const { return IsBreak(Super::Peek(i)); }
    constexpr bool IsASCII(size_t i) const { return IsASCII(Super::Peek(i)); }
    constexpr bool IsSpace(size_t i) const { return IsSpace(Super::Peek(i)); }
    constexpr bool IsAlpha(size_t i) const { return IsAlpha(Super::Peek(i)); }
    constexpr bool IsAlNum(size_t i) const { return IsAlNum(Super::Peek(i)); }
    constexpr bool IsUpper(size_t i) const { return IsUpper(Super::Peek(i)); }
    constexpr bool IsLower(size_t i) const { return IsLower(Super::Peek(i)); }
    constexpr bool IsDigit(size_t i) const { return IsDigit(Super::Peek(i)); }
    constexpr bool IsPunct(size_t i) const { return IsPunct(Super::Peek(i)); }
    constexpr bool IsSlash(size_t i) const { return IsSlash(Super::Peek(i)); }
    constexpr bool IsOctal(size_t i) const { return IsOctal(Super::Peek(i)); }
    constexpr bool IsHex  (size_t i) const { return IsHex(Super::Peek(i)); }
    constexpr bool IsIdent(size_t i) const { return IsIdent(Super::Peek(i)); }
    constexpr bool IsBinary(size_t i) const { return IsBinary(Super::Peek(i)); }
    constexpr bool IsIdentStart(size_t i) const { return IsIdentStart(Super::Peek(i)); }
    constexpr bool IsValueStart(size_t i) const { return IsValueStart(Super::Peek(i)); }
    constexpr bool IsStatementStart(size_t i) const { return IsStatementStart(Super::Peek(i)); }
    constexpr bool IsUnaryPrefixStart(size_t i) const { return IsUnaryPrefixStart(Super::Peek(i)); }
    constexpr bool IsUnaryPostfixStart(size_t i) const { return IsUnaryPostfixStart(Super::Peek(i)); }
    constexpr bool IsCallablePrefixStart(size_t i) const { return IsCallablePrefixStart(Super::Peek(i)); }
    constexpr bool IsCallablePostfixStart(size_t i) const { return IsCallablePostfixStart(Super::Peek(i)); }
    constexpr bool IsBinaryStart(size_t i) const { return IsBinaryStart(Super::Peek(i)); }
    constexpr bool IsModifierStart(size_t i) const { return IsModifierStart(Super::Peek(i)); }
    constexpr bool IsNumberStart(size_t i) const { return IsNumberStart(Super::Peek(i)); }
    constexpr bool IsPossibleSpaceStart(size_t i) const { return IsPossibleSpaceStart(Super::Peek(i)); }
    constexpr bool IsPossibleIdentifierStart(size_t i) const { return IsPossibleIdentifierStart(Super::Peek(i)); }
    constexpr bool IsURLProtocol(size_t i) const { return IsURLProtocol(Super::Peek(i)); }
    constexpr bool IsURLHost(size_t i) const { return IsURLHost(Super::Peek(i)); }
    constexpr bool IsURLPathname(size_t i) const { return IsURLPathname(Super::Peek(i)); }
    constexpr bool IsURLSearch(size_t i) const { return IsURLSearch(Super::Peek(i)); }
    constexpr bool IsURLHash(size_t i) const { return IsURLHash(Super::Peek(i)); }
    constexpr bool IsURLAuthority(size_t i) const { return IsURLAuthority(Super::Peek(i)); }
    constexpr bool IsFilePath(size_t i) const { return IsFilePath(Super::Peek(i)); }

    // constexpr static bool IsBreak(const char c) { return c == '\n'; }
    constexpr static bool IsASCII(const char c) { return static_cast<uint8_t>(c) < 0x80; }
    constexpr static bool IsSpace(const char c) { return WHITESPACE[static_cast<uint8_t>(c)]; }
    constexpr static bool IsAlpha(const char c) { return ALPHA[static_cast<uint8_t>(c)]; }
    constexpr static bool IsAlNum(const char c) { return ALNUM[static_cast<uint8_t>(c)]; }
    constexpr static bool IsUpper(const char c) { return UPPER[static_cast<uint8_t>(c)]; }
    constexpr static bool IsLower(const char c) { return LOWER[static_cast<uint8_t>(c)]; }
    constexpr static bool IsDigit(const char c) { return DIGIT[static_cast<uint8_t>(c)]; }
    constexpr static bool IsPunct(const char c) { return PUNCT[static_cast<uint8_t>(c)]; }
    constexpr static bool IsSlash(const char c) { return SLASH[static_cast<uint8_t>(c)]; }
    constexpr static bool IsOctal(const char c) { return OCTAL[static_cast<uint8_t>(c)]; }
    constexpr static bool IsHex  (const char c) { return HEX  [static_cast<uint8_t>(c)]; }
    constexpr static bool IsIdent(const char c) { return IDENTIFIER[static_cast<uint8_t>(c)]; }
    constexpr static bool IsBinary(const char c) { return c == '0'|| c == '1'; }
    constexpr static bool IsIdentStart(const char c) { return IDENTIFIER_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsValueStart(const char c) { return VALUE_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsStatementStart(const char c) { return STATEMENT_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsUnaryPrefixStart(const char c) { return UNARY_PREFIX_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsUnaryPostfixStart(const char c) { return UNARY_POSTFIX_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsCallablePrefixStart(const char c) { return CALLABLE_PREFIX_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsCallablePostfixStart(const char c) { return CALLABLE_POSTFIX_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsBinaryStart(const char c) { return BINARY_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsModifierStart(const char c) { return MODIFIER_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsNumberStart(const char c) { return NUMBER_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsPossibleSpaceStart(const char c) { return POSSIBLE_SPACE_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsPossibleIdentifierStart(const char c) { return POSSIBLE_IDENTIFIER_START[static_cast<uint8_t>(c)]; }
    constexpr static bool IsURLProtocol(const char c) { return URL_PROTOCOL[static_cast<uint8_t>(c)]; }
    constexpr static bool IsURLHost(const char c) { return URL_HOST[static_cast<uint8_t>(c)]; }
    constexpr static bool IsURLPathname(const char c) { return URL_PATHNAME[static_cast<uint8_t>(c)]; }
    constexpr static bool IsURLSearch(const char c) { return URL_SEARCH[static_cast<uint8_t>(c)]; }
    constexpr static bool IsURLHash(const char c) { return URL_HASH[static_cast<uint8_t>(c)]; }
    constexpr static bool IsURLAuthority(const char c) { return URL_AUTHORITY[static_cast<uint8_t>(c)]; }
    constexpr static bool IsFilePath(const char c) { return FILE_PATH[static_cast<uint8_t>(c)]; }
  };
};