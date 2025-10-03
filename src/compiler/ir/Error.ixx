export module compiler.ir.Error;

namespace compiler::ir {
  export enum class Error : uint8_t {
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
    MultiLineCommentExpectedClosingAsteriskSlash, // Expected to find a closing `*/` to end the multi-line comment.
  };
};