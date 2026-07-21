#pragma once

#include <cstdint>
#include <bitset>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <expected>

#include "compiler/ir/Token.hpp"
#include "compiler/ir/Error.hpp"
#include "compiler/text/cursor/String.hpp"

namespace compiler::input {
  class Lexer {
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

    bool IsBreak() const noexcept;
    bool IsASCII() const noexcept;
    bool IsSpace() const noexcept;
    bool IsAlpha() const noexcept;
    bool IsAlNum() const noexcept;
    bool IsUpper() const noexcept;
    bool IsLower() const noexcept;
    bool IsDigit() const noexcept;
    bool IsPunct() const noexcept;
    bool IsSlash() const noexcept;
    bool IsOctal() const noexcept;
    bool IsHex  () const noexcept;
    bool IsIdent() const noexcept;
    bool IsBinary() const noexcept;
    bool IsURLScheme() const noexcept;
    bool IsURLHost() const noexcept;
    bool IsURLSegment() const noexcept;
    bool IsURLSegmentNC() const noexcept;
    bool IsURLPath() const noexcept;
    bool IsURLSearch() const noexcept;
    bool IsURLFragment() const noexcept;
    bool IsURLAuthority() const noexcept;
    bool IsFilePath() const noexcept;
    bool IsBinaryStart() const noexcept;
    bool IsNumberStart() const noexcept;
    bool IsIdentStart() const noexcept;
    bool IsOperandStart() const noexcept;
    bool IsStatementStart() const noexcept;
    bool IsUnaryPrefixStart() const noexcept;
    bool IsUnaryPostfixStart() const noexcept;
    bool IsModifierStart() const noexcept;
    bool IsPossibleSpaceStart() const noexcept;
    bool IsPossibleIdentifierStart() const noexcept;

    bool IsBreak(size_t i) const noexcept;
    bool IsASCII(size_t i) const noexcept;
    bool IsSpace(size_t i) const noexcept;
    bool IsAlpha(size_t i) const noexcept;
    bool IsAlNum(size_t i) const noexcept;
    bool IsUpper(size_t i) const noexcept;
    bool IsLower(size_t i) const noexcept;
    bool IsDigit(size_t i) const noexcept;
    bool IsPunct(size_t i) const noexcept;
    bool IsSlash(size_t i) const noexcept;
    bool IsOctal(size_t i) const noexcept;
    bool IsHex  (size_t i) const noexcept;
    bool IsIdent(size_t i) const noexcept;
    bool IsBinary(size_t i) const noexcept;
    bool IsBinaryStart(size_t i) const noexcept;
    bool IsNumberStart(size_t i) const noexcept;
    bool IsURLScheme(size_t i) const noexcept;
    bool IsURLHost(size_t i) const noexcept;
    bool IsURLSegment(size_t i) const noexcept;
    bool IsURLSegmentNC(size_t i) const noexcept;
    bool IsURLPath(size_t i) const noexcept;
    bool IsURLSearch(size_t i) const noexcept;
    bool IsURLFragment(size_t i) const noexcept;
    bool IsURLAuthority(size_t i) const noexcept;
    bool IsFilePath(size_t i) const noexcept;
    bool IsIdentStart(size_t i) const noexcept;
    bool IsOperandStart(size_t i) const noexcept;
    bool IsStatementStart(size_t i) const noexcept;
    bool IsUnaryPrefixStart(size_t i) const noexcept;
    bool IsUnaryPostfixStart(size_t i) const noexcept;
    bool IsModifierStart(size_t i) const noexcept;
    bool IsPossibleSpaceStart(size_t i) const noexcept;
    bool IsPossibleIdentifierStart(size_t i) const noexcept;

    static bool IsASCII(const char c) noexcept;
    static bool IsSpace(const char c) noexcept;
    static bool IsAlpha(const char c) noexcept;
    static bool IsAlNum(const char c) noexcept;
    static bool IsUpper(const char c) noexcept;
    static bool IsLower(const char c) noexcept;
    static bool IsDigit(const char c) noexcept;
    static bool IsPunct(const char c) noexcept;
    static bool IsSlash(const char c) noexcept;
    static bool IsOctal(const char c) noexcept;
    static bool IsHex  (const char c) noexcept;
    static bool IsIdent(const char c) noexcept;
    static bool IsBinary(const char c) noexcept;
    static bool IsIdentStart(const char c) noexcept;
    static bool IsOperandStart(const char c) noexcept;
    static bool IsStatementStart(const char c) noexcept;
    static bool IsUnaryPrefixStart(const char c) noexcept;
    static bool IsUnaryPostfixStart(const char c) noexcept;
    static bool IsBinaryStart(const char c) noexcept;
    static bool IsModifierStart(const char c) noexcept;
    static bool IsNumberStart(const char c) noexcept;
    static bool IsPossibleSpaceStart(const char c) noexcept;
    static bool IsPossibleIdentifierStart(const char c) noexcept;
    static bool IsURLScheme(const char c) noexcept;
    static bool IsURLHost(const char c) noexcept;
    static bool IsURLSegment(const char c) noexcept;
    static bool IsURLSegmentNC(const char c) noexcept;
    static bool IsURLPath(const char c) noexcept;
    static bool IsURLSearch(const char c) noexcept;
    static bool IsURLFragment(const char c) noexcept;
    static bool IsURLAuthority(const char c) noexcept;
    static bool IsFilePath(const char c) noexcept;
  };
};
