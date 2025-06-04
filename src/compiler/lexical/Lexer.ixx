export module lexical.lexer;

import ir.context._module;

import node.scope.context._module;
import lexical.source;
import lexical.tokens;
import lexical.cursor.string;
import lexical.cursor.code_points;
import lexical.cursor.token;
import utility.print;
import lexical.unicode;
import <vector>;
import <string>;
import <string_view>;
import <type_traits>;
import <algorithm>;
import <charconv>;
import <stdexcept>;
import <utility>;

namespace lexical {
  export class Lexer {
  private:
    using Cursor = typename lexical::cursor::CodePoints;
    using Iterator = typename Cursor::Iterator;

    ir::context::Module& mod;
    Cursor cursor;
    Iterator furthest;
  public:
    constexpr bool Done() const { return cursor.Done(); }
    constexpr bool Valid() const { return cursor.Valid(); }
  private:
    struct Position {
      Iterator start; // The saved cursor position
      size_t size; // The saved tokens length
    };

    constexpr void Advance(size_t n = 1) { cursor.Advance(n); }
    constexpr void Retreat(size_t n = 1) { cursor.Retreat(n); }
    constexpr void Retreat(Iterator iter) { cursor.Retreat(iter); }
    constexpr decltype(auto) Peek() const { return cursor.Peek(); }
    constexpr decltype(auto) Peek(size_t n) const { return cursor.Peek(n); }
    constexpr bool Check(const char c) const { return cursor.Check(c); }
    // constexpr bool Check(const char32_t c) const { return cursor.Check(c); }
    constexpr bool Check(const std::string_view view) const { return cursor.Check(view); }
    // constexpr bool Check(const std::u32string_view view) const { return cursor.Check(view); }
    constexpr bool Match(const char c) { return cursor.Match(c); }
    // constexpr bool Match(const char32_t c) { return cursor.Match(c); }
    constexpr bool Match(const std::string_view view) { return cursor.Match(view); }
    // constexpr bool Match(const std::u32string_view view) { return cursor.Match(view); }
    constexpr bool Keyword(const std::string_view view) { return cursor.Keyword(view); }
    // constexpr bool Keyword(const std::u32string_view view) { return cursor.Keyword(view); }
    constexpr decltype(auto) cbegin() const { return cursor.cbegin(); }
    constexpr decltype(auto) cend() const { return cursor.cend(); }
    Position Start() const { return Position{cursor.cbegin(), mod.Tokens().size()}; }

    void Save(lexical::Tokens type, uint32_t width) {
      uint32_t index = static_cast<uint32_t>(std::distance(mod.Tokens().cbegin(), cbegin()));
      // uint32_t index = mod.Characters().size();

      while (converted < cbegin()) {
        auto length = lexical::Unicode::GetCodePointLength(converted);
        char32_t code_point = lexical::Unicode::GetCodePoint(converted, length);

        converted += length;
        mod.Append(code_point);
      }

      mod.Append(lexical::Token{type, static_cast<uint16_t>(width), index});
    }
  public:
    Lexer(ir::context::Module& mod)
      : mod{mod}, cursor{mod.Source()}
    {
      WS(); // Consume any leading white space
    }

    bool WhiteSpace() {
      auto start = Start();
      uint32_t width = 0;

      while (Valid()) {
        switch (Peek()) {
          case U'\u2028': // Unicode line separator
          case U'\u2029': // Unicode paragraph separator
          case U'\n': {
            if (width > 0) {
              Save(lexical::Tokens::WHITESPACE, width);
              width = 0;
            }

            Advance();
            Save(lexical::Tokens::NEWLINE, 1);
            break;
          }
          case U'\r':
          case U'\t':
          case U'\v':
          case U'\f':
          case U' ': {
            Advance();
            ++width;
            break;
          }
          case U'/': {
            if (width > 0) {
              Save(lexical::Tokens::WHITESPACE, width);
              width = 0;
            }

            if (Peek(1) == U'/') {
              Advance(2);
              Save(lexical::Tokens::COMMENT_OPEN, 2);

              while (true) {
                if (Done()) {
                  if (width > 0) {
                    Save(lexical::Tokens::COMMENT, width);
                  }

                  Save(lexical::Tokens::COMMENT_CLOSE, 0);
                  return true;
                } else if (cursor.IsBreak()) {
                  if (width > 0) {
                    Save(lexical::Tokens::COMMENT, width);
                    width = 0;
                  }

                  Save(lexical::Tokens::COMMENT_CLOSE, 0);
                  Advance();
                  Save(lexical::Tokens::NEWLINE, 1);
                  break;
                }

                Advance();
                ++width;
              }
            } else if (Peek(1) == U'*') {
              Advance(2);
              Save(lexical::Tokens::MULTI_LINE_COMMENT_OPEN, 2);

              while (Valid()) {
                if (Peek() == U'*' && Peek(1) == U'/') {
                  if (width > 0) {
                    Save(lexical::Tokens::COMMENT, width);
                    width = 0;
                  }

                  Advance(2);
                  Save(lexical::Tokens::MULTI_LINE_COMMENT_CLOSE, 2);
                  break;
                }

                Advance();
                ++width;
              }
            } else {
              return mod.Tokens().size() > start.size;
            }

            break;
          }
          default: {
            if (static_cast<uint8_t>(Peek()) > 127) {
              auto iter = cbegin();
              auto code_point = CodePoint();

              // Check for a line or paragraph separator
              if (code_point == 0x2028 || code_point == 0x2029) {
                if (width > 0) {
                  Save(lexical::Tokens::WHITESPACE, width);
                  width = 0;
                }

                Save(lexical::Tokens::NEWLINE, 3);
              } else if (Unicode::IsWhiteSpace(code_point)) {
                width += static_cast<uint32_t>(std::distance(iter, cbegin()));
              } else {
                Retreat(iter); // Undo the `CodePoint` because it wasn't whitespace

                if (width > 0) {
                  Save(lexical::Tokens::WHITESPACE, width);
                }

                return mod.Tokens().size() > start.size;
              }
            }
          }
        }
      }

      return mod.Tokens().size() > start.size;
    }

    bool WS() {
      return cursor.IsPossibleSpaceStart() && WhiteSpace();
    }

    // Optional white space
    bool OWS() { WS(); return true; }

    // Keywords
    bool Null() { return Keyword(lexical::Tokens::_NULL, "null"); }
    bool Undefined() { return Keyword(lexical::Tokens::UNDEFINED, "undefined"); }
    bool True() { return Keyword(lexical::Tokens::TRUE, "true"); }
    bool False() { return Keyword(lexical::Tokens::FALSE, "false"); }
    bool Import() { return Keyword(lexical::Tokens::IMPORT, "import"); }
    bool Register() { return Keyword(lexical::Tokens::REGISTER, "register"); }
    bool From() { return Keyword(lexical::Tokens::FROM, "from"); }
    bool With() { return Keyword(lexical::Tokens::WITH, "with"); }
    bool If() { return Keyword(lexical::Tokens::IF, "if"); }
    bool Else() { return Keyword(lexical::Tokens::ELSE, "else"); }
    bool Export() { return Keyword(lexical::Tokens::EXPORT, "export"); }
    bool Do() { return Keyword(lexical::Tokens::DO, "do"); }
    bool While() { return Keyword(lexical::Tokens::WHILE, "while"); }
    bool Is() { return Keyword(lexical::Tokens::IS, "is"); }
    bool Has() { return Keyword(lexical::Tokens::HAS, "has"); }
    bool In() { return Keyword(lexical::Tokens::IN, "in"); }
    bool For() { return Keyword(lexical::Tokens::FOR, "for"); }
    bool As() { return Keyword(lexical::Tokens::AS, "as"); }
    bool Use() { return Keyword(lexical::Tokens::USE, "use"); }
    bool Default() { return Keyword(lexical::Tokens::DEFAULT, "default"); }
    bool Auto() { return Keyword(lexical::Tokens::AUTO, "auto"); }
    bool Void() { return Keyword(lexical::Tokens::VOID, "void"); }
    bool Match() { return Keyword(lexical::Tokens::MATCH, "match"); }
    // bool Cover() { return Keyword(lexical::Tokens::COVER, "cover"); }
    bool Yield() { return Keyword(lexical::Tokens::YIELD, "yield"); }
    bool Await() { return Keyword(lexical::Tokens::AWAIT, "await"); }
    bool Async() { return Keyword(lexical::Tokens::ASYNC, "async"); }
    bool Compiler() { return Keyword(lexical::Tokens::COMPILER, "compiler"); }
    bool Comtime() { return Keyword(lexical::Tokens::COMTIME, "comtime"); }
    bool Runtime() { return Keyword(lexical::Tokens::RUNTIME, "runtime"); }
    bool Break() { return Keyword(lexical::Tokens::BREAK, "break"); }
    bool Case() { return Keyword(lexical::Tokens::CASE, "case"); }
    bool Continue() { return Keyword(lexical::Tokens::CONTINUE, "continue"); }
    bool Return() { return Keyword(lexical::Tokens::RETURN, "return"); }
    bool This() { return Keyword(lexical::Tokens::THIS, "this"); }
    // bool Self() { return Keyword(lexical::Tokens::SELF, "self"); }
    bool Super() { return Keyword(lexical::Tokens::SUPER, "super"); }
    bool NotKeyword() { return Keyword(lexical::Tokens::NOT, "not"); }
    bool AndKeyword() { return Keyword(lexical::Tokens::AND, "and"); }
    bool OrKeyword() { return Keyword(lexical::Tokens::OR, "or"); }
    bool Private() { return Keyword(lexical::Tokens::PRIVATE, "private"); }
    bool Public() { return Keyword(lexical::Tokens::PUBLIC, "public"); }
    bool Protected() { return Keyword(lexical::Tokens::PROTECTED, "protected"); }
    bool Const() { return Keyword(lexical::Tokens::CONST, "const"); }
    bool Let() { return Keyword(lexical::Tokens::LET, "let"); }
    // bool Mutable() { return Keyword(lexical::Tokens::MUTABLE, "mutable"); }
    // bool Immutable() { return Keyword(lexical::Tokens::IMMUTABLE, "immutable"); }
    bool Static() { return Keyword(lexical::Tokens::STATIC, "static"); }
    bool Expected() { return Keyword(lexical::Tokens::EXPECTED, "expected"); }
    bool Unexpected() { return Keyword(lexical::Tokens::UNEXPECTED, "unexpected"); }

    // Symbols
    bool ParameterOpen() { return Test(lexical::Tokens::PARAMETER_OPEN, '('); }
    bool ParameterClose() { return Test(lexical::Tokens::PARAMETER_CLOSE, ')'); }
    bool CaptureOpen() { return Test(lexical::Tokens::CAPTURE_OPEN, '['); }
    bool CaptureClose() { return Test(lexical::Tokens::CAPTURE_CLOSE, ']'); }
    bool TupleOpen() { return Test(lexical::Tokens::TUPLE_OPEN, '('); }
    bool TupleClose() { return Test(lexical::Tokens::TUPLE_CLOSE, ')'); }
    bool ScopeOpen() { return Test(lexical::Tokens::SCOPE_OPEN, '{'); }
    bool ScopeClose() { return Test(lexical::Tokens::SCOPE_CLOSE, '}'); }
    bool ObjectOpen() { return Test(lexical::Tokens::OBJECT_OPEN, '{'); }
    bool ObjectClose() { return Test(lexical::Tokens::OBJECT_CLOSE, '}'); }
    bool ArrayOpen() { return Test(lexical::Tokens::ARRAY_OPEN, '['); }
    bool ArrayClose() { return Test(lexical::Tokens::ARRAY_CLOSE, ']'); }
    bool EnumOpen() { return Test(lexical::Tokens::ENUM_OPEN, '<'); }
    bool EnumClose() { return Test(lexical::Tokens::ENUM_CLOSE, '>'); }
    // bool ImportsOpen() { return Test(lexical::Tokens::IMPORTS_OPEN, '{'); }
    // bool ImportsClose() { return Test(lexical::Tokens::IMPORTS_CLOSE, '}'); }
    bool CharOpen() { return Test(lexical::Tokens::CHAR_OPEN, '\''); }
    bool CharClose() { return Test(lexical::Tokens::CHAR_CLOSE, '\''); }
    bool StringOpen() { return Test(lexical::Tokens::STRING_OPEN, '"'); }
    bool StringClose() { return Test(lexical::Tokens::STRING_CLOSE, '"'); }
    bool TemplateStringOpen() { return Test(lexical::Tokens::TEMPLATE_STRING_OPEN, '`'); }
    bool TemplateStringClose() { return Test(lexical::Tokens::TEMPLATE_STRING_CLOSE, '`'); }
    bool TemplateStringExpressionOpen() { return Test(lexical::Tokens::TEMPLATE_STRING_EXPRESSION_OPEN, '{'); }
    bool TemplateStringExpressionClose() { return Test(lexical::Tokens::TEMPLATE_STRING_EXPRESSION_CLOSE, '}'); }
    bool EnumPropertyOpen() { return Test(lexical::Tokens::ENUM_PROPERTY_OPEN, '<'); }
    bool EnumPropertyClose() { return Test(lexical::Tokens::ENUM_PROPERTY_CLOSE, '>'); }
    bool ConditionOpen() { return Test(lexical::Tokens::CONDITION_OPEN, '('); }
    bool ConditionClose() { return Test(lexical::Tokens::CONDITION_CLOSE, ')'); }
    bool Arrow() { return Test(lexical::Tokens::ARROW, "->"); }
    bool FromShortcut() { return Test(lexical::Tokens::FROM_SHORTCUT, ':'); }
    bool Wildcard() { return Test(lexical::Tokens::WILDCARD, '*'); }
    bool Comma() { return Test(lexical::Tokens::COMMA, ','); }
    bool Semicolon() { return Test(lexical::Tokens::SEMICOLON, ';'); }
    bool CommentOpen() { return Test(lexical::Tokens::COMMENT_OPEN, "//"); }
    bool CommentClose() { Save(lexical::Tokens::COMMENT_CLOSE, 0); return true; }
    bool MultiLineCommentOpen() { return Test(lexical::Tokens::MULTI_LINE_COMMENT_OPEN, "/*"); }
    bool MultiLineCommentClose() { return Test(lexical::Tokens::MULTI_LINE_COMMENT_CLOSE, "*/"); }
    bool DestructuredArrayOpen() { return Test(lexical::Tokens::DESTRUCTURED_ARRAY_OPEN, '['); }
    bool DestructuredArrayClose() { return Test(lexical::Tokens::DESTRUCTURED_ARRAY_CLOSE, ']'); }
    bool DestructuredTupleOpen() { return Test(lexical::Tokens::DESTRUCTURED_TUPLE_OPEN, '('); }
    bool DestructuredTupleClose() { return Test(lexical::Tokens::DESTRUCTURED_TUPLE_CLOSE, ')'); }
    bool DestructuredObjectOpen() { return Test(lexical::Tokens::DESTRUCTURED_OBJECT_OPEN, '{'); }
    bool DestructuredObjectClose() { return Test(lexical::Tokens::DESTRUCTURED_OBJECT_CLOSE, '}'); }
    bool DestructuredEnumOpen() { return Test(lexical::Tokens::DESTRUCTURED_ENUM_OPEN, '<'); }
    bool DestructuredEnumClose() { return Test(lexical::Tokens::DESTRUCTURED_ENUM_CLOSE, '>'); }

    // Unary operators
    bool Reference() { return Test(lexical::Tokens::REFERENCE, '&'); }
    bool OptionalReference() { return Test(lexical::Tokens::OPTIONAL_REFERENCE, '^'); }
    bool Symbol() { return Test(lexical::Tokens::SYMBOL, '$'); }
    // bool Optional() { return Test(lexical::Tokens::OPTIONAL, '?'); }
    bool Copy() { return Test(lexical::Tokens::COPY, '@'); }
    // bool Borrow() { return Test(lexical::Tokens::BORROW, '#'); }
    bool Positive() { return Test(lexical::Tokens::POSITIVE, '+'); }
    bool Negative() { return Test(lexical::Tokens::NEGATIVE, '-'); }
    bool Increment() { return Test(lexical::Tokens::INCREMENT, "++"); }
    bool Decrement() { return Test(lexical::Tokens::DECREMENT, "--"); }
    bool Not() { return Test(lexical::Tokens::NOT, '!'); }
    bool Contract() { return Test(lexical::Tokens::CONTRACT, '!'); }
    bool Spread() { return Test(lexical::Tokens::SPREAD, "..."); }
    // bool Move() { return Test(lexical::Tokens::MOVE, '='); }
    bool BitwiseNot() { return Test(lexical::Tokens::BITWISE_NOT, '~'); }

    bool UnaryPrefixOperatorHelper() {
      switch (Peek()) {
        case '+': return Increment() || Positive();
        case '-': return Decrement() || Negative();
        case '&': return Reference();
        case '^': return OptionalReference();
        case '$': return Symbol();
        case '@': return Copy();
        case '!': return Not();
        case '.': return Spread();
        // case '=': return Move();
        case '~': return BitwiseNot();
        case 'a': return Await();
        case 'n': return KeywordNot();
        case 'e': return Expected();
        case 'u': return Unexpected();
        default: return false;
      }
    }

    inline bool UnaryPrefixOperator() { return cursor.IsUnaryPrefixStart() && UnaryPrefixOperatorHelper(); }

    // Binary operators
    bool Assign() { return Test(lexical::Tokens::ASSIGN, '='); }
    // bool Constructor() { return Test(lexical::Tokens::CONSTRUCTOR, "=>"); }
    // bool Emplace() { return Test(lexical::Tokens::EMPLACE, "=?"); }
    bool Equal() { return Test(lexical::Tokens::EQUAL, "=="); }
    bool AssertEqual() { return Test(lexical::Tokens::ASSERT_EQUAL, "==="); }
    bool NotEqual() { return Test(lexical::Tokens::NOT_EQUAL, "!="); }
    bool AssertNotEqual() { return Test(lexical::Tokens::ASSERT_NOT_EQUAL, "!=="); }
    bool Add() { return Test(lexical::Tokens::ADD, '+'); }
    bool AssignAdd() { return Test(lexical::Tokens::ASSIGN_ADD, "+="); }
    bool Subtract() { return Test(lexical::Tokens::SUBTRACT, '-'); }
    bool AssignSubtract() { return Test(lexical::Tokens::ASSIGN_SUBTRACT, "-="); }
    bool Multiply() { return Test(lexical::Tokens::MULTIPLY, '*'); }
    bool AssignMultiply() { return Test(lexical::Tokens::ASSIGN_MULTIPLY, "*="); }
    bool Exponent() { return Test(lexical::Tokens::EXPONENT, "**"); }
    bool AssignExponent() { return Test(lexical::Tokens::ASSIGN_EXPONENT, "**="); }
    bool Divide() { return Test(lexical::Tokens::DIVIDE, '/'); }
    bool AssignDivide() { return Test(lexical::Tokens::ASSIGN_DIVIDE, "/="); }
    bool Modulo() { return Test(lexical::Tokens::MODULO, '%'); }
    bool AssignModulo() { return Test(lexical::Tokens::ASSIGN_MODULO, "%="); }
    bool BitwiseXor() { return Test(lexical::Tokens::BITWISE_XOR, '^'); }
    bool BitwiseAssignXor() { return Test(lexical::Tokens::BITWISE_ASSIGN_XOR, "^="); }
    bool BitwiseOr() { return Test(lexical::Tokens::BITWISE_OR, '|'); }
    bool BitwiseAssignOr() { return Test(lexical::Tokens::BITWISE_ASSIGN_OR, "|="); }
    bool InclusiveRange() { return Test(lexical::Tokens::INCLUSIVE_RANGE, "..."); }
    bool ExclusiveRange() { return Test(lexical::Tokens::EXCLUSIVE_RANGE, ".."); }
    bool MemberAccess() { return Test(lexical::Tokens::MEMBER_ACCESS, '.'); }
    // bool StaticMemberAccess() { return Test(lexical::Tokens::STATIC_MEMBER_ACCESS, ':'); }
    bool ConditionalMemberAccess() { return Test(lexical::Tokens::CONDITIONAL_MEMBER_ACCESS, "?."); }
    bool SymbolMemberAccess() { return Test(lexical::Tokens::SYMBOL_MEMBER_ACCESS, "$."); }
    bool Lesser() { return Test(lexical::Tokens::LESSER, '<'); }
    bool LesserOrEqual() { return Test(lexical::Tokens::LESSER_OR_EQUAL, "<="); }
    bool AssertLesserOrEqual() { return Test(lexical::Tokens::ASSERT_LESSER_OR_EQUAL, "<=="); }
    bool Greater() { return Test(lexical::Tokens::GREATER, '>'); }
    bool GreaterOrEqual() { return Test(lexical::Tokens::GREATER_OR_EQUAL, ">="); }
    bool AssertGreaterOrEqual() { return Test(lexical::Tokens::ASSERT_GREATER_OR_EQUAL, ">=="); }
    bool TripleLeftShift() { return Test(lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT, "<<<"); }
    bool BitwiseLeftShift() { return Test(lexical::Tokens::BITWISE_LEFT_SHIFT, "<<"); }
    bool BitwiseAssignLeftShift() { return Test(lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT, "<<="); }
    bool BitwiseTripleRightShift() { return Test(lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT, ">>>"); }
    bool BitwiseRightShift() { return Test(lexical::Tokens::BITWISE_RIGHT_SHIFT, ">>"); }
    bool BitwiseAssignRightShift() { return Test(lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT, ">>="); }
    bool And() { return Test(lexical::Tokens::AND, "&&"); }
    bool Or() { return Test(lexical::Tokens::OR, "||"); }
    bool Unwrap() { return Test(lexical::Tokens::UNWRAP, "??"); }
    // bool TypeStart() { return Test(lexical::Tokens::TYPE_START, ':'); }

    bool BinaryOperatorHelper(bool in_enum = false) {
      switch (Peek()) {
        case '!': {
          switch (Peek(1)) {
            // case '!': {
            //   switch (Peek(2)) {
            //     case '=': return Skip(lexical::Tokens::ASSIGN_TRUTHY_AND, 3);
            //     default: return Skip(lexical::Tokens::TRUTHY_AND, 2);
            //   }
            // }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_NOT_EQUAL, 3);
                default: return Skip(lexical::Tokens::NOT_EQUAL, 2);
              }
            }
            default: return false;
          }
        }
        case '=': {
          switch (Peek(1)) {
            // case '>': return Skip(lexical::Tokens::CONSTRUCTOR, 2);
            // case '?': return Skip(lexical::Tokens::EMPLACE, 2);
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_EQUAL, 3);
                default: return Skip(lexical::Tokens::EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::ASSIGN, 1);
          }
        }
        case '<': {
          switch (Peek(1)) {
            case '<': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT, 3);
                case '<': return Skip(lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT, 3);
                default: return Skip(lexical::Tokens::BITWISE_LEFT_SHIFT, 2);
              }
            }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_LESSER_OR_EQUAL, 3);
                default: return Skip(lexical::Tokens::LESSER_OR_EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::LESSER, 1);
          }
        }
        case '>': {
          if (in_enum) return false;

          switch (Peek(1)) {
            case '>': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT, 3);
                case '>': return Skip(lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT, 3);
                default: return Skip(lexical::Tokens::BITWISE_RIGHT_SHIFT, 2);
              }
            }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_GREATER_OR_EQUAL, 3);
                default: return Skip(lexical::Tokens::GREATER_OR_EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::GREATER, 1);
          }
        }
        case '+': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_ADD, 2);
            default: return Skip(lexical::Tokens::ADD, 1);
          }
        }
        case '-': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_SUBTRACT, 2);
            default: return Skip(lexical::Tokens::SUBTRACT, 1);
          }
        }
        case '*': {
          switch (Peek(1)) {
            case '*': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_EXPONENT, 3);
                default: return Skip(lexical::Tokens::EXPONENT, 2);
              }
            }
            case '=': return Skip(lexical::Tokens::ASSIGN_MULTIPLY, 2);
            default: return Skip(lexical::Tokens::MULTIPLY, 1);
          }
        }
        case '/': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_DIVIDE, 2);
            default: return Skip(lexical::Tokens::DIVIDE, 1);
          }
        }
        case '%': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_MODULO, 2);
            default: return Skip(lexical::Tokens::MODULO, 1);
          }
        }
        case '^': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_XOR, 2);
            default: return Skip(lexical::Tokens::BITWISE_XOR, 1);
          }
        }
        case '?': {
          switch (Peek(1)) {
            case '.': return Skip(lexical::Tokens::CONDITIONAL_MEMBER_ACCESS, 2);
            case '?': return Skip(lexical::Tokens::UNWRAP, 2);
            // case '=': return Skip(lexical::Tokens::ASSIGN_OPTIONAL, 2);
            // default: return Skip(lexical::Tokens::TERNARY_CONDITION, 1);
            default: return false;
          }
        }
        case '$': {
          switch (Peek(1)) {
            case '.': return Skip(lexical::Tokens::SYMBOL_MEMBER_ACCESS, 2);
            default: return false;
          }
        }
        case '&': {
          switch (Peek(1)) {
            case '&': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_AND, 3);
                default: return Skip(lexical::Tokens::AND, 2);
              }
            }
            default: return Skip(lexical::Tokens::BITWISE_AND, 1);
          }
        }
        case '|': {
          switch (Peek(1)) {
            case '|': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_OR, 3);
                default: return Skip(lexical::Tokens::OR, 2);
              }
            }
            case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_OR, 2);
            default: return Skip(lexical::Tokens::BITWISE_OR, 1);
          }
        }
        case 'o': return OrKeyword();
        case 'a': return AndKeyword();
        case 'w': return With();
        case 'u': return Use();
        case '.': {
          switch (Peek(1)) {
            case '.': {
              switch (Peek(2)) {
                case '.': return Skip(lexical::Tokens::INCLUSIVE_RANGE, 3);
                default: return Skip(lexical::Tokens::EXCLUSIVE_RANGE, 2);
              }
            }
            default: return Skip(lexical::Tokens::MEMBER_ACCESS, 1);
          }
        }
      }

      return false;
    }

    inline bool BinaryOperator(bool in_enum = false) { return cursor.IsBinaryStart() && BinaryOperatorHelper(in_enum); }

    bool Escape() {
      if (Peek() != '\\') return false;

      Advance(); // Consume '\\'
      uint32_t width = 1;

      // Unicode escape or code point
      if (Peek() == 'u') {
        Advance();
        ++width;

        // Unicode code point escape \u{XXXXX...}
        if (Peek() == '{') {
          Advance();
          ++width;

          while (Valid() && Peek() != '}') {
            Advance(); // Consume characters within {}
            ++width;
          }

          if (Peek() == '}') {
            Advance();
            ++width;
          } else {
            return Error("Escape sequence expected a closing '}' brace");
          }

          Save(lexical::Tokens::ESCAPE_UNICODE_CODEPOINT, width);
        } else { // Unicode escape \uXXXX
          for (int i = 0; i < 4; ++i) {
            if (Done() || !cursor.IsHex()) {
              return Error("Escape sequence expected 4 hexadecimal digits");
            }

            Advance(); // Consume hexadecimal digit
            ++width;
          }

          Save(lexical::Tokens::ESCAPE_UNICODE_SHORT, width);
        }
      } else if (Peek() == 'x') { // Hexadecimal escape \xXX
        Advance();
        ++width;

        for (int i = 0; i < 2; ++i) {
          if (Done() || !cursor.IsHex()) {
            return Error("Escape sequence expected 2 hexadecimal digits");
          }

          Advance(); // Consume hexadecimal digit
          ++width;
        }

        Save(lexical::Tokens::ESCAPE_HEX_CODE, width);
      } else if (Valid()) { // Single character escape
        auto c = Peek();
        Advance(); // Consume the character following '\'
        ++width;

        if      (c == '\n') Save(lexical::Tokens::ESCAPE_NEWLINE, width);
        else if (c == '\t') Save(lexical::Tokens::ESCAPE_TAB, width);
        else if (c == '\b') Save(lexical::Tokens::ESCAPE_BACKSPACE, width);
        else if (c == '\r') Save(lexical::Tokens::ESCAPE_RETURN, width);
        else if (c == '\f') Save(lexical::Tokens::ESCAPE_FORM_FEED, width);
        else Save(lexical::Tokens::ESCAPE_LITERAL, width);
      } else {
        return Error("Unexpected end of content in escape sequence");
      }

      return true;
    }
  };

  export class Lexer {
  private:
    node::scope::context::Module& mod;
    lexical::Source& source;
    lexical::cursor::String cursor;
    // std::string_view stored;
    bool maybe_declaration = false;
    // std::vector<char32_t> saved;
  public:
    constexpr bool Done() const { return cursor.Done(); }
    constexpr bool Valid() const { return cursor.Valid(); }
  private:
    struct Position {
      std::string::const_iterator start;
      size_t size;
    };

    constexpr void Advance(size_t n = 1) { cursor.Advance(n); }
    constexpr void Retreat(size_t n = 1) { cursor.Retreat(n); }
    constexpr void Retreat(std::string::const_iterator iter) { cursor.Retreat(iter); }
    constexpr decltype(auto) Peek() const { return cursor.Peek(); }
    constexpr decltype(auto) Peek(size_t n) const { return cursor.Peek(n); }
    constexpr bool Check(const char c) const { return cursor.Check(c); }
    constexpr bool Check(const std::string_view view) const { return cursor.Check(view); }
    constexpr bool Match(const char c) { return cursor.Match(c); }
    constexpr bool Match(const std::string_view view) { return cursor.Match(view); }
    constexpr bool Keyword(const std::string_view view) { return cursor.Keyword(view); }
    constexpr decltype(auto) cbegin() const { return cursor.cbegin(); }
    constexpr decltype(auto) cend() const { return cursor.cend(); }
    decltype(auto) CodePoint() { return cursor.CodePoint(); }
    Position Start() const { return Position{cursor.cbegin(), source.Size()}; }

    lexical::Tokens Last(size_t i = 0) {
      const auto& tokens = source.Tokens();

      while (tokens.size() > i) {
        const auto& token = tokens.at(tokens.size() - 1 - i++);
        if (!lexical::cursor::Token::IsTrivial(token.Type())) return token.Type();
      }

      return lexical::Tokens::NONE;
    }

    void Rollback(Position position) {
      cursor.Retreat(position.start);
      source.Rollback(position.size);
    }

    void Save(lexical::Tokens type, uint32_t width, uint32_t offset = 0) {
      source.Push(type, width, offset);

      if (!lexical::cursor::Token::IsTrivial(type)) {
        utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(source.Read()));
        // utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(std::string{cbegin() - width, cbegin()}));
      }
    }

    void Store(lexical::Tokens type, uint32_t width, uint32_t offset = 0) {
      return Save(type, width, offset);
      // stored = std::string_view{cbegin(), width};
      // source.Push(type, width, offset);

      // if (!lexical::cursor::Token::IsTrivial(type)) {
      //   utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(source.Read()));
      //   // utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(std::string{cbegin() - width, cbegin()}));
      // }
    }

    bool Skip(lexical::Tokens type, size_t width) {
      Advance(width);
      Save(type, static_cast<uint32_t>(width));
      WS();
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

    template<std::invocable Func>
    bool Run(Func fn) {
      if constexpr (std::same_as<std::invoke_result_t<Func>, void>) {
        fn();
        return true;
      } else if constexpr (std::same_as<std::invoke_result_t<Func>, bool>) {
        return fn();
      } else {
        static_assert(std::disjunction_v<std::is_same<std::invoke_result_t<Func>, void>, 
                                        std::is_same<std::invoke_result_t<Func>, bool>>, 
                                        "Func must return either void or bool.");
      }
    }

    bool ZeroOrMore(const auto& fn) {
      if (Done()) return false;

      while (true) {
        auto start = Start();

        if (!fn()) {
          Rollback(start);
          break;
        }
      }

      return true;
    }

    bool ZeroOrMore(const auto& fn1, const auto& fn2) {
      if (Done()) return false;

      while (true) {
        auto start = Start();

        if (!fn1()) {
          Rollback(start);
          break;
        } else if (!fn2()) {
          break; // Break without rolling back
        }
      }

      return true;
    }

    bool OneOrMore(const auto& fn1, const auto& fn2) {
      if (Done()) return false;

      auto start = Start();

      // Make sure it matches at least once
      if (fn1()) {
        if (!fn2()) {
          return true;
        }
      }
      else {
        Rollback(start);
        return false;
      }

      while (true) {
        auto start = Start();

        if (!fn1()) {
          Rollback(start);
          break;
        } else if (!fn2()) {
          break; // Break without rolling back
        }
      }

      return true;
    }

    bool OneOrMore(const auto& fn) {
      if (Done()) return false;

      auto start = Start();

      // Make sure it matches at least once
      if (!fn()) {
        Rollback(start);
        return false;
      }

      while (true) {
        auto start = Start();

        if (!fn()) {
          Rollback(start);
          break;
        }
      }

      return true;
    }

    bool Test(lexical::Tokens type, const std::string_view value) {
      if (!Match(value)) return false;

      Save(type, static_cast<uint32_t>(value.size()));
      WS();
      return true;
    }

    bool Test(lexical::Tokens type, const char value) {
      if (!Match(value)) return false;

      Save(type, 1u);
      WS();

      return true;
    }

    bool Test(lexical::Tokens type, const auto value, const std::string_view expected) {
      if (Test(type, value)) {
        return true;
      } else {
        return Error(type, expected);
      }
    }

    bool Error(lexical::Tokens type, const std::string_view expected) {
      // Print("ERROR: ", expected, " token ", token);

      // Display();

      // recovery = true;
      // Save(lexical::Tokens::ERROR, expected);
      // utility::Debug("Saving token", lexical::Token{type}, utility::Quoted(source.Read()));
      utility::Debug("ERROR:", expected, "token", lexical::Token{type});
      Save(lexical::Tokens::ERROR, 0);
      WS();
      return false;
    }

    bool Error(const std::string_view expected) {
      // Print("ERROR: ", expected);

      // Display();

      // recovery = true;
      // Save(lexical::Tokens::ERROR, expected);
      utility::Debug("ERROR:", expected);
      Save(lexical::Tokens::ERROR, 0);
      WS();
      return false;
    }

    bool Keyword(lexical::Tokens type, const std::string_view value) {
      if (!Keyword(value)) return false;

      Save(type, static_cast<uint32_t>(value.size()));
      WS();
      return true;
    }

    bool Keyword(lexical::Tokens type, const std::string_view value, const std::string_view expected) {
      if (Keyword(type, value)) {
        return true;
      } else {
        return Error(type, expected);
      }
    }
  public:
    Lexer(node::scope::context::Module& mod, lexical::Source& source)
      : mod{mod}, source{source}, cursor{source.Text()}
    {
      WS(); // Consume any leading white space
    }

    bool WhiteSpace() {
      auto start = Start();
      uint32_t width = 0;
      uint32_t offset = 0;

      while (Valid()) {
        switch (Peek()) {
          case '\n': {
            if (width > 0) {
              Save(lexical::Tokens::WHITESPACE, width, offset);
              width = 0;
              offset = 0;
            }

            Advance();
            Save(lexical::Tokens::NEWLINE, 1);
            break;
          }
          case '\t':
          case '\v':
          case '\f':
          case '\r': {
            Advance();

            if (Valid() && Peek() == '\n') {
              if (width > 0) {
                Save(lexical::Tokens::WHITESPACE, width, offset);
                width = 0;
                offset = 0;
              }

              Advance();
              Save(lexical::Tokens::NEWLINE, 2);
            }

            break;
          }
          case ' ': {
            Advance();
            ++width;
            break;
          }
          case '/': {
            if (width > 0) {
              Save(lexical::Tokens::WHITESPACE, width, offset);
              width = 0;
              offset = 0;
            }

            if (Peek(1) == '/') {
              Advance(2);
              Save(lexical::Tokens::COMMENT_OPEN, 2);

              while (true) {
                if (Done()) {
                  if (width > 0) {
                    Save(lexical::Tokens::COMMENT, width, offset);
                  }

                  return true;
                } else if (Peek() == '\n') {
                  if (width > 0) {
                    Save(lexical::Tokens::COMMENT, width, offset);
                    width = 0;
                    offset = 0;
                  }

                  Save(lexical::Tokens::COMMENT_CLOSE, 0);
                  Advance();
                  Save(lexical::Tokens::NEWLINE, 1);
                  break;
                }

                Advance();
                ++width;
              }
            } else if (Peek(1) == '*') {
              Advance(2);
              Save(lexical::Tokens::MULTI_LINE_COMMENT_OPEN, 2);

              while (Valid()) {
                if (Peek() == '*' && Peek(1) == '/') {
                  if (width > 0) {
                    Save(lexical::Tokens::COMMENT, width, offset);
                    width = 0;
                    offset = 0;
                  }

                  Advance(2);
                  Save(lexical::Tokens::MULTI_LINE_COMMENT_CLOSE, 2);
                  break;
                }

                Advance();
                ++width;
              }
            } else {
              return source.Size() > start.size;
            }

            break;
          }
          default: {
            if (static_cast<uint8_t>(Peek()) > 127) {
              auto iter = cbegin();
              auto code_point = CodePoint();

              // Check for a line or paragraph separator
              if (code_point == 0x2028 || code_point == 0x2029) {
                if (width > 0) {
                  Save(lexical::Tokens::WHITESPACE, width, offset);
                  width = 0;
                  offset = 0;
                }

                Save(lexical::Tokens::NEWLINE, 3, 2);
              } else if (Unicode::IsWhiteSpace(code_point)) {
                uint32_t length = static_cast<uint32_t>(std::distance(iter, cbegin()));
                width += length;
                offset += length - 1;
              } else {
                Retreat(iter); // Undo the `CodePoint`
              }
            }

            if (width > 0) {
              Save(lexical::Tokens::WHITESPACE, width, offset);
            }

            return source.Size() > start.size;
          }
        }
      }

      return source.Size() > start.size;
    }

    bool WS() {
      return cursor.IsPossibleSpaceStart() && WhiteSpace();
    }

    // Optional white space
    bool OWS() { WS(); return true; }

    // Keywords
    bool Null() { return Keyword(lexical::Tokens::_NULL, "null"); }
    bool Undefined() { return Keyword(lexical::Tokens::UNDEFINED, "undefined"); }
    bool True() { return Keyword(lexical::Tokens::TRUE, "true"); }
    bool False() { return Keyword(lexical::Tokens::FALSE, "false"); }
    bool Import() { return Keyword(lexical::Tokens::IMPORT, "import"); }
    bool Register() { return Keyword(lexical::Tokens::REGISTER, "register"); }
    bool From() { return Keyword(lexical::Tokens::FROM, "from"); }
    bool With() { return Keyword(lexical::Tokens::WITH, "with"); }
    bool If() { return Keyword(lexical::Tokens::IF, "if"); }
    bool Else() { return Keyword(lexical::Tokens::ELSE, "else"); }
    bool Export() { return Keyword(lexical::Tokens::EXPORT, "export"); }
    bool Do() { return Keyword(lexical::Tokens::DO, "do"); }
    bool While() { return Keyword(lexical::Tokens::WHILE, "while"); }
    bool Is() { return Keyword(lexical::Tokens::IS, "is"); }
    bool In() { return Keyword(lexical::Tokens::IN, "in"); }
    bool For() { return Keyword(lexical::Tokens::FOR, "for"); }
    bool As() { return Keyword(lexical::Tokens::AS, "as"); }
    bool Use() { return Keyword(lexical::Tokens::USE, "use"); }
    bool Default() { return Keyword(lexical::Tokens::DEFAULT, "default"); }
    bool Auto() { return Keyword(lexical::Tokens::AUTO, "auto"); }
    bool Void() { return Keyword(lexical::Tokens::VOID, "void"); }
    bool Match() { return Keyword(lexical::Tokens::MATCH, "match"); }
    bool Cover() { return Keyword(lexical::Tokens::COVER, "cover"); }
    bool Yield() { return Keyword(lexical::Tokens::YIELD, "yield"); }
    bool Await() { return Keyword(lexical::Tokens::AWAIT, "await"); }
    bool Async() { return Keyword(lexical::Tokens::ASYNC, "async"); }
    bool Compiler() { return Keyword(lexical::Tokens::COMPILER, "compiler"); }
    bool Comtime() { return Keyword(lexical::Tokens::COMTIME, "comtime"); }
    bool Runtime() { return Keyword(lexical::Tokens::RUNTIME, "runtime"); }
    bool Break() { return Keyword(lexical::Tokens::BREAK, "break"); }
    bool Case() { return Keyword(lexical::Tokens::CASE, "case"); }
    bool Continue() { return Keyword(lexical::Tokens::CONTINUE, "continue"); }
    bool Return() { return Keyword(lexical::Tokens::RETURN, "return"); }
    bool This() { return Keyword(lexical::Tokens::THIS, "this"); }
    bool Self() { return Keyword(lexical::Tokens::SELF, "self"); }
    bool Super() { return Keyword(lexical::Tokens::SUPER, "super"); }
    bool NotKeyword() { return Keyword(lexical::Tokens::NOT, "not"); }
    bool AndKeyword() { return Keyword(lexical::Tokens::AND, "and"); }
    bool OrKeyword() { return Keyword(lexical::Tokens::OR, "or"); }
    bool Private() { return Keyword(lexical::Tokens::PRIVATE, "private"); }
    bool Public() { return Keyword(lexical::Tokens::PUBLIC, "public"); }
    bool Protected() { return Keyword(lexical::Tokens::PROTECTED, "protected"); }
    bool Const() { return Keyword(lexical::Tokens::CONST, "const"); }
    bool Let() { return Keyword(lexical::Tokens::LET, "let"); }
    bool Mutable() { return Keyword(lexical::Tokens::MUTABLE, "mutable"); }
    bool Immutable() { return Keyword(lexical::Tokens::IMMUTABLE, "immutable"); }
    bool Static() { return Keyword(lexical::Tokens::STATIC, "static"); }

    // Symbols
    bool ParameterOpen() { return Test(lexical::Tokens::PARAMETER_OPEN, '('); }
    bool ParameterClose() { return Test(lexical::Tokens::PARAMETER_CLOSE, ')'); }
    bool CaptureOpen() { return Test(lexical::Tokens::CAPTURE_OPEN, '['); }
    bool CaptureClose() { return Test(lexical::Tokens::CAPTURE_CLOSE, ']'); }
    bool TupleOpen() { return Test(lexical::Tokens::TUPLE_OPEN, '('); }
    bool TupleClose() { return Test(lexical::Tokens::TUPLE_CLOSE, ')'); }
    bool ScopeOpen() { return Test(lexical::Tokens::SCOPE_OPEN, '{'); }
    bool ScopeClose() { return Test(lexical::Tokens::SCOPE_CLOSE, '}'); }
    bool ObjectOpen() { return Test(lexical::Tokens::OBJECT_OPEN, '{'); }
    bool ObjectClose() { return Test(lexical::Tokens::OBJECT_CLOSE, '}'); }
    bool ArrayOpen() { return Test(lexical::Tokens::ARRAY_OPEN, '['); }
    bool ArrayClose() { return Test(lexical::Tokens::ARRAY_CLOSE, ']'); }
    bool EnumOpen() { return Test(lexical::Tokens::ENUM_OPEN, '<'); }
    bool EnumClose() { return Test(lexical::Tokens::ENUM_CLOSE, '>'); }
    bool ImportsOpen() { return Test(lexical::Tokens::IMPORTS_OPEN, '{'); }
    bool ImportsClose() { return Test(lexical::Tokens::IMPORTS_CLOSE, '}'); }
    bool CharOpen() { return Test(lexical::Tokens::CHAR_OPEN, '\''); }
    bool CharClose() { return Test(lexical::Tokens::CHAR_CLOSE, '\''); }
    bool StringOpen() { return Test(lexical::Tokens::STRING_OPEN, '"'); }
    bool StringClose() { return Test(lexical::Tokens::STRING_CLOSE, '"'); }
    bool TemplateStringOpen() { return Test(lexical::Tokens::TEMPLATE_STRING_OPEN, '`'); }
    bool TemplateStringClose() { return Test(lexical::Tokens::TEMPLATE_STRING_CLOSE, '`'); }
    bool TemplateStringExpressionOpen() { return Test(lexical::Tokens::TEMPLATE_STRING_EXPRESSION_OPEN, '{'); }
    bool TemplateStringExpressionClose() { return Test(lexical::Tokens::TEMPLATE_STRING_EXPRESSION_CLOSE, '}'); }
    bool ComputedPropertyOpen() { return Test(lexical::Tokens::COMPUTED_PROPERTY_OPEN, '<'); }
    bool ComputedPropertyClose() { return Test(lexical::Tokens::COMPUTED_PROPERTY_CLOSE, '>'); }
    bool ConditionOpen() { return Test(lexical::Tokens::CONDITION_OPEN, '('); }
    bool ConditionClose() { return Test(lexical::Tokens::CONDITION_CLOSE, ')'); }
    bool Arrow() { return Test(lexical::Tokens::ARROW, "->"); }
    bool InlineScopeStart() { return Test(lexical::Tokens::INLINE_SCOPE_START, ':'); }
    bool Wildcard() { return Test(lexical::Tokens::WILDCARD, '*'); }
    bool Comma() { return Test(lexical::Tokens::COMMA, ','); }
    bool Semicolon() { return Test(lexical::Tokens::SEMICOLON, ';'); }
    bool CommentOpen() { return Test(lexical::Tokens::COMMENT_OPEN, "//"); }
    bool CommentClose() { Save(lexical::Tokens::COMMENT_CLOSE, 0); return true; }
    bool MultiLineCommentOpen() { return Test(lexical::Tokens::MULTI_LINE_COMMENT_OPEN, "/*"); }
    bool MultiLineCommentClose() { return Test(lexical::Tokens::MULTI_LINE_COMMENT_CLOSE, "*/"); }
    bool DestructuredArrayOpen() { return Test(lexical::Tokens::DESTRUCTURED_ARRAY_OPEN, '['); }
    bool DestructuredArrayClose() { return Test(lexical::Tokens::DESTRUCTURED_ARRAY_CLOSE, ']'); }
    bool DestructuredTupleOpen() { return Test(lexical::Tokens::DESTRUCTURED_TUPLE_OPEN, '('); }
    bool DestructuredTupleClose() { return Test(lexical::Tokens::DESTRUCTURED_TUPLE_CLOSE, ')'); }
    bool DestructuredObjectOpen() { return Test(lexical::Tokens::DESTRUCTURED_OBJECT_OPEN, '{'); }
    bool DestructuredObjectClose() { return Test(lexical::Tokens::DESTRUCTURED_OBJECT_CLOSE, '}'); }
    bool DestructuredEnumOpen() { return Test(lexical::Tokens::DESTRUCTURED_ENUM_OPEN, '<'); }
    bool DestructuredEnumClose() { return Test(lexical::Tokens::DESTRUCTURED_ENUM_CLOSE, '>'); }

    bool NewLine() {
      switch (Peek()) {
        case '\r': {
          Advance();
          if (Peek() == '\n') {
            Advance();
            Save(lexical::Tokens::NEWLINE, 2);
            return true;
          } else {
            Retreat();
            return false;
          }
        }
        case '\n': {
          Advance();
          Save(lexical::Tokens::NEWLINE, 1);
          return true;
        }
      }

      return false;
    }

    bool OptionalSemicolon() { return Semicolon() || true; }

    // Unary operators
    bool Virtual() { return Test(lexical::Tokens::VIRTUAL, '*'); }
    bool Reference() { return Test(lexical::Tokens::REFERENCE, '&'); }
    bool OptionalReference() { return Test(lexical::Tokens::OPTIONAL_REFERENCE, '^'); }
    bool Symbol() { return Test(lexical::Tokens::SYMBOL, '$'); }
    bool Optional() { return Test(lexical::Tokens::OPTIONAL, '?'); }
    bool Copy() { return Test(lexical::Tokens::COPY, '@'); }
    bool Borrow() { return Test(lexical::Tokens::BORROW, '#'); }
    bool Positive() { return Test(lexical::Tokens::POSITIVE, '+'); }
    bool Negative() { return Test(lexical::Tokens::NEGATIVE, '-'); }
    bool Increment() { return Test(lexical::Tokens::INCREMENT, "++"); }
    bool Decrement() { return Test(lexical::Tokens::DECREMENT, "--"); }
    bool Not() { return Test(lexical::Tokens::NOT, '!'); }
    bool Contract() { return Test(lexical::Tokens::CONTRACT, '!'); }
    bool Spread() { return Test(lexical::Tokens::SPREAD, "..."); }
    bool Move() { return Test(lexical::Tokens::MOVE, '='); }
    bool BitwiseNot() { return Test(lexical::Tokens::BITWISE_NOT, '~'); }

    bool UnaryPrefixOperatorHelper() {
      switch (Peek()) {
        case '+': return Increment() || Positive();
        case '-': return Decrement() || Negative();
        case '*': return Virtual();
        case '&': return Reference();
        case '^': return OptionalReference();
        case '$': return Symbol();
        case '#': return Borrow();
        case '@': return Copy();
        case '!': return Not();
        case '.': return Spread();
        case '=': return Move();
        case '~': return BitwiseNot();
        case 'a': return Keyword(lexical::Tokens::AWAIT, "await");
        case 'n': return Keyword(lexical::Tokens::NOT, "not");
        case 'e': return Keyword(lexical::Tokens::EXPECTED, "expected");
        case 'u': return Keyword(lexical::Tokens::UNEXPECTED, "unexpected");
        default: return false;
      }
    }

    bool UnaryPostfixOperatorHelper() {
      switch (Peek()) {
        case '*': return Test(lexical::Tokens::POSTFIX_VIRTUAL, '*');
        case '&': return Test(lexical::Tokens::POSTFIX_REFERENCE, '&');
        case '$': return Test(lexical::Tokens::POSTFIX_SYMBOL, '$');
        case '?': return Test(lexical::Tokens::POSTFIX_OPTIONAL, '?');
        case '@': return Test(lexical::Tokens::POSTFIX_COPY, '@');
        case '#': return Test(lexical::Tokens::POSTFIX_BORROW, '#');
        case '+': return Test(lexical::Tokens::POSTFIX_INCREMENT, "++");
        case '-': return Test(lexical::Tokens::POSTFIX_DECREMENT, "--");
        case '!': return Test(lexical::Tokens::POSTFIX_CONTRACT, '!');
        case '.': return Test(lexical::Tokens::POSTFIX_SPREAD, "...");
        default: return false;
      }
    }

    inline bool UnaryPrefixOperator() { return cursor.IsUnaryPrefixStart() && UnaryPrefixOperatorHelper(); }
    inline bool UnaryPostfixOperator() { return cursor.IsUnaryPostfixStart() && UnaryPostfixOperatorHelper(); }

    // Binary operators
    bool Assign() { return Test(lexical::Tokens::ASSIGN, '='); }
    bool Constructor() { return Test(lexical::Tokens::CONSTRUCTOR, "=>"); }
    bool Emplace() { return Test(lexical::Tokens::EMPLACE, "=?"); }
    bool Equal() { return Test(lexical::Tokens::EQUAL, "=="); }
    bool AssertEqual() { return Test(lexical::Tokens::ASSERT_EQUAL, "==="); }
    bool NotEqual() { return Test(lexical::Tokens::NOT_EQUAL, "!="); }
    bool AssertNotEqual() { return Test(lexical::Tokens::ASSERT_NOT_EQUAL, "!=="); }
    bool Add() { return Test(lexical::Tokens::ADD, '+'); }
    bool AssignAdd() { return Test(lexical::Tokens::ASSIGN_ADD, "+="); }
    bool Subtract() { return Test(lexical::Tokens::SUBTRACT, '-'); }
    bool AssignSubtract() { return Test(lexical::Tokens::ASSIGN_SUBTRACT, "-="); }
    bool Multiply() { return Test(lexical::Tokens::MULTIPLY, '*'); }
    bool AssignMultiply() { return Test(lexical::Tokens::ASSIGN_MULTIPLY, "*="); }
    bool Exponent() { return Test(lexical::Tokens::EXPONENT, "**"); }
    bool AssignExponent() { return Test(lexical::Tokens::ASSIGN_EXPONENT, "**="); }
    bool Divide() { return Test(lexical::Tokens::DIVIDE, '/'); }
    bool AssignDivide() { return Test(lexical::Tokens::ASSIGN_DIVIDE, "/="); }
    bool Modulo() { return Test(lexical::Tokens::MODULO, '%'); }
    bool AssignModulo() { return Test(lexical::Tokens::ASSIGN_MODULO, "%="); }
    bool BitwiseXor() { return Test(lexical::Tokens::BITWISE_XOR, '^'); }
    bool BitwiseAssignXor() { return Test(lexical::Tokens::BITWISE_ASSIGN_XOR, "^="); }
    bool BitwiseOr() { return Test(lexical::Tokens::BITWISE_OR, '|'); }
    bool BitwiseAssignOr() { return Test(lexical::Tokens::BITWISE_ASSIGN_OR, "|="); }
    bool InclusiveRange() { return Test(lexical::Tokens::INCLUSIVE_RANGE, "..."); }
    bool ExclusiveRange() { return Test(lexical::Tokens::EXCLUSIVE_RANGE, ".."); }
    bool MemberAccess() { return Test(lexical::Tokens::MEMBER_ACCESS, '.'); }
    bool StaticMemberAccess() { return Test(lexical::Tokens::STATIC_MEMBER_ACCESS, ':'); }
    bool OptionalMemberAccess() { return Test(lexical::Tokens::OPTIONAL_MEMBER_ACCESS, "?."); }
    bool Lesser() { return Test(lexical::Tokens::LESSER, '<'); }
    bool LesserOrEqual() { return Test(lexical::Tokens::LESSER_OR_EQUAL, "<="); }
    bool AssertLesserOrEqual() { return Test(lexical::Tokens::ASSERT_LESSER_OR_EQUAL, "<=="); }
    bool Greater() { return Test(lexical::Tokens::GREATER, '>'); }
    bool GreaterOrEqual() { return Test(lexical::Tokens::GREATER_OR_EQUAL, ">="); }
    bool AssertGreaterOrEqual() { return Test(lexical::Tokens::ASSERT_GREATER_OR_EQUAL, ">=="); }
    bool TripleLeftShift() { return Test(lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT, "<<<"); }
    bool BitwiseLeftShift() { return Test(lexical::Tokens::BITWISE_LEFT_SHIFT, "<<"); }
    bool BitwiseAssignLeftShift() { return Test(lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT, "<<="); }
    bool BitwiseTripleRightShift() { return Test(lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT, ">>>"); }
    bool BitwiseRightShift() { return Test(lexical::Tokens::BITWISE_RIGHT_SHIFT, ">>"); }
    bool BitwiseAssignRightShift() { return Test(lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT, ">>="); }
    bool And() { return Test(lexical::Tokens::AND, "&&"); }
    bool Or() { return Test(lexical::Tokens::OR, "||"); }
    bool TypeStart() { return Test(lexical::Tokens::TYPE_START, ':'); }

    bool BinaryOperatorHelper(bool in_enum = false) {
      switch (Peek()) {
        case '!': {
          switch (Peek(1)) {
            case '!': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_TRUTHY_AND, 3);
                default: return Skip(lexical::Tokens::TRUTHY_AND, 2);
              }
            }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_NOT_EQUAL, 3);
                default: return Skip(lexical::Tokens::NOT_EQUAL, 2);
              }
            }
            default: return false;
          }
        }
        case '=': {
          switch (Peek(1)) {
            case '>': return Skip(lexical::Tokens::CONSTRUCTOR, 2);
            case '?': return Skip(lexical::Tokens::EMPLACE, 2);
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_EQUAL, 3);
                default: return Skip(lexical::Tokens::EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::ASSIGN, 1);
          }
        }
        case '<': {
          switch (Peek(1)) {
            case '<': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_LEFT_SHIFT, 3);
                case '<': return Skip(lexical::Tokens::BITWISE_TRIPLE_LEFT_SHIFT, 3);
                default: return Skip(lexical::Tokens::BITWISE_LEFT_SHIFT, 2);
              }
            }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_LESSER_OR_EQUAL, 3);
                default: return Skip(lexical::Tokens::LESSER_OR_EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::LESSER, 1);
          }
        }
        case '>': {
          if (in_enum) return false;

          switch (Peek(1)) {
            case '>': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_RIGHT_SHIFT, 3);
                case '>': return Skip(lexical::Tokens::BITWISE_TRIPLE_RIGHT_SHIFT, 3);
                default: return Skip(lexical::Tokens::BITWISE_RIGHT_SHIFT, 2);
              }
            }
            case '=': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSERT_GREATER_OR_EQUAL, 3);
                default: return Skip(lexical::Tokens::GREATER_OR_EQUAL, 2);
              }
            }
            default: return Skip(lexical::Tokens::GREATER, 1);
          }
        }
        case '+': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_ADD, 2);
            default: return Skip(lexical::Tokens::ADD, 1);
          }
        }
        case '-': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_SUBTRACT, 2);
            default: return Skip(lexical::Tokens::SUBTRACT, 1);
          }
        }
        case '*': {
          switch (Peek(1)) {
            case '*': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_EXPONENT, 3);
                default: return Skip(lexical::Tokens::EXPONENT, 2);
              }
            }
            case '=': return Skip(lexical::Tokens::ASSIGN_MULTIPLY, 2);
            default: return Skip(lexical::Tokens::MULTIPLY, 1);
          }
        }
        case '/': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_DIVIDE, 2);
            default: return Skip(lexical::Tokens::DIVIDE, 1);
          }
        }
        case '%': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::ASSIGN_MODULO, 2);
            default: return Skip(lexical::Tokens::MODULO, 1);
          }
        }
        case '^': {
          switch (Peek(1)) {
            case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_XOR, 2);
            default: return Skip(lexical::Tokens::BITWISE_XOR, 1);
          }
        }
        case '?': {
          switch (Peek(1)) {
            case '.': return Skip(lexical::Tokens::OPTIONAL_MEMBER_ACCESS, 2);
            case '?': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_TRUTHY_OR, 3);
                default: return Skip(lexical::Tokens::TRUTHY_OR, 2);
              }
            }
            case '=': return Skip(lexical::Tokens::ASSIGN_OPTIONAL, 2);
            default: return Skip(lexical::Tokens::TERNARY_CONDITION, 1);
          }
        }
        case '&': {
          switch (Peek(1)) {
            case '&': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_AND, 3);
                default: return Skip(lexical::Tokens::AND, 2);
              }
            }
            default: return Skip(lexical::Tokens::BITWISE_AND, 1);
          }
        }
        case '|': {
          switch (Peek(1)) {
            case '|': {
              switch (Peek(2)) {
                case '=': return Skip(lexical::Tokens::ASSIGN_OR, 3);
                default: return Skip(lexical::Tokens::OR, 2);
              }
            }
            case '=': return Skip(lexical::Tokens::BITWISE_ASSIGN_OR, 2);
            default: return Skip(lexical::Tokens::BITWISE_OR, 1);
          }
        }
        case 'o': return Keyword(lexical::Tokens::OR, "or");
        case 'a': return Keyword(lexical::Tokens::AND, "and");
        case 'w': return With();
        case 'u': return Use();
        case '.': {
          switch (Peek(1)) {
            case '.': {
              switch (Peek(2)) {
                case '.': return Skip(lexical::Tokens::INCLUSIVE_RANGE, 3);
                default: return Skip(lexical::Tokens::EXCLUSIVE_RANGE, 2);
              }
            }
            default: return Skip(lexical::Tokens::MEMBER_ACCESS, 1);
          }
        }
        case ':': {
          switch (Peek(1)) {
            case ':': return Skip(lexical::Tokens::STATIC_MEMBER_ACCESS, 2);
            // default: return Skip(lexical::Tokens::TERNARY_ALTERNATE, 1);
            default: return false;
          }
        }
      }

      return false;
    }

    inline bool BinaryOperator(bool in_enum = false) { return cursor.IsBinaryStart() && BinaryOperatorHelper(in_enum); }

    bool Escape() {
      if (Peek() != '\\') return false;

      Advance(); // Consume '\\'
      uint32_t width = 1;

      // Unicode escape or code point
      if (Peek() == 'u') {
        Advance();
        ++width;

        // Unicode code point escape \u{XXXXX...}
        if (Peek() == '{') {
          Advance();
          ++width;

          while (Valid() && Peek() != '}') {
            Advance(); // Consume characters within {}
            ++width;
          }

          if (Peek() == '}') {
            Advance();
            ++width;
          } else {
            return Error("Escape sequence expected a closing '}' brace");
          }

          Save(lexical::Tokens::ESCAPE_UNICODE_CODEPOINT, width);
        } else { // Unicode escape \uXXXX
          for (int i = 0; i < 4; ++i) {
            if (Done() || !cursor.IsHex()) {
              return Error("Escape sequence expected 4 hexadecimal digits");
            }

            Advance(); // Consume hexadecimal digit
            ++width;
          }

          Save(lexical::Tokens::ESCAPE_UNICODE_SHORT, width);
        }
      } else if (Peek() == 'x') { // Hexadecimal escape \xXX
        Advance();
        ++width;

        for (int i = 0; i < 2; ++i) {
          if (Done() || !cursor.IsHex()) {
            return Error("Escape sequence expected 2 hexadecimal digits");
          }

          Advance(); // Consume hexadecimal digit
          ++width;
        }

        Save(lexical::Tokens::ESCAPE_HEX_CODE, width);
      } else if (Valid()) { // Single character escape
        auto c = Peek();
        Advance(); // Consume the character following '\'
        ++width;

        if      (c == '\n') Save(lexical::Tokens::ESCAPE_NEWLINE, width);
        else if (c == '\t') Save(lexical::Tokens::ESCAPE_TAB, width);
        else if (c == '\b') Save(lexical::Tokens::ESCAPE_BACKSPACE, width);
        else if (c == '\r') Save(lexical::Tokens::ESCAPE_RETURN, width);
        else if (c == '\f') Save(lexical::Tokens::ESCAPE_FORM_FEED, width);
        else Save(lexical::Tokens::ESCAPE_LITERAL, width);
      } else {
        return Error("Unexpected end of content in escape sequence");
      }

      return true;
    }

    bool Text(const char end = '\0') {
      uint32_t width = 0;

      while (true) {
        if (Done()) {
          return Error("Unexpected end of content in text sequence");
        }

        auto c = Peek();
        
        if (c == end) {
          Store(lexical::Tokens::TEXT, width); // Save the content between the quotes as a TEXT token
          break;
        } else if (c == '\\') {
          Store(lexical::Tokens::TEXT, width); // Save the content between the quotes as a TEXT token

          if (Escape()) {
            return Text(end); // Match more text after the escape
          } else {
            return Error("Text sequence expected an escape sequence after the backslash '\\' character");
          }
        }

        Advance(); // Consume the character
        ++width;
      }

      return true;
    }

    bool ValueShortcut() {
      switch (Peek()) {
        case '.':
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
        case '-': return ArrowFunction() || Number();
        case '(': return ParameterFunction() || Tuple();
        case '[': return CaptureFunction() || Array();
        case '{': return Object();
        case '<': return Enum();
        default: return false;
      }
    }

    bool Value() {
      return (cursor.IsValueStart() && ValueShortcut()) || Identifier();
    }

    bool InitialValue() {
      if (cursor.IsValueStart() && ValueShortcut()) {
        maybe_declaration = false;
      }

      return Identifier();
    }

    bool Char() {
      return CharOpen()
          && Text('\'')
          && (CharClose() || Error("Unexpected end of input within character literal."))
          && (IdentifiedExpression() || OWS());
    }

    bool String() {
      return StringOpen()
          && Text('"')
          && (StringClose() || Error("Unexpected end of input within string literal."))
          && (IdentifiedExpression() || OWS());
    }

    bool TemplateString() {
      if (!TemplateStringOpen()) return false;

      uint32_t width = 0;

      while (true) {
        if (Done()) return Error("Unexpected end of file in template string literal");

        if (Peek() == '`') {
          if (width > 0) {
            Save(lexical::Tokens::TEXT, width); // Exclude the closing quote from saved range
          }

          return TemplateStringClose() && (IdentifiedExpression() || OWS());
        } else if (Peek() == '\\') {
          Save(lexical::Tokens::TEXT, width);
          
          if (!Escape()) return Error("Template string literal expected an escape sequence after '\\'");

          width = 0; // Update the width for the next TEXT chunk
        } else if (Peek() == '{') {
          if (width > 0) {
            Save(lexical::Tokens::TEXT, width);
          }
          
          if (!TemplateStringExpressionOpen()) {
            return Error("Template string literal didn't find the opening '{'");
          }

          if (!Expression()) return Error("Template string literal expected an expression after '{'");
          if (!TemplateStringExpressionClose()) return Error("Template string literal expected a closing '}' after expression");

          width = 0; // Update the width for the next TEXT chunk
        } else {
          Advance(); // Consume the characters inside the quotes
          ++width;
        }
      }

      throw std::runtime_error("Template string literal didn't hit the end of the file or a closing token");
    }

    bool Hex(bool negative) {
      // Skip the `0x` or `0X`
      Advance(2);
      uint32_t width = 2;
      uint64_t current = 0;

      while (Valid()) {
        if (Peek() == '_') {
          Advance();
          ++width;
          continue;
        } else if (!cursor.IsHex()) {
          break;
        }

        char digit = Peek();
        uint64_t numValue;

        // Convert hex digit to numerical value
        if (digit >= '0' && digit <= '9') {
          numValue = digit - '0';
        } else if (digit >= 'A' && digit <= 'F') {
          numValue = 10 + (digit - 'A');
        } else if (digit >= 'a' && digit <= 'f') {
          numValue = 10 + (digit - 'a');
        } else {
          // Handle invalid hex character
          return Error("Unexpected non-hex character while matching a hexadecimal literal");
        }

        // Shift current value left by 4 bits and add new digit
        current = (current << 4) | numValue;

        Advance(); // Move to the next character
        ++width;
      }

      if (negative) {
        Store(lexical::Tokens::HEX_INTEGER, width);
      } else {
        Store(lexical::Tokens::HEX_NUMBER, width);
      }

      return IdentifiedExpression() || OWS();
    }

    bool Octal(bool negative) {
      // Skip the `0o` or `0O` (assuming this is how octal is denoted)
      Advance(2);
      uint32_t width = 2;
      uint64_t current = 0;

      while (Valid()) {
        if (Peek() == '_') {
          Advance();
          ++width;
          continue;
        } else if (!cursor.IsOctal()) {
          break;
        }

        char digit = Peek();
        uint64_t numValue;

        // Convert octal digit to numerical value
        if (digit >= '0' && digit <= '7') {
          numValue = digit - '0';
        } else {
          // Handle invalid octal character
          return Error("Unexpected non-octal character while matching an octal literal");
        }

        // Shift current value left by 3 bits (for octal) and add new digit
        current = (current << 3) | numValue;

        Advance(); // Move to the next character
        ++width;
      }

      if (negative) {
        Store(lexical::Tokens::OCTAL_INTEGER, width);
      } else {
        Store(lexical::Tokens::OCTAL_NUMBER, width);
      }
      
      return IdentifiedExpression() || OWS();
    }

    bool Binary(bool negative) {
      // Skip the `0b` or `0B`
      Advance(2);
      uint32_t width = 2;
      std::bitset<64> bits;

      uint64_t index = 0;
      while (Valid() && index < 64) {
        if (Peek() == '_') {
          Advance();
          ++width;
          continue;
        } else if (!cursor.IsBinary()) {
          break;
        }

        if (Peek() == '1') {
          bits.set(index);
        }

        Advance();
        ++index;
        ++width;
      }

      if (index > 0) {
        // uint64_t current = bits.to_ullong();

        if (negative) {
          Store(lexical::Tokens::BINARY_INTEGER, width);
        } else {
          Store(lexical::Tokens::BINARY_NUMBER, width);
        }

        return IdentifiedExpression() || OWS();
      } else {
        Retreat(width);
        return false;
      }
    }

    bool NumberHelper() {
      auto start = Start();
      bool negative = false;

      if (Peek() == '-') {
        negative = true;
        Advance();
      } else if (Peek() == '+') {
        // A leading + is allowed for symmetry, but has no meaning as it's implicit
        Advance();
      }

      if (Peek() == '0') {
        // Peek the next character
        switch (Peek(1)) {
          case 'x':
          case 'X': return Hex(negative);
          case 'o':
          case 'O': return Octal(negative);
          case 'b':
          case 'B': return Binary(negative);
        }
      }

      bool decimal_point_encountered = false;
      bool e_encountered = false;
      bool exponent_sign_encountered = false;
      bool digit_encountered = false;

      while (Valid()) {
        char c = Peek();

        if (c == '_' && digit_encountered) {
          Advance();
          continue; // Allow _ as a separator
        } else if (c == '.') {
          if (decimal_point_encountered || e_encountered) break;
          decimal_point_encountered = true;

          // A trailing decimal is not allowed
          // TODO: How does this interact with scientific notation? IDK
          if (!cursor.IsDigit(Peek(1))) {
            break;
          }
        } else if (c == 'E' || c == 'e') {
          if (e_encountered) break;
          e_encountered = true;
          decimal_point_encountered = true; // Prevents further decimals
        } else if ((c == '+' || c == '-') && e_encountered && !exponent_sign_encountered) {
          exponent_sign_encountered = true;
        } else if (cursor.IsDigit(c)) {
          digit_encountered = true;
        } else {
          break;
        }

        Advance();
      }

      uint32_t size = static_cast<uint32_t>(std::distance(start.start, cursor.cbegin()));

      if (size == 0) {
        return false;
      } else if (!digit_encountered) {
        Retreat(size);
        return false;
      }

      // Decide how to store the value based on flags
      if (decimal_point_encountered) {
        Store(lexical::Tokens::DECIMAL, size);
      } else if (negative) {
        Store(lexical::Tokens::INTEGER, size);
      } else {
        Store(lexical::Tokens::NUMBER, size);
      }

      return IdentifiedExpression() || OWS();
    }

    inline bool Number() { return cursor.IsNumberStart() && NumberHelper(); }

    bool Parameters() { return ParameterOpen() && ParameterDeclarationList() && ParameterClose(); }
    bool Captures() { return CaptureOpen() && CaptureDeclarationList() && CaptureClose(); }
    bool FunctionBody() { return ScopeOpen() && FunctionStatementList() && ScopeClose(); }
    bool InlineFunctionBody() { return Expression(); }
    bool Array() { return ArrayOpen() && ExpressionList() && ArrayClose(); }
    bool Tuple() { return TupleOpen() && ExpressionList() && TupleClose(); }
    bool Object() { return ObjectOpen() && MemberDeclarationList() && ObjectClose(); }
    bool Enum() { return EnumOpen() && EnumExpressionList() && EnumClose(); }
    bool ArrowFunction() { return Arrow() && InlineFunctionBody(); }
    bool CaptureFunction() { return Try([&]{ return Captures() && (Parameters() || true) && FunctionBody() || ArrowFunction(); }); }
    bool ParameterFunction() { return Try([&]{ return Parameters() && (Captures() || true) && FunctionBody() || ArrowFunction(); }); }

    bool Function() {
      return Try([&]{
        if (Parameters()) return (Captures() || true) && FunctionBody() || ArrowFunction();
        else return Captures() && (Parameters() || true) && FunctionBody() || ArrowFunction();
      });
    }

    bool DestructuredArray() { return DestructuredArrayOpen() && DeclarationList() && DestructuredArrayClose(); }
    bool DestructuredTuple() { return DestructuredTupleOpen() && DeclarationList() && DestructuredTupleClose(); }
    bool DestructuredObject() { return DestructuredObjectOpen() && DeclarationList() && DestructuredObjectClose(); }
    bool DestructuredEnum() { return DestructuredEnumOpen() && DeclarationList() && DestructuredEnumClose(); }

    bool Destructured() {
      switch (Peek()) {
        case '(': return DestructuredTuple();
        case '{': return DestructuredObject();
        case '<': return DestructuredEnum();
        case '[': return DestructuredArray();
        default: return false;
      }
    }

    bool IdentifierList() { return ZeroOrMore([&]{ return Identifier(); }, [&]{ return Comma(); }); }
    bool CaptureDeclarationList() { return ZeroOrMore([&]{ return CaptureDeclaration(); }, [&]{ return Comma(); }); }
    bool DeclarationList() { return ZeroOrMore([&]{ return Declaration(); }, [&]{ return Comma(); }); }
    bool MemberDeclarationList() { return ZeroOrMore([&]{ return MemberDeclaration(); }, [&]{ return Comma(); }); }
    bool ExpressionList() { return ZeroOrMore([&]{ return Expression(); }, [&]{ return Comma(); }); }
    bool EnumExpressionList() { return ZeroOrMore([&]{ return EnumExpression(); }, [&]{ return Comma(); }); }
    bool ParameterDeclarationList() { return ZeroOrMore([&]{ return ParameterDeclaration(); }, [&]{ return Comma(); }); }

    bool CaptureDeclaration() {
      switch (Peek()) {
        case '=': {
          auto c = Peek(1);
          if (c == ',' || c == ']' || cursor.IsSpace(c)) {
            return Assign();
          }
          break;
        }
        case '@': {
          auto c = Peek(1);
          if (c == ',' || c == ']' || cursor.IsSpace(c)) {
            return Copy();
          }
          break;
        }
        case '&': {
          auto c = Peek(1);
          if (c == ',' || c == ']' || cursor.IsSpace(c)) {
            return Reference();
          }
          break;
        }
      }

      return Declaration();
    }

    bool IdentifiedExpression() {
      return cursor.IsIdent() && Identifier() && ContinueExpression();
    }

    bool DeclarationExpression() {
      // Handle unary prefix operators at the beginning of an expression
      while (UnaryPrefixOperator()) {}

      auto start = cbegin();
      while (Value() && ContinueExpression()) {}

      return cbegin() > start;
    }

    bool Expression() {
      // maybe_declaration = true;

      // Handle unary prefix operators at the beginning of an expression
      while (UnaryPrefixOperator()) {
        // maybe_declaration = false;
      }

      // The next element should be a value
      if (!Value()) {
        return false;
      }

      return ContinueExpression();
    }

    bool EnumExpression() {
      // Handle unary prefix operators at the beginning of an expression
      while (UnaryPrefixOperator()) {
      }

      // The next element should be a value
      if (!Value()) {
        return false;
      }

      return ContinueEnumExpression();
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
      while (Valid()) {
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
      while (Valid()) {
        if (BinaryOperator(true)) {
          while (UnaryPrefixOperator()) {}

          if (!Value()) return false;
        } else if (!CallablePostfixLiteral()) {
          break; // No more operators or values
        }
      }

      return true;
    }

    bool CallablePrefixLiteralHelper() {
      switch (Peek()) {
        case '\'': return Char();
        case '"': return String();
        case '`': return TemplateString();
        default: return Number();
      }
    }

    bool CallablePostfixLiteralHelper() {
      switch (Peek()) {
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

    bool ModifierHelper() {
      switch (Peek()) {
        case 'a': return Async();
        case 'c': return Const() || Comtime();
        case 'e': return Export();
        case 'i': return Immutable(); // Remove? I think 'let' takes its place
        case 'm': return Mutable();
        case 'p': return Private() || Public() || Protected();
        case 's': return Static();
        case 'l': return Let();
        case 'r': return Runtime();
        default: return false;
      }
    }

    inline bool Modifier() { return cursor.IsModifierStart() && ModifierHelper(); }
    inline bool CallablePrefixLiteral() { return cursor.IsCallablePrefixStart() && CallablePrefixLiteralHelper(); }
    inline bool CallablePostfixLiteral() { return cursor.IsCallablePostfixStart() && CallablePostfixLiteralHelper(); }

    bool DeclarationValue() {
      // Any number of postfix unary operators are legal
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
      // Consume any number of modifier keywords
      int modifiers = Modifiers();

      if (UnaryStartedExpression()) return true;

      if (Identifier()) {
        if (Peek() == ',') return true;
      }
      else {
        if (modifiers > 0) {
          return Error("Declaration expected an identifier after the modifiers");
        }

        return false;
      }

      return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
    }

    bool ParameterDeclaration() {
      int modifiers = Modifiers();

      if (Identifier()) {
        if (Peek() == ',') return true;
      }
      else {
        if (modifiers > 0) {
          return Error("Declaration expected an identifier after the modifiers");
        }

        return false;
      }

      return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
    }

    bool MemberDeclaration() {
      // utility::Debug("Parsing a member declaration list");

      // Consume any number of modifier keywords
      int modifiers = Modifiers();

      // Capture unary expressions like `@a`
      if (UnaryStartedExpression()) return true;

      if (ComputedProperty()) {
        Alias(); // Optional
      }
      else if (Identifier()) {
        if (Peek() == ',') return true;
      }
      else {
        if (modifiers > 0) {
          return Error("Declaration expected an identifier after the modifiers");
        }

        return false;
      }

      return DeclarationValue() || true; // The value is optional, unless it's a declaration statement
    }

    bool Condition() {
      if (!ConditionOpen()) return false;
      if (!Expression()) return Error("Condition expected an expression after opening '('");
      
      return ConditionClose() || Error("Condition expected a closing ')'");
    }

    // bool Condition() {
    //   auto start = Start();
    //   auto last = start;
    //   auto last_successful = last; // This will store the size before the last attempt

    //   while (UnaryPrefixOperator()) {}

    //   if (Value()) {
    //     while (Valid()) {
    //       last = Start();

    //       if (BinaryOperator()) {
    //         while (UnaryPrefixOperator()) {}

    //         if (!Value()) return false;
    //       } else if (!CallablePostfixLiteral()) {
    //         break; // No more operators or values
    //       }

    //       last_successful = last; // Since we successfully passed the conditions, update last_successful
    //     }
    //   }
    //   else {
    //     Rollback(start);
    //     return false;
    //   }

    //   if (Done() || (Peek() != ':' && Peek() != '{')) {
    //     lexical::Tokens type = Last();

    //     // If the last thing was a `}` then it was actually suppose to be the scope block
    //     if (type == lexical::Tokens::SCOPE_CLOSE) {
    //       // Debug([]{ Print("Rolling back function literal match"); });

    //       Rollback(last_successful);

    //       // If specifically a SCOPE_CLOSE, then it was a function, so check for a tuple (parameters) and optionally an array (captures)
    //       return Tuple() && (Array() || true);
    //     }
    //     else if (type == lexical::Tokens::OBJECT_CLOSE) {
    //       // Debug([]{ Print("Rolling back object literal match"); });

    //       Rollback(last_successful);

    //       // If it was an OBJECT_CLOSE, then the scope block was wrongly parsed as an object literal, so just undo it and return true, allowing it to be parsed correctly
    //       return true;
    //     }
    //   }

    //   return true;
    // }

    bool IdentifierHelper() {
      // return false;

      if (Done()) return false;

      auto start = cbegin();
      uint32_t offset = 0;

      // Check if the first character is a valid starting character for an identifier
      if (cursor.IsIdentStart()) {
        if (Peek() == '\\') {
          throw std::runtime_error("Haven't supported escape sequences in identifiers yet");
        } else {
          Advance(); // Consume the starting character
        }
      } else if (static_cast<uint8_t>(Peek()) > 127) {
        auto code_point = cursor.CodePoint();

        if (Unicode::IsIdentifierStart(code_point)) {
          offset += static_cast<uint32_t>(std::distance(start, cbegin())) - 1;
        } else {
          Error("Found illegal unicode character " + std::to_string(code_point));
          return false; // Not a valid starting character
        }
      } else {
        return false;
      }

      // Consume remaining characters if they are alphanumeric or underscores
      while (Valid()) {
        if (cursor.IsIdent()) {
          Advance();
        } else if (static_cast<uint8_t>(Peek()) > 127) {
          auto start = cbegin();
          auto code_point = cursor.CodePoint();

          if (Unicode::IsIdentifierContinue(code_point)) {
            offset += static_cast<uint32_t>(std::distance(start, cbegin())) - 1;
          } else {
            Error("Found illegal unicode character " + std::to_string(code_point));
            break;
          }
        } else {
          break;
        }
      }

      std::string_view text{start, cbegin()};

      auto type = lexical::cursor::Token::FindKeyword(text);
      if (!lexical::cursor::Token::IsIdentifiable(type)) {
        Retreat(start); // Reset
        return false;
      }

      Store(type, static_cast<uint32_t>(text.size()), offset);
      
      if (WS() && Peek() == 'a') {
        return (As() && (Identifier() || Error("Expected an identifier after the `as` keyword"))) || true; // Still matched an identifier even if it isn't an alias next
      } else {
        return true;
      }
    }

    bool Identifier() { return cursor.IsPossibleIdentifierStart() && IdentifierHelper(); }

    bool Alias() { return As() && Identifier(); }

    bool IdentifierExpression() {
      if (Identifier()) {
        if (As()) {
          return Identifier();
        }

        return true;
      }

      return false;
    }

    bool InlineScopeStatement() {
      return Statement()
          && Last() == lexical::Tokens::SEMICOLON; // Required
    }

    bool ScopeStatement() {
      return ScopeOpen()
          && StatementList()
          && (ScopeClose() || Error("Block statement expected a closing `}` brace"));
    }

    bool FlexibleScopeStatement() {
      return ScopeStatement() || InlineScopeStatement();
    }

    bool ForStatement() {
      if (!For()) return false;
      if (!ConditionOpen()) return Error("For statement expected an opening '(' after keyword 'for'");

      int modifiers = Modifiers();
      
      if (Destructured()) {

      }
      else if (Identifier()) {
        int operators = UnaryPostfixOperators();

        if (TypeStart()) {
          if (operators > 0) {
            return Error("For statement declaration cannot specify a type if it is using unary postfix operators");
          }

          if (!Expression()) {
            return Error("For statement declaration expected a type expression after ':'");
          }
        }
      }
      else {
        return Error("For statement declaration expected an identifier or destructuring syntax");
      }

      if (!In()) return Error("For statement expected the `in` keyword after the declaration");
      if (!Expression()) return Error("For statement expected an expression after the `in` keyword");
      if (!ConditionClose()) return Error("For statement expected a closing ')' after the condition");
      if (!FlexibleScopeStatement()) return Error("For statement expected a scope block or inline statement after the closing ')'");

      return OptionalSemicolon();
    }

    bool ElseStatement() {
      return Else() && (IfStatement() || FlexibleScopeStatement());
    }

    bool IfStatement() {
      return If()
          && Condition()
          && FlexibleScopeStatement()
          && (ElseStatement() || true)
          && OptionalSemicolon();
    }

    bool WhileStatement() {
      return While()
          && Condition()
          && FlexibleScopeStatement()
          && OptionalSemicolon();
    }

    bool DoStatement() {
      if (!Do()) return false;

      if (!ScopeStatement()) return Error("Do statement expected a scope after the 'do' keyword");

      if (While()) {
        if (Condition()) {
          return Semicolon() || Error("The semicolon is mandatory after a condition in a 'do' loop");
        }
        else {
          return OptionalSemicolon();
        }
      }
      else {
        return true;
      }
    }

    bool ImportStatementIdentifierList() {
      if (!ImportsOpen()) return false;

      if (!ZeroOrMore([this]{ return Declaration(); }, [this]{ return Comma(); }) || Done()) {
        return Error("Unexpected end of file while parsing an import statement identifier list");
      }

      return ImportsClose();
    }

    bool ImportWildcard() {
      // TODO: This will probably be deprecated
      if (!Wildcard()) return false;

      if (!As()) {
        return Error("Import statement expected an 'as' keyword after the wildcard '*'");
      }

      if (!Identifier()) {
        return Error("Import statement expected an identifier after the 'as' keyword in wildcard '*' import");
      }

      return true;
    }

    bool StringSpecifier() {
      if (!String()) return false;

      if (In()) {
        return String() || Error("String specifier expected a module name string literal after the 'in' keyword");
      }

      return true;
    }

    bool WildcardSpecifier() {
      if (!Wildcard()) return false;

      if (In()) {
        return String() || Error("Wildcard specifier expected a module name string literal after the 'in' keyword");
      }

      return true;
    }

    bool Specifier() { return WildcardSpecifier() || StringSpecifier(); }

    bool ImportStatement() {
      return Import()
          && (ImportWildcard() || ImportStatementIdentifierList() || Identifier())
          && (From() || Error("Import statement expected the from keyword"))
          && (StringSpecifier() || Error("Import statement expected a string literal as the specifier"))
          // && Run([this]{ mod.Import(std::move(Read(lexical::Tokens::TEXT))); return true; })
          && (Semicolon() || Error("Import statement expected an ending semicolon"));
    }

    bool RegisterStatement() {
      if (!Register()) return false;

      // There is no need to attempt to register the package early as the standard library will provide plenty to keep the workers busy.
      return (String() || Error("Register statement expected a string literal as the path"))
          && (As() || Error("Register statement expected the `as` keyword after the path"))
          && (String() || Error("Register statement expected a string literal as the specifier"))
          && (With() || Error("Register statement expected the `with` keyword after the specifier"))
          && Run([&]{
            return (ArrayOpen() || Error("Register statement expected an array literal of specifiers"))
                && Run([&]{
                  while (Peek() != ']') {
                    if (!Specifier()) break;
                    if (!Comma()) break;
                  }

                  return true;
                })
                && (ArrayClose() || Error("Register statement expected to find a closing `]` symbol"));
          })
          && (Semicolon() || Error("Register statement expected an ending semicolon"));
    }

    bool ExportStatement() {
      return Export()
          && Run([&]{
            if (Wildcard()) {
              return (From() || Error("Aggregate export statement expected the from keyword"))
                  && (String() || Error("Aggregate export expected a string literal as the specifier"))
                  ;
            } else {
              return Expression();
            }
          })
          && (Semicolon() || Error("Export statement expected a semicolon"));
    }

    bool _PropertyDeclaration() {
      while (Modifier()) {}

      if (!Destructured() && !Identifier()) {
        return Error("Property declaration statement expected destructuring enum syntax or an identifier after the modifiers");
      }

      Alias(); // Optional

      if (CallablePostfixLiteral()) {
        return Semicolon() || Error("Property declaration statement expected a semicolon");
      }

      if (TypeStart()) {
        if (!Expression()) {
          return Error("Property declaration statement expected a type expression after ':'");
        }
      }
      
      if (Peek() == '=') {
        if (Assign()) {
          if (Expression()) {
            return Semicolon() || Error("Property declaration statement expected a semicolon");
          }
          else {
            return Error("Property declaration statement expected an expression after initialization operator");
          }
        }
        else {
          return Error("Property declaration statement expected an initialization operator");
        }
      }
      else {
        return Error("Property declaration statement expected an initialization operator");
      }
    }

    bool ComputedProperty() {
      if (!ComputedPropertyOpen()) return false;

      int count = 0;
      while (Valid()) {
        if (!EnumExpression()) break;

        count += 1;
        if (!Comma()) break;
      }

      if (count == 0) {
        return Error("Declaration expected at least one expression after the opening '<' in the computed property name");
      }

      if (!ComputedPropertyClose()) {
        return Error("Declaration expected a closing '>' after the expression in the computed property name");
      }

      return true;
    }

    int Modifiers() {
      int count = 0;
      while (Modifier()) {
        count += 1;
      }

      return count;
    }

    int UnaryPostfixOperators() {
      int count = 0;
      while (UnaryPostfixOperator()) {
        count += 1;
      }

      return count;
    }

    int UnaryPrefixOperators() {
      int count = 0;
      while (UnaryPrefixOperator()) {
        count += 1;
      }

      return count;
    }

    bool DeclarationStatement() {
      if (Modifiers() == 0) return false;

      if (Destructured()) {
        if (Assign()) {
          if (Expression()) {
            return Semicolon() || Error("Declaration statement expected a semicolon");
          }
          else {
            return Error("Declaration statement expected an expression after assignment '=' operator");
          }
        }
        else {
          return Error("Declaration statement expected a right-hand side initialization value after the destructuring syntax");
        }
      }
      else if (Identifier()) {
        // Any number of postfix unary operators are legal
        int operators = UnaryPostfixOperators();

        if (TypeStart()) {
          if (operators > 0) {
            return Error("Declaration statement cannot specify a type if it is using unary postfix operators");
          }

          if (!Expression()) {
            return Error("Declaration statement expected a type expression after ':'");
          }

          if (Assign()) {
            if (Expression()) {
              return Semicolon() || Error("Declaration statement expected a semicolon");
            }
            else {
              return Error("Declaration statement expected an expression after assignment '=' operator");
            }
          }
          else {
            return Error("Declaration statement expected a right-hand side initialization value after the type");
          }
        }
        else if (CallablePostfixLiteral()) {
          if (operators > 0) {
            return Error("Declaration statement cannot skip the '=' operator if it is using unary postfix operators");
          }

          return Semicolon() || Error("Declaration statement expected a semicolon");
        }
        else if (Assign()) {
          if (Expression()) {
            return Semicolon() || Error("Declaration statement expected a semicolon");
          }
          else {
            return Error("Declaration statement expected an expression after assignment '=' operator");
          }
        }
        else {
          return Error("Declaration statement expected a right-hand side initialization value");
        }
      }
      else {
        return Error("Declaration statement expected either destructuring syntax or an identifier after the modifiers");
      }
    }

    bool ExpressionStatement() {
      return Expression()
          && (Semicolon() || Error("Expression statement expected a semicolon"));
    }

    bool ContinueStatement() {
      return OneOrMore([this]{ return Continue(); })
          && (Semicolon() || Error("Continue statement expected a semicolon"));
    }

    bool BreakStatement() { return OneOrMore([this]{ return Break(); }) && (Semicolon() || Error("Break statement expected a semicolon")); }
    // bool YieldStatement() { return Yield() && (Expression() || true) && (Semicolon() || Error("Yield statement expected a semicolon")); }

    bool YieldStatement() {
      if (!Yield()) return false;
      if (Semicolon()) return true; // Valid to just end right away

      return (Expression() || Error("Yield statement expected an expression or a semicolon"))
          && (Semicolon() || Error("Yield statement expected a semicolon"));
    }

    bool ReturnStatement() {
      if (!Return()) return false;
      if (Semicolon()) return true; // Valid to just end right away

      return (Expression() || Error("Return statement expected an expression or a semicolon"))
          && (Semicolon() || Error("Return statement expected a semicolon"));
    }

    bool CaseStatement() {
      if (!Case()) return false;
      if (Semicolon()) return true; // Valid to just end right away

      return (Expression() || Error("Case statement expected an expression or a semicolon"))
          && (Semicolon() || Error("Case statement expected a semicolon"));
    }

    bool MatchStatement() {
      return Match()
          && (Condition() || Error("Match statement expected a condition"))
          && (ScopeStatement() || Error("Match statement expected a scope block"))
          && OptionalSemicolon();
    }

    bool CoverStatement() {
      return Cover()
          && (Condition() || Error("Cover statement expected a condition"))
          && (ScopeStatement() || Error("Cover statement expected a scope block"))
          && OptionalSemicolon();
    }

    bool IsStatement() {
      return Is()
          && (Condition() || Error("Is statement expected a condition"))
          && (IsStatement() || (FlexibleScopeStatement() && OptionalSemicolon()));
    }

    bool DefaultStatement() {
      return Default()
          && (FlexibleScopeStatement() || Error("Default statement expected a block or inline statement"));
          // && (Semicolon() || Error("Default statement expected a semicolon"));
    }

    bool StatementShortcut() {
      switch (Peek()) {
        case 'b': return BreakStatement();
        case 'c': return ContinueStatement() || CaseStatement() || CoverStatement();
        case 'd': return DoStatement() || DefaultStatement();
        case 'e': return ExportStatement();
        case 'f': return ForStatement();
        case 'i': return IfStatement() || ImportStatement() || IsStatement();
        case 'm': return MatchStatement();
        case 'r': return ReturnStatement() || RegisterStatement();
        case 'w': return WhileStatement();
        case 'y': return YieldStatement();
        default: return false;
      }
    }

    bool Statement() { return (cursor.IsStatementStart() && StatementShortcut()) || DeclarationStatement() || ExpressionStatement(); }
    
    bool StatementList() {
      // std::cout << "Starting StatementList" << std::endl;

      while (Valid()) {
        // // Check for either of these
        // if (!Compiletime()) {
        //   Runtime();
        // }

        if (!Statement()) {
          // if (recovery && !Recover()) {
          //   break;
          // }

          break;
        }
      }

      return true;
    }

    bool FunctionStatementList() { return StatementList(); }
  };
};