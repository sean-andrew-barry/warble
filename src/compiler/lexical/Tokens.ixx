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
    CHARACTER1,
    CHARACTER3,
    CHARACTER5,
    CHARACTER7,
    CHARACTER8,
    CHARACTER16,
    DIGIT1,
    DIGIT3,
    DIGIT5,
    DIGIT7,
    DIGIT8,
    DIGIT16,
    NEWLINE_LF1, // `\n` - Line feed character
    NEWLINE_LF2,
    NEWLINE_LF3,
    NEWLINE_LF4,
    NEWLINE_LF5,
    NEWLINE_LF6,
    NEWLINE_LF7,
    NEWLINE_LF8,
    NEWLINE_LF16,
    NEWLINE_CRLF1, // `\r\n` - Combination carriage return and line feed
    NEWLINE_CRLF2,
    NEWLINE_CRLF3,
    NEWLINE_CRLF4,
    NEWLINE_CRLF5,
    NEWLINE_CRLF6,
    NEWLINE_CRLF7,
    NEWLINE_CRLF8,
    NEWLINE_CRLF16,
    NEWLINE_CR, // `\r` - Carriage return character
    NEWLINE_LS, // `'\u2028'` - Unicode line separator
    NEWLINE_PS, // `\u2029` - Unicode paragraph separator
    VERTICAL_TAB, // `\v`
    FORM_FEED, // `\f`
    UNDERSCORE, // `_` - Used as a spacer in numeric literals
    COMMENT_OPEN, // `//` - Starts a single line comment
    COMMENT_CLOSE, // Simple marker, doesn't represent a character
    MULTI_LINE_COMMENT_CLOSE,
    MULTI_LINE_COMMENT_OPEN,
    ERROR, // Special error marker indicating an error symbol was created
    UNDEFINED,
    _NULL,
    TRUE,
    FALSE,
    THIS, // `this` - Used in object literals and functions
    HEX, // `0x` or `0X`
    OCTAL, // `0o` or `0O`
    BINARY, // `0b` or `0B`
    ESCAPE_NEWLINE, // (\n) - A newline character
    ESCAPE_TAB, // (\t) - A tab character
    ESCAPE_BACKSPACE, // (\b) - A backspace
    ESCAPE_RETURN, // (\r) - A carriage return
    ESCAPE_FORM_FEED, // (\f) - A form feed
    ESCAPE_LITERAL, // (\X) - A literal character
    ESCAPE_UNICODE_CODEPOINT_START, // `\u{` - A variable length Unicode code point like `\u{XXXXX...}`
    ESCAPE_UNICODE_CODEPOINT_END, // `}` - The end marker
    ESCAPE_UNICODE_SHORT, // (\uXXXX) - A Unicode character using exactly four hexadecimal digits
    ESCAPE_HEX_CODE, // (\xXX) - A character specified by a hexadecimal value of exactly two digits
    ADD, // +
    AND, // &&
    OR, // ||
    WRAP, // `!!` - Builds a variant from values
    UNWRAP, // `??` - Conditionally unwraps a variant
    INLINE_FUNCTION_ARROW, // `=>` - Used to mark the body of an inline function
    DIVIDE, // /
    EQUAL, // ==
    EXPONENT, // **
    GREATER_OR_EQUAL, // >=
    GREATER, // >
    EXCLUSIVE_RANGE, // ..
    INCLUSIVE_RANGE, // ...
    LESSER_OR_EQUAL, // <=
    LESSER, // <
    MEMBER_ACCESS, // `.` - Standard identifier based lookup
    MEMBER_ACCESS_OPTIONAL, // `?.` - Returns an optional for the member
    MEMBER_ACCESS_STATIC, // `:` - Used to access static members via a symbol
    MEMBER_ACCESS_STATIC_OPTIONAL, // `?:` - Returns an optional for the static member
    MODULO, // %
    MULTIPLY, // *
    NOT_EQUAL, // !=
    SUBTRACT, // -
    TRIPLE_LEFT_SHIFT, // <<<
    TRIPLE_RIGHT_SHIFT, // >>>
    ASSIGN_OPTIONAL, // `?=` - Conditional assignment
    ASSERT_EQUAL, // ===
    ASSERT_GREATER_OR_EQUAL, // >==
    ASSERT_LESSER_OR_EQUAL, // <==
    ASSERT_NOT_EQUAL, // !==
    ASSIGN, // =
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
    AWAIT, // await
    COPY, // @
    COUNTED, // #
    SYMBOL, // $
    REFERENCE, // &
    MUTABLE_REFERENCE, // *
    MOVE, // `=` - Unary prefix, distinct from `ASSIGN`, used in shorthand
    SPREAD, // ...
    DECREMENT, // --
    INCREMENT, // ++
    NEGATIVE, // -
    NOT, // !
    POSITIVE, // +
    BITWISE_NOT, // ~
    AS, // `as` - Used for aliasing
    HAS,
    BREAK,
    CONTINUE,
    DEFAULT,
    DO,
    ELSE,
    FOR,
    REGISTER,
    WITH, // Used in `register` statements to specify the allow list
    FROM,
    IF,
    RETURN,
    MATCH,
    IS,
    IN,
    IMPORT,
    EXPORT,
    WHILE,
    REPEAT,
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
    CALL, // ->
    TEMPLATE_OPEN, // <
    TEMPLATE_CLOSE, // >
    CAPTURE_OPEN, // [
    CAPTURE_CLOSE, // ]
    PARAMETER_OPEN, // (
    PARAMETER_CLOSE, // )
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
    WILDCARD, // `*` - Used in aggregating export statements
  };
};