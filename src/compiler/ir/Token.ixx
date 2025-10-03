export module compiler.ir.Token;

import <cstdint>;

namespace compiler::ir {
  export enum class Token : uint8_t {
    None,

    // Symbol tokens each represent that a symbol was created for that token
    Error, // Indicates a symbol was created for an error
    Identifier, // Indicates a symbol was created for an identifier
    String, // Indicates a symbol was created for a string literal
    Character, // Indicates a symbol was created for a character literal
    Integer, // Indicates a symbol was created for an integer literal
    Decimal, // Indicates a symbol was created for a decimal literal
    Comment, // Indicates a symbol was created for a comment
    MultiLineComment, // Indicates a symbol was created for a multi-line comment

    // Variable width tokens represent a sequence where the count is stored separately
    Spaces, // One or more space characters
    Tabs, // One or more tab characters
    Digits, // One or more digit characters (including hex, octal, and binary)
    Characters, // Marks a variable number of arbitrary characters
    NewlinesLF, // `\n` - Line feed character
    NewlinesCRLF, // `\r\n` - Combination carriage return and line feed
    NewlinesCR, // `\r` - Carriage return character
    NewlinesLS, // `'\u2028'` - Unicode line separator
    NewlinesPS, // `\u2029` - Unicode paragraph separator

    // Fixed width tokens represent a fixed sequence of characters
    VerticalTab, // `\v`
    FormFeed, // `\f`
    Underscore, // `_` - Used as a spacer in numeric literals
    Dot, // `.` - Used in decimal literals
    Exponent, // `e` or `E` - Used in decimal literals
    HexStart, // `0x`
    OctalStart, // `0o`
    BinaryStart, // `0b`
    CommentOpen, // `//` - Starts a single line comment
    CommentClose, // Simple marker, doesn't represent a character
    MultiLineCommentOpen, // `/*` - Starts a multi-line comment
    MultiLineCommentClose, // `*/` - Ends a multi-line comment
    Undefined, // `undefined`
    Null, // `null`
    True, // `true`
    False, // `false`
    This, // `this` - Used in object literals and functions
    EscapeNewline, // (\n) - A newline character
    EscapeTab, // (\t) - A tab character
    EscapeBackspace, // (\b) - A backspace
    EscapeReturn, // (\r) - A carriage return
    EscapeFormFeed, // (\f) - A form feed
    EscapeNull, // (\0) - The null character
    EscapeCharacter, // (\cX) - An escaped alphabetic character
    EscapeSyntax, // (\S) - Here `S` is one of `^, $, \, ., *, +, ?, (, ), [, ], {, }, |, \/`
    EscapeUnicodeCodepointStart, // `\u{` - The start of a variable length Unicode code point of 1 to 6 hexadecimal digits like `\u{HHHHHH}`
    EscapeUnicodeCodepointEnd, // `}` - The end marker
    EscapeUnicodeShort, // (\uXXXX) - A Unicode character using exactly four hexadecimal digits
    EscapeHexCode, // (\xXX) - A character specified by a hexadecimal value of exactly two digits
    Add, // +
    And, // &&
    Or, // ||
    Wrap, // `!!` - Builds a variant from values
    Unwrap, // `??` - Conditionally unwraps a variant
    InlineFunctionArrow, // `=>` - Used to mark the body of an inline function
    Divide, // /
    Equal, // ==
    Exponent, // **
    GreaterOrEqual, // >=
    Greater, // >
    ExclusiveRange, // ..
    InclusiveRange, // ...
    LesserOrEqual, // <=
    Lesser, // <
    MemberAccess, // `.` - Standard identifier based lookup
    MemberAccessOptional, // `?.` - Returns an optional for the member
    MemberAccessStatic, // `:` - Used to access static members via a symbol
    MemberAccessStaticOptional, // `?:` - Returns an optional for the static member
    Modulo, // %
    Multiply, // *
    NotEqual, // !=
    Subtract, // -
    TripleLeftShift, // <<<
    TripleRightShift, // >>>
    AssignOptional, // `?=` - Conditional assignment
    AssertEqual, // ===
    AssertGreaterOrEqual, // >==
    AssertLesserOrEqual, // <==
    AssertNotEqual, // !==
    Assign, // =
    AssignAdd, // +=
    AssignDivide, // /=
    AssignExponent, // **=
    AssignModulo, // %=
    AssignMultiply, // *=
    AssignSubtract, // -=
    AssignAnd, // &&=
    AssignOr, // ||=
    AssignTruthyAnd, // !!=
    AssignTruthyOr, // ??=
    BitwiseLeftShift, // <<
    BitwiseRightShift, // >>
    BitwiseTripleLeftShift, // <<<
    BitwiseTripleRightShift, // >>>
    BitwiseAssignTripleLeftShift, // <<<=
    BitwiseAssignTripleRightShift, // >>>=
    BitwiseAssignLeftShift, // <<=
    BitwiseAssignRightShift, // >>=
    BitwiseAnd, // &
    BitwiseXor, // ^
    BitwiseOr, // |
    BitwiseAssignAnd, // &=
    BitwiseAssignXor, // ^=
    BitwiseAssignOr, // |=
    Yield, // yield
    Await, // await
    Copy, // @
    Counted, // #
    Symbol, // $
    Reference, // &
    MutableReference, // *
    Move, // `=` - Unary prefix, distinct from `ASSIGN`, used in shorthand
    Spread, // ...
    Decrement, // --
    Increment, // ++
    Negative, // -
    Positive, // +
    Not, // !
    BitwiseNot, // ~
    As, // `as` - Used for aliasing
    Has,
    Break,
    Continue,
    Default,
    Do,
    Else,
    For,
    Register,
    With, // `with` - Used in `register` statements to specify the allow list
    From,
    If,
    Return,
    Match,
    Is,
    In,
    Import,
    Export,
    While,
    Repeat,
    Case,
    Let,
    Compiler,
    Auto, // auto
    Void, // void
    ArrayOpen, // [
    ArrayClose, // ]
    EnumOpen, // <
    EnumClose, // >
    ObjectOpen, // {
    ObjectClose, // }
    TupleOpen, // (
    TupleClose, // )
    DestructuredArrayOpen, // [
    DestructuredArrayClose, // ]
    DestructuredEnumOpen, // <
    DestructuredEnumClose, // >
    DestructuredObjectOpen, // {
    DestructuredObjectClose, // }
    DestructuredTupleOpen, // (
    DestructuredTupleClose, // )
    CharOpen, // '
    CharClose, // '
    Comma, // ,
    ConditionOpen, // (
    ConditionClose, // )
    Call, // ->
    CaptureOpen, // [
    CaptureClose, // ]
    ParameterOpen, // (
    ParameterClose, // )
    ScopeOpen, // {
    ScopeClose, // }
    Semicolon, // ;
    TemplateStringOpen, // `
    TemplateStringClose, // `
    TemplateStringExpressionOpen, // {
    TemplateStringExpressionClose, // }
    StringOpen, // "
    StringClose, // "
    TypeStart, // :
    Wildcard, // `*` - Used in aggregating export statements
  };
};