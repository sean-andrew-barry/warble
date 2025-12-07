export module compiler.ir.Error;

import <cstdint>;
import <string_view>;

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
    EscapeInvalidPercentEncoding,
    CharacterLiteralExpectedContent,
    CharacterLiteralExpectedEscapeAfterBackslash,
    CharacterLiteralExpectedClosingSingleQuote,
    CharacterLiteralUnexpectedLineBreak,
    StringLiteralExpectedClosingDoubleQuote,
    SpecifierLiteralExpectedClosingDoubleQuote,
    StringLiteralUnexpectedLineBreak,
    StringLiteralExpectedEscapeAfterBackslash,
    TemplateStringLiteralExpectedExpression,
    TemplateStringLiteralExpectedClosingBrace,
    TemplateStringLiteralExpectedClosingBacktick,
    UnaryPrefixOperatorExpectedValue,
    BinaryOperatorExpectedLHS,
    MultiLineCommentExpectedClosingAsteriskSlash,
    ArrowFunctionExpectedExpression,
    ArrowFunctionExpectedArrowAfterFunctionHeader,
    SpecifierExpectedPathAfterScheme,
    SpecifierExpectedHostAfterAuthority,
    ConditionExpectedExpression,
    ConditionExpectedClosingParenthesis,
    BlockExpectedClosingCurlyBrace,
    SelectorOperatorExpectedSelector,
    UnaryPrefixOperatorExpectedSelector,

    SpecifierExpectedHost,
    SpecifierExpectedAuthority,
    SpecifierBadPathAfterAuthority,
    SpecifierBadAbsolutePath,
    SpecifierBadRootlessOrEmpty,

    IsPatternExpectedCondition,
    IsPatternExpectedScopeBlock,
    HasPatternExpectedCondition,
    HasPatternExpectedScopeBlock,
    FromPatternExpectedCondition,
    FromPatternExpectedScopeBlock,
    
    // Unicode errors
    UnicodeCodePointOutOfRange,
    UnicodeCodePointInvalid,
    UnicodeSurrogateCodePointIsNotAValidScalarValue,

    // Declaration/statement lexer errors
    DeclarationExplicitTypeCannotBeUsedWithPostfixUnaryOperators,
    DeclarationExpectedTypeExpressionAfterColon,
    DeclarationExpectedExpressionAfterAssignmentOperator,
    DeclarationCannotSkipAssignmentOperatorWithPostfixUnaryOperators,
    DeclarationStatementExpectedIdentifier,
    DeclarationStatementExpectedTypeExpressionAfterColon,
    DeclarationStatementExpectedInitializerExpressionAfterEquals,
    DeclarationStatementExpectedInitializer,
    DeclarationStatementExpectedSemicolon,
    DeclarationExpectedIdentifier,
    DeclarationExpectedInitializerExpressionAfterEquals,
    DeclarationPrefixFormCannotHaveInitializer,
    ExpressionStatementExpectedSemicolon,
    EnumExpectedExpression,
    EnumExpectedClosingAngleBracket,
    ArrayExpectedExpression,
    ArrayExpectedCountExpression,
    ArrayExpectedClosingBracket,
    TupleExpectedExpression,
    TupleExpectedClosingParenthesis,
    ObjectExpectedDeclaration,
    ObjectExpectedClosingCurlyBrace,
    ImportExpectedIdentifier,
    ImportExpectedFromKeyword,
    ImportExpectedSpecifier,
    ImportExpectedIdentifierAfterIn,
    ImportExpectedSemicolon,
    RegisterExpectedIdentifier,
    RegisterExpectedFromKeyword,
    RegisterExpectedSpecifier,
    RegisterExpectedIdentifierAfterWith,
    RegisterExpectedSemicolon,
    DoExpectedScopeBlock,
    LoopExpectedScopeBlock,
    WhileExpectedCondition,
    WhileExpectedScopeBlock,
    IfExpectedCondition,
    IfExpectedScopeBlock,
    IfPatternExpectedCondition,
    IfPatternExpectedScopeBlock,
    ElseStatementExpectedScopeBlock,
    IfStatementExpectedCondition,
    IfStatementExpectedScopeBlockOrPatternArms,
    TryStatementExpectedScopeBlock,
    BreakStatementExpectedSemicolon,
    ContinueStatementExpectedSemicolon,
    ReturnStatementExpectedSemicolon,
    PanicStatementExpectedSemicolon,
    YieldStatementExpectedSemicolon,
    UnaryPrefixOperatorExpectedExpression,
    BinaryOperatorExpectedExpression,
    RepeatExpectedScopeBlock,
    RepeatExpectedWhileKeyword,
    RepeatExpectedWhileCondition,
    ForExpectedBinding,
    ForExpectedInKeyword,
    ForExpectedCondition,
    ForExpectedScopeBlock,

    // Parser errors
    ParserExpectedObjectDeclaration,
    ParserBreakOutsideLoop,
    ParserContinueOutsideLoop,
    ParserExpectedWhenArmBody,
    ParserDuplicateDefaultInWhen,
    ParserExpectedWhenArm,
    ParserExpectedHexStart,
    ParserExpectedOctalStart,
    ParserExpectedBinaryStart,
    ParserExpectedDecimal,
  };

  export constexpr std::string_view ToString(Error error) {
    switch (error) {
      case Error::EscapeSequenceUnexpectedEndOfInput: return "EscapeSequenceUnexpectedEndOfInput";
      case Error::EscapeSequenceExpectedClosingBrace: return "EscapeSequenceExpectedClosingBrace";
      case Error::EscapeSequenceExpectedTwoHexDigits: return "EscapeSequenceExpectedTwoHexDigits";
      case Error::EscapeSequenceExpectedFourHexDigits: return "EscapeSequenceExpectedFourHexDigits";
      case Error::EscapeSequenceExpectedOneOrMoreHexDigits: return "EscapeSequenceExpectedOneOrMoreHexDigits";
      case Error::EscapeSequenceExpectedSixOrFewerHexDigits: return "EscapeSequenceExpectedSixOrFewerHexDigits";
      case Error::EscapeSequenceUnexpectedEnd: return "EscapeSequenceUnexpectedEnd";
      case Error::EscapeSequenceExpectedAfterBackslash: return "EscapeSequenceExpectedAfterBackslash";
      case Error::EscapeInvalidPercentEncoding: return "EscapeInvalidPercentEncoding";
      case Error::CharacterLiteralExpectedContent: return "CharacterLiteralExpectedContent";
      case Error::CharacterLiteralExpectedEscapeAfterBackslash: return "CharacterLiteralExpectedEscapeAfterBackslash";
      case Error::CharacterLiteralExpectedClosingSingleQuote: return "CharacterLiteralExpectedClosingSingleQuote";
      case Error::CharacterLiteralUnexpectedLineBreak: return "CharacterLiteralUnexpectedLineBreak";
      case Error::StringLiteralExpectedClosingDoubleQuote: return "StringLiteralExpectedClosingDoubleQuote";
      case Error::SpecifierLiteralExpectedClosingDoubleQuote: return "SpecifierLiteralExpectedClosingDoubleQuote";
      case Error::StringLiteralUnexpectedLineBreak: return "StringLiteralUnexpectedLineBreak";
      case Error::StringLiteralExpectedEscapeAfterBackslash: return "StringLiteralExpectedEscapeAfterBackslash";
      case Error::TemplateStringLiteralExpectedExpression: return "TemplateStringLiteralExpectedExpression";
      case Error::TemplateStringLiteralExpectedClosingBrace: return "TemplateStringLiteralExpectedClosingBrace";
      case Error::TemplateStringLiteralExpectedClosingBacktick: return "TemplateStringLiteralExpectedClosingBacktick";
      case Error::UnaryPrefixOperatorExpectedValue: return "UnaryPrefixOperatorExpectedValue";
      case Error::BinaryOperatorExpectedLHS: return "BinaryOperatorExpectedLHS";
      case Error::MultiLineCommentExpectedClosingAsteriskSlash: return "MultiLineCommentExpectedClosingAsteriskSlash";
      case Error::ArrowFunctionExpectedExpression: return "ArrowFunctionExpectedExpression";
      case Error::ArrowFunctionExpectedArrowAfterFunctionHeader: return "ArrowFunctionExpectedArrowAfterFunctionHeader";
      case Error::SpecifierExpectedPathAfterScheme: return "SpecifierExpectedPathAfterScheme";
      case Error::SpecifierExpectedHostAfterAuthority: return "SpecifierExpectedHostAfterAuthority";
      case Error::ConditionExpectedExpression: return "ConditionExpectedExpression";
      case Error::ConditionExpectedClosingParenthesis: return "ConditionExpectedClosingParenthesis";
      case Error::BlockExpectedClosingCurlyBrace: return "BlockExpectedClosingCurlyBrace";
      case Error::SelectorOperatorExpectedSelector: return "SelectorOperatorExpectedSelector";
      case Error::UnaryPrefixOperatorExpectedSelector: return "UnaryPrefixOperatorExpectedSelector";
      case Error::SpecifierExpectedHost: return "SpecifierExpectedHost";
      case Error::SpecifierExpectedAuthority: return "SpecifierExpectedAuthority";
      case Error::SpecifierBadPathAfterAuthority: return "SpecifierBadPathAfterAuthority";
      case Error::SpecifierBadAbsolutePath: return "SpecifierBadAbsolutePath";
      case Error::SpecifierBadRootlessOrEmpty: return "SpecifierBadRootlessOrEmpty";
      case Error::IsPatternExpectedCondition: return "IsPatternExpectedCondition";
      case Error::IsPatternExpectedScopeBlock: return "IsPatternExpectedScopeBlock";
      case Error::HasPatternExpectedCondition: return "HasPatternExpectedCondition";
      case Error::HasPatternExpectedScopeBlock: return "HasPatternExpectedScopeBlock";
      case Error::FromPatternExpectedCondition: return "FromPatternExpectedCondition";
      case Error::FromPatternExpectedScopeBlock: return "FromPatternExpectedScopeBlock";
      case Error::UnicodeCodePointOutOfRange: return "UnicodeCodePointOutOfRange";
      case Error::UnicodeCodePointInvalid: return "UnicodeCodePointInvalid";
      case Error::UnicodeSurrogateCodePointIsNotAValidScalarValue: return "UnicodeSurrogateCodePointIsNotAValidScalarValue";
      case Error::DeclarationExplicitTypeCannotBeUsedWithPostfixUnaryOperators: return "DeclarationExplicitTypeCannotBeUsedWithPostfixUnaryOperators";
      case Error::DeclarationExpectedTypeExpressionAfterColon: return "DeclarationExpectedTypeExpressionAfterColon";
      case Error::DeclarationExpectedExpressionAfterAssignmentOperator: return "DeclarationExpectedExpressionAfterAssignmentOperator";
      case Error::DeclarationCannotSkipAssignmentOperatorWithPostfixUnaryOperators: return "DeclarationCannotSkipAssignmentOperatorWithPostfixUnaryOperators";
      case Error::DeclarationStatementExpectedIdentifier: return "DeclarationStatementExpectedIdentifier";
      case Error::DeclarationStatementExpectedTypeExpressionAfterColon: return "DeclarationStatementExpectedTypeExpressionAfterColon";
      case Error::DeclarationStatementExpectedInitializerExpressionAfterEquals: return "DeclarationStatementExpectedInitializerExpressionAfterEquals";
      case Error::DeclarationStatementExpectedInitializer: return "DeclarationStatementExpectedInitializer";
      case Error::DeclarationStatementExpectedSemicolon: return "DeclarationStatementExpectedSemicolon";
      case Error::DeclarationExpectedIdentifier: return "DeclarationExpectedIdentifier";
      case Error::DeclarationExpectedInitializerExpressionAfterEquals: return "DeclarationExpectedInitializerExpressionAfterEquals";
      case Error::DeclarationPrefixFormCannotHaveInitializer: return "DeclarationPrefixFormCannotHaveInitializer";
      case Error::ExpressionStatementExpectedSemicolon: return "ExpressionStatementExpectedSemicolon";
      case Error::EnumExpectedExpression: return "EnumExpectedExpression";
      case Error::EnumExpectedClosingAngleBracket: return "EnumExpectedClosingAngleBracket";
      case Error::ArrayExpectedExpression: return "ArrayExpectedExpression";
      case Error::ArrayExpectedCountExpression: return "ArrayExpectedCountExpression";
      case Error::ArrayExpectedClosingBracket: return "ArrayExpectedClosingBracket";
      case Error::TupleExpectedExpression: return "TupleExpectedExpression";
      case Error::TupleExpectedClosingParenthesis: return "TupleExpectedClosingParenthesis";
      case Error::ObjectExpectedDeclaration: return "ObjectExpectedDeclaration";
      case Error::ObjectExpectedClosingCurlyBrace: return "ObjectExpectedClosingCurlyBrace";
      case Error::ImportExpectedIdentifier: return "ImportExpectedIdentifier";
      case Error::ImportExpectedFromKeyword: return "ImportExpectedFromKeyword";
      case Error::ImportExpectedSpecifier: return "ImportExpectedSpecifier";
      case Error::ImportExpectedIdentifierAfterIn: return "ImportExpectedIdentifierAfterIn";
      case Error::ImportExpectedSemicolon: return "ImportExpectedSemicolon";
      case Error::RegisterExpectedIdentifier: return "RegisterExpectedIdentifier";
      case Error::RegisterExpectedFromKeyword: return "RegisterExpectedFromKeyword";
      case Error::RegisterExpectedSpecifier: return "RegisterExpectedSpecifier";
      case Error::RegisterExpectedIdentifierAfterWith: return "RegisterExpectedIdentifierAfterWith";
      case Error::RegisterExpectedSemicolon: return "RegisterExpectedSemicolon";
      case Error::DoExpectedScopeBlock: return "DoExpectedScopeBlock";
      case Error::LoopExpectedScopeBlock: return "LoopExpectedScopeBlock";
      case Error::WhileExpectedCondition: return "WhileExpectedCondition";
      case Error::WhileExpectedScopeBlock: return "WhileExpectedScopeBlock";
      case Error::IfExpectedCondition: return "IfExpectedCondition";
      case Error::IfExpectedScopeBlock: return "IfExpectedScopeBlock";
      case Error::IfPatternExpectedCondition: return "IfPatternExpectedCondition";
      case Error::IfPatternExpectedScopeBlock: return "IfPatternExpectedScopeBlock";
      case Error::ElseStatementExpectedScopeBlock: return "ElseStatementExpectedScopeBlock";
      case Error::IfStatementExpectedCondition: return "IfStatementExpectedCondition";
      case Error::IfStatementExpectedScopeBlockOrPatternArms: return "IfStatementExpectedScopeBlockOrPatternArms";
      case Error::TryStatementExpectedScopeBlock: return "TryStatementExpectedScopeBlock";
      case Error::BreakStatementExpectedSemicolon: return "BreakStatementExpectedSemicolon";
      case Error::ContinueStatementExpectedSemicolon: return "ContinueStatementExpectedSemicolon";
      case Error::ReturnStatementExpectedSemicolon: return "ReturnStatementExpectedSemicolon";
      case Error::PanicStatementExpectedSemicolon: return "PanicStatementExpectedSemicolon";
      case Error::YieldStatementExpectedSemicolon: return "YieldStatementExpectedSemicolon";
      case Error::UnaryPrefixOperatorExpectedExpression: return "UnaryPrefixOperatorExpectedExpression";
      case Error::BinaryOperatorExpectedExpression: return "BinaryOperatorExpectedExpression";
      case Error::RepeatExpectedScopeBlock: return "RepeatExpectedScopeBlock";
      case Error::RepeatExpectedWhileKeyword: return "RepeatExpectedWhileKeyword";
      case Error::RepeatExpectedWhileCondition: return "RepeatExpectedWhileCondition";
      case Error::ForExpectedBinding: return "ForExpectedBinding";
      case Error::ForExpectedInKeyword: return "ForExpectedInKeyword";
      case Error::ForExpectedCondition: return "ForExpectedCondition";
      case Error::ForExpectedScopeBlock: return "ForExpectedScopeBlock";
      case Error::ParserExpectedObjectDeclaration: return "ParserExpectedObjectDeclaration";
      case Error::ParserBreakOutsideLoop: return "ParserBreakOutsideLoop";
      case Error::ParserContinueOutsideLoop: return "ParserContinueOutsideLoop";
      case Error::ParserExpectedWhenArmBody: return "ParserExpectedWhenArmBody";
      case Error::ParserDuplicateDefaultInWhen: return "ParserDuplicateDefaultInWhen";
      case Error::ParserExpectedWhenArm: return "ParserExpectedWhenArm";
      case Error::ParserExpectedHexStart: return "ParserExpectedHexStart";
      case Error::ParserExpectedOctalStart: return "ParserExpectedOctalStart";
      case Error::ParserExpectedBinaryStart: return "ParserExpectedBinaryStart";
      case Error::ParserExpectedDecimal: return "ParserExpectedDecimal";
    }
  
    return "Unknown";
  }
}