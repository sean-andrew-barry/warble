export module compiler.ir.Error;

import <cstdint>;

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
    CharacterLiteralExpectedContent,
    CharacterLiteralExpectedEscapeAfterBackslash,
    CharacterLiteralExpectedClosingSingleQuote,
    StringLiteralExpectedClosingDoubleQuote,
    StringLiteralUnexpectedLineBreak,
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

    // Declaration/statement lexer errors
    DeclarationExpectedIdentifierAfterModifiers,
    DeclarationExplicitTypeCannotBeUsedWithPostfixUnaryOperators,
    DeclarationExpectedTypeExpressionAfterColon,
    DeclarationExpectedExpressionAfterAssignmentOperator,
    DeclarationCannotSkipAssignmentOperatorWithPostfixUnaryOperators,

    // Parser errors
    ParserExpectedObjectDeclaration,
  };
};