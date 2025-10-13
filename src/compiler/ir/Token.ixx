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
    EscapeASCII, // Single character escape such as `\n`, not to be confused with an actual line break token like `LineFeed`
    EscapeHex, // `\xXX` - A character specified by a hexadecimal value of exactly two digits
    EscapeUnicode, // `\uXXXX` - A Unicode character using exactly four hexadecimal digits
    EscapeUnicodeBraced1, // `\u{H}` - A Unicode character using 1 hexadecimal digit
    EscapeUnicodeBraced2, // `\u{HH}` - A Unicode character using 2 hexadecimal digits
    EscapeUnicodeBraced3, // `\u{HHH}` - A Unicode character using 3 hexadecimal digits
    EscapeUnicodeBraced4, // `\u{HHHH}` - A Unicode character using 4 hexadecimal digits
    EscapeUnicodeBraced5, // `\u{HHHHH}` - A Unicode character using 5 hexadecimal digits
    EscapeUnicodeBraced6, // `\u{HHHHHH}` - A Unicode character using 6 hexadecimal digits
    LineFeed, // `\n`
    CarriageReturnLineFeed, // `\r\n`
    CarriageReturn, // `\r`
    LineSeparator, // `\u2028`
    ParagraphSeparator, // `\u2029`
    VerticalTab, // `\v`
    FormFeed, // `\f`
    Underscore, // `_` - Used as a spacer in numeric literals
    Dot, // `.` - Used in decimal literals
    Exponent, // `e` or `E` - Used in decimal literals
    HexStart, // `0x` or `0X`
    OctalStart, // `0o` or `0O`
    BinaryStart, // `0b` or `0B`
    CommentOpen, // `//` - Starts a single line comment
    CommentClose, // Simple marker, doesn't represent any characters
    MultiLineCommentOpen, // `/*` - Starts a multi-line comment
    MultiLineCommentClose, // `*/` - Ends a multi-line comment
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
    Compiler, // `compiler` - Used to access compiler built-ins
    Auto, // `auto` - Used for type inference
    Void, // `void` - Used as the type of `null`
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
    Yield, // `yield` - Unary prefix keyword used to yield a value from a generator
    Await, // `await` - Unary prefix keyword used to wait for a promise to resolve
    Expect, // `expect` - Unary prefix keyword used for assertions in tests
    Copy, // `@` - Unary prefix used to create a copy of a value
    Counted, // `#` - Unary prefix used to indicate a counted loop
    Symbol, // `$` - Unary prefix used to access value's symbol
    Reference, // `&` - Unary prefix used to denote a reference
    MutableReference, // `*` - Unary prefix used to denote a mutable reference
    Spread, // `...` - Unary prefix used to spread elements from one structured literal to another
    Decrement, // `--` - Unary prefix used for decrementing a value
    Increment, // `++` - Unary prefix used for incrementing a value
    Negative, // `-` - Unary prefix used to indicate a negative value
    Positive, // `+` - Unary prefix used to indicate a positive value
    Not, // `!` - Unary prefix used for logical NOT
    BitwiseNot, // `~` - Unary prefix used for bitwise NOT
    As, // `as` - Used for aliasing
    Do, // `do` - Used to declare an unconditional scope block
    Else, // `else` - Used for conditional statements
    Register, // `register` - Used to define a new package
    With, // `with` - Used in `register` statements to specify the allow list
    Import, // `import` - Used to import modules
    From, // `from` - Used to specify the source of an import and in `when` statements
    If, // `if` - Used for conditional statements
    Return, // `return` - Used to return a value from a function
    When, // `when` - Used for pattern matching, sometimes known as `match` in other languages
    Is, // `is` - Used for type checking
    Has, // `has` - Used in `when` statements
    Export, // `export` - Modifier applied to declarations to make them available to `import` statements
    For, // `for` - Used for iterating over collections
    While, // `while` - Used for conditional loops
    Repeat, // `repeat` - Used for unconditional loops, or can be augmented with a trailing `while` condition
    In, // `in` - Used inside `for` loops
    Break, // `break` - Used to exit loops, can be stacked
    Continue, // `continue` - Used to skip the current iteration of a loop, can be stacked
    Default, // `default` - Used to specify the default case in `when` statements
    Case,
    Let, // `let` - Used to declare a variable
    TypeStart, // :
    Wildcard, // `*` - Used in aggregating export statements
  };
};