export module compiler.ir.Token;

import <cstdint>;

namespace compiler::ir {
  export enum class Token : uint8_t {
    None,
    Spaces0,
    Spaces1,
    Spaces2,
    Spaces3,
    Spaces4,
    Spaces5,
    Spaces6,
    Spaces7,
    Spaces8,
    Spaces9,
    SpacesA,
    SpacesB,
    SpacesC,
    SpacesD,
    SpacesE,
    SpacesF,
    Tabs0,
    Tabs1,
    Tabs2,
    Tabs3,
    Tabs4,
    Tabs5,
    Tabs6,
    Tabs7,
    Tabs8,
    Tabs9,
    TabsA,
    TabsB,
    TabsC,
    TabsD,
    TabsE,
    TabsF,
    Digits0,
    Digits1,
    Digits2,
    Digits3,
    Digits4,
    Digits5,
    Digits6,
    Digits7,
    Digits8,
    Digits9,
    DigitsA,
    DigitsB,
    DigitsC,
    DigitsD,
    DigitsE,
    DigitsF,
    Characters0,
    Characters1,
    Characters2,
    Characters3,
    Characters4,
    Characters5,
    Characters6,
    Characters7,
    Characters8,
    Characters9,
    CharactersA,
    CharactersB,
    CharactersC,
    CharactersD,
    CharactersE,
    CharactersF,
    EscapeASCII, // Single character escape such as `\n`
    EscapeHex, // `\xXX` - A character specified by a hexadecimal value of exactly two digits
    EscapeUnicode, // `\uXXXX` - A Unicode character using exactly four hexadecimal digits
    EscapeUnicodeBraced1, // `\u{H}` - A Unicode character using 1 hexadecimal digit
    EscapeUnicodeBraced2, // `\u{HH}` - A Unicode character using 2 hexadecimal digits
    EscapeUnicodeBraced3, // `\u{HHH}` - A Unicode character using 3 hexadecimal digits
    EscapeUnicodeBraced4, // `\u{HHHH}` - A Unicode character using 4 hexadecimal digits
    EscapeUnicodeBraced5, // `\u{HHHHH}` - A Unicode character using 5 hexadecimal digits
    EscapeUnicodeBraced6, // `\u{HHHHHH}` - A Unicode character using 6 hexadecimal digits
    LineFeed,
    CarriageReturnLineFeed,
    CarriageReturn,
    LineSeparator,
    ParagraphSeparator,
    VerticalTab, // `\v`
    FormFeed, // `\f`
    Underscore, // `_` - Used as a spacer in numeric literals
    Dot, // `.` - Used in decimal literals
    Exponent, // `e` or `E` - Used in decimal literals
    HexStart, // `0x`
    OctalStart, // `0o`
    BinaryStart, // `0b`
    CommentOpen, // `//` - Starts a single line comment
    CommentClose, // Simple marker, doesn't represent any characters
    MultiLineCommentOpen, // `/*` - Starts a multi-line comment
    MultiLineCommentClose, // `*/` - Ends a multi-line comment
    Undefined, // `undefined`
    Null, // `null`
    True, // `true`
    False, // `false`
    This, // `this` - Used in object literals and functions
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
    When,
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