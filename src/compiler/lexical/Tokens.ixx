export module lexical.tokens;

import <cstdint>;

namespace lexical {
  export enum class Tokens : uint8_t {
    // Trivial
    NONE,
    COMMENT,
    WHITESPACE,
    NEWLINE,
    TAB,
    ZERO_WIDTH_SPACE,
    COMMENT_CLOSE,
    COMMENT_OPEN,
    MULTI_LINE_COMMENT_CLOSE,
    MULTI_LINE_COMMENT_OPEN,

    // Literals
    UNDEFINED,
    _NULL,
    FALSE,
    TRUE,
    NUMBER,
    INTEGER,
    DECIMAL,
    HEX_INTEGER,
    HEX_NUMBER,
    OCTAL_INTEGER,
    OCTAL_NUMBER,
    BINARY_INTEGER,
    BINARY_NUMBER,

    // Escapes
    ESCAPE_NEWLINE, // (\n) - A newline character.
    ESCAPE_TAB, // (\t) - A tab character.
    ESCAPE_BACKSPACE, // (\b) - A backspace.
    ESCAPE_RETURN, // (\r) - A carriage return.
    ESCAPE_FORM_FEED, // (\f) - A form feed.
    ESCAPE_LITERAL, // (\X) - A literal character
    ESCAPE_UNICODE_CODEPOINT, // (\u{XXXXX...}) - A variable length Unicode code point
    ESCAPE_UNICODE_SHORT, // (\uXXXX) - A Unicode character using exactly four hexadecimal digits.
    ESCAPE_HEX_CODE, // (\xXX) - A character specified by a hexadecimal value of exactly two digits.

    // Contextual keywords
    SUPER,
    THIS,
    SELF,

    // Binary operators
    WITH,
    USE,
    ADD, // +
    AND, // &&
    OR, // ||
    UNWRAP, // ??
    TRUTHY_AND, // !!
    TRUTHY_OR, // ??
    ASSIGN, // =
    CONSTRUCTOR, // =>
    DIVIDE, // /
    EMPLACE, // =?
    EQUAL, // ==
    EXPONENT, // **
    GREATER_OR_EQUAL, // >=
    GREATER, // >
    EXCLUSIVE_RANGE, // ..
    INCLUSIVE_RANGE, // ...
    LESSER_OR_EQUAL, // <=
    LESSER, // <
    MEMBER_ACCESS, // .
    MODULO, // %
    MULTIPLY, // *
    NOT_EQUAL, // !=
    ASSIGN_OPTIONAL, // ?=
    OPTIONAL_MEMBER_ACCESS, // ?.
    CONDITIONAL_MEMBER_ACCESS, // ?.
    SYMBOL_MEMBER_ACCESS, // $.
    SUBTRACT, // -
    STATIC_MEMBER_ACCESS, // ::
    TRIPLE_LEFT_SHIFT, // <<<
    TRIPLE_RIGHT_SHIFT, // >>>
    THREE_WAY_COMPARISON, // <=>
    TERNARY_CONDITION, // ?
    TERNARY_ALTERNATE, // :
    ASSERT_EQUAL, // ===
    ASSERT_GREATER_OR_EQUAL, // >==
    ASSERT_LESSER_OR_EQUAL, // <==
    ASSERT_NOT_EQUAL, // !==
    ASSIGN_ADD, // +=
    ASSIGN_DIVIDE, // /=
    ASSIGN_EXPONENT, // **=
    ASSIGN_MODULO, // %=
    ASSIGN_MULTIPLY, // *=
    ASSIGN_SUBTRACT, // -=
    ASSIGN_AND, // &&=
    ASSIGN_OR, // ||=
    ASSIGN_TRUTHY_AND, // !!=
    ASSIGN_TRUTHY_OR, // ??=
    BITWISE_LEFT_SHIFT, // <<
    BITWISE_RIGHT_SHIFT, // >>
    BITWISE_TRIPLE_LEFT_SHIFT, // <<<
    BITWISE_TRIPLE_RIGHT_SHIFT, // >>>
    BITWISE_ASSIGN_TRIPLE_LEFT_SHIFT, // <<<=
    BITWISE_ASSIGN_TRIPLE_RIGHT_SHIFT, // >>>=
    BITWISE_ASSIGN_LEFT_SHIFT, // <<=
    BITWISE_ASSIGN_RIGHT_SHIFT, // >>=
    BITWISE_AND, // &
    BITWISE_XOR, // ^
    BITWISE_OR, // |
    BITWISE_ASSIGN_AND, // &=
    BITWISE_ASSIGN_XOR, // ^=
    BITWISE_ASSIGN_OR, // |=

    // Unary operators
    YIELD, // yield
    EXPECTED, // expected
    UNEXPECTED, // unexpected
    AWAIT, // await
    CONTRACT, // !
    COPY, // @
    DECREMENT, // --
    INCREMENT, // ++
    BORROW, // #
    NEGATIVE, // -
    NOT, // !
    OPTIONAL, // ?
    POSITIVE, // +
    REFERENCE, // &
    OPTIONAL_REFERENCE, // ^
    SPREAD, // ...
    SYMBOL, // $
    VIRTUAL, // *
    MOVE, // =
    BITWISE_NOT, // ~
    POSTFIX_CONTRACT,
    POSTFIX_COPY,
    POSTFIX_DECREMENT,
    POSTFIX_INCREMENT,
    POSTFIX_BORROW,
    POSTFIX_OPTIONAL,
    POSTFIX_REFERENCE,
    POSTFIX_SPREAD,
    POSTFIX_SYMBOL,
    POSTFIX_VIRTUAL,

    // Statements
    AS,
    HAS,
    ASYNC,
    BREAK,
    CONTINUE,
    COVER,
    DEFAULT,
    DO,
    ELSE,
    FOR,
    REGISTER,
    FROM,
    IF,
    RETURN,
    MATCH,
    IS,
    IN,
    IMPORT,
    TRANSFORM,
    WHILE,
    CASE,

    // Modifiers
    LET,
    CONST,
    COMTIME,
    COMPILER,
    EXPORT,
    RUNTIME,
    IMMUTABLE,
    MUTABLE,
    PRIVATE,
    PROTECTED,
    PUBLIC,
    STATIC,

    // Types
    AUTO,
    ARRAY,
    BOOL,
    CLASS,
    BYTE,
    CHAR,
    CHAR8,
    CHAR16,
    CHAR32,
    ENUM,
    FLOAT,
    FLOAT16,
    FLOAT32,
    FLOAT64,
    FUNCTION,
    INT,
    INT8,
    INT16,
    INT32,
    INT64,
    OBJECT,
    STRING,
    TUPLE,
    UINT,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    VARIANT,
    VOID,
    
    // Symbols
    ARRAY_OPEN, // [
    ARRAY_CLOSE, // ]
    ENUM_OPEN, // <
    ENUM_CLOSE, // >
    OBJECT_OPEN, // {
    OBJECT_CLOSE, // }
    TUPLE_OPEN, // (
    TUPLE_CLOSE, // )
    DESTRUCTURED_ARRAY_OPEN, // [
    DESTRUCTURED_ARRAY_CLOSE, // ]
    DESTRUCTURED_ENUM_OPEN, // <
    DESTRUCTURED_ENUM_CLOSE, // >
    DESTRUCTURED_OBJECT_OPEN, // {
    DESTRUCTURED_OBJECT_CLOSE, // }
    DESTRUCTURED_TUPLE_OPEN, // (
    DESTRUCTURED_TUPLE_CLOSE, // )
    CHAR_CLOSE, // '
    CHAR_OPEN, // '
    COMMA, // ,
    COMPUTED_PROPERTY_OPEN, // [
    COMPUTED_PROPERTY_CLOSE, // ]
    ENUM_PROPERTY_OPEN, // <
    ENUM_PROPERTY_CLOSE, // >
    CONDITION_OPEN, // (
    CONDITION_CLOSE, // )
    IMPORTS_OPEN, // {
    IMPORTS_CLOSE, // }
    ARROW, // ->
    CAPTURE_OPEN, // [
    CAPTURE_CLOSE, // ]
    PARAMETER_OPEN, // (
    PARAMETER_CLOSE, // )
    RETURN_TYPE_OPEN, // <
    RETURN_TYPE_CLOSE, // >
    INLINE_SCOPE_START, // :
    SCOPE_OPEN, // {
    SCOPE_CLOSE, // }
    SEMICOLON, // ;
    TEMPLATE_STRING_OPEN, // `
    TEMPLATE_STRING_CLOSE, // `
    TEMPLATE_STRING_EXPRESSION_OPEN, // {
    TEMPLATE_STRING_EXPRESSION_CLOSE, // }
    STRING_CLOSE, // "
    STRING_OPEN, // "
    TYPE_START, // :
    WILDCARD, // *

    // Misc
    IDENTIFIER,
    TEXT,
    ERROR,
  };
};