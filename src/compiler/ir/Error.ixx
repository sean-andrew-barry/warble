export module compiler.ir.Error;

namespace compiler::ir {
  export enum class Error : uint8_t {
    // Lexical errors
    EscapeSequenceUnexpectedEndOfInput,
    EscapeSequenceExpectedClosingBrace,
    EscapeSequenceExpectedTwoHexDigits,
    EscapeSequenceExpectedFourHexDigits,
    EscapeSequenceExpectedOneOrMoreHexDigits,
    EscapeSequenceExpectedSixOrFewerHexDigits,
    EscapeSequenceUnexpectedEnd,
    EscapeSequenceExpectedAfterBackslash,
    CharacterLiteralExpectedClosingSingleQuote,
    StringLiteralExpectedClosingDoubleQuote,
    TemplateStringLiteralExpectedExpression,
    TemplateStringLiteralExpectedClosingBrace,
    TemplateStringLiteralExpectedClosingBacktick,
    UnaryPrefixOperatorExpectedValue,
    BinaryOperatorExpectedLHS,
    MultiLineCommentExpectedClosingAsteriskSlash,

    // Unicode errors
    UnicodeCodePointOutOfRange,
    UnicodeCodePointInvalid,
    UnicodeSurrogateCodePointIsNotAValidScalarValue,
  };
};