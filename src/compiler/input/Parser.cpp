import compiler.input.Lexer;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Error;
import compiler.ir.Opcode;
import compiler.ir.symbol.Type;
import compiler.text.cursor.String;
import compiler.text.Unicode;

namespace compiler::input {
  ir::Index Parser::NextSymbol() {
    if (symbols_index >= mod.Symbols().Count()) {
      return ir::Index{};
    }

    return ir::Index{static_cast<uint32_t>(symbols_index++)};
  }

  void Parser::ConsumeCharactersToken() {
    if (cursor.Peek() != ir::Token::Characters) {
      return;
    }

    if (widths_index < mod.Widths().size()) {
      widths_index += 1;
    }

    Advance();
  }

  void Parser::ConsumeEscapeSequence() {
    while (IsEscapeToken(cursor.Peek())) {
      Advance();

      while (cursor.Peek() == ir::Token::Characters) {
        ConsumeCharactersToken();
      }
    }
  }

  ir::Index Parser::ReportError(ir::Error error, ir::Index parent) {
    return Create(ir::symbol::Type::Error, parent, static_cast<uint64_t>(error));
  }

  int Parser::Precedence(ir::Token token) const {
    switch (token) {
      // Property / member access (highest)
      case ir::Token::MemberAccess: // x.y
      case ir::Token::MemberAccessStatic: // x:y
      case ir::Token::MemberAccessOptional: // x?.y
      case ir::Token::MemberAccessStaticOptional: // x?:y
        return 20;

      // Function calls
      case ir::Token::Call: // x -> y
      case ir::Token::ParameterOpen: // x(){ return y; }
      case ir::Token::CaptureOpen: // x[]{ return y; }
      case ir::Token::TupleOpen: // x(y)
      case ir::Token::ArrayOpen: // x[y]
      case ir::Token::ObjectOpen: // x{y}
      case ir::Token::EnumOpen: // x<y>
      case ir::Token::CharOpen: // x'y'
      case ir::Token::StringOpen: // x"y"
      case ir::Token::TemplateStringOpen: // x`y`
        return 19;

      // Unary prefix operators
      case ir::Token::BitwiseNot: // ~x
      case ir::Token::Increment: // ++x
      case ir::Token::Positive: // +x
      case ir::Token::Decrement: // --x
      case ir::Token::Negative: // -x
      case ir::Token::MutableReference: // *x
      case ir::Token::Reference: // &x
      case ir::Token::Symbol: // $x
      case ir::Token::Counted: // #x
      case ir::Token::Copy: // @x
      case ir::Token::Not: // !x
      case ir::Token::Move: // =x (unary move)
      case ir::Token::Spread: // ...x (prefix spread)
        return 18;

      // Await / yield style (kept distinct if needed)
      case ir::Token::Await: // await x
      case ir::Token::Yield: // yield x (when treated unary)
        return 17;

      case ir::Token::Exponent: // x ** y
        return 16;

      // Multiplicative
      case ir::Token::Multiply: // x * y
      case ir::Token::Divide: // x / y
      case ir::Token::Modulo: // x % y
        return 15;

      // Additive
      case ir::Token::Add: // x + y
      case ir::Token::Subtract: // x - y
        return 14;

      // Shifts
      case ir::Token::BitwiseLeftShift: // x << y
      case ir::Token::BitwiseRightShift: // x >> y
      case ir::Token::BitwiseTripleLeftShift: // x <<< y
      case ir::Token::BitwiseTripleRightShift: // x >>> y
        return 13;

      // Relational
      case ir::Token::Lesser: // x < y
      case ir::Token::LesserOrEqual: // x <= y
      case ir::Token::Greater: // x > y
      case ir::Token::GreaterOrEqual: // x >= y
      case ir::Token::AssertLesserOrEqual: // x <== y
      case ir::Token::AssertGreaterOrEqual: // x >== y
        return 11;

      // Equality
      case ir::Token::Equal: // x == y
      case ir::Token::NotEqual: // x != y
      case ir::Token::AssertEqual: // x === y
      case ir::Token::AssertNotEqual: // x !== y
        return 10;

      // Bitwise logical
      case ir::Token::BitwiseAnd: // x & y
        return 9;
      case ir::Token::BitwiseXor: // x ^ y
        return 8;
      case ir::Token::BitwiseOr: // x | y
        return 7;

      // Logical
      case ir::Token::And: // x && y
      case ir::Token::Wrap: // x !! y (truthy and)
        return 6;
      case ir::Token::Or: // x || y
      case ir::Token::Unwrap: // x ?? y (truthy or / unwrap)
        return 5;

      // Range construction
      case ir::Token::ExclusiveRange: // x..y
      case ir::Token::InclusiveRange: // x...y
        return 4;

      // Assignment family
      case ir::Token::Assign: // =
      case ir::Token::AssignAnd: // &&=
      case ir::Token::AssignOr: // ||=
      case ir::Token::AssignTruthyAnd: // !!=
      case ir::Token::AssignTruthyOr: // ??=
      case ir::Token::AssignOptional: // ?=
      case ir::Token::AssignAdd: // +=
      case ir::Token::AssignSubtract: // -=
      case ir::Token::AssignMultiply: // *=
      case ir::Token::AssignDivide: // /=
      case ir::Token::AssignModulo: // %=
      case ir::Token::AssignExponent: // **=
      case ir::Token::BitwiseAssignAnd: // &=
      case ir::Token::BitwiseAssignXor: // ^=
      case ir::Token::BitwiseAssignOr: // |=
      case ir::Token::BitwiseAssignLeftShift: // <<=
      case ir::Token::BitwiseAssignRightShift: // >>=
      case ir::Token::BitwiseAssignTripleLeftShift: // <<<=
      case ir::Token::BitwiseAssignTripleRightShift: // >>>=
        return 3;

      // Spread already handled above as unary (18). Keep Yield again for safety.
      default:
        return -1; // None / unhandled token has no precedence
    }
  }

  ir::Index Parser::ParsePrimary(ir::Index parent) {
    ir::Token token = cursor.Peek();

    switch (token) {
      case ir::Token::Undefined: return Undefined(parent);
      case ir::Token::Null: return Null(parent);
      case ir::Token::True:
      case ir::Token::False: return Boolean(parent);
      case ir::Token::CharOpen: return Character(parent);
      case ir::Token::Integer:
      case ir::Token::Digit:
      case ir::Token::Hex:
      case ir::Token::Octal:
      case ir::Token::Binary: return Integer(parent);
      case ir::Token::Decimal: return Decimal(parent);
      case ir::Token::StringOpen: return String(parent);
      case ir::Token::TemplateStringOpen: return TemplateString(parent);
      case ir::Token::ArrayOpen: return Array(parent);
      case ir::Token::EnumOpen: return Enum(parent);
      case ir::Token::TupleOpen: return Tuple(parent);
      case ir::Token::ObjectOpen: return Object(parent);
      case ir::Token::Identifier: return Identifier(parent);
      case ir::Token::Error: return Error(parent);
      case ir::Token::ConditionOpen: {
        if (!Match(ir::Token::ConditionOpen)) {
          return ReportError(ir::Error::ParserExpectedConditionOpen, parent);
        }

        ir::Index inner = Expression(parent);

        if (!Match(ir::Token::ConditionClose)) {
          ReportError(ir::Error::ParserExpectedConditionClose, parent);
        }

        return inner;
      }
      default:
        return ReportError(ir::Error::ParserExpectedPrimaryExpression, parent);
    }
  }

  ir::Index Parser::ParseUnary(ir::Index parent) {
    while (IsUnaryPrefixToken(cursor.Peek())) {
      Advance();
    }

    return ParsePrimary(parent);
  }

  ir::Index Parser::ParseBinary(ir::Index parent, int min_precedence) {
    ir::Index lhs = ParseUnary(parent);

    while (true) {
      ir::Token op = cursor.Peek();
      int precedence = Precedence(op);

      if (precedence < min_precedence) {
        break;
      }

      Match(op);

      ir::Index rhs = ParseUnary(parent);
      (void)rhs; // TODO: Build expression tree once symbol format is defined
    }

    return lhs;
  }

  Parser::Parser(input::Lexer& lexer)
    : tokens{lexer.TakeTokens()}
    , characters{lexer.TakeCharacters()}
    , line_starts{lexer.TakeLines()}
    , string_starts{lexer.TakeStrings()}
    , symbols{}
    , instructions{}
    , cursor{tokens}
  {
    Skip(); // Skip any leading trivial tokens
  }

  void Parser::None() {
    cursor.Advance(); // Eat the `None` token
  }

  void Parser::Spaces() {
    cursor.Advance(); // Eat the `Spaces*` token
  }

  void Parser::Tabs() {
    cursor.Advance(); // Eat the `Tabs*` token
  }

  void Parser::NewLine() {
    cursor.Advance();
  }

  void Parser::Comment() {
    // TODO
  }

  void Parser::MultiLineComment() {
    // TODO
  }

  void Parser::Trivial() {
    switch (cursor.Peek()) {
      case ir::Token::VerticalTab:
      case ir::Token::FormFeed:
      case ir::Token::None: return None();

      case ir::Token::Spaces0:
      case ir::Token::Spaces1:
      case ir::Token::Spaces2:
      case ir::Token::Spaces3:
      case ir::Token::Spaces4:
      case ir::Token::Spaces5:
      case ir::Token::Spaces6:
      case ir::Token::Spaces7:
      case ir::Token::Spaces8:
      case ir::Token::Spaces9:
      case ir::Token::SpacesA:
      case ir::Token::SpacesB:
      case ir::Token::SpacesC:
      case ir::Token::SpacesD:
      case ir::Token::SpacesE:
      case ir::Token::SpacesF: return Spaces();

      case ir::Token::Tabs0:
      case ir::Token::Tabs1:
      case ir::Token::Tabs2:
      case ir::Token::Tabs3:
      case ir::Token::Tabs4:
      case ir::Token::Tabs5:
      case ir::Token::Tabs6:
      case ir::Token::Tabs7:
      case ir::Token::Tabs8:
      case ir::Token::Tabs9:
      case ir::Token::TabsA:
      case ir::Token::TabsB:
      case ir::Token::TabsC:
      case ir::Token::TabsD:
      case ir::Token::TabsE:
      case ir::Token::TabsF: return Tabs();

      case ir::Token::LineFeed:
      case ir::Token::CarriageReturnLineFeed:
      case ir::Token::CarriageReturn:
      case ir::Token::LineSeparator:
      case ir::Token::ParagraphSeparator: return NewLine();

      case ir::Token::CommentOpen: return Comment();
      case ir::Token::MultiLineCommentOpen: return MultiLineComment();
      default: return; // Not a trivial token
    }
  }

  void Parser::Skip() {
    while (!cursor.Done() && IsTrivialToken(cursor.Peek())) {
      Trivial();
    }
  }

  void Parser::Advance(size_t count) {
    Advance(count);
    Skip();
  }

  bool Parser::Match(ir::Token token) {
    if (cursor.Match(token)) {
      Skip(); // Only skip after a successful match
      return true;
    }

    return false;
  }

  bool Parser::Check(ir::Token token) {
    Skip();
    return cursor.Check(token);
  }

  ir::Token Parser::Peek() {
    Skip();
    return cursor.Peek();
  }

  ir::Index Parser::Create(ir::symbol::Type type, ir::Index parent) {
    ir::Index index = mod.AddSymbol();
    index.Parent(mod, parent);
    index.Type(mod, type);

    // Link into parent's children list if parent is structured
    if (parent.IsStructured(mod)) {
      auto last = parent.LastChild(mod);
      if (last.IsValid(mod)) {
        // There is already at least one child, link after it
        uint64_t value = last.Value(mod);
        value &= 0xFFFFFFFFu; // Keep the start of the range
        value |= (static_cast<uint64_t>(index.Value()) << 32); // Set the end of the range to this new symbol
        last.Value(mod, value);
      } else {
        // This is the first child, set both start and end to this new symbol
        uint64_t value = static_cast<uint64_t>(index.Value());
        value |= (static_cast<uint64_t>(index.Value()) << 32);
        parent.Value(mod, value);
      }
    }

    return index;
  }

  ir::Index Parser::Create(ir::symbol::Type type, ir::Index parent, uint64_t value) {
    ir::Index index = Create(type, parent);
    index.Value(mod, value);

    return index;
  }

  ir::Index Parser::Create(ir::symbol::Type type, ir::Index parent, double value) {
    ir::Index index = Create(type, parent);
    index.Value(mod, static_cast<uint64_t>(value));

    return index;
  }

  ir::Index Parser::Create(ir::symbol::Type type, ir::Index parent, char32_t value) {
    ir::Index index = Create(type, parent);
    index.Value(mod, static_cast<uint64_t>(value));

    return index;
  }

  ir::Index Parser::Create(ir::symbol::Type type, ir::Index parent, bool value) {
    ir::Index index = Create(type, parent);
    index.Value(mod, static_cast<uint64_t>(value));

    return index;
  }

  ir::Index Parser::Expect(ir::Token token, ir::Error error, ir::Index parent) {
    if (!Match(token)) {
      return ReportError(error, parent);
    }

    return ir::Index{};
  }

  void Parser::Instruct(ir::Opcode opcode, ir::Index res, ir::Index lhs, ir::Index rhs) {
    mod.AddInstruction(opcode, res, lhs, rhs);
  }

  ir::Index Parser::Undefined(ir::Index parent) {
    Match(ir::Token::Undefined);

    return ir::Index{0}; // The module always reserves index 0 for the `undefined` symbol
  }

  ir::Index Parser::Null(ir::Index parent) {
    // A required `Null` token
    // Return a `Void` symbol (yes, we use the `Void` type for `null`)
    // The lexer doesn't create these ones

    Match(ir::Token::Null);

    return Create(ir::symbol::Type::Void, parent);
  }

  ir::Index Parser::Boolean(ir::Index parent) {
    // A required `True` or `False` token
    // Return a `Boolean` symbol with the value set to true or false
    // The lexer doesn't create these ones

    bool value = false;

    if (Match(ir::Token::True)) {
      value = true;
    } else if (Match(ir::Token::False)) {
      value = false;
    } else {
      return ReportError(ir::Error::ParserExpectedBooleanLiteral, parent);
    }

    return Create(ir::symbol::Type::Boolean, parent, value);
  }

  ir::Index Parser::Character(ir::Index parent) {
    // A required `CharacterOpen` token
    // One `Characters` OR one of the various escape tokens. The `Characters` is a variable width token, so we need to advance the widths index
    // One required `Character` token marking the location of the symbol that was already created by the lexer, advance the symbol index
    // A required `CharacterClose` token
    // Return the symbol index created by the lexer

    if (!Match(ir::Token::CharOpen)) {
      return ReportError(ir::Error::ParserExpectedCharacterOpenQuote, parent);
    }

    if (cursor.Peek() == ir::Token::Characters) {
      ConsumeCharactersToken();
    } else {
      ConsumeEscapeSequence();
    }

    if (!Match(ir::Token::Character)) {
      return ReportError(ir::Error::ParserExpectedCharacterToken, parent);
    }
    ir::Index index = NextSymbol();

    if (!Match(ir::Token::CharClose)) {
      ReportError(ir::Error::ParserExpectedCharacterCloseQuote, parent);
    }

    return index;
  }

  ir::Index Parser::Integer(ir::Index parent) {
    ir::Token token = cursor.Peek();

    switch (token) {
      case ir::Token::Integer:
      case ir::Token::Digit:
      case ir::Token::Hex:
      case ir::Token::Octal:
      case ir::Token::Binary:
        Match(token);
        break;
      default:
        return ReportError(ir::Error::ParserExpectedIntegerLiteral, parent);
    }

    // TODO: Capture literal value once numeric storage is wired up
    return Create(ir::symbol::Type::Integer, parent, static_cast<uint64_t>(0));
  }

  ir::Index Parser::Number(ir::Index parent) {
    ir::Index symbol = Create(ir::symbol::Type::Float, parent);
    Push(symbol);

    if (!Match(ir::Token::Float)) {
      return ReportError(ir::Error::ParserExpectedFloatLiteral, symbol);
    }

    // TODO: Capture literal value once numeric storage is wired up
    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::String(ir::Index parent) {
    // A required `StringOpen` token
    // Zero or more `Characters` or the various escape tokens. The `Characters` are variable width tokens, so we need to advance the widths index for each one we see
    // One required `String` token marking the location of the symbol that was already created by the lexer, advance the symbol index
    // A required `StringClose` token
    // Return the symbol index created by the lexer

    ir::Index symbol = Create(ir::symbol::Type::String, parent);
    Push(symbol);

    if (!Match(ir::Token::StringOpen)) {
      return ReportError(ir::Error::ParserExpectedStringOpenQuote, symbol);
    }

    while (true) {
      ir::Token token = cursor.Peek();
      if (token == ir::Token::Characters) {
        ConsumeCharactersToken();
        continue;
      }

      if (IsEscapeToken(token)) {
        ConsumeEscapeSequence();
        continue;
      }

      break;
    }

    if (!Match(ir::Token::StringClose)) {
      ReportError(ir::Error::ParserExpectedStringCloseQuote, symbol);
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::Array(ir::Index parent) {
    // A required `ArrayOpen` token
    // Create an `Array` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `ArrayClose` token
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Array, parent);
    Push(symbol);
    
    if (!Match(ir::Token::ArrayOpen)) {
      return ReportError(ir::Error::ParserExpectedArrayOpenBracket, symbol);
    }

    if (!Match(ir::Token::ArrayClose)) {
      while (true) {
        Expression(symbol);

        if (Match(ir::Token::Comma)) {
          if (Match(ir::Token::ArrayClose)) {
            break;
          }
          continue;
        }

        if (!Match(ir::Token::ArrayClose)) {
          ReportError(ir::Error::ParserExpectedArrayCloseBracket, symbol);
        }
        break;
      }
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::Enum(ir::Index parent) {
    // A required `EnumOpen` token
    // Create an `Enum` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `EnumClose` token
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Enum, parent);
    Push(symbol);

    if (!Match(ir::Token::EnumOpen)) {
      return ReportError(ir::Error::ParserExpectedEnumOpenChevron, symbol);
    }

    if (!Match(ir::Token::EnumClose)) {
      while (true) {
        Expression(symbol);

        if (Match(ir::Token::Comma)) {
          if (Match(ir::Token::EnumClose)) {
            break;
          }
          continue;
        }

        if (!Match(ir::Token::EnumClose)) {
          ReportError(ir::Error::ParserExpectedEnumCloseChevron, symbol);
        }
        break;
      }
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::Tuple(ir::Index parent) {
    // A required `TupleOpen` token
    // Create a `Tuple` symbol to serve as the parent of the subsequent `Expression` groups
    // Zero or more `Expression` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `TupleClose` token
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Tuple, parent);
    Push(symbol);

    if (!Match(ir::Token::TupleOpen)) {
      return ReportError(ir::Error::ParserExpectedTupleOpenParen, symbol);
    }

    if (!Match(ir::Token::TupleClose)) {
      while (true) {
        Expression(symbol);

        if (Match(ir::Token::Comma)) {
          if (Match(ir::Token::TupleClose)) {
            break;
          }
          continue;
        }

        if (!Match(ir::Token::TupleClose)) {
          ReportError(ir::Error::ParserExpectedTupleCloseParen, symbol);
        }
        break;
      }
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::Object(ir::Index parent) {
    // A required `ObjectOpen` token
    // Create an `Object` symbol to serve as the parent of the subsequent `ObjectDeclaration` groups
    // Zero or more `ObjectDeclaration` groups, separated by `Comma` tokens (trailing comma is legal)
    // A required `ObjectClose` token
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Object, parent);
    Push(symbol);

    if (!Match(ir::Token::ObjectOpen)) {
      return ReportError(ir::Error::ParserExpectedObjectOpenBrace, symbol);
    }

    if (!Match(ir::Token::ObjectClose)) {
      while (true) {
        ir::Index declaration = ObjectDeclaration(symbol);

        if (!declaration) {
          ReportError(ir::Error::ParserExpectedObjectDeclaration, symbol);
        }

        if (Match(ir::Token::Comma)) {
          if (Match(ir::Token::ObjectClose)) {
            break;
          }
          continue;
        }

        if (!Match(ir::Token::ObjectClose)) {
          ReportError(ir::Error::ParserExpectedObjectCloseBrace, symbol);
        }
        break;
      }
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::TemplateString(ir::Index parent) {
    ir::Index symbol = Create(ir::symbol::Type::TemplateString, parent);
    Push(symbol);

    if (!Match(ir::Token::TemplateStringOpen)) {
      return ReportError(ir::Error::ParserExpectedTemplateStringOpenBacktick, parent);
    }

    while (true) {
      if (Match(ir::Token::TemplateStringClose)) {
        break;
      }

      if (cursor.Done()) {
        ReportError(ir::Error::ParserExpectedTemplateStringCloseBacktick, symbol);
        break;
      }

      ir::Token token = cursor.Peek();

      if (token == ir::Token::TemplateStringExpressionOpen) {
        Match(ir::Token::TemplateStringExpressionOpen);
        Expression(symbol);

        if (!Match(ir::Token::TemplateStringExpressionClose)) {
          ReportError(ir::Error::ParserExpectedTemplateExpressionCloseBrace, symbol);
        }
        continue;
      }

      if (token == ir::Token::Characters) {
        ConsumeCharactersToken();
        continue;
      }

      if (IsEscapeToken(token)) {
        ConsumeEscapeSequence();
        continue;
      }

      if (token == ir::Token::String) {
        Match(ir::Token::String);
        NextSymbol();
        continue;
      }

      // Unknown token inside template string; advance to avoid infinite loop
      Advance();
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::Function(ir::Index parent) {
    // TODO
  }

  ir::Index Parser::Value(ir::Index parent) {
    switch (cursor.Peek()) {
      // Any of these have to be an identifier, since they aren't inside a string/character literal
      case ir::Token::Characters0:
      case ir::Token::Characters1:
      case ir::Token::Characters2:
      case ir::Token::Characters3:
      case ir::Token::Characters4:
      case ir::Token::Characters5:
      case ir::Token::Characters6:
      case ir::Token::Characters7:
      case ir::Token::Characters8:
      case ir::Token::Characters9:
      case ir::Token::CharactersA:
      case ir::Token::CharactersB:
      case ir::Token::CharactersC:
      case ir::Token::CharactersD:
      case ir::Token::CharactersE:
      case ir::Token::CharactersF:
      case ir::Token::EscapeASCII:
      case ir::Token::EscapeHex:
      case ir::Token::EscapeUnicode:
      case ir::Token::EscapeUnicodeBraced1:
      case ir::Token::EscapeUnicodeBraced2:
      case ir::Token::EscapeUnicodeBraced3:
      case ir::Token::EscapeUnicodeBraced4:
      case ir::Token::EscapeUnicodeBraced5:
      case ir::Token::EscapeUnicodeBraced6: return Identifier(parent);

      // Any of these have to be a number. (Floats cannot start with a dot.)
      case ir::Token::Digits0:
      case ir::Token::Digits1:
      case ir::Token::Digits2:
      case ir::Token::Digits3:
      case ir::Token::Digits4:
      case ir::Token::Digits5:
      case ir::Token::Digits6:
      case ir::Token::Digits7:
      case ir::Token::Digits8:
      case ir::Token::Digits9:
      case ir::Token::DigitsA:
      case ir::Token::DigitsB:
      case ir::Token::DigitsC:
      case ir::Token::DigitsD:
      case ir::Token::DigitsE:
      case ir::Token::DigitsF:
      case ir::Token::HexStart:
      case ir::Token::OctalStart:
      case ir::Token::BinaryStart: return Number(parent);

      case ir::Token::True:
      case ir::Token::False: return Boolean(parent);

      case ir::Token::Null: return Null(parent);
      case ir::Token::Undefined: return Undefined(parent);
      case ir::Token::This: return This(parent);
      case ir::Token::StringOpen: return String(parent);
      case ir::Token::CharOpen: return Character(parent);
      case ir::Token::ArrayOpen: return Array(parent);
      case ir::Token::EnumOpen: return Enum(parent);
      case ir::Token::TupleOpen: return Tuple(parent);
      case ir::Token::ObjectOpen: return Object(parent);
      case ir::Token::TemplateStringOpen: return TemplateString(parent);

      // Functions may start with a capture list or parameter list
      case ir::Token::CaptureOpen:
      case ir::Token::ParameterOpen: return Function(parent);

      default: return ir::Index{};
    }
  }

  ir::Index Parser::Identifier(ir::Index parent) {
    if (!Match(ir::Token::Identifier)) {
      return ReportError(ir::Error::ParserExpectedIdentifier, parent);
    }
    ir::Index index = NextSymbol();

    if (index.IsValid(mod) && parent.IsValid(mod)) {
      index.Parent(mod, parent);
    }

    return index;
  }

  ir::Index Parser::Destructuring(ir::Index parent) {
    switch (cursor.Peek()) {
      case ir::Token::ArrayOpen: return Array(parent);
      case ir::Token::TupleOpen: return Tuple(parent);
      case ir::Token::ObjectOpen: return Object(parent);
      default:
        return ReportError(ir::Error::ParserExpectedDestructuringPattern, parent);
    }
  }

  ir::Index Parser::Error(ir::Index parent) {
    if (!Match(ir::Token::Error)) {
      return ReportError(ir::Error::ParserExpectedPrimaryExpression, parent);
    }
    return NextSymbol();
  }

  ir::Index Parser::Expression(ir::Index parent) {
    return ParseBinary(parent, 0);
  }

  ir::Index Parser::ObjectDeclaration(ir::Index parent) {
    ir::Token token = cursor.Peek();

    if (token == ir::Token::Comma) {
      return ir::Index{};
    }

    ir::Index key;

    switch (token) {
      case ir::Token::Identifier:
        key = Identifier(parent);
        break;
      case ir::Token::StringOpen:
        key = String(parent);
        break;
      case ir::Token::CharOpen:
        key = Character(parent);
        break;
      default:
        return Expression(parent);
    }

    if (Match(ir::Token::Colon)) {
      return Expression(parent);
    }

    return key;
  }

  ir::Index Parser::Declaration(ir::Index parent) {
    // A `Declaration` is different from the `DeclarationStatement` statement, as it can appear in other places such as function parameters or destructured objects/arrays
    // It has slightly different rules and does not end with a semicolon

    // *Zero* or more modifier tokens, such as `Let`, `Const`, `Mutable`, `Static`, `Private`, `Public`, or `Protected`
    // A required `Identifier` group OR a `Destructuring` group
    // An optional type annotation, beginning with a `TypeStart` token, then a required `Expression` group
    // An optional assignment, beginning with an `Assign` token, then a required `Expression` group

    // NOTE: Unsure what we return here. If there was an expression, we return that symbol. If not, do we create a new `Declaration` symbol? Or just return the identifier/destructuring symbol? This will need more thought.

    while (IsModifierToken(cursor.Peek())) {
      Advance();
    }

    ir::Index target;

    if (cursor.Peek() == ir::Token::Identifier) {
      target = Identifier(parent);
    } else {
      target = Destructuring(parent);
    }

    if (Match(ir::Token::TypeStart)) {
      Expression(parent);
    }

    if (Match(ir::Token::Assign)) {
      return Expression(parent);
    }

    return target;
  }

  // Statements
  ir::Index Parser::ImportStatement(ir::Index parent) {
    // A required `Import` token
    // An optional `Identifier` or a destructured object, this can be skipped to import the module for side effects only
    // A required `From` token
    // A required `String` group
    // An optional `In` token, followed by a required `String` group for a target package
    // A required `Semicolon` token

    ir::Index symbol = Create(ir::symbol::Type::Import, parent);
    Push(symbol);

    if (!Match(ir::Token::Import)) {
      return ReportError(ir::Error::ParserExpectedImportKeyword, symbol);
    }

    if (cursor.Peek() == ir::Token::Identifier) {
      Identifier(symbol);
    } else if (cursor.Peek() == ir::Token::ObjectOpen) {
      Destructuring(symbol);
    }

    if (!Match(ir::Token::From)) {
      return ReportError(ir::Error::ParserExpectedFromAfterImport, symbol);
    }

    String(symbol);

    if (Match(ir::Token::In)) {
      String(symbol);
    }

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterImport, symbol);
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::Permissions(ir::Index parent) {
    // A required `With` token
    // A required `Array` group
    // A required `In` token
    // A required `String` group specifying the package name this permission is granting access to
    if (!Match(ir::Token::With)) {
      return ReportError(ir::Error::ParserExpectedWithKeyword, parent);
    }

    Array(parent);

    if (!Match(ir::Token::In)) {
      return ReportError(ir::Error::ParserExpectedInAfterPermissions, parent);
    }

    String(parent);
    return ir::Index{};
  }

  ir::Index Parser::RegisterStatement(ir::Index parent) {
    // A required `Register` token
    // A required `String` group naming the package
    // A required `From` token
    // A required `String` group setting the package path/url
    // Zero or more `Permissions` groups
    // A required `Semicolon` token

    ir::Index symbol = Create(ir::symbol::Type::Register, parent);
    Push(symbol);

    if (!Match(ir::Token::Register)) {
      return ReportError(ir::Error::ParserExpectedRegisterKeyword, symbol);
    }

    String(symbol);

    if (!Match(ir::Token::From)) {
      return ReportError(ir::Error::ParserExpectedFromAfterRegister, symbol);
    }

    String(symbol);

    while (cursor.Peek() == ir::Token::With) {
      Permissions(symbol);
    }

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterRegister, symbol);
    }

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::BreakStatement(ir::Index parent) {
    // One or more stacked `Break` tokens, each one targeting the next enclosing loop
    // A required `Semicolon` token

    // NOTE: Unsure what we return here, if anything
    if (!Match(ir::Token::Break)) {
      return ReportError(ir::Error::ParserExpectedBreakKeyword, parent);
    }

    ir::Index loop = parent.Loop(mod);
    while (Match(ir::Token::Break)) {
      loop = loop.Loop(mod);
    }

    if (!loop.IsValid(mod)) {
      loop = parent;
    }

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterBreak, parent);
    }
    Instruct(ir::Opcode::Break, loop);

    return ir::Index{};
  }

  ir::Index Parser::ContinueStatement(ir::Index parent) {
    // One or more stacked `Continue` tokens, each one targeting the next enclosing loop
    // A required `Semicolon` token

    if (!Match(ir::Token::Continue)) {
      return ReportError(ir::Error::ParserExpectedContinueKeyword, parent);
    }

    ir::Index loop = parent.Loop(mod);
    while (Match(ir::Token::Continue)) {
      loop = loop.Loop(mod);

      // TODO: Error if no loop found
    }

    if (!loop.IsValid(mod)) {
      loop = parent;
    }

    // TODO: Error if `loop` isn't `IsLoop()`

    // Create the instruction to jump to the correct loop
    Instruct(ir::Opcode::Continue, loop);

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterContinue, parent);
    }

    // I don't believe this produces a new symbol, so no index to return
    return ir::Index{};
  }

  ir::Index Parser::ReturnStatement(ir::Index parent) {
    // A required `Return` token
    // An optional `Expression` group
    // A required `Semicolon` token
    if (!Match(ir::Token::Return)) {
      return ReportError(ir::Error::ParserExpectedReturnKeyword, parent);
    }

    ir::Index value{};
    if (cursor.Peek() != ir::Token::Semicolon) {
      value = Expression(parent);
    }

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterReturn, parent);
    }

    Instruct(ir::Opcode::Return, value);

    return value;
  }

  ir::Index Parser::DoStatement(ir::Index parent) {
    // A required `Do` token
    // Create a new `Do` symbol to serve as the parent of the subsequent groups
    // A required `Scope` group
    // An optional `Semicolon` token
    // Return the symbol
    ir::Index symbol = Create(ir::symbol::Type::Do, parent);
    Push(symbol);

    if (!Match(ir::Token::Do)) {
      return ReportError(ir::Error::ParserExpectedDoKeyword, parent);
    }

    Scope(symbol);
    Match(ir::Token::Semicolon);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::WhileStatement(ir::Index parent) {
    // A required `While` token
    // Create a new `While` symbol to serve as the parent of the subsequent groups
    // A required `Condition` group
    // A required `Scope` group
    // An optional `Semicolon` token
    // Return the symbol
    
    ir::Index symbol = Create(ir::symbol::Type::While, parent);
    Push(symbol);

    if (!Match(ir::Token::While)) {
      return ReportError(ir::Error::ParserExpectedWhileKeyword, parent);
    }

    Condition(symbol);
    Scope(symbol);
    Match(ir::Token::Semicolon);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::RepeatStatement(ir::Index parent) {
    // A required `Repeat` token
    // Create a new `Repeat` symbol to serve as the parent of the subsequent groups
    // A required `Scope` group
    // An optional `While` token, followed by a required `Condition` group
    // NOTE: If a `While` was found, upgrade the symbol type to `RepeatWhile` instead of `Repeat`
    // An optional `Semicolon` token
    // Return the symbol
    if (!Match(ir::Token::Repeat)) {
      return ReportError(ir::Error::ParserExpectedRepeatKeyword, parent);
    }

    ir::Index symbol = Create(ir::symbol::Type::Repeat, parent);
    Push(symbol);
    Scope(symbol);

    if (Match(ir::Token::While)) {
      Condition(symbol);
      symbol.Type(mod, ir::symbol::Type::RepeatWhile);
    }

    Match(ir::Token::Semicolon);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::ForStatement(ir::Index parent) {
    // A required `For` token
    // Create a new `For` symbol to serve as the parent of the subsequent groups
    // A required `ConditionOpen` token
    // A required `Declaration` group for the loop variable
    // A required `In` token
    // A required `Expression` group for the iterable
    // A required `ConditionClose` token
    // A required `Scope` group
    // An optional `Semicolon` token
    // Return the symbol
    if (!Match(ir::Token::For)) {
      return ReportError(ir::Error::ParserExpectedForKeyword, parent);
    }

    ir::Index symbol = Create(ir::symbol::Type::For, parent);
    Push(symbol);

    if (!Match(ir::Token::ConditionOpen)) {
      ReportError(ir::Error::ParserExpectedConditionOpen, symbol);
    }
    Declaration(symbol);
    bool has_in = Match(ir::Token::In);
    if (!has_in) {
      ReportError(ir::Error::ParserExpectedInAfterFor, symbol);
    }

    Expression(symbol);

    if (!Match(ir::Token::ConditionClose)) {
      ReportError(ir::Error::ParserExpectedForConditionClose, symbol);
    }

    Scope(symbol);
    Match(ir::Token::Semicolon);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::ElseIfStatement(ir::Index parent) {
    // A required `Else` token
    // A required `If` token
    // Create a new `ElseIf` symbol to serve as the parent of the subsequent groups
    // A required `Condition` group
    // A required `Scope` group
    // Return the symbol
    if (!Match(ir::Token::Else)) {
      return ReportError(ir::Error::ParserExpectedElseKeyword, parent);
    }

    if (!Match(ir::Token::If)) {
      ReportError(ir::Error::ParserExpectedIfKeywordAfterElse, parent);
    }

    ir::Index symbol = Create(ir::symbol::Type::ElseIf, parent);
    Push(symbol);

    Condition(symbol);
    Scope(symbol);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::ElseStatement(ir::Index parent) {
    // A required `Else` token
    // Create a new `Else` symbol to serve as the parent of the subsequent groups
    // A required `Scope` group
    // Return the symbol
    if (!Match(ir::Token::Else)) {
      return ReportError(ir::Error::ParserExpectedElseKeyword, parent);
    }

    ir::Index symbol = Create(ir::symbol::Type::Else, parent);
    Push(symbol);

    Scope(symbol);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::IfStatement(ir::Index parent) {
    // A required `If` token
    // Create a new `If` symbol to serve as the parent of the subsequent groups
    // A required `Condition` group
    // A required `Scope` group
    // Zero or more `ElseIf` groups
    // An optional trailing `Else` group
    // An optional `Semicolon` token
    // Return the symbol
    
    ir::Index symbol = Create(ir::symbol::Type::If, parent);
    Push(symbol);

    Expect(ir::Token::If, ir::Error::ParserExpectedIfKeyword);

    Condition(symbol);
    Scope(symbol);

    // TODO: This has a flaw because `Peek` doesn't consider trivial tokens, so if there are comments or whitespace between the `If` block and the `Else` token, it won't be seen here.
    while (cursor.Peek() == ir::Token::Else) {
      if (cursor.Peek(1) == ir::Token::If) {
        ElseIfStatement(symbol);
      } else {
        ElseStatement(symbol);
        break;
      }
    }

    Match(ir::Token::Semicolon);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::IsStatement(ir::Index parent) {
    // A required `Is` token
    // Create a new `Is` symbol
    // A required `Condition` group
    // A required `Scope` group
    // Return the symbol

    // TODO: `Is` and `Has` are suppose to be able to be chained without a scope block, allowing any number of them to share a single scope block

    ir::Index symbol = Create(ir::symbol::Type::Is);
    Push(symbol);

    Expect(ir::Token::Is, ir::Error::ParserExpectedIsKeyword);

    Condition();
    Scope();

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::HasStatement(ir::Index parent) {
    // A required `Has` keyword
    // Create a new `Has` symbol
    // A required `Condition` group
    // A required `Scope` group
    // Return the symbol

    // TODO: `Is` and `Has` are suppose to be able to be chained without a scope block, allowing any number of them to share a single scope block

    ir::Index symbol = Create(ir::symbol::Type::Has);
    Push(symbol);

    Expect(ir::Token::Has, ir::Error::ParserExpectedHasKeyword);

    Condition();
    Scope();

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::FromStatement(ir::Index parent) {
    // A required `From` keyword
    // Create a new `From` symbol
    // A required `Condition` group
    // A required `Scope` group
    // Return the symbol

    // TODO: `Is` and `Has` and `From` are suppose to be able to be chained without a scope block, allowing any number of them to share a single scope block

    ir::Index symbol = Create(ir::symbol::Type::From, parent);
    Push(symbol);

    Expect(ir::Token::From, ir::Error::ParserExpectedFromKeyword, symbol);

    Condition(symbol);
    Scope(symbol);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::DefaultStatement(ir::Index parent) {
    // A required `Default` keyword
    // Create a new `Default` symbol
    // A required `Scope` group
    // Return the symbol

    ir::Index symbol = Create(ir::symbol::Type::Default);
    Push(symbol);

    if (!Match(ir::Token::Default)) {
      return ReportError(ir::Error::ParserExpectedDefaultKeyword);
    }

    Scope();

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::WhenStatement(ir::Index parent) {
    // A required `When` token
    // Create a new `When` symbol
    // A required `Condition` group
    // A required `ScopeOpen` token
    // Zero or more `IsStatement` or `HasStatement` groups and no more than one `DefaultStatement` group, in any order
    // A required `ScopeClose` token
    // An optional `Semicolon` token
    // Return the symbol
    
    ir::Index symbol = Create(ir::symbol::Type::When);
    Push(symbol);
    
    Expect(ir::Token::When, ir::Error::ParserExpectedWhenKeyword);
    Condition();

    // NOTE: We do not use a `Scope` group here because the contents are not normal statements
    Expect(ir::Token::ScopeOpen, ir::Error::ParserExpectedWhenScopeOpen);
    
    bool default_seen = false;
    while (!cursor.Done() && cursor.Peek() != ir::Token::ScopeClose) {
      if (cursor.Peek() == ir::Token::Is) {
        IsStatement();
      } else if (cursor.Peek() == ir::Token::Has) {
        HasStatement();
      } else if (cursor.Peek() == ir::Token::Default) {
        if (!default_seen) {
          DefaultStatement();
          default_seen = true;
        } else {
          DefaultStatement(); // Parse to keep progress; diagnostics TBD
        }
      } else {
        break;
      }
    }

    Expect(ir::Token::ScopeClose, ir::Error::ParserExpectedWhenScopeClose);
    Match(ir::Token::Semicolon);

    Pop(symbol);
    return symbol;
  }

  ir::Index Parser::DeclarationStatement(ir::Index parent) {
    // One or more modifier tokens, such as `Let`, `Const`, `Mutable`, `Static`, `Private`, `Public`, `Protected`, or `Export`
    // A required identifier
    // An optional type annotation, beginning with a `TypeStart` token then a required expression
    // A required assignment, beginning with `Assign` token then an `Expression` group
    // A required `Semicolon` token
    // Return the expression symbol created by the assignment

    // NOTE: We do not create a symbol for the declaration itself, we use the symbol from the expression
    while (IsModifierToken(cursor.Peek())) {
      Advance();
    }

    ir::Index target = Identifier(parent);

    if (Match(ir::Token::TypeStart)) {
      Expression(parent);
    }

    bool has_assign = Match(ir::Token::Assign);
    if (!has_assign) {
      ReportError(ir::Error::ParserExpectedInitializerInDeclaration, parent);
    }

    ir::Index value = Expression(parent);

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterDeclaration, parent);
    }

    return value;
  }

  ir::Index Parser::ExpressionStatement(ir::Index parent) {
    // A required `Expression` group
    // A required `Semicolon` token
    // Return the expression symbol

    ir::Index value = Expression(parent);

    if (!Match(ir::Token::Semicolon)) {
      ReportError(ir::Error::ParserExpectedSemicolonAfterExpression, parent);
    }

    return value;
  }

  ir::Index Parser::Statement(ir::Index parent) {
    switch (cursor.Peek()) {
      case ir::Token::Import: return ImportStatement(parent);
      case ir::Token::Register: return RegisterStatement(parent);
      case ir::Token::Break: return BreakStatement(parent);
      case ir::Token::Continue: return ContinueStatement(parent);
      case ir::Token::Return: return ReturnStatement(parent);
      case ir::Token::Do: return DoStatement(parent);
      case ir::Token::While: return WhileStatement(parent);
      case ir::Token::Repeat: return RepeatStatement(parent);
      case ir::Token::For: return ForStatement(parent);
      case ir::Token::If: return IfStatement(parent);
      case ir::Token::When: return WhenStatement(parent);

      // Modifier keywords that begin a declaration statement
      case ir::Token::Let:
      case ir::Token::Const:
      case ir::Token::Mutable:
      case ir::Token::Static:
      case ir::Token::Private:
      case ir::Token::Public:
      case ir::Token::Protected:
      case ir::Token::Export: return DeclarationStatement(parent);
      default: return ExpressionStatement(parent);
    }
  }

  ir::Index Parser::Scope(ir::Index parent) {
    // A required `ScopeOpen` token
    // Zero or more `Statement` groups
    // A required `ScopeClose` token
    // We do NOT handle a semicolon here, that is the responsibility of the caller
    // NOTE: Unsure what we return here. Perhaps a default `ir::Index{}` since the statements are linked to the parent already?
    if (!Match(ir::Token::ScopeOpen)) {
      ReportError(ir::Error::ParserExpectedScopeOpen, parent);
    }

    while (!cursor.Done() && cursor.Peek() != ir::Token::ScopeClose) {
      Statement(parent);
    }

    if (!Match(ir::Token::ScopeClose)) {
      ReportError(ir::Error::ParserExpectedScopeClose, parent);
    }

    return ir::Index{};
  }

  ir::Index Parser::Condition(ir::Index parent) {
    // A required `ConditionOpen` token
    // A required `Expression` group
    // A required `ConditionClose` token
    // Return the expression symbol

    if (!Match(ir::Token::ConditionOpen)) {
      ReportError(ir::Error::ParserExpectedConditionOpen, parent);
    }

    ir::Index expr = Expression(parent);

    if (!Match(ir::Token::ConditionClose)) {
      ReportError(ir::Error::ParserExpectedConditionClose, parent);
    }

    return expr;
  }
};