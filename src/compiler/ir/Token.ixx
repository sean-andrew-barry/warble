export module compiler.ir.Token;

import <cstdint>;
import <string_view>;

namespace compiler::ir {
  export enum class Token : uint8_t {
    // General / sentinel
    None,
    Error, // Marks that an error code was added to the `errors` side-table

    // Character classes and low-level scanning
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

    // Escapes and line terminators
    EscapeASCII, // Single character escape such as `\x`, not to be confused with an actual line break token like `LineFeed`
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

    // Numeric literals and prefixes
    Underscore, // `_` - Used as a spacer in numeric literals
    Dot, // `.` - Used in decimal literals
    Exponent, // `e` or `E` - Used in decimal literals
    Power, // `p` or `P` - Used in hexadecimal float literals
    Limbs, // Used to mark the length of the limb data for big integer and big float literals
    HexStart, // `0x` or `0X`
    OctalStart, // `0o` or `0O`
    BinaryStart, // `0b` or `0B`

    // Comments
    CommentOpen, // `//` - Starts a single line comment
    CommentClose, // Simple marker, doesn't represent any characters
    MultiLineCommentOpen, // `/*` - Starts a multi-line comment
    MultiLineCommentClose, // `*/` - Ends a multi-line comment

    // Delimiters and punctuation used in expressions/statements
    ArrayOpen, // `[` 
    ArrayClose, // `]`
    EnumOpen, // `<`
    EnumClose, // `>`
    ObjectOpen, // `{`
    ObjectClose, // `}`
    TupleOpen, // `(`
    TupleClose, // `)`
    CharacterOpen, // `'`
    CharacterClose, // `'`
    ConditionOpen, // `(`
    ConditionClose, // `)`
    CaptureOpen, // `[` 
    CaptureClose, // `]`
    ParameterOpen, // `(`
    ParameterClose, // `)`
    ScopeOpen, // `{`
    ScopeClose, // `}`
    StringOpen, // `"`
    StringClose, // `"`
    TemplateStringOpen, // `"` - Same as a normal string
    TemplateStringClose, // `"`
    TemplateStringExpressionOpen, // `{`
    TemplateStringExpressionClose, // `}`
    Comma, // `,`
    Semicolon, // `;`

    // Expression-term keywords and primitive-like terms
    Compiler, // `compiler` - Used to access compiler built-ins
    Auto, // `auto` - Used for type inference
    Undefined, // `undefined`
    Null, // `null`
    Readonly, // `readonly`
    True, // `true`
    False, // `false`
    This, // `this` - Used in object literals and some statements to refer to the top of the topic stack
    That, // `that` - Used in object literals and some statements to refer to the second item on the topic stack

    // Declaration-related punctuation and modifiers
    TypeStart, // `:` - Used to denote the start of a type annotation in a declaration
    Wildcard, // `*` - Used in aggregating export statements. TODO: Consider using spread `...` instead?

    // Binary operators (including assignment and relational)
    Add, // `+`
    Subtract, // `-`
    Multiply, // `*`
    Divide, // `/`
    Modulo, // `%`
    Range, // `..`
    MemberReference, // `.` - Standard identifier based lookup
    MutableMemberReference, // `:` - Used to access mutable members via a symbol
    StaticMemberReference, // `::` - Used to access symbols statically
    And, // `&&`
    Or, // `||`
    Pipeline, // `->` - Binary operator used to pipe a value into a function
    Equal, // `==`
    NotEqual, // `!=`
    GreaterOrEqual, // `>=`
    Greater, // `>`
    LesserOrEqual, // `<=`
    Lesser, // `<`
    AssertEqual, // `===`
    AssertGreaterOrEqual, // `>==`
    AssertLesserOrEqual, // `<==`
    AssertNotEqual, // `!==`
    BitwiseLeftShift, // `<<`
    BitwiseRightShift, // `>>`
    TripleLeftShift, // `<<<`
    TripleRightShift, // `>>>`
    BitwiseTripleLeftShift, // `<<<`
    BitwiseTripleRightShift, // `>>>`
    BitwiseAnd, // `&`
    BitwiseXor, // `^`
    BitwiseOr, // `|`
    To, // `to` - Binary operator used for type conversions. TODO: Consider `as`?
    Arrow, // `=>` - Used to mark the body of an inline function

    // Assignment and compound assignment operators
    Initialize, // `=` - Used in declarations to assign an initial value
    Assign, // `=`
    AssignAdd, // `+=`
    AssignSubtract, // `-=`
    AssignMultiply, // `*=`
    AssignDivide, // `/=`
    AssignModulo, // `%=`
    AssignAnd, // `&&=` - AKA `a && (a = b)` AKA `if (a) a = b;`
    AssignOr, // `||=` - AKA `a || (a = b)` AKA `if (!a) a = b;`
    BitwiseAssignLeftShift, // `<<=`
    BitwiseAssignRightShift, // `>>=`
    BitwiseAssignTripleLeftShift, // `<<<=`
    BitwiseAssignTripleRightShift, // `>>>=`
    BitwiseAssignAnd, // `&=`
    BitwiseAssignXor, // `^=`
    BitwiseAssignOr, // `|=`

    // Unary operators (including keyword forms)
    Runtime,
    Comtime,
    Await, // `await` - Unary prefix keyword used to wait for a promise to resolve
    Assume, // `assume` - Unary prefix keyword used for panicking on failure
    Expect, // `expect` - Unary prefix keyword used for failure propagation
    CopyOf, // `@` - Unary prefix used to create a copy of a value. TODO: Change to mean a decorator?
    LengthOf, // `#` - Unary prefix length
    SymbolOf, // `$` - Unary prefix used to access value's symbol
    ReferenceOf, // `&` - Unary prefix used to denote a reference
    MutableReferenceOf, // `*` - Unary prefix used to denote a mutable reference
    Spread, // `...` - Unary prefix used to spread elements from one structured literal to another
    Decrement, // `--` - Unary prefix used for decrementing a value
    Increment, // `++` - Unary prefix used for incrementing a value
    Negative, // `-` - Unary prefix used to indicate a negative value, also used in floating point literals
    Positive, // `+` - Unary prefix used to indicate a positive value, also used in floating point literals
    Not, // `!` - Unary prefix used for logical NOT
    BitwiseNot, // `~` - Unary prefix used for bitwise NOT
    Guard, // `?` - Unary postfix used for conditional short-circuiting
    ArrowHead, // A special zero width marker that says the following identifier is an arrow function parameter

    // Statement keywords (flow control and blocks)
    Do, // `do` - Used to declare an unconditional scope block
    Else, // `else` - Used for conditional statements
    If, // `if` - Used for conditional statements
    Try, // `try` - Used to declare a try block
    Yield, // `yield` - Used to yield a value from a generator
    Return, // `return` - Used to return a value from a function
    Panic, // `panic` - Used to abort the current execution path with an error
    Async, // `async` - Marks asynchronous imports and functions (modifier for exits)
    Pass, // `pass` - Decorates a `return/yield` to indicate a passing result
    Fail, // `fail` - Decorates a `return/yield` to indicate a failing result
    Default, // `default` - Used to specify the default case in `when` statements
    When, // `when` - Used for pattern matching, sometimes known as `match` in other languages. TODO: Deprecate
    Is, // `is` - Used for type checking
    Has, // `has` - Used in `when` statements
    For, // `for` - Used for iterating over collections
    While, // `while` - Used for conditional loops
    Loop, // `loop` - Used for unconditional loops
    Repeat, // `repeat` - Used for post-checked loops that require a trailing `while` condition
    In, // `in` - Used inside `for` loops and in `import` statements
    As, // `as` - Used for aliasing in imports and object destructuring
    Break, // `break` - Used to exit loops, can be stacked
    Continue, // `continue` - Used to skip the current iteration of a loop, can be stacked
    Let, // `let` - Used to begin a statement mode declaration

    // Module and package keywords
    Register, // `register` - Used to define a new package
    With, // `with` - Used in `register` statements to specify the allow list
    Import, // `import` - Used to import modules
    From, // `from` - Used to specify the source of an import and in `when` statements
    Export, // `export` - Modifier applied to declarations to make them available to `import` statements

    // Declaration modifiers
    Const, // `const` - Used to declare a constant
    Mut, // `mut` - Used to declare a mutable variable
    Public, // `public` - Used to declare a public declaration
    Private, // `private` - Used to declare a private declaration
    Protected, // `protected` - Used to declare a protected declaration

    // URL tokens
    Drive, // Ends with `:`
    Scheme, // Ends with `:`
    Authority, // `//` - The authority section of a URL
    Username, // Ends with `:` if password is present, otherwise ends with `@`
    Password, // Ends with `@`
    Hostname, // The full hostname string section
    IPv4,
    IPv6,
    Path, // `/` - Used to separate path segments
    Port, // A number, be preceded by `:`
    QueryKey, // Preceded by `?` or `&`
    QueryValue, // Preceded by `=`
    Fragment, // Preceded by `#`
    EscapePercent, // `%XX` - A percent encoded character in a URL
  };

  export constexpr std::string_view ToString(Token token) {
    switch (token) {
      case Token::None: return "None";
      case Token::Error: return "Error";
      case Token::Spaces0: return "Spaces0";
      case Token::Spaces1: return "Spaces1";
      case Token::Spaces2: return "Spaces2";
      case Token::Spaces3: return "Spaces3";
      case Token::Spaces4: return "Spaces4";
      case Token::Spaces5: return "Spaces5";
      case Token::Spaces6: return "Spaces6";
      case Token::Spaces7: return "Spaces7";
      case Token::Spaces8: return "Spaces8";
      case Token::Spaces9: return "Spaces9";
      case Token::SpacesA: return "SpacesA";
      case Token::SpacesB: return "SpacesB";
      case Token::SpacesC: return "SpacesC";
      case Token::SpacesD: return "SpacesD";
      case Token::SpacesE: return "SpacesE";
      case Token::SpacesF: return "SpacesF";
      case Token::Tabs0: return "Tabs0";
      case Token::Tabs1: return "Tabs1";
      case Token::Tabs2: return "Tabs2";
      case Token::Tabs3: return "Tabs3";
      case Token::Tabs4: return "Tabs4";
      case Token::Tabs5: return "Tabs5";
      case Token::Tabs6: return "Tabs6";
      case Token::Tabs7: return "Tabs7";
      case Token::Tabs8: return "Tabs8";
      case Token::Tabs9: return "Tabs9";
      case Token::TabsA: return "TabsA";
      case Token::TabsB: return "TabsB";
      case Token::TabsC: return "TabsC";
      case Token::TabsD: return "TabsD";
      case Token::TabsE: return "TabsE";
      case Token::TabsF: return "TabsF";
      case Token::Digits0: return "Digits0";
      case Token::Digits1: return "Digits1";
      case Token::Digits2: return "Digits2";
      case Token::Digits3: return "Digits3";
      case Token::Digits4: return "Digits4";
      case Token::Digits5: return "Digits5";
      case Token::Digits6: return "Digits6";
      case Token::Digits7: return "Digits7";
      case Token::Digits8: return "Digits8";
      case Token::Digits9: return "Digits9";
      case Token::DigitsA: return "DigitsA";
      case Token::DigitsB: return "DigitsB";
      case Token::DigitsC: return "DigitsC";
      case Token::DigitsD: return "DigitsD";
      case Token::DigitsE: return "DigitsE";
      case Token::DigitsF: return "DigitsF";
      case Token::Characters0: return "Characters0";
      case Token::Characters1: return "Characters1";
      case Token::Characters2: return "Characters2";
      case Token::Characters3: return "Characters3";
      case Token::Characters4: return "Characters4";
      case Token::Characters5: return "Characters5";
      case Token::Characters6: return "Characters6";
      case Token::Characters7: return "Characters7";
      case Token::Characters8: return "Characters8";
      case Token::Characters9: return "Characters9";
      case Token::CharactersA: return "CharactersA";
      case Token::CharactersB: return "CharactersB";
      case Token::CharactersC: return "CharactersC";
      case Token::CharactersD: return "CharactersD";
      case Token::CharactersE: return "CharactersE";
      case Token::CharactersF: return "CharactersF";
      case Token::EscapeASCII: return "EscapeASCII";
      case Token::EscapeHex: return "EscapeHex";
      case Token::EscapeUnicode: return "EscapeUnicode";
      case Token::EscapeUnicodeBraced1: return "EscapeUnicodeBraced1";
      case Token::EscapeUnicodeBraced2: return "EscapeUnicodeBraced2";
      case Token::EscapeUnicodeBraced3: return "EscapeUnicodeBraced3";
      case Token::EscapeUnicodeBraced4: return "EscapeUnicodeBraced4";
      case Token::EscapeUnicodeBraced5: return "EscapeUnicodeBraced5";
      case Token::EscapeUnicodeBraced6: return "EscapeUnicodeBraced6";
      case Token::LineFeed: return "LineFeed";
      case Token::CarriageReturnLineFeed: return "CarriageReturnLineFeed";
      case Token::CarriageReturn: return "CarriageReturn";
      case Token::LineSeparator: return "LineSeparator";
      case Token::ParagraphSeparator: return "ParagraphSeparator";
      case Token::VerticalTab: return "VerticalTab";
      case Token::FormFeed: return "FormFeed";
      case Token::Underscore: return "Underscore";
      case Token::Dot: return "Dot";
      case Token::Exponent: return "Exponent";
      case Token::Power: return "Power";
      case Token::Limbs: return "Limbs";
      case Token::HexStart: return "HexStart";
      case Token::OctalStart: return "OctalStart";
      case Token::BinaryStart: return "BinaryStart";
      case Token::CommentOpen: return "CommentOpen";
      case Token::CommentClose: return "CommentClose";
      case Token::MultiLineCommentOpen: return "MultiLineCommentOpen";
      case Token::MultiLineCommentClose: return "MultiLineCommentClose";
      case Token::ArrayOpen: return "ArrayOpen";
      case Token::ArrayClose: return "ArrayClose";
      case Token::EnumOpen: return "EnumOpen";
      case Token::EnumClose: return "EnumClose";
      case Token::ObjectOpen: return "ObjectOpen";
      case Token::ObjectClose: return "ObjectClose";
      case Token::TupleOpen: return "TupleOpen";
      case Token::TupleClose: return "TupleClose";
      case Token::CharacterOpen: return "CharacterOpen";
      case Token::CharacterClose: return "CharacterClose";
      case Token::Comma: return "Comma";
      case Token::ConditionOpen: return "ConditionOpen";
      case Token::ConditionClose: return "ConditionClose";
      case Token::Pipeline: return "Pipeline";
      case Token::CaptureOpen: return "CaptureOpen";
      case Token::CaptureClose: return "CaptureClose";
      case Token::ParameterOpen: return "ParameterOpen";
      case Token::ParameterClose: return "ParameterClose";
      case Token::ScopeOpen: return "ScopeOpen";
      case Token::ScopeClose: return "ScopeClose";
      case Token::Semicolon: return "Semicolon";
      case Token::StringOpen: return "StringOpen";
      case Token::StringClose: return "StringClose";
      case Token::TemplateStringOpen: return "TemplateStringOpen";
      case Token::TemplateStringClose: return "TemplateStringClose";
      case Token::TemplateStringExpressionOpen: return "TemplateStringExpressionOpen";
      case Token::TemplateStringExpressionClose: return "TemplateStringExpressionClose";
      case Token::Compiler: return "Compiler";
      case Token::Auto: return "Auto";
      case Token::Undefined: return "Undefined";
      case Token::Null: return "Null";
      case Token::Readonly: return "Readonly";
      case Token::True: return "True";
      case Token::False: return "False";
      case Token::This: return "This";
      case Token::That: return "That";
      case Token::TypeStart: return "TypeStart";
      case Token::Wildcard: return "Wildcard";
      case Token::Add: return "Add";
      case Token::And: return "And";
      case Token::Or: return "Or";
      case Token::Arrow: return "Arrow";
      case Token::ArrowHead: return "ArrowHead";
      case Token::Divide: return "Divide";
      case Token::Equal: return "Equal";
      case Token::GreaterOrEqual: return "GreaterOrEqual";
      case Token::Greater: return "Greater";
      case Token::Range: return "Range";
      case Token::LesserOrEqual: return "LesserOrEqual";
      case Token::Lesser: return "Lesser";
      case Token::MemberReference: return "MemberReference";
      case Token::MutableMemberReference: return "MutableMemberReference";
      case Token::StaticMemberReference: return "StaticMemberReference";
      case Token::Modulo: return "Modulo";
      case Token::Multiply: return "Multiply";
      case Token::NotEqual: return "NotEqual";
      case Token::Subtract: return "Subtract";
      case Token::TripleLeftShift: return "TripleLeftShift";
      case Token::TripleRightShift: return "TripleRightShift";
      case Token::AssertEqual: return "AssertEqual";
      case Token::AssertGreaterOrEqual: return "AssertGreaterOrEqual";
      case Token::AssertLesserOrEqual: return "AssertLesserOrEqual";
      case Token::AssertNotEqual: return "AssertNotEqual";
      case Token::Initialize: return "Initialize";
      case Token::Assign: return "Assign";
      case Token::AssignAdd: return "AssignAdd";
      case Token::AssignDivide: return "AssignDivide";
      case Token::AssignModulo: return "AssignModulo";
      case Token::AssignMultiply: return "AssignMultiply";
      case Token::AssignSubtract: return "AssignSubtract";
      case Token::AssignAnd: return "AssignAnd";
      case Token::AssignOr: return "AssignOr";
      case Token::BitwiseLeftShift: return "BitwiseLeftShift";
      case Token::BitwiseRightShift: return "BitwiseRightShift";
      case Token::BitwiseTripleLeftShift: return "BitwiseTripleLeftShift";
      case Token::BitwiseTripleRightShift: return "BitwiseTripleRightShift";
      case Token::BitwiseAssignTripleLeftShift: return "BitwiseAssignTripleLeftShift";
      case Token::BitwiseAssignTripleRightShift: return "BitwiseAssignTripleRightShift";
      case Token::BitwiseAssignLeftShift: return "BitwiseAssignLeftShift";
      case Token::BitwiseAssignRightShift: return "BitwiseAssignRightShift";
      case Token::BitwiseAnd: return "BitwiseAnd";
      case Token::BitwiseXor: return "BitwiseXor";
      case Token::BitwiseOr: return "BitwiseOr";
      case Token::BitwiseAssignAnd: return "BitwiseAssignAnd";
      case Token::BitwiseAssignXor: return "BitwiseAssignXor";
      case Token::BitwiseAssignOr: return "BitwiseAssignOr";
      case Token::Yield: return "Yield";
      case Token::Runtime: return "Runtime";
      case Token::Comtime: return "Comtime";
      case Token::Await: return "Await";
      case Token::Assume: return "Assume";
      case Token::Async: return "Async";
      case Token::Expect: return "Expect";
      case Token::CopyOf: return "CopyOf";
      case Token::LengthOf: return "LengthOf";
      case Token::SymbolOf: return "SymbolOf";
      case Token::ReferenceOf: return "ReferenceOf";
      case Token::MutableReferenceOf: return "MutableReferenceOf";
      case Token::Spread: return "Spread";
      case Token::Decrement: return "Decrement";
      case Token::Increment: return "Increment";
      case Token::Negative: return "Negative";
      case Token::Positive: return "Positive";
      case Token::Not: return "Not";
      case Token::BitwiseNot: return "BitwiseNot";
      case Token::Guard: return "Guard";
      case Token::To: return "To";
      case Token::Do: return "Do";
      case Token::Else: return "Else";
      case Token::Register: return "Register";
      case Token::With: return "With";
      case Token::Import: return "Import";
      case Token::From: return "From";
      case Token::If: return "If";
      case Token::Try: return "Try";
      case Token::Return: return "Return";
      case Token::Panic: return "Panic";
      case Token::When: return "When";
      case Token::Is: return "Is";
      case Token::Has: return "Has";
      case Token::Export: return "Export";
      case Token::For: return "For";
      case Token::While: return "While";
      case Token::Loop: return "Loop";
      case Token::Repeat: return "Repeat";
      case Token::In: return "In";
      case Token::As: return "As";
      case Token::Break: return "Break";
      case Token::Continue: return "Continue";
      case Token::Default: return "Default";
      case Token::Pass: return "Pass";
      case Token::Fail: return "Fail";
      case Token::Let: return "Let";
      case Token::Const: return "Const";
      case Token::Mut: return "Mut";
      case Token::Public: return "Public";
      case Token::Private: return "Private";
      case Token::Protected: return "Protected";
      case Token::Drive: return "Drive";
      case Token::Scheme: return "Scheme";
      case Token::Authority: return "Authority";
      case Token::Username: return "Username";
      case Token::Password: return "Password";
      case Token::Hostname: return "Hostname";
      case Token::IPv4: return "IPv4";
      case Token::IPv6: return "IPv6";
      case Token::Path: return "Path";
      case Token::Port: return "Port";
      case Token::QueryKey: return "QueryKey";
      case Token::QueryValue: return "QueryValue";
      case Token::Fragment: return "Fragment";
      case Token::EscapePercent: return "EscapePercent";
    }
  
    return "Unknown";
  }
}
