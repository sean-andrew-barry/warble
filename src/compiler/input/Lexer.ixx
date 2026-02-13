export module compiler.input.Lexer;

import <cstdint>;
import <bitset>;
import <vector>;
import <array>;
import <string>;
import <stdexcept>;
import <expected>;

import compiler.ir.Token;
import compiler.ir.Error;
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

  inline constexpr auto OPERAND_START = MakeBitset([](auto& bs){
    for (char c : "tfnrua'\"`+-([{<.") bs.set(c);
    bs.set('0'); // 
    bs.set('1'); // 
    bs.set('2'); // 
    bs.set('3'); // 
    bs.set('4'); // 
    bs.set('5'); // 
    bs.set('6'); // 
    bs.set('7'); // 
    bs.set('8'); // 
    bs.set('9'); // 
    bs.set('a'); // `auto`
    bs.set('n'); // `null`
    bs.set('u'); // `undefined`
    bs.set('\''); // Character
    bs.set('"'); // String
    bs.set('('); // Tuple or parameter
    bs.set('['); // Array
    bs.set('<'); // Enum
    bs.set('{'); // Object
    bs.set('d'); // `do`
    bs.set('i'); // `if`
    bs.set('t'); // `try` or `true` or `this` or `that`
    bs.set('l'); // `loop`
    bs.set('w'); // `while`
    bs.set('r'); // `repeat` or `readonly`
    bs.set('f'); // `for` or `false`
  }) | DIGIT;

  inline constexpr auto STATEMENT_START = MakeBitset([](auto& bs){
    bs.set('b'); // `break`
    bs.set('c'); // `continue`
    bs.set('d'); // `do`
    bs.set('f'); // `for`
    bs.set('i'); // `if`
    bs.set('l'); // `loop` or `let`
    bs.set('p'); // `panic`
    bs.set('t'); // `try`
    bs.set('r'); // `repeat`
    bs.set('w'); // `while`
    bs.set('y'); // `yield`
    bs.set(';'); // `;` (empty statement)
  });

  inline constexpr auto UNARY_PREFIX_START = MakeBitset([](auto& bs){
    // for (char c : "+-*&$#@!?^=.~aneu") bs.set(c);

    bs.set('+');
    bs.set('-');
    bs.set('*');
    bs.set('&');
    bs.set('$');
    bs.set('#');
    bs.set('@');
    bs.set('!');
    bs.set('.');
    bs.set('~');
    bs.set('a'); // `await` or `assume`
    bs.set('e'); // `expect`
    bs.set('r'); // `runtime`
    bs.set('c'); // `comtime`
  });

  inline constexpr auto UNARY_POSTFIX_START = MakeBitset([](auto& bs){
    for (char c : "?'\"`([{<") bs.set(c);
  });

  inline constexpr auto BINARY_START = MakeBitset([](auto& bs){
    for (char c : "!=<>+-*/%^?&|oawut.:") bs.set(c);
  });

  inline constexpr auto MODIFIER_START = MakeBitset([](auto& bs){
    // I don't think `let` is actually a modifier anymore
    // bs.set('l'); // `let`
    bs.set('c'); // `const`
    bs.set('m'); // `mut`
    bs.set('p'); // `private`, `public`, `protected`
  });

  inline constexpr auto NUMBER_START = DIGIT;

  inline constexpr auto NOT_ASCII = MakeBitset([](auto& bs){
    // Set bits for lead bytes of UTF-8 encoded non-ASCII characters (128-255) to 1
    for (int i = 128; i < 256; ++i) {
      bs.set(i);
    }
  });

  inline constexpr auto POSSIBLE_SPACE_START = MakeBitset([](auto& bs){
    bs.set('/'); // Comments are treated as whitespace for this bitset
  }) | WHITESPACE | NOT_ASCII;

  inline constexpr auto URL_SCHEME = MakeBitset([](auto& bs){
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

  inline constexpr auto URL_SUB_DELIMS = MakeBitset([](auto& bs){
    for (char c : "!$&'()*+,;=") bs.set(c);
  });

  inline constexpr auto URL_UNRESERVED = MakeBitset([](auto& bs){
    for (char c : ".-_~") bs.set(c);
  }) | ALNUM;

  inline constexpr auto URL_SEGMENT = MakeBitset([](auto& bs){
    for (char c : ":@") bs.set(c);
  }) | URL_UNRESERVED | URL_SUB_DELIMS;

  inline constexpr auto URL_SEGMENT_NC = MakeBitset([](auto& bs){
    for (char c : "@") bs.set(c); // Excludes ':'
  }) | URL_UNRESERVED | URL_SUB_DELIMS;

  inline constexpr auto URL_PATH = MakeBitset([](auto& bs){
    // Reserved characters
    for (char c : ":@&=+$,") bs.set(c);

    // Sub-delimiters
    for (char c : "!\'()*;%") bs.set(c);
  }) | URL_UNRESERVED;

  inline constexpr auto URL_SEARCH = MakeBitset([](auto& bs){
    // Reserved characters
    for (char c : "!$\'()*,;:@%") bs.set(c);
  }) | URL_UNRESERVED;

  inline constexpr auto URL_FRAGMENT = MakeBitset([](auto& bs){
    // Reserved characters
    for (char c : "!$&\'()*+,;=@/?%") bs.set(c);
  }) | URL_UNRESERVED;

  inline constexpr auto FILE_PATH = MakeBitset([](auto& bs){
    for (char c : "-_!#$%&'()[]{}+@~^. ") bs.set(c);
  }) | ALNUM;

  inline constexpr auto POSSIBLE_IDENTIFIER_START = IDENTIFIER_START | NOT_ASCII;

  export class Lexer {
  private:
  protected:
    std::string source;
    std::vector<ir::Token> tokens;
    std::vector<ir::Error> errors;
    std::vector<uint32_t> data;
    std::vector<uint32_t> temp_le; // Shared little-endian workspace for numeric accumulation

    text::cursor::String cursor;
    std::string::const_iterator furthest; // Highest source iterator reached to suppress duplicate side-effects after rollback.
    bool has_backtracked{false}; // Indicates whether a rollback has occurred.
  protected:
    struct Position {
      std::string::const_iterator cursor;
      size_t token;
      size_t error;
    };

    // Capture the current cursor and the current token count so speculative
    // lexing can roll back only tokens produced after this point.
    // NOTE: We intentionally record size(), not size()-1. Using size()-1 would
    // erase one pre-existing token on rollback and underflow when empty.
    Position Start();

    void Rollback(const Position& position);
    bool Emit(ir::Token token);
    bool Match(const char c, ir::Token token);
    bool Match(const std::string_view s, ir::Token token);
    bool MatchWS(const char c, ir::Token token);
    bool MatchWS(const std::string_view s, ir::Token token);
    bool Error(ir::Error error);
    bool EmitAndAdvance(ir::Token token, size_t count = 1);
    bool Keyword(const std::string_view text);
    bool Keyword(const std::string_view text, ir::Token token);
    bool IsBacktracked() const { return has_backtracked && cursor.cbegin() <= furthest; }

    // Emit the given count as big-endian hexadecimal nibbles using Characters0..F tokens.
    // Returns true if any tokens were emitted (count > 0).
    bool EmitCharactersNibbles(uint32_t count);
    // Emit the given count as big-endian hexadecimal nibbles using Digits0..F tokens.
    // Used for numeric literal format masks (digit run lengths).
    // Returns true if any tokens were emitted (count > 0).
    bool EmitDigitsNibbles(uint32_t count);
    bool EmitSpacesNibbles(uint32_t count);
    bool EmitTabsNibbles(uint32_t count);

    // Handle a single non-ASCII whitespace code point. Returns true if whitespace was consumed and tokens emitted.
    bool HandleNonASCIIWhitespace();

    void PushCharacter(char32_t cp);
    void PushLimb(uint32_t word);

    bool CaptureCharacters(const auto& fn) {
      uint32_t count = 0;
      while (!cursor.Done() && fn()) {
        auto cp = cursor.CodePoint();

        PushCharacter(cp);
        ++count;
      }

      if (count > 0) {
        EmitCharactersNibbles(count);
        return true;
      }

      return false;
    }

    bool CaptureCharacters(const auto& fn, const auto& separator) {
      bool matched = false;

      while (!cursor.Done()) {
        matched = CaptureCharacters(fn) || matched; // At least one run matched
        if (separator()) continue;

        break;
      }

      return matched;
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
      while (fn()) {}
      return true; // Always succeeds (matches empty sequence)
    }

    // ZeroOrMore(item, sep): matches item (sep item)* and allows a trailing separator.
    bool ZeroOrMore(const auto& item, const auto& sep) {
      while (true) {
        if (!item()) break;
        if (!sep()) break;
      }
      return true;
    }

    // OneOrMore(fn): require at least one match, then continue matching.
    bool OneOrMore(const auto& fn) {
      if (!fn()) return false;
      while (fn()) {}
      return true;
    }

    // OneOrMore(item, sep): require one item then repeat (sep item)*, permitting a trailing separator.
    bool OneOrMore(const auto& item, const auto& sep) {
      if (!item()) return false;

      while (true) {
        if (!sep()) break;
        if (!item()) break;
      }
      return true;
    }

    uint32_t Repeat(const auto& fn) {
      uint32_t count = 0;

      while (!cursor.Done()) {
        if (!fn()) break;
        ++count;
        cursor.Advance();
      }

      return count;
    }
  public:
    Lexer(std::string&& source);

    virtual ~Lexer() = default;

    std::string Source() && { return std::move(source); }
    std::vector<ir::Token> Tokens() && { return std::move(tokens); }
    std::vector<uint32_t> Data() && { return std::move(data); }
    std::vector<ir::Error> Errors() && { return std::move(errors); }

    const std::string& Source() const & { return source; }
    const std::vector<ir::Token>& Tokens() const & { return tokens; }
    const std::vector<uint32_t>& Data() const & { return data; }
    const std::vector<ir::Error>& Errors() const & { return errors; }

    bool WhiteSpace();

    bool WS();

    // Optional white space
    bool OWS();

    bool Null();
    bool Undefined();
    bool Readonly();
    bool True();
    bool False();
    bool This();
    bool That();
    bool Import();
    bool Export();
    bool Register();
    bool From();
    bool With();
    bool Has();
    bool If();
    bool Try();
    bool Else();
    bool Do();
    bool While();
    bool Loop();
    bool Repeat();
    bool Is();
    bool In();
    bool As();
    bool For();
    bool Default();
    bool Auto();
    bool When();
    bool Runtime();
    bool Comtime();
    bool Await();
    bool Assume();
    bool Expect();
    bool Async();
    bool Compiler();
    bool Break();
    bool Continue();
    bool Return();
    bool Panic();
    bool Yield();
    bool Pass();
    bool Fail();
    bool Let();
    bool Const();
    bool Mut();
    bool Public();
    bool Protected();
    bool Private();

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

    bool CharacterOpen();
    bool CharacterClose();
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
    bool Pipeline();
    bool Arrow();
    bool Path();
    bool TypeStart();
    bool Initialize();
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

    // Unary operators
    bool ReferenceOf();
    bool MutableReferenceOf();
    bool SymbolOf();
    bool CopyOf();
    bool LengthOf();
    bool Positive();
    bool Negative();
    bool Increment();
    bool Decrement();
    bool Not();
    bool Spread();
    bool Move();
    bool BitwiseNot();
    bool Guard();

    bool UnaryPrefixOperatorHelper();
    bool UnaryPostfixOperatorHelper();
    bool ModifierHelper();

    bool UnaryPrefixOperator();
    bool UnaryPostfixOperator();
    bool Modifier();
    bool Modifiers();

    // Generic escape sequence parser (new design).
    //  - Consumes an escape beginning with '\\' and emits exactly one Escape* token that
    //    encodes the source format (ASCII, \xHH, \uXXXX, or \u{H…H}).
    //  - Decodes the escaped character to a UTF-32 code point and appends it to `data`.
    //  - Does NOT emit any Characters* tokens for the escape (Escape* implies Characters1).
    //  - Returns true on success; on failure, calls Error(...) with a precise code and returns false.
    bool Escape();

    bool TrueLiteral();
    bool FalseLiteral();
    bool ThisLiteral();
    bool ThatLiteral();
    bool NullLiteral();
    bool UndefinedLiteral();
    bool ReadonlyLiteral();
    bool CharacterLiteral();
    bool StringLiteral();
    bool HexLiteral();
    bool OctalLiteral();
    bool BinaryLiteral();
    bool DecimalLiteral();
    bool NumberLiteral();
    bool IdentifierHelper();
    bool Identifier();
    bool IdentifierOrArrowFunction();
    bool BinaryOperator(bool in_enum, bool in_type);
    bool ParameterList();
    bool CaptureList();
    bool ArrowFunction();
    bool FunctionHeader();
    bool FunctionBody();
    bool ParameterFunctionLiteral();
    bool CaptureFunctionLiteral();
    bool EnumLiteral();
    bool ObjectLiteral();
    bool ArrayLiteral();
    bool TupleLiteral();
    bool OperandShortcut();
    bool Operand();
    bool Expression(bool in_enum = false, bool in_type = false);
    bool ExpressionStatement();
    bool DefaultExpression();
    bool DoExpression();
    bool IfExpression();
    bool TryExpression();
    bool LoopExpression();
    bool WhileExpression();
    bool RepeatExpression();
    bool ForExpression();
    bool DeclarationInternal(bool require_keyword, bool allow_prefix, bool allow_initializer, bool emit_errors);
    bool LetStatement();
    bool StatementModeDeclaration();
    bool ParameterModeDeclaration();
    bool PropertyModeDeclaration();
    bool CaptureModeDeclaration();
    bool LoopModeDeclaration();
    bool DestructureArray();
    bool DestructureTuple();
    bool DestructureEnum();
    bool DestructureObject();
    bool Destructure();
    bool PrefixedCopyOf();
    bool PrefixedReferenceOf();
    bool PrefixedMutableReferenceOf();
    bool PrefixedSymbolOf();
    bool PrefixedSpread();
    bool PrefixedExpression();
    bool TypeExpression();
    bool InitializerExpression();
    bool DeclarationOrExpressionStatement();
    bool StatementShortcut();
    bool Statement();
    bool Condition();
    bool Block();
    bool StatementList();
    bool EscapePercent();
    bool EscapeURL();
    bool Scheme();
    bool Authority();
    bool Userinfo();
    bool Hostname();
    bool IPv4();
    bool IPv6();
    bool Host();
    bool PathSegments();
    bool PathAbsolute();
    bool PathRootless();
    bool PathNoScheme();
    bool Port();
    bool Query();
    bool Fragment();
    bool Specifier();

    bool ImportStatement();
    bool RegisterStatement();
    bool BreakStatement();
    bool ContinueStatement();
    bool ReturnStatement();
    bool PanicStatement();
    bool YieldStatement();
    bool DoStatement();
    bool IfStatement();
    bool TryStatement();
    bool ElseStatement();
    bool ForStatement();
    bool LoopStatement();
    bool RepeatStatement();
    bool WhileStatement();
    bool IsPattern();
    bool HasPattern();
    bool FromPattern();
    bool Pattern();

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
    constexpr bool IsURLScheme() const { return IsURLScheme(cursor.Peek()); }
    constexpr bool IsURLHost() const { return IsURLHost(cursor.Peek()); }
    constexpr bool IsURLSegment() const { return IsURLSegment(cursor.Peek()); }
    constexpr bool IsURLSegmentNC() const { return IsURLSegmentNC(cursor.Peek()); }
    constexpr bool IsURLPath() const { return IsURLPath(cursor.Peek()); }
    constexpr bool IsURLSearch() const { return IsURLSearch(cursor.Peek()); }
    constexpr bool IsURLFragment() const { return IsURLFragment(cursor.Peek()); }
    constexpr bool IsURLAuthority() const { return IsURLAuthority(cursor.Peek()); }
    constexpr bool IsFilePath() const { return IsFilePath(cursor.Peek()); }
    constexpr bool IsBinaryStart() const { return IsBinaryStart(cursor.Peek()); }
    constexpr bool IsNumberStart() const { return IsNumberStart(cursor.Peek()); }
    constexpr bool IsIdentStart() const { return IsIdentStart(cursor.Peek()); }
    constexpr bool IsOperandStart() const { return IsOperandStart(cursor.Peek()); }
    constexpr bool IsStatementStart() const { return IsStatementStart(cursor.Peek()); }
    constexpr bool IsUnaryPrefixStart() const { return IsUnaryPrefixStart(cursor.Peek()); }
    constexpr bool IsUnaryPostfixStart() const { return IsUnaryPostfixStart(cursor.Peek()); }
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
    constexpr bool IsURLScheme(size_t i) const { return IsURLScheme(cursor.Peek(i)); }
    constexpr bool IsURLHost(size_t i) const { return IsURLHost(cursor.Peek(i)); }
    constexpr bool IsURLSegment(size_t i) const { return IsURLSegment(cursor.Peek(i)); }
    constexpr bool IsURLSegmentNC(size_t i) const { return IsURLSegmentNC(cursor.Peek(i)); }
    constexpr bool IsURLPath(size_t i) const { return IsURLPath(cursor.Peek(i)); }
    constexpr bool IsURLSearch(size_t i) const { return IsURLSearch(cursor.Peek(i)); }
    constexpr bool IsURLFragment(size_t i) const { return IsURLFragment(cursor.Peek(i)); }
    constexpr bool IsURLAuthority(size_t i) const { return IsURLAuthority(cursor.Peek(i)); }
    constexpr bool IsFilePath(size_t i) const { return IsFilePath(cursor.Peek(i)); }
    constexpr bool IsIdentStart(size_t i) const { return IsIdentStart(cursor.Peek(i)); }
    constexpr bool IsOperandStart(size_t i) const { return IsOperandStart(cursor.Peek(i)); }
    constexpr bool IsStatementStart(size_t i) const { return IsStatementStart(cursor.Peek(i)); }
    constexpr bool IsUnaryPrefixStart(size_t i) const { return IsUnaryPrefixStart(cursor.Peek(i)); }
    constexpr bool IsUnaryPostfixStart(size_t i) const { return IsUnaryPostfixStart(cursor.Peek(i)); }
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
    constexpr bool IsOperandStart(const char c) const { return OPERAND_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsStatementStart(const char c) const { return STATEMENT_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsUnaryPrefixStart(const char c) const { return UNARY_PREFIX_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsUnaryPostfixStart(const char c) const { return UNARY_POSTFIX_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsBinaryStart(const char c) const { return BINARY_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsModifierStart(const char c) const { return MODIFIER_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsNumberStart(const char c) const { return NUMBER_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsPossibleSpaceStart(const char c) const { return POSSIBLE_SPACE_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsPossibleIdentifierStart(const char c) const { return POSSIBLE_IDENTIFIER_START[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLScheme(const char c) const { return URL_SCHEME[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLHost(const char c) const { return URL_HOST[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLSegment(const char c) const { return URL_SEGMENT[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLSegmentNC(const char c) const { return URL_SEGMENT_NC[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLPath(const char c) const { return URL_PATH[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLSearch(const char c) const { return URL_SEARCH[static_cast<uint8_t>(c)]; }
    constexpr bool IsURLFragment(const char c) const { return URL_FRAGMENT[static_cast<uint8_t>(c)]; }
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
