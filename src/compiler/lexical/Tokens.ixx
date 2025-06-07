export module lexical.tokens;

import <cstdint>;

namespace lexical {
  export enum class Tokens : uint8_t {
    NONE,
    SPACE1,
    SPACE2,
    SPACE3,
    SPACE4,
    SPACE5,
    SPACE6,
    SPACE7,
    SPACE8,
    SPACE16,
    TAB1,
    TAB2,
    TAB3,
    TAB4,
    TAB5,
    TAB6,
    TAB7,
    TAB8,
    TAB16,
    IDENTIFIER1,                  
    IDENTIFIER3,
    IDENTIFIER5,
    IDENTIFIER7,
    IDENTIFIER8,
    IDENTIFIER16,
    TEXT1,
    TEXT3,
    TEXT5,
    TEXT7,
    TEXT8,
    TEXT16,
    DIGIT1,
    DIGIT3,
    DIGIT5,
    DIGIT7,
    DIGIT8,
    DIGIT16,
    NEWLINE,
    UNDERSCORE,
    ZERO_WIDTH_SPACE,
    COMMENT_CLOSE,
    COMMENT_OPEN,
    MULTI_LINE_COMMENT_CLOSE,
    MULTI_LINE_COMMENT_OPEN,
    UNDEFINED,
    _NULL,
    TRUE,
    FALSE,
    HEX, // 0x
    OCTAL, // 0o
    BINARY, // 0b
    ESCAPE_NEWLINE, // (\n) - A newline character.
    ESCAPE_TAB, // (\t) - A tab character.
    ESCAPE_BACKSPACE, // (\b) - A backspace.
    ESCAPE_RETURN, // (\r) - A carriage return.
    ESCAPE_FORM_FEED, // (\f) - A form feed.
    ESCAPE_LITERAL, // (\X) - A literal character
    ESCAPE_UNICODE_CODEPOINT_START, // (\u{XXXXX...}) - A variable length Unicode code point
    ESCAPE_UNICODE_CODEPOINT_END, // }
    ESCAPE_UNICODE_SHORT, // (\uXXXX) - A Unicode character using exactly four hexadecimal digits.
    ESCAPE_HEX_CODE, // (\xXX) - A character specified by a hexadecimal value of exactly two digits.
    THIS,
    WITH,
    USE,
    ADD, // +
    AND, // &&
    OR, // ||
    WRAP, // !!
    UNWRAP, // ??
    ASSIGN, // =
    CONSTRUCTOR, // =>
    DIVIDE, // /
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
    YIELD, // yield
    EXPECTED, // expected
    UNEXPECTED, // unexpected
    AWAIT, // await
    COPY, // @
    DECREMENT, // --
    INCREMENT, // ++
    BORROW, // #
    NEGATIVE, // -
    NOT, // !
    OPTIONAL, // ?
    POSITIVE, // +
    REFERENCE, // &
    MUTABLE_REFERENCE, // *
    OPTIONAL_REFERENCE, // ^
    SPREAD, // ...
    SYMBOLOF, // $
    RETURNOF, // #
    MOVE, // =
    BITWISE_NOT, // ~
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
    WHILE,
    CASE,
    LET,
    COMPILER,
    AUTO, // auto
    VOID, // void
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
    CHAR_OPEN, // '
    CHAR_CLOSE, // '
    COMMA, // ,
    CONDITION_OPEN, // (
    CONDITION_CLOSE, // )
    IMPORTS_OPEN, // {
    IMPORTS_CLOSE, // }
    ARROW, // ->
    CAPTURE_OPEN, // [
    CAPTURE_CLOSE, // ]
    PARAMETER_OPEN, // (
    PARAMETER_CLOSE, // )
    TEMPLATE_OPEN, // <
    TEMPLATE_CLOSE, // >
    SCOPE_OPEN, // {
    SCOPE_CLOSE, // }
    SEMICOLON, // ;
    TEMPLATE_STRING_OPEN, // `
    TEMPLATE_STRING_CLOSE, // `
    TEMPLATE_STRING_EXPRESSION_OPEN, // {
    TEMPLATE_STRING_EXPRESSION_CLOSE, // }
    STRING_OPEN, // "
    STRING_CLOSE, // "
    TYPE_START, // :
    WILDCARD, // *
    ERROR,
  };
};