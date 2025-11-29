import compiler.text.Builder;

import compiler.ir.Token;
import compiler.ir.Error;
import compiler.ir.Opcode;
import compiler.ir.Index;
import compiler.ir.Instruction;
import compiler.input.Lexer;
import compiler.text.Unicode;

import <string>;
import <string_view>;
import <algorithm>;
import <format>;
import <optional>;
import <vector>;
import <initializer_list>;

namespace {
  using compiler::ir::Token;

  constexpr bool IsCharactersToken(Token token) {
    return token >= Token::Characters0 && token <= Token::CharactersF;
  }

  constexpr uint32_t CharactersDigit(Token token) {
    return static_cast<uint32_t>(token) - static_cast<uint32_t>(Token::Characters0);
  }

  constexpr bool IsDigitsToken(Token token) {
    return token >= Token::Digits0 && token <= Token::DigitsF;
  }

  constexpr uint32_t DigitValue(Token token) {
    return static_cast<uint32_t>(token) - static_cast<uint32_t>(Token::Digits0);
  }

  constexpr char HexChar(uint32_t value) {
    return value < 10 ? static_cast<char>('0' + value) : static_cast<char>('A' + (value - 10));
  }

  bool IsEscapeToken(Token token) {
    switch (token) {
      case Token::EscapeASCII:
      case Token::EscapeHex:
      case Token::EscapeUnicode:
      case Token::EscapeUnicodeBraced1:
      case Token::EscapeUnicodeBraced2:
      case Token::EscapeUnicodeBraced3:
      case Token::EscapeUnicodeBraced4:
      case Token::EscapeUnicodeBraced5:
      case Token::EscapeUnicodeBraced6:
      // case Token::EscapePercent:
        return true;
      default:
        return false;
    }
  }

  std::string SanitizeUtf8(std::string_view text) {
    std::string sanitized;
    sanitized.reserve(text.size());

    for (unsigned char ch : text) {
      switch (ch) {
        case '\\': sanitized += "\\\\"; break;
        case '"': sanitized += "\\\""; break;
        case '\n': sanitized += "\\n"; break;
        case '\r': sanitized += "\\r"; break;
        case '\t': sanitized += "\\t"; break;
        case '\v': sanitized += "\\v"; break;
        case '\f': sanitized += "\\f"; break;
        default:
          if (ch < 0x20) {
            sanitized += "\\x";
            sanitized.push_back(HexChar((ch >> 4) & 0xF));
            sanitized.push_back(HexChar(ch & 0xF));
          } else {
            sanitized.push_back(static_cast<char>(ch));
          }
          break;
      }
    }

    return sanitized;
  }
}

namespace compiler::text {
  void Builder::Append(compiler::ir::Token t) {
    switch (t) {
      case compiler::ir::Token::None: output += "None"; return;
      case compiler::ir::Token::Error: output += "Error"; return;
      case compiler::ir::Token::Spaces0: output += "Spaces0"; return;
      case compiler::ir::Token::Spaces1: output += "Spaces1"; return;
      case compiler::ir::Token::Spaces2: output += "Spaces2"; return;
      case compiler::ir::Token::Spaces3: output += "Spaces3"; return;
      case compiler::ir::Token::Spaces4: output += "Spaces4"; return;
      case compiler::ir::Token::Spaces5: output += "Spaces5"; return;
      case compiler::ir::Token::Spaces6: output += "Spaces6"; return;
      case compiler::ir::Token::Spaces7: output += "Spaces7"; return;
      case compiler::ir::Token::Spaces8: output += "Spaces8"; return;
      case compiler::ir::Token::Spaces9: output += "Spaces9"; return;
      case compiler::ir::Token::SpacesA: output += "SpacesA"; return;
      case compiler::ir::Token::SpacesB: output += "SpacesB"; return;
      case compiler::ir::Token::SpacesC: output += "SpacesC"; return;
      case compiler::ir::Token::SpacesD: output += "SpacesD"; return;
      case compiler::ir::Token::SpacesE: output += "SpacesE"; return;
      case compiler::ir::Token::SpacesF: output += "SpacesF"; return;
      case compiler::ir::Token::Tabs0: output += "Tabs0"; return;
      case compiler::ir::Token::Tabs1: output += "Tabs1"; return;
      case compiler::ir::Token::Tabs2: output += "Tabs2"; return;
      case compiler::ir::Token::Tabs3: output += "Tabs3"; return;
      case compiler::ir::Token::Tabs4: output += "Tabs4"; return;
      case compiler::ir::Token::Tabs5: output += "Tabs5"; return;
      case compiler::ir::Token::Tabs6: output += "Tabs6"; return;
      case compiler::ir::Token::Tabs7: output += "Tabs7"; return;
      case compiler::ir::Token::Tabs8: output += "Tabs8"; return;
      case compiler::ir::Token::Tabs9: output += "Tabs9"; return;
      case compiler::ir::Token::TabsA: output += "TabsA"; return;
      case compiler::ir::Token::TabsB: output += "TabsB"; return;
      case compiler::ir::Token::TabsC: output += "TabsC"; return;
      case compiler::ir::Token::TabsD: output += "TabsD"; return;
      case compiler::ir::Token::TabsE: output += "TabsE"; return;
      case compiler::ir::Token::TabsF: output += "TabsF"; return;
      case compiler::ir::Token::Digits0: output += "Digits0"; return;
      case compiler::ir::Token::Digits1: output += "Digits1"; return;
      case compiler::ir::Token::Digits2: output += "Digits2"; return;
      case compiler::ir::Token::Digits3: output += "Digits3"; return;
      case compiler::ir::Token::Digits4: output += "Digits4"; return;
      case compiler::ir::Token::Digits5: output += "Digits5"; return;
      case compiler::ir::Token::Digits6: output += "Digits6"; return;
      case compiler::ir::Token::Digits7: output += "Digits7"; return;
      case compiler::ir::Token::Digits8: output += "Digits8"; return;
      case compiler::ir::Token::Digits9: output += "Digits9"; return;
      case compiler::ir::Token::DigitsA: output += "DigitsA"; return;
      case compiler::ir::Token::DigitsB: output += "DigitsB"; return;
      case compiler::ir::Token::DigitsC: output += "DigitsC"; return;
      case compiler::ir::Token::DigitsD: output += "DigitsD"; return;
      case compiler::ir::Token::DigitsE: output += "DigitsE"; return;
      case compiler::ir::Token::DigitsF: output += "DigitsF"; return;
      case compiler::ir::Token::Characters0: output += "Characters0"; return;
      case compiler::ir::Token::Characters1: output += "Characters1"; return;
      case compiler::ir::Token::Characters2: output += "Characters2"; return;
      case compiler::ir::Token::Characters3: output += "Characters3"; return;
      case compiler::ir::Token::Characters4: output += "Characters4"; return;
      case compiler::ir::Token::Characters5: output += "Characters5"; return;
      case compiler::ir::Token::Characters6: output += "Characters6"; return;
      case compiler::ir::Token::Characters7: output += "Characters7"; return;
      case compiler::ir::Token::Characters8: output += "Characters8"; return;
      case compiler::ir::Token::Characters9: output += "Characters9"; return;
      case compiler::ir::Token::CharactersA: output += "CharactersA"; return;
      case compiler::ir::Token::CharactersB: output += "CharactersB"; return;
      case compiler::ir::Token::CharactersC: output += "CharactersC"; return;
      case compiler::ir::Token::CharactersD: output += "CharactersD"; return;
      case compiler::ir::Token::CharactersE: output += "CharactersE"; return;
      case compiler::ir::Token::CharactersF: output += "CharactersF"; return;
      case compiler::ir::Token::Redirect0: output += "Redirect0"; return;
      case compiler::ir::Token::Redirect1: output += "Redirect1"; return;
      case compiler::ir::Token::Redirect2: output += "Redirect2"; return;
      case compiler::ir::Token::Redirect3: output += "Redirect3"; return;
      case compiler::ir::Token::Redirect4: output += "Redirect4"; return;
      case compiler::ir::Token::Redirect5: output += "Redirect5"; return;
      case compiler::ir::Token::Redirect6: output += "Redirect6"; return;
      case compiler::ir::Token::Redirect7: output += "Redirect7"; return;
      case compiler::ir::Token::EscapeASCII: output += "EscapeASCII"; return;
      case compiler::ir::Token::EscapeHex: output += "EscapeHex"; return;
      case compiler::ir::Token::EscapeUnicode: output += "EscapeUnicode"; return;
      case compiler::ir::Token::EscapeUnicodeBraced1: output += "EscapeUnicodeBraced1"; return;
      case compiler::ir::Token::EscapeUnicodeBraced2: output += "EscapeUnicodeBraced2"; return;
      case compiler::ir::Token::EscapeUnicodeBraced3: output += "EscapeUnicodeBraced3"; return;
      case compiler::ir::Token::EscapeUnicodeBraced4: output += "EscapeUnicodeBraced4"; return;
      case compiler::ir::Token::EscapeUnicodeBraced5: output += "EscapeUnicodeBraced5"; return;
      case compiler::ir::Token::EscapeUnicodeBraced6: output += "EscapeUnicodeBraced6"; return;
      case compiler::ir::Token::LineFeed: output += "LineFeed"; return;
      case compiler::ir::Token::CarriageReturnLineFeed: output += "CarriageReturnLineFeed"; return;
      case compiler::ir::Token::CarriageReturn: output += "CarriageReturn"; return;
      case compiler::ir::Token::LineSeparator: output += "LineSeparator"; return;
      case compiler::ir::Token::ParagraphSeparator: output += "ParagraphSeparator"; return;
      case compiler::ir::Token::VerticalTab: output += "VerticalTab"; return;
      case compiler::ir::Token::FormFeed: output += "FormFeed"; return;
      case compiler::ir::Token::Underscore: output += "Underscore"; return;
      case compiler::ir::Token::Dot: output += "Dot"; return;
      case compiler::ir::Token::Exponent: output += "Exponent"; return;
      case compiler::ir::Token::Power: output += "Power"; return;
      case compiler::ir::Token::Mantissa: output += "Mantissa"; return;
      case compiler::ir::Token::HexStart: output += "HexStart"; return;
      case compiler::ir::Token::OctalStart: output += "OctalStart"; return;
      case compiler::ir::Token::BinaryStart: output += "BinaryStart"; return;
      case compiler::ir::Token::Plus: output += "Plus"; return;
      case compiler::ir::Token::Minus: output += "Minus"; return;
      case compiler::ir::Token::CommentOpen: output += "CommentOpen"; return;
      case compiler::ir::Token::CommentClose: output += "CommentClose"; return;
      case compiler::ir::Token::MultiLineCommentOpen: output += "MultiLineCommentOpen"; return;
      case compiler::ir::Token::MultiLineCommentClose: output += "MultiLineCommentClose"; return;
      case compiler::ir::Token::ArrayOpen: output += "ArrayOpen"; return;
      case compiler::ir::Token::ArrayClose: output += "ArrayClose"; return;
      case compiler::ir::Token::EnumOpen: output += "EnumOpen"; return;
      case compiler::ir::Token::EnumClose: output += "EnumClose"; return;
      case compiler::ir::Token::ObjectOpen: output += "ObjectOpen"; return;
      case compiler::ir::Token::ObjectClose: output += "ObjectClose"; return;
      case compiler::ir::Token::TupleOpen: output += "TupleOpen"; return;
      case compiler::ir::Token::TupleClose: output += "TupleClose"; return;
      case compiler::ir::Token::CharOpen: output += "CharOpen"; return;
      case compiler::ir::Token::CharClose: output += "CharClose"; return;
      case compiler::ir::Token::Comma: output += "Comma"; return;
      case compiler::ir::Token::ConditionOpen: output += "ConditionOpen"; return;
      case compiler::ir::Token::ConditionClose: output += "ConditionClose"; return;
      case compiler::ir::Token::Pipeline: output += "Pipeline"; return;
      case compiler::ir::Token::CaptureOpen: output += "CaptureOpen"; return;
      case compiler::ir::Token::CaptureClose: output += "CaptureClose"; return;
      case compiler::ir::Token::ParameterOpen: output += "ParameterOpen"; return;
      case compiler::ir::Token::ParameterClose: output += "ParameterClose"; return;
      case compiler::ir::Token::ScopeOpen: output += "ScopeOpen"; return;
      case compiler::ir::Token::ScopeClose: output += "ScopeClose"; return;
      case compiler::ir::Token::Semicolon: output += "Semicolon"; return;
      case compiler::ir::Token::TemplateStringOpen: output += "TemplateStringOpen"; return;
      case compiler::ir::Token::TemplateStringClose: output += "TemplateStringClose"; return;
      case compiler::ir::Token::TemplateStringExpressionOpen: output += "TemplateStringExpressionOpen"; return;
      case compiler::ir::Token::TemplateStringExpressionClose: output += "TemplateStringExpressionClose"; return;
      case compiler::ir::Token::StringOpen: output += "StringOpen"; return;
      case compiler::ir::Token::StringClose: output += "StringClose"; return;
      case compiler::ir::Token::Compiler: output += "Compiler"; return;
      case compiler::ir::Token::Auto: output += "Auto"; return;
      case compiler::ir::Token::Void: output += "Void"; return;
      case compiler::ir::Token::Undefined: output += "Undefined"; return;
      case compiler::ir::Token::Null: output += "Null"; return;
      case compiler::ir::Token::Readonly: output += "Readonly"; return;
      case compiler::ir::Token::True: output += "True"; return;
      case compiler::ir::Token::False: output += "False"; return;
      case compiler::ir::Token::This: output += "This"; return;
      case compiler::ir::Token::That: output += "That"; return;
      case compiler::ir::Token::TypeStart: output += "TypeStart"; return;
      case compiler::ir::Token::Wildcard: output += "Wildcard"; return;
      case compiler::ir::Token::Add: output += "Add"; return;
      case compiler::ir::Token::And: output += "And"; return;
      case compiler::ir::Token::Or: output += "Or"; return;
      case compiler::ir::Token::Unwrap: output += "Unwrap"; return;
      case compiler::ir::Token::Arrow: output += "Arrow"; return;
      case compiler::ir::Token::ArrowHead: output += "ArrowHead"; return;
      case compiler::ir::Token::Divide: output += "Divide"; return;
      case compiler::ir::Token::Equal: output += "Equal"; return;
      case compiler::ir::Token::ExponentOperator: output += "ExponentOperator"; return;
      case compiler::ir::Token::GreaterOrEqual: output += "GreaterOrEqual"; return;
      case compiler::ir::Token::Greater: output += "Greater"; return;
      case compiler::ir::Token::ExclusiveRange: output += "ExclusiveRange"; return;
      case compiler::ir::Token::InclusiveRange: output += "InclusiveRange"; return;
      case compiler::ir::Token::LesserOrEqual: output += "LesserOrEqual"; return;
      case compiler::ir::Token::Lesser: output += "Lesser"; return;
      case compiler::ir::Token::MemberAccess: output += "MemberAccess"; return;
      case compiler::ir::Token::MemberAccessOptional: output += "MemberAccessOptional"; return;
      case compiler::ir::Token::MemberAccessStatic: output += "MemberAccessStatic"; return;
      case compiler::ir::Token::MemberAccessStaticOptional: output += "MemberAccessStaticOptional"; return;
      case compiler::ir::Token::Modulo: output += "Modulo"; return;
      case compiler::ir::Token::Multiply: output += "Multiply"; return;
      case compiler::ir::Token::NotEqual: output += "NotEqual"; return;
      case compiler::ir::Token::Subtract: output += "Subtract"; return;
      case compiler::ir::Token::TripleLeftShift: output += "TripleLeftShift"; return;
      case compiler::ir::Token::TripleRightShift: output += "TripleRightShift"; return;
      case compiler::ir::Token::AssignOptional: output += "AssignOptional"; return;
      case compiler::ir::Token::AssertEqual: output += "AssertEqual"; return;
      case compiler::ir::Token::AssertGreaterOrEqual: output += "AssertGreaterOrEqual"; return;
      case compiler::ir::Token::AssertLesserOrEqual: output += "AssertLesserOrEqual"; return;
      case compiler::ir::Token::AssertNotEqual: output += "AssertNotEqual"; return;
      case compiler::ir::Token::Assign: output += "Assign"; return;
      case compiler::ir::Token::AssignAdd: output += "AssignAdd"; return;
      case compiler::ir::Token::AssignDivide: output += "AssignDivide"; return;
      case compiler::ir::Token::AssignExponent: output += "AssignExponent"; return;
      case compiler::ir::Token::AssignModulo: output += "AssignModulo"; return;
      case compiler::ir::Token::AssignMultiply: output += "AssignMultiply"; return;
      case compiler::ir::Token::AssignSubtract: output += "AssignSubtract"; return;
      case compiler::ir::Token::AssignAnd: output += "AssignAnd"; return;
      case compiler::ir::Token::AssignOr: output += "AssignOr"; return;
      case compiler::ir::Token::AssignTruthyAnd: output += "AssignTruthyAnd"; return;
      case compiler::ir::Token::AssignTruthyOr: output += "AssignTruthyOr"; return;
      case compiler::ir::Token::BitwiseLeftShift: output += "BitwiseLeftShift"; return;
      case compiler::ir::Token::BitwiseRightShift: output += "BitwiseRightShift"; return;
      case compiler::ir::Token::BitwiseTripleLeftShift: output += "BitwiseTripleLeftShift"; return;
      case compiler::ir::Token::BitwiseTripleRightShift: output += "BitwiseTripleRightShift"; return;
      case compiler::ir::Token::BitwiseAssignTripleLeftShift: output += "BitwiseAssignTripleLeftShift"; return;
      case compiler::ir::Token::BitwiseAssignTripleRightShift: output += "BitwiseAssignTripleRightShift"; return;
      case compiler::ir::Token::BitwiseAssignLeftShift: output += "BitwiseAssignLeftShift"; return;
      case compiler::ir::Token::BitwiseAssignRightShift: output += "BitwiseAssignRightShift"; return;
      case compiler::ir::Token::BitwiseAnd: output += "BitwiseAnd"; return;
      case compiler::ir::Token::BitwiseXor: output += "BitwiseXor"; return;
      case compiler::ir::Token::BitwiseOr: output += "BitwiseOr"; return;
      case compiler::ir::Token::BitwiseAssignAnd: output += "BitwiseAssignAnd"; return;
      case compiler::ir::Token::BitwiseAssignXor: output += "BitwiseAssignXor"; return;
      case compiler::ir::Token::BitwiseAssignOr: output += "BitwiseAssignOr"; return;
      case compiler::ir::Token::Yield: output += "Yield"; return;
      case compiler::ir::Token::Await: output += "Await"; return;
      case compiler::ir::Token::Async: output += "Async"; return;
      case compiler::ir::Token::Expect: output += "Expect"; return;
      case compiler::ir::Token::Copy: output += "Copy"; return;
      case compiler::ir::Token::Counted: output += "Counted"; return;
      case compiler::ir::Token::Symbol: output += "Symbol"; return;
      case compiler::ir::Token::Reference: output += "Reference"; return;
      case compiler::ir::Token::MutableReference: output += "MutableReference"; return;
      case compiler::ir::Token::Spread: output += "Spread"; return;
      case compiler::ir::Token::Decrement: output += "Decrement"; return;
      case compiler::ir::Token::Increment: output += "Increment"; return;
      case compiler::ir::Token::Negative: output += "Negative"; return;
      case compiler::ir::Token::Positive: output += "Positive"; return;
      case compiler::ir::Token::Not: output += "Not"; return;
      case compiler::ir::Token::BitwiseNot: output += "BitwiseNot"; return;
      case compiler::ir::Token::To: output += "To"; return;
      case compiler::ir::Token::Do: output += "Do"; return;
      case compiler::ir::Token::Else: output += "Else"; return;
      case compiler::ir::Token::Register: output += "Register"; return;
      case compiler::ir::Token::With: output += "With"; return;
      case compiler::ir::Token::Import: output += "Import"; return;
      case compiler::ir::Token::From: output += "From"; return;
      case compiler::ir::Token::If: output += "If"; return;
      case compiler::ir::Token::Try: output += "Try"; return;
      case compiler::ir::Token::Return: output += "Return"; return;
      case compiler::ir::Token::Panic: output += "Panic"; return;
      case compiler::ir::Token::When: output += "When"; return;
      case compiler::ir::Token::Is: output += "Is"; return;
      case compiler::ir::Token::Has: output += "Has"; return;
      case compiler::ir::Token::Export: output += "Export"; return;
      case compiler::ir::Token::For: output += "For"; return;
      case compiler::ir::Token::While: output += "While"; return;
      case compiler::ir::Token::Loop: output += "Loop"; return;
      case compiler::ir::Token::Repeat: output += "Repeat"; return;
      case compiler::ir::Token::In: output += "In"; return;
      case compiler::ir::Token::Break: output += "Break"; return;
      case compiler::ir::Token::Continue: output += "Continue"; return;
      case compiler::ir::Token::Default: output += "Default"; return;
      case compiler::ir::Token::Case: output += "Case"; return;
      case compiler::ir::Token::Let: output += "Let"; return;
      case compiler::ir::Token::Const: output += "Const"; return;
      case compiler::ir::Token::Drive: output += "Drive"; return;
      case compiler::ir::Token::Scheme: output += "Scheme"; return;
      case compiler::ir::Token::Authority: output += "Authority"; return;
      case compiler::ir::Token::Username: output += "Username"; return;
      case compiler::ir::Token::Password: output += "Password"; return;
      case compiler::ir::Token::Hostname: output += "Hostname"; return;
      case compiler::ir::Token::IPv4: output += "IPv4"; return;
      case compiler::ir::Token::IPv6: output += "IPv6"; return;
      case compiler::ir::Token::Path: output += "Path"; return;
      case compiler::ir::Token::Port: output += "Port"; return;
      case compiler::ir::Token::QueryKey: output += "QueryKey"; return;
      case compiler::ir::Token::QueryValue: output += "QueryValue"; return;
      case compiler::ir::Token::Fragment: output += "Fragment"; return;
      case compiler::ir::Token::EscapePercent: output += "EscapePercent"; return;
    }
  
    output += "Unknown";
  }

  void Builder::Append(compiler::ir::Error e) {
    switch (e) {
      case compiler::ir::Error::EscapeSequenceUnexpectedEndOfInput: output += "EscapeSequenceUnexpectedEndOfInput"; return;
      case compiler::ir::Error::EscapeSequenceExpectedClosingBrace: output += "EscapeSequenceExpectedClosingBrace"; return;
      case compiler::ir::Error::EscapeSequenceExpectedTwoHexDigits: output += "EscapeSequenceExpectedTwoHexDigits"; return;
      case compiler::ir::Error::EscapeSequenceExpectedFourHexDigits: output += "EscapeSequenceExpectedFourHexDigits"; return;
      case compiler::ir::Error::EscapeSequenceExpectedOneOrMoreHexDigits: output += "EscapeSequenceExpectedOneOrMoreHexDigits"; return;
      case compiler::ir::Error::EscapeSequenceExpectedSixOrFewerHexDigits: output += "EscapeSequenceExpectedSixOrFewerHexDigits"; return;
      case compiler::ir::Error::EscapeSequenceUnexpectedEnd: output += "EscapeSequenceUnexpectedEnd"; return;
      case compiler::ir::Error::EscapeSequenceExpectedAfterBackslash: output += "EscapeSequenceExpectedAfterBackslash"; return;
      case compiler::ir::Error::EscapeInvalidPercentEncoding: output += "EscapeInvalidPercentEncoding"; return;
      case compiler::ir::Error::CharacterLiteralExpectedContent: output += "CharacterLiteralExpectedContent"; return;
      case compiler::ir::Error::CharacterLiteralExpectedEscapeAfterBackslash: output += "CharacterLiteralExpectedEscapeAfterBackslash"; return;
      case compiler::ir::Error::CharacterLiteralExpectedClosingSingleQuote: output += "CharacterLiteralExpectedClosingSingleQuote"; return;
      case compiler::ir::Error::CharacterLiteralUnexpectedLineBreak: output += "CharacterLiteralUnexpectedLineBreak"; return;
      case compiler::ir::Error::StringLiteralExpectedClosingDoubleQuote: output += "StringLiteralExpectedClosingDoubleQuote"; return;
      case compiler::ir::Error::SpecifierLiteralExpectedClosingDoubleQuote: output += "SpecifierLiteralExpectedClosingDoubleQuote"; return;
      case compiler::ir::Error::StringLiteralUnexpectedLineBreak: output += "StringLiteralUnexpectedLineBreak"; return;
      case compiler::ir::Error::StringLiteralExpectedEscapeAfterBackslash: output += "StringLiteralExpectedEscapeAfterBackslash"; return;
      case compiler::ir::Error::TemplateStringLiteralExpectedExpression: output += "TemplateStringLiteralExpectedExpression"; return;
      case compiler::ir::Error::TemplateStringLiteralExpectedClosingBrace: output += "TemplateStringLiteralExpectedClosingBrace"; return;
      case compiler::ir::Error::TemplateStringLiteralExpectedClosingBacktick: output += "TemplateStringLiteralExpectedClosingBacktick"; return;
      case compiler::ir::Error::UnaryPrefixOperatorExpectedValue: output += "UnaryPrefixOperatorExpectedValue"; return;
      case compiler::ir::Error::BinaryOperatorExpectedLHS: output += "BinaryOperatorExpectedLHS"; return;
      case compiler::ir::Error::MultiLineCommentExpectedClosingAsteriskSlash: output += "MultiLineCommentExpectedClosingAsteriskSlash"; return;
      case compiler::ir::Error::ArrowFunctionExpectedExpression: output += "ArrowFunctionExpectedExpression"; return;
      case compiler::ir::Error::ArrowFunctionExpectedArrowAfterFunctionHeader: output += "ArrowFunctionExpectedArrowAfterFunctionHeader"; return;
      case compiler::ir::Error::SpecifierExpectedPathAfterScheme: output += "SpecifierExpectedPathAfterScheme"; return;
      case compiler::ir::Error::SpecifierExpectedHostAfterAuthority: output += "SpecifierExpectedHostAfterAuthority"; return;
      case compiler::ir::Error::ConditionExpectedExpression: output += "ConditionExpectedExpression"; return;
      case compiler::ir::Error::ConditionExpectedClosingParenthesis: output += "ConditionExpectedClosingParenthesis"; return;
      case compiler::ir::Error::BlockExpectedClosingCurlyBrace: output += "BlockExpectedClosingCurlyBrace"; return;
      case compiler::ir::Error::SelectorOperatorExpectedSelector: output += "SelectorOperatorExpectedSelector"; return;
      case compiler::ir::Error::UnaryPrefixOperatorExpectedSelector: output += "UnaryPrefixOperatorExpectedSelector"; return;
      case compiler::ir::Error::SpecifierExpectedHost: output += "SpecifierExpectedHost"; return;
      case compiler::ir::Error::SpecifierExpectedAuthority: output += "SpecifierExpectedAuthority"; return;
      case compiler::ir::Error::SpecifierBadPathAfterAuthority: output += "SpecifierBadPathAfterAuthority"; return;
      case compiler::ir::Error::SpecifierBadAbsolutePath: output += "SpecifierBadAbsolutePath"; return;
      case compiler::ir::Error::SpecifierBadRootlessOrEmpty: output += "SpecifierBadRootlessOrEmpty"; return;
      case compiler::ir::Error::IsPatternExpectedCondition: output += "IsPatternExpectedCondition"; return;
      case compiler::ir::Error::IsPatternExpectedScopeBlock: output += "IsPatternExpectedScopeBlock"; return;
      case compiler::ir::Error::HasPatternExpectedCondition: output += "HasPatternExpectedCondition"; return;
      case compiler::ir::Error::HasPatternExpectedScopeBlock: output += "HasPatternExpectedScopeBlock"; return;
      case compiler::ir::Error::FromPatternExpectedCondition: output += "FromPatternExpectedCondition"; return;
      case compiler::ir::Error::FromPatternExpectedScopeBlock: output += "FromPatternExpectedScopeBlock"; return;
      case compiler::ir::Error::UnicodeCodePointOutOfRange: output += "UnicodeCodePointOutOfRange"; return;
      case compiler::ir::Error::UnicodeCodePointInvalid: output += "UnicodeCodePointInvalid"; return;
      case compiler::ir::Error::UnicodeSurrogateCodePointIsNotAValidScalarValue: output += "UnicodeSurrogateCodePointIsNotAValidScalarValue"; return;
      case compiler::ir::Error::DeclarationExplicitTypeCannotBeUsedWithPostfixUnaryOperators: output += "DeclarationExplicitTypeCannotBeUsedWithPostfixUnaryOperators"; return;
      case compiler::ir::Error::DeclarationExpectedTypeExpressionAfterColon: output += "DeclarationExpectedTypeExpressionAfterColon"; return;
      case compiler::ir::Error::DeclarationExpectedExpressionAfterAssignmentOperator: output += "DeclarationExpectedExpressionAfterAssignmentOperator"; return;
      case compiler::ir::Error::DeclarationCannotSkipAssignmentOperatorWithPostfixUnaryOperators: output += "DeclarationCannotSkipAssignmentOperatorWithPostfixUnaryOperators"; return;
      case compiler::ir::Error::DeclarationStatementExpectedIdentifier: output += "DeclarationStatementExpectedIdentifier"; return;
      case compiler::ir::Error::DeclarationStatementExpectedTypeExpressionAfterColon: output += "DeclarationStatementExpectedTypeExpressionAfterColon"; return;
      case compiler::ir::Error::DeclarationStatementExpectedInitializerExpressionAfterEquals: output += "DeclarationStatementExpectedInitializerExpressionAfterEquals"; return;
      case compiler::ir::Error::DeclarationStatementExpectedInitializer: output += "DeclarationStatementExpectedInitializer"; return;
      case compiler::ir::Error::DeclarationStatementExpectedSemicolon: output += "DeclarationStatementExpectedSemicolon"; return;
      case compiler::ir::Error::DeclarationExpectedIdentifier: output += "DeclarationExpectedIdentifier"; return;
      case compiler::ir::Error::DeclarationExpectedInitializerExpressionAfterEquals: output += "DeclarationExpectedInitializerExpressionAfterEquals"; return;
      case compiler::ir::Error::DeclarationPrefixFormCannotHaveInitializer: output += "DeclarationPrefixFormCannotHaveInitializer"; return;
      case compiler::ir::Error::ExpressionStatementExpectedSemicolon: output += "ExpressionStatementExpectedSemicolon"; return;
      case compiler::ir::Error::EnumExpectedExpression: output += "EnumExpectedExpression"; return;
      case compiler::ir::Error::EnumExpectedClosingAngleBracket: output += "EnumExpectedClosingAngleBracket"; return;
      case compiler::ir::Error::ArrayExpectedExpression: output += "ArrayExpectedExpression"; return;
      case compiler::ir::Error::ArrayExpectedCountExpression: output += "ArrayExpectedCountExpression"; return;
      case compiler::ir::Error::ArrayExpectedClosingBracket: output += "ArrayExpectedClosingBracket"; return;
      case compiler::ir::Error::TupleExpectedExpression: output += "TupleExpectedExpression"; return;
      case compiler::ir::Error::TupleExpectedClosingParenthesis: output += "TupleExpectedClosingParenthesis"; return;
      case compiler::ir::Error::ObjectExpectedDeclaration: output += "ObjectExpectedDeclaration"; return;
      case compiler::ir::Error::ObjectExpectedClosingCurlyBrace: output += "ObjectExpectedClosingCurlyBrace"; return;
      case compiler::ir::Error::ImportExpectedIdentifier: output += "ImportExpectedIdentifier"; return;
      case compiler::ir::Error::ImportExpectedFromKeyword: output += "ImportExpectedFromKeyword"; return;
      case compiler::ir::Error::ImportExpectedSpecifier: output += "ImportExpectedSpecifier"; return;
      case compiler::ir::Error::ImportExpectedIdentifierAfterIn: output += "ImportExpectedIdentifierAfterIn"; return;
      case compiler::ir::Error::ImportExpectedSemicolon: output += "ImportExpectedSemicolon"; return;
      case compiler::ir::Error::RegisterExpectedIdentifier: output += "RegisterExpectedIdentifier"; return;
      case compiler::ir::Error::RegisterExpectedFromKeyword: output += "RegisterExpectedFromKeyword"; return;
      case compiler::ir::Error::RegisterExpectedSpecifier: output += "RegisterExpectedSpecifier"; return;
      case compiler::ir::Error::RegisterExpectedIdentifierAfterWith: output += "RegisterExpectedIdentifierAfterWith"; return;
      case compiler::ir::Error::RegisterExpectedSemicolon: output += "RegisterExpectedSemicolon"; return;
      case compiler::ir::Error::DoExpectedScopeBlock: output += "DoExpectedScopeBlock"; return;
      case compiler::ir::Error::LoopExpectedScopeBlock: output += "LoopExpectedScopeBlock"; return;
      case compiler::ir::Error::WhileExpectedCondition: output += "WhileExpectedCondition"; return;
      case compiler::ir::Error::WhileExpectedScopeBlock: output += "WhileExpectedScopeBlock"; return;
      case compiler::ir::Error::IfExpectedCondition: output += "IfExpectedCondition"; return;
      case compiler::ir::Error::IfExpectedScopeBlock: output += "IfExpectedScopeBlock"; return;
      case compiler::ir::Error::IfPatternExpectedCondition: output += "IfPatternExpectedCondition"; return;
      case compiler::ir::Error::IfPatternExpectedScopeBlock: output += "IfPatternExpectedScopeBlock"; return;
      case compiler::ir::Error::ElseStatementExpectedScopeBlock: output += "ElseStatementExpectedScopeBlock"; return;
      case compiler::ir::Error::IfStatementExpectedCondition: output += "IfStatementExpectedCondition"; return;
      case compiler::ir::Error::IfStatementExpectedScopeBlockOrPatternArms: output += "IfStatementExpectedScopeBlockOrPatternArms"; return;
      case compiler::ir::Error::TryStatementExpectedScopeBlock: output += "TryStatementExpectedScopeBlock"; return;
      case compiler::ir::Error::BreakStatementExpectedSemicolon: output += "BreakStatementExpectedSemicolon"; return;
      case compiler::ir::Error::ContinueStatementExpectedSemicolon: output += "ContinueStatementExpectedSemicolon"; return;
      case compiler::ir::Error::ReturnStatementExpectedSemicolon: output += "ReturnStatementExpectedSemicolon"; return;
      case compiler::ir::Error::PanicStatementExpectedSemicolon: output += "PanicStatementExpectedSemicolon"; return;
      case compiler::ir::Error::YieldStatementExpectedSemicolon: output += "YieldStatementExpectedSemicolon"; return;
      case compiler::ir::Error::UnaryPrefixOperatorExpectedExpression: output += "UnaryPrefixOperatorExpectedExpression"; return;
      case compiler::ir::Error::BinaryOperatorExpectedExpression: output += "BinaryOperatorExpectedExpression"; return;
      case compiler::ir::Error::RepeatExpectedScopeBlock: output += "RepeatExpectedScopeBlock"; return;
      case compiler::ir::Error::RepeatExpectedWhileKeyword: output += "RepeatExpectedWhileKeyword"; return;
      case compiler::ir::Error::RepeatExpectedWhileCondition: output += "RepeatExpectedWhileCondition"; return;
      case compiler::ir::Error::ForExpectedBinding: output += "ForExpectedBinding"; return;
      case compiler::ir::Error::ForExpectedInKeyword: output += "ForExpectedInKeyword"; return;
      case compiler::ir::Error::ForExpectedCondition: output += "ForExpectedCondition"; return;
      case compiler::ir::Error::ForExpectedScopeBlock: output += "ForExpectedScopeBlock"; return;
      case compiler::ir::Error::ParserExpectedObjectDeclaration: output += "ParserExpectedObjectDeclaration"; return;
      case compiler::ir::Error::ParserBreakOutsideLoop: output += "ParserBreakOutsideLoop"; return;
      case compiler::ir::Error::ParserContinueOutsideLoop: output += "ParserContinueOutsideLoop"; return;
      case compiler::ir::Error::ParserExpectedWhenArmBody: output += "ParserExpectedWhenArmBody"; return;
      case compiler::ir::Error::ParserDuplicateDefaultInWhen: output += "ParserDuplicateDefaultInWhen"; return;
      case compiler::ir::Error::ParserExpectedWhenArm: output += "ParserExpectedWhenArm"; return;
      case compiler::ir::Error::ParserExpectedHexStart: output += "ParserExpectedHexStart"; return;
      case compiler::ir::Error::ParserExpectedOctalStart: output += "ParserExpectedOctalStart"; return;
      case compiler::ir::Error::ParserExpectedBinaryStart: output += "ParserExpectedBinaryStart"; return;
      case compiler::ir::Error::ParserExpectedDecimal: output += "ParserExpectedDecimal"; return;
    }

    output += "Unknown";
  }

  void Builder::Append(compiler::ir::Opcode opcode) {
    switch (opcode) {
      case compiler::ir::Opcode::Extension: output += "Extension"; return;
      case compiler::ir::Opcode::Anchor: output += "Anchor"; return;
      case compiler::ir::Opcode::Label: output += "Label"; return;
      case compiler::ir::Opcode::NoOperation: output += "NoOperation"; return;
      case compiler::ir::Opcode::Unreachable: output += "Unreachable"; return;
      case compiler::ir::Opcode::Abort: output += "Abort"; return;
      case compiler::ir::Opcode::Panic: output += "Panic"; return;
      case compiler::ir::Opcode::Assume: output += "Assume"; return;
      case compiler::ir::Opcode::Call: output += "Call"; return;
      case compiler::ir::Opcode::CallWithArray: output += "CallWithArray"; return;
      case compiler::ir::Opcode::CallWithTuple: output += "CallWithTuple"; return;
      case compiler::ir::Opcode::CallWithObject: output += "CallWithObject"; return;
      case compiler::ir::Opcode::CallWithEnum: output += "CallWithEnum"; return;
      case compiler::ir::Opcode::CallWithString: output += "CallWithString"; return;
      case compiler::ir::Opcode::CallWithTemplateString: output += "CallWithTemplateString"; return;
      case compiler::ir::Opcode::Return: output += "Return"; return;
      case compiler::ir::Opcode::Jump: output += "Jump"; return;
      case compiler::ir::Opcode::Branch: output += "Branch"; return;
      case compiler::ir::Opcode::Fork: output += "Fork"; return;
      case compiler::ir::Opcode::BranchIs: output += "BranchIs"; return;
      case compiler::ir::Opcode::BranchHas: output += "BranchHas"; return;
      case compiler::ir::Opcode::BranchFrom: output += "BranchFrom"; return;
      case compiler::ir::Opcode::BranchNotIs: output += "BranchNotIs"; return;
      case compiler::ir::Opcode::BranchNotHas: output += "BranchNotHas"; return;
      case compiler::ir::Opcode::BranchNotFrom: output += "BranchNotFrom"; return;
      case compiler::ir::Opcode::Allocate: output += "Allocate"; return;
      case compiler::ir::Opcode::Deallocate: output += "Deallocate"; return;
      case compiler::ir::Opcode::Move: output += "Move"; return;
      case compiler::ir::Opcode::Load: output += "Load"; return;
      case compiler::ir::Opcode::Store: output += "Store"; return;
      case compiler::ir::Opcode::Index: output += "Index"; return;
      case compiler::ir::Opcode::Select: output += "Select"; return;
      case compiler::ir::Opcode::Spill: output += "Spill"; return;
      case compiler::ir::Opcode::Restore: output += "Restore"; return;
      case compiler::ir::Opcode::Construct: output += "Construct"; return;
      case compiler::ir::Opcode::Destruct: output += "Destruct"; return;
      case compiler::ir::Opcode::AddressOf: output += "AddressOf"; return;
      case compiler::ir::Opcode::SymbolOf: output += "SymbolOf"; return;
      case compiler::ir::Opcode::CopyOf: output += "CopyOf"; return;
      case compiler::ir::Opcode::Exchange: output += "Exchange"; return;
      case compiler::ir::Opcode::CompareAndExchange: output += "CompareAndExchange"; return;
      case compiler::ir::Opcode::Fence: output += "Fence"; return;
      case compiler::ir::Opcode::Barrier: output += "Barrier"; return;
      case compiler::ir::Opcode::Spread: output += "Spread"; return;
      case compiler::ir::Opcode::Add: output += "Add"; return;
      case compiler::ir::Opcode::AddFloat: output += "AddFloat"; return;
      case compiler::ir::Opcode::AddWithCarry: output += "AddWithCarry"; return;
      case compiler::ir::Opcode::AddExchange: output += "AddExchange"; return;
      case compiler::ir::Opcode::Subtract: output += "Subtract"; return;
      case compiler::ir::Opcode::SubtractFloat: output += "SubtractFloat"; return;
      case compiler::ir::Opcode::SubtractWithBorrow: output += "SubtractWithBorrow"; return;
      case compiler::ir::Opcode::Multiply: output += "Multiply"; return;
      case compiler::ir::Opcode::MultiplyFloat: output += "MultiplyFloat"; return;
      case compiler::ir::Opcode::Divide: output += "Divide"; return;
      case compiler::ir::Opcode::DivideSigned: output += "DivideSigned"; return;
      case compiler::ir::Opcode::DivideFloat: output += "DivideFloat"; return;
      case compiler::ir::Opcode::Modulo: output += "Modulo"; return;
      case compiler::ir::Opcode::ModuloSigned: output += "ModuloSigned"; return;
      case compiler::ir::Opcode::ModuloFloat: output += "ModuloFloat"; return;
      case compiler::ir::Opcode::Exponent: output += "Exponent"; return;
      case compiler::ir::Opcode::Absolute: output += "Absolute"; return;
      case compiler::ir::Opcode::Negate: output += "Negate"; return;
      case compiler::ir::Opcode::FusedMultiplyAdd: output += "FusedMultiplyAdd"; return;
      case compiler::ir::Opcode::CompareTrue: output += "CompareTrue"; return;
      case compiler::ir::Opcode::CompareFalse: output += "CompareFalse"; return;
      case compiler::ir::Opcode::CompareTruthy: output += "CompareTruthy"; return;
      case compiler::ir::Opcode::CompareFalsy: output += "CompareFalsy"; return;
      case compiler::ir::Opcode::CompareZero: output += "CompareZero"; return;
      case compiler::ir::Opcode::CompareNotZero: output += "CompareNotZero"; return;
      case compiler::ir::Opcode::ComparePositive: output += "ComparePositive"; return;
      case compiler::ir::Opcode::CompareNegative: output += "CompareNegative"; return;
      case compiler::ir::Opcode::CompareEqual: output += "CompareEqual"; return;
      case compiler::ir::Opcode::CompareNotEqual: output += "CompareNotEqual"; return;
      case compiler::ir::Opcode::CompareGreater: output += "CompareGreater"; return;
      case compiler::ir::Opcode::CompareGreaterOrEqual: output += "CompareGreaterOrEqual"; return;
      case compiler::ir::Opcode::CompareLess: output += "CompareLess"; return;
      case compiler::ir::Opcode::CompareLessOrEqual: output += "CompareLessOrEqual"; return;
      case compiler::ir::Opcode::Test: output += "Test"; return;
      case compiler::ir::Opcode::And: output += "And"; return;
      case compiler::ir::Opcode::Or: output += "Or"; return;
      case compiler::ir::Opcode::Not: output += "Not"; return;
      case compiler::ir::Opcode::NotAnd: output += "NotAnd"; return;
      case compiler::ir::Opcode::NotOr: output += "NotOr"; return;
      case compiler::ir::Opcode::BitwiseAnd: output += "BitwiseAnd"; return;
      case compiler::ir::Opcode::BitwiseOr: output += "BitwiseOr"; return;
      case compiler::ir::Opcode::BitwiseExclusiveOr: output += "BitwiseExclusiveOr"; return;
      case compiler::ir::Opcode::BitwiseNot: output += "BitwiseNot"; return;
      case compiler::ir::Opcode::BitwiseShiftLeft: output += "BitwiseShiftLeft"; return;
      case compiler::ir::Opcode::BitwiseShiftRight: output += "BitwiseShiftRight"; return;
      case compiler::ir::Opcode::BitwiseShiftRightSigned: output += "BitwiseShiftRightSigned"; return;
      case compiler::ir::Opcode::BitwiseRotateLeft: output += "BitwiseRotateLeft"; return;
      case compiler::ir::Opcode::BitwiseRotateRight: output += "BitwiseRotateRight"; return;
      case compiler::ir::Opcode::BitwiseShiftLeftWithCarry: output += "BitwiseShiftLeftWithCarry"; return;
      case compiler::ir::Opcode::BitwiseShiftRightWithCarry: output += "BitwiseShiftRightWithCarry"; return;
      case compiler::ir::Opcode::BitwiseRotateLeftWithCarry: output += "BitwiseRotateLeftWithCarry"; return;
      case compiler::ir::Opcode::BitwiseRotateRightWithCarry: output += "BitwiseRotateRightWithCarry"; return;
      case compiler::ir::Opcode::BitwiseLowestSetIsolate: output += "BitwiseLowestSetIsolate"; return;
      case compiler::ir::Opcode::BitwiseLowestSetReset: output += "BitwiseLowestSetReset"; return;
      case compiler::ir::Opcode::BitwiseLowestSetMask: output += "BitwiseLowestSetMask"; return;
      case compiler::ir::Opcode::CheckBit: output += "CheckBit"; return;
      case compiler::ir::Opcode::CheckBitAndToggle: output += "CheckBitAndToggle"; return;
      case compiler::ir::Opcode::CheckBitAndReset: output += "CheckBitAndReset"; return;
      case compiler::ir::Opcode::CheckBitAndSet: output += "CheckBitAndSet"; return;
      case compiler::ir::Opcode::CountLeadingZeros: output += "CountLeadingZeros"; return;
      case compiler::ir::Opcode::CountTrailingZeros: output += "CountTrailingZeros"; return;
      case compiler::ir::Opcode::CountPopulation: output += "CountPopulation"; return;
      case compiler::ir::Opcode::SwapEndian: output += "SwapEndian"; return;
      case compiler::ir::Opcode::SwapEndianToBig: output += "SwapEndianToBig"; return;
      case compiler::ir::Opcode::SwapEndianToLittle: output += "SwapEndianToLittle"; return;
      case compiler::ir::Opcode::Cast: output += "Cast"; return;
      case compiler::ir::Opcode::Convert: output += "Convert"; return;
      case compiler::ir::Opcode::Prefetch: output += "Prefetch"; return;
    }

    output += "Unknown";
  }

  void Builder::Append(compiler::ir::Index index) {
    Append(index.Row());
  }

  void Builder::Append(const compiler::ir::Instruction& instruction) {
    Append(instruction.Opcode());
    output += '(';
    Append(instruction.Result());
    output += ", ";
    Append(instruction.Primary());
    output += ", ";
    Append(instruction.Secondary());
    output += ')';
  }

  void Builder::Append(compiler::ir::Instruction& instruction) { Append(std::as_const(instruction)); }
  void Builder::Append(compiler::input::Lexer& lexer) { Append(std::as_const(lexer)); }

  void Builder::Append(const compiler::input::Lexer& lexer) {
    const auto& tokens = lexer.Tokens();
    const auto& characters = lexer.Characters();
    // const auto& limbs = lexer.Limbs();
    const auto& errors = lexer.Errors();

    size_t token_index = 0;
    size_t character_index = 0;
    // size_t limb_index = 0;
    size_t error_index = 0;

    while (token_index < tokens.size()) {
      auto token = tokens[token_index];

      if (IsCharactersToken(token)) {
        uint32_t run_length = 0;
        while (token_index < tokens.size() && IsCharactersToken(tokens[token_index])) {
          // Append(tokens[token_index]);
          // Append(", ");
          run_length = (run_length << 4) | CharactersDigit(tokens[token_index]);
          ++token_index;
        }

        Append("Characters");
        Append(run_length);
        Append('(');
        Append('"');
        for (size_t count = 0; count < run_length; ++count) {
          if (character_index < characters.size()) {
            Append(characters[character_index++]);
          } else {
            Append("<missing character>");
          }
        }
        Append('"');
        Append(')');
      } else if (IsEscapeToken(token)) {
        Append(tokens[token_index++]);
        Append('(');
        Append(characters[character_index++]);
        Append(')');
      } else if (token == compiler::ir::Token::Error) {
        Append(tokens[token_index++]);
        Append('(');
        if (error_index < errors.size()) {
          Append(errors[error_index++]);
        } else {
          Append("<missing error>");
        }
        Append(')');
      } else {
        Append(tokens[token_index++]);
      }

      Append(", ");
    }
  }
}
