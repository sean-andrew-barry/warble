import lexical.parser;
import utility.print;
import lexical.source;
import lexical.source.span;
import lexical.tokens;
import lexical.token;
import lexical.cursor.token;
import lexical.text;

import node;
import node.block;
import node.scope;
// import node.scopes;
import node.scope.conditional;
import node.scope.conditional._if;
import node.scope.conditional._else;
import node.scope.conditional._else_if;
import node.scope.conditional.loop;
import node.scope.conditional.loop._do;
import node.scope.conditional.loop._while;
import node.scope.conditional.loop._for;
import node.scope.conditional.pattern;
import node.scope.conditional.pattern.cover;
import node.scope.conditional.pattern.match;
import node.scope.conditional.pattern._default;
import node.scope.conditional.pattern.is;
import node.scope.context;
import node.scope.context._module;
import node.scope.context.function;
import node.scope.context.function.inline_function;
import node._module;
import node.declaration;
import node.declaration.modifiers;
import node.declaration.automatic;
import node.declaration.forward;
import node.declaration._import;
import node.declaration.spread;
import node.declaration.typed;
import node.declaration.lvalue.named;
import node.declaration.lvalue.phi;
import node.declaration.lvalue.linked;
import node.declaration.rvalue.temporary;
import node.declaration.rvalue.literal;
import node.declaration.rvalue.literal.primitive;
import node.declaration.rvalue.literal.primitive.boolean;
import node.declaration.rvalue.literal.primitive.character;
import node.declaration.rvalue.literal.primitive.null;
import node.declaration.rvalue.literal.primitive.undefined;
import node.declaration.rvalue.literal.primitive.numeric;
import node.declaration.rvalue.literal.primitive.numeric.decimal;
import node.declaration.rvalue.literal.primitive.numeric.integer;
import node.declaration.rvalue.literal.primitive.numeric.number;
import node.declaration.rvalue.literal.structured;
import node.declaration.rvalue.literal.structured.array;
import node.declaration.rvalue.literal.structured._enum;
import node.declaration.rvalue.literal.structured.object;
import node.declaration.rvalue.literal.structured.string;
import node.declaration.rvalue.literal.structured.template_string;
import node.declaration.rvalue.literal.structured.tuple;
import node.declaration.rvalue.literal.structured.function;
import node.instruction;
import node.instruction.memory;
import node.instruction.memory.construct;
import node.instruction.memory.destruct;
import node.instruction.memory.move;
import node.instruction.memory.store;
import node.instruction.memory.load;
import node.instruction.control;
import node.instruction.control._case;
import node.instruction.control._return;
import node.instruction.control._continue;
import node.instruction.control._break;
import node.instruction.control.jump;
import node.instruction.control.jump.absolute;
import node.instruction.control.jump.relative;
import node.instruction.control.jump.equal;
import node.instruction.control.jump.zero;
import node.instruction.control.jump.not_equal;
import node.instruction.control.jump.not_zero;
import node.instruction.control.jump.greater;
import node.instruction.control.jump.greater_or_equal;
import node.instruction.control.jump.less;
import node.instruction.control.jump.less_or_equal;
import node.instruction.control.jump.sign;
import node.instruction.control.jump.not_sign;
import node.instruction.control.jump.above;
import node.instruction.control.jump.above_or_equal;
import node.instruction.control.jump.below;
import node.instruction.control.jump.below_or_equal;
import node.instruction.unary;
import node.instruction.unary.bitwise_not;
import node.instruction.unary.copy;
import node.instruction.unary.reference;
import node.instruction.unary.spread;
import node.instruction.unary.optional;
import node.instruction.unary.symbol;
import node.instruction.unary.length;
import node.instruction.unary._virtual;
import node.instruction.unary._not;
import node.instruction.unary.increment;
import node.instruction.unary.decrement;
import node.instruction.unary.positive;
import node.instruction.unary.negative;
import node.instruction.unary.await;
import node.instruction.unary.expected;
import node.instruction.unary.unexpected;
import node.instruction.unary.yield;
import node.instruction.unary.postfix;
import node.instruction.unary.postfix.increment;
import node.instruction.unary.postfix.decrement;
import node.instruction.binary;
import node.instruction.binary.assert;
import node.instruction.binary.assert.equal;
import node.instruction.binary.assert.not_equal;
import node.instruction.binary.assert.greater_or_equal;
import node.instruction.binary.assert.lesser_or_equal;
import node.instruction.binary.nullish;
import node.instruction.binary.logical._or;
import node.instruction.binary.logical._and;
import node.instruction.binary.logical.equal;
import node.instruction.binary.logical.not_equal;
import node.instruction.binary.logical.greater;
import node.instruction.binary.logical.greater_or_equal;
import node.instruction.binary.logical.lesser;
import node.instruction.binary.logical.lesser_or_equal;
import node.instruction.binary.use;
import node.instruction.binary.math.add;
import node.instruction.binary.math.subtract;
import node.instruction.binary.math.multiply;
import node.instruction.binary.math.divide;
import node.instruction.binary.math.modulo;
import node.instruction.binary.math.exponent;
import node.instruction.binary.assign;
import node.instruction.binary.assign.add;
import node.instruction.binary.assign.subtract;
import node.instruction.binary.assign.multiply;
import node.instruction.binary.assign.divide;
import node.instruction.binary.assign.modulo;
import node.instruction.binary.assign.exponent;
import node.instruction.binary.access;
import node.instruction.binary.access.member;
import node.instruction.binary.access.member._static;
import node.instruction.binary.access.member.optional;
import node.instruction.binary.bitwise;
import node.instruction.binary.bitwise._and;
import node.instruction.binary.bitwise._or;
import node.instruction.binary.bitwise._xor;
import node.instruction.binary.bitwise.shift;
import node.instruction.binary.bitwise.shift.left;
import node.instruction.binary.bitwise.shift.right;
import node.instruction.binary.bitwise.shift.triple;
import node.instruction.binary.bitwise.shift.triple.left;
import node.instruction.binary.bitwise.shift.triple.right;
import node.instruction.binary.bitwise.assign;
import node.instruction.binary.bitwise.assign._and;
import node.instruction.binary.bitwise.assign._xor;
import node.instruction.binary.bitwise.assign._or;
import node.instruction.binary.bitwise.assign.shift;
import node.instruction.binary.bitwise.assign.shift.left;
import node.instruction.binary.bitwise.assign.shift.right;
import node.instruction.binary.bitwise.assign.shift.triple;
import node.instruction.binary.bitwise.assign.shift.triple.left;
import node.instruction.binary.bitwise.assign.shift.triple.right;
import node.instruction.binary.range;
import node.instruction.binary.range.exclusive;
import node.instruction.binary.range.inclusive;
import node.instruction.binary.call;
import node.instruction.binary.call.character;
import node.instruction.binary.call.decimal;
import node.instruction.binary.call.integer;
import node.instruction.binary.call.number;
import node.instruction.binary.call.array;
import node.instruction.binary.call._enum;
import node.instruction.binary.call.object;
import node.instruction.binary.call.string;
import node.instruction.binary.call.template_string;
import node.instruction.binary.call.tuple;
import node.instruction.binary.call.function;
import node.instruction.ternary_condition;
// import node.instruction.phi;

import <vector>;
import <string>;
import <string_view>;
// import <charconv>;
import <cassert>;
import <utility>;
import <cstddef>;
import <bitset>;
import <unordered_map>;
import <memory>;
import <ranges>;
import <algorithm>;
import <concepts>;

namespace lexical {
  void Parser::Advance(size_t n) {
    // utility::Print("Advanced to", Peek(n), source.Read(Peek(), Peek(n)));
    cursor.Advance(n);
    // utility::Print("Advanced to", Peek(), utility::Quoted(Start().View()));
  }

  void Parser::Push(std::shared_ptr<node::Declaration> declaration, node::instruction::memory::Destruct* destructor) {
    declarations.push_back(std::move(declaration));
    destructors.push_back(destructor);
  }

  void Parser::Push(std::shared_ptr<node::Declaration>&& declaration, node::instruction::memory::Destruct* destructor) {
    declarations.push_back(std::move(declaration));
    destructors.push_back(destructor);
  }

  lexical::source::Span Parser::Start() { return lexical::source::Span{source.Text().cbegin(), cursor.cbegin(), cursor.cbegin()}; }
  void Parser::End(lexical::source::Span& span) { span.End(cursor.cbegin()); }
  void Parser::End(Node& node) { node.End(cursor.cbegin()); }
  void Parser::End(Node* node) { End(*node); }
  // void Parser::End(std::shared_ptr<Node>& node) { End(node.get()); }
  // void Parser::End(std::shared_ptr<node::Declaration>& node) { End(node.get()); }
  void Parser::End(node::Scope* scope) { End(*scope); }

  void Parser::End(node::Scope& scope) {
    scope.End(cbegin());

    while (!declarations.empty()) {
      // Pop declarations until we find one from a different scope
      if (&declarations.back()->Scope() != &scope) break;

      declarations.pop_back();
      destructors.pop_back();
    }
  }

  std::string_view Parser::Normalized(const std::string_view view) {
    auto id = identifiers.find(view);
    if (id != identifiers.end()) {
      return id->second;
    }
    else {
      return view;
    }
  }

  std::shared_ptr<node::Declaration> Parser::Find(const std::string_view name) {
    auto normalized = Normalized(name);

    for (auto dec : std::ranges::reverse_view{declarations}) {
      if (Normalized(dec->Name()) == normalized) {
        return dec;
      }
    }

    return nullptr;
  }

  Parser::Parser(node::scope::context::Module& mod, lexical::Source& source)
    : mod{mod}
    , source{source}
    , cursor{source.Tokens()}
  {
    // Declare<node::declaration::Import>(mod, Start(), "compiler");
    // Declare<node::declaration::Import>(mod, Start(), "void");
    // Declare<node::declaration::Import>(mod, Start(), "auto");
    // Declare<node::declaration::Import>(mod, Start(), "bool");
    // Declare<node::declaration::Import>(mod, Start(), "char");
    // Declare<node::declaration::Import>(mod, Start(), "char8");
    // Declare<node::declaration::Import>(mod, Start(), "char16");
    // Declare<node::declaration::Import>(mod, Start(), "char32");
    // Declare<node::declaration::Import>(mod, Start(), "int");
    // Declare<node::declaration::Import>(mod, Start(), "int8");
    // Declare<node::declaration::Import>(mod, Start(), "int16");
    // Declare<node::declaration::Import>(mod, Start(), "int32");
    // Declare<node::declaration::Import>(mod, Start(), "int64");
    // Declare<node::declaration::Import>(mod, Start(), "uint");
    // Declare<node::declaration::Import>(mod, Start(), "uint8");
    // Declare<node::declaration::Import>(mod, Start(), "uint16");
    // Declare<node::declaration::Import>(mod, Start(), "uint32");
    // Declare<node::declaration::Import>(mod, Start(), "uint64");
    // Declare<node::declaration::Import>(mod, Start(), "utf8");
    // Declare<node::declaration::Import>(mod, Start(), "utf16");
    // Declare<node::declaration::Import>(mod, Start(), "utf32");
    // Declare<node::declaration::Import>(mod, Start(), "utf");
    // Declare<node::declaration::Import>(mod, Start(), "string");
    // Declare<node::declaration::Import>(mod, Start(), "text");
    // Declare<node::declaration::Import>(mod, Start(), "float");
    // Declare<node::declaration::Import>(mod, Start(), "float32");
    // Declare<node::declaration::Import>(mod, Start(), "float64");
  }

  std::runtime_error Parser::ErrorUnexpectedEnd(std::string_view message, lexical::Token token) const {
    return utility::Error("Unexpected end of input while looking for a", token, "token type");
  }

  std::runtime_error Parser::ErrorUnexpectedToken(std::string_view message, lexical::Token token) const {
    return utility::Error("Token", Peek(), "was unexpected, it should have been a", token, "token type");
  }

  std::runtime_error Parser::ErrorExpectedToken(std::string_view message, lexical::Token token) const {
    return utility::Error("Expected a", token, "token type");
  }

  void Parser::Expect(lexical::Tokens type) {
    // utility::Debug("Expected", lexical::Token{type});

    if (!Match(type)) {
      throw utility::Error("Expected a", lexical::Token{type}, "token type, but got", Peek());
    }
  }

  std::shared_ptr<node::declaration::rvalue::literal::primitive::Null> Parser::NullLiteral(node::Scope& scope) {
    auto span = Start();
    Expect(lexical::Tokens::_NULL);

    End(span);
    return Declare<node::declaration::rvalue::literal::primitive::Null>(scope, span);
  }

  std::shared_ptr<node::declaration::rvalue::literal::primitive::Undefined> Parser::UndefinedLiteral(node::Scope& scope) {
    auto span = Start();
    Expect(lexical::Tokens::UNDEFINED);

    End(span);
    return Declare<node::declaration::rvalue::literal::primitive::Undefined>(scope, span);
  }

  std::shared_ptr<node::declaration::rvalue::literal::primitive::Boolean> Parser::TrueLiteral(node::Scope& scope) {
    auto span = Start();
    Expect(lexical::Tokens::TRUE);

    End(span);
    return Declare<node::declaration::rvalue::literal::primitive::Boolean>(scope, span, true);
  }

  std::shared_ptr<node::declaration::rvalue::literal::primitive::Boolean> Parser::FalseLiteral(node::Scope& scope) {
    auto span = Start();
    Expect(lexical::Tokens::FALSE);

    End(span);
    return Declare<node::declaration::rvalue::literal::primitive::Boolean>(scope, span, false);
  }

  std::shared_ptr<node::declaration::rvalue::literal::primitive::numeric::Number> Parser::NumberLiteral(node::Scope& scope) {
    auto span = Start();

    std::string_view view = span.View();
    uint64_t value = 0;
    std::errc ec;

    if (Match(lexical::Tokens::NUMBER)) {
      std::tie(value, ec) = lexical::Text::ParseU64(view);
    }
    else if (Match(lexical::Tokens::BINARY_NUMBER)) {
      view.remove_prefix(2); // Skip '0b'
      std::tie(value, ec) = lexical::Text::ParseU64(view, 2);
    }
    else if (Match(lexical::Tokens::HEX_NUMBER)) {
      view.remove_prefix(2); // Skip '0x'
      std::tie(value, ec) = lexical::Text::ParseU64(view, 16);
    }
    else if (Match(lexical::Tokens::OCTAL_NUMBER)) {
      view.remove_prefix(1); // Skip '0'
      std::tie(value, ec) = lexical::Text::ParseU64(view, 8);
    }
    else {
      throw utility::Error("Number literal expected a NUMBER, BINARY_NUMBER, HEX_NUMBER, or OCTAL_NUMBER token");
    }

    // Check if the parsing was successful
    if (ec != std::errc()) {
      // Handle the error appropriately.
      throw utility::Error("Invalid number literal format");
    }

    End(span);

    auto declaration = Declare<node::declaration::rvalue::literal::primitive::numeric::Number>(scope, span, value);
    return declaration;
  }

  std::shared_ptr<node::declaration::rvalue::literal::primitive::numeric::Integer> Parser::IntegerLiteral(node::Scope& scope) {
    auto span = Start();

    std::string_view view = span.View(); // Includes the '-' sign
    int64_t value = 0;
    std::errc ec;

    if (Match(lexical::Tokens::INTEGER)) {
      view.remove_prefix(1); // Skip '-'
      std::tie(value, ec) = lexical::Text::ParseI64(view);
    }
    else if (Match(lexical::Tokens::BINARY_INTEGER)) {
      view.remove_prefix(3); // Skip '-0b'
      std::tie(value, ec) = lexical::Text::ParseI64(view, 2);
    }
    else if (Match(lexical::Tokens::HEX_INTEGER)) {
      view.remove_prefix(3); // Skip '-0x'
      std::tie(value, ec) = lexical::Text::ParseI64(view, 16);
    }
    else if (Match(lexical::Tokens::OCTAL_INTEGER)) {
      view.remove_prefix(3); // Skip '-0o'
      std::tie(value, ec) = lexical::Text::ParseI64(view, 8);
    }
    else {
      throw utility::Error("Integer literal expected an INTEGER, BINARY_INTEGER, HEX_INTEGER, or OCTAL_INTEGER token");
    }

    // Check if the parsing was successful
    if (ec != std::errc()) {
      // Handle the error appropriately.
      throw utility::Error("Invalid integer literal format");
    }

    End(span);
    auto declaration = Declare<node::declaration::rvalue::literal::primitive::numeric::Integer>(scope, span, -value);
    return declaration;
  }

  std::shared_ptr<node::declaration::rvalue::literal::primitive::numeric::Decimal> Parser::DecimalLiteral(node::Scope& scope) {
    auto span = Start();

    if (!Match(lexical::Tokens::DECIMAL)) {
      throw utility::Error("Decimal literal expected a DECIMAL token");
    }

    std::string_view view = span.View();

    // Attempt to parse the double value from the string_view
    auto [value, ec] = lexical::Text::ParseF64(view);

    // Check if the parsing was successful
    if (ec != std::errc()) {
      // Handle the error, for example, by returning a nullptr or logging an error message.
      // Depending on your error handling strategy, you might want to return a more descriptive error.
      throw utility::Error("Invalid decimal literal format");
    }

    End(span);
    return Declare<node::declaration::rvalue::literal::primitive::numeric::Decimal>(scope, span, value);
  }

  std::string Parser::ParseText(node::Scope& scope, std::string_view caller, lexical::Tokens end1, lexical::Tokens end2) {
    std::string value;

    while (Peek().Type() != end1 && Peek().Type() != end2) {
      if (Done()) {
        throw ErrorUnexpectedEnd(caller, end1);
      }

      auto span = Start();

      if (Match(lexical::Tokens::TEXT)) {
        value += span.View();
      }
      else if (Match(lexical::Tokens::ESCAPE_NEWLINE)) {
        value += '\n';
      }
      else if (Match(lexical::Tokens::ESCAPE_TAB)) {
        value += '\t';
      }
      else if (Match(lexical::Tokens::ESCAPE_BACKSPACE)) {
        value += '\b';
      }
      else if (Match(lexical::Tokens::ESCAPE_RETURN)) {
        value += '\r';
      }
      else if (Match(lexical::Tokens::ESCAPE_FORM_FEED)) {
        value += '\f';
      }
      else if (Match(lexical::Tokens::ESCAPE_LITERAL)) {
        value += span.View()[1]; // Get the character after the backslash from the source span
      }
      else if (Match(lexical::Tokens::ESCAPE_UNICODE_CODEPOINT)) {
        value += lexical::Text::ParseEscapeUnicodeCodepoint(span.View());
      }
      else if (Match(lexical::Tokens::ESCAPE_UNICODE_SHORT)) {
        value += lexical::Text::ParseEscapeUnicodeShort(span.View());
      }
      else if (Match(lexical::Tokens::ESCAPE_HEX_CODE)) {
        value += lexical::Text::ParseEscapeHexCode(span.View());
      }
      else {
        throw ErrorUnexpectedToken(caller);
      }
    }

    return value;
  }

  std::shared_ptr<node::declaration::rvalue::literal::primitive::Character> Parser::CharLiteral(node::Scope& scope) {
    auto start = Start();

    if (!Match(lexical::Tokens::CHAR_OPEN)) {
      throw ErrorExpectedToken("Character literal", lexical::Tokens::CHAR_OPEN);
    }

    // This could be potentially optimized a bit to get both the raw char32_t and the UTF-8 string
    std::string value = ParseText(scope, "Character literal", lexical::Tokens::CHAR_CLOSE);

    Expect(lexical::Tokens::CHAR_CLOSE);

    if (value.empty()) {
      throw utility::Error("Character literal cannot be empty");
    }

    if (value.length() > 4) {
      throw utility::Error("Character literal must represent a single 32 bit Unicode code point");
    }

    End(start);
    return Declare<node::declaration::rvalue::literal::primitive::Character>(scope, start, std::move(value));
  }

  std::shared_ptr<node::declaration::rvalue::literal::structured::String> Parser::StringLiteral(node::Scope& scope) {
    auto start = Start();

    if (!Match(lexical::Tokens::STRING_OPEN)) {
      throw ErrorExpectedToken("String literal", lexical::Tokens::STRING_OPEN);
    }

    // No need to do any validation on value as it is allowed to be any length, including empty
    std::string value = ParseText(scope, "String literal", lexical::Tokens::STRING_CLOSE);

    Expect(lexical::Tokens::STRING_CLOSE);

    End(start);
    return Declare<node::declaration::rvalue::literal::structured::String>(scope, start, std::move(value));
  }

  std::shared_ptr<node::declaration::rvalue::literal::structured::Tuple> Parser::TupleLiteral(node::Scope& scope) {
    auto start = Start();

    if (!Match(lexical::Tokens::TUPLE_OPEN)) {
      throw ErrorExpectedToken("Tuple literal", lexical::Tokens::TUPLE_OPEN);
    }

    auto tuple = Declare<node::declaration::rvalue::literal::structured::Tuple>(scope, start);

    // Loop until we can no longer parse valid expressions or until we hit TUPLE_CLOSE.
    while (true) {
      // Try to parse an expression inside the tuple.
      auto value = Expression(scope);

      if (!value) {
        // If we failed to get a value, it could mean we are at the end of the tuple
        // or there's a syntax error. Let's check for TUPLE_CLOSE to be sure.
        if (Match(lexical::Tokens::TUPLE_CLOSE)) {
          break; // Properly closed tuple, end parsing.
        }
        else {
          throw utility::Error("Expected an expression or closing parenthesis in tuple literal");
        }
      }

      tuple->Value(std::move(value));

      // After parsing an expression, check for a comma or a closing parenthesis.
      if (Match(lexical::Tokens::COMMA)) {
        continue; // Found a comma, expect another expression next.
      }
      else if (Match(lexical::Tokens::TUPLE_CLOSE)) {
        break; // Properly closed tuple, end parsing.
      }
      else {
        throw utility::Error("Expected a comma or closing parenthesis in tuple literal");
      }
    }

    End(tuple);
    return tuple;
  }

  std::shared_ptr<node::declaration::rvalue::literal::structured::TemplateString> Parser::TemplateStringLiteral(node::Scope& scope) {
    auto start = Start();

    if (!Match(lexical::Tokens::TEMPLATE_STRING_OPEN)) {
      throw ErrorExpectedToken("Template string literal", lexical::Tokens::TEMPLATE_STRING_OPEN);
    }

    auto template_string = Declare<node::declaration::rvalue::literal::structured::TemplateString>(scope, start);

    // Loop until we can no longer parse valid expressions or until we hit TEMPLATE_STRING_CLOSE.
    while (!Match(lexical::Tokens::TEMPLATE_STRING_CLOSE)) {
      if (Done()) {
        throw ErrorUnexpectedEnd("Template string literal", lexical::Tokens::TEMPLATE_STRING_CLOSE);
      }

      if (Match(lexical::Tokens::TEMPLATE_STRING_EXPRESSION_OPEN)) {
        auto expression = Expression(scope);

        if (!expression) {
          throw utility::Error("Expected an expression inside template string literal");
        }

        template_string->Value(std::move(expression));

        Expect(lexical::Tokens::TEMPLATE_STRING_EXPRESSION_CLOSE);
      }
      else {
        auto string_start = Start();

        std::string text = ParseText(scope, "Template string literal", lexical::Tokens::TEMPLATE_STRING_CLOSE, lexical::Tokens::TEMPLATE_STRING_EXPRESSION_OPEN);

        string_start.End(cbegin());
        auto declaration = Declare<node::declaration::rvalue::literal::structured::String>(scope, string_start, std::move(text));

        template_string->Value(declaration);
      }
    }

    End(template_string);
    return template_string;
  }

  std::shared_ptr<node::declaration::rvalue::literal::structured::Array> Parser::ArrayLiteral(node::Scope& scope) {
    auto start = Start();

    if (!Match(lexical::Tokens::ARRAY_OPEN)) {
      throw ErrorExpectedToken("Array literal", lexical::Tokens::ARRAY_OPEN);
    }

    auto array = Declare<node::declaration::rvalue::literal::structured::Array>(scope, start);

    // Loop until we can no longer parse valid expressions or until we hit ARRAY_CLOSE.
    while (true) {
      auto value = Expression(scope);

      if (!value) {
        // If we failed to get a value, it could mean we are at the end of the array
        // or there's a syntax error. Let's check for ARRAY_CLOSE to be sure.
        if (Match(lexical::Tokens::ARRAY_CLOSE)) {
          break;
        }
        else {
          throw utility::Error("Expected an expression or closing parenthesis in array literal");
        }
      }

      array->Value(std::move(value));

      // After parsing an expression, check for a comma or a closing parenthesis.
      if (Match(lexical::Tokens::COMMA)) {
        continue; // Found a comma, expect another expression next.
      }
      else if (Match(lexical::Tokens::ARRAY_CLOSE)) {
        break;
      }
      else {
        throw utility::Error("Expected a comma or closing parenthesis in array literal");
      }
    }

    End(array);
    return array;
  }

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Parser::ObjectLiteral(node::Scope& scope) {
    auto start = Start();

    Expect(lexical::Tokens::OBJECT_OPEN);

    auto object = Declare<node::declaration::rvalue::literal::structured::Object>(scope, start);

    while (Valid()) {
      auto value = DeclarationExpression(scope);

      if (!value) {
        if (Match(lexical::Tokens::OBJECT_CLOSE)) {
          break;
        }
        else {
          throw utility::Error("Expected a declaration or closing brace in object literal");
        }
      }

      object->Value(std::move(value));

      if (Match(lexical::Tokens::COMMA)) {
        continue;
      }
      else if (Match(lexical::Tokens::OBJECT_CLOSE)) {
        break;
      }
      else {
        throw utility::Error("Expected a comma or closing brace in object literal");
      }
    }

    End(object);
    return object;
  }

  std::shared_ptr<node::declaration::rvalue::literal::structured::Enum> Parser::EnumLiteral(node::Scope& scope) {
    auto start = Start();

    Expect(lexical::Tokens::ENUM_OPEN);

    auto _enum = Declare<node::declaration::rvalue::literal::structured::Enum>(scope, start);

    while (true) {
      auto value = Expression(scope);

      if (!value) {
        if (Match(lexical::Tokens::ENUM_CLOSE)) {
          break;
        }
        else {
          throw utility::Error("Expected an expression or closing angle in enum literal");
        }
      }

      _enum->Value(std::move(value));

      if (Match(lexical::Tokens::COMMA)) {
        continue;
      }
      else if (Match(lexical::Tokens::ENUM_CLOSE)) {
        break;
      }
      else {
        throw utility::Error("Expected a comma or closing angle in enum literal");
      }
    }

    End(_enum);
    return _enum;
  }

  void Parser::ParseParameters(node::Scope& scope, std::shared_ptr<node::declaration::rvalue::literal::structured::Function>& function) {
    Expect(lexical::Tokens::PARAMETER_OPEN);

    while (!Match(lexical::Tokens::PARAMETER_CLOSE)) {
      auto value = DeclarationExpression(scope);

      if (!value) {
        if (Match(lexical::Tokens::PARAMETER_CLOSE)) {
          break;
        }
        else {
          throw utility::Error("Expected a declaration or closing parenthesis in function parameters");
        }
      }

      function->Parameter(std::move(value));

      if (Match(lexical::Tokens::COMMA)) {
        continue;
      }
      else if (Match(lexical::Tokens::PARAMETER_CLOSE)) {
        break;
      }
      else {
        throw utility::Error("Expected a comma or closing parenthesis in function parameters");
      }
    }
  }

  void Parser::ParseCaptures(node::Scope& scope, std::shared_ptr<node::declaration::rvalue::literal::structured::Function>& function) {
    Expect(lexical::Tokens::CAPTURE_OPEN);

    while (!Match(lexical::Tokens::CAPTURE_CLOSE)) {
      auto value = DeclarationExpression(scope);

      if (!value) {
        if (Match(lexical::Tokens::CAPTURE_CLOSE)) {
          break;
        }
        else {
          throw utility::Error("Expected a declaration or closing bracket in function captures");
        }
      }

      function->Capture(std::move(value));

      if (Match(lexical::Tokens::COMMA)) {
        continue;
      }
      else if (Match(lexical::Tokens::CAPTURE_CLOSE)) {
        break;
      }
      else {
        throw utility::Error("Expected a comma or closing bracket in function captures");
      }
    }
  }

  std::vector<Node*> Parser::ParseScopeBlock(node::Scope& scope) {
    Expect(lexical::Tokens::SCOPE_OPEN);

    std::vector<Node*> results;

    while (!Match(lexical::Tokens::SCOPE_CLOSE)) {
      if (Done()) {
        throw utility::Error("Unexpected end of tokens while parsing a scope block");
      }
      else {
        auto result = Statement(scope);
        if (result) {
          results.push_back(result);
        }
        else if (!Match(lexical::Tokens::SCOPE_CLOSE)) {
          throw utility::Error("Scope block failed to parse a statement or a scope close token");
        }
      }
    }

    return results;
  }

  node::Scope* Parser::ScopeStatement(node::Scope& scope) {
    Expect(lexical::Tokens::SCOPE_OPEN);

    while (!Match(lexical::Tokens::SCOPE_CLOSE)) {
      if (Done()) {
        throw utility::Error("Unexpected end of tokens while parsing a scope block");
      }
      else {
        auto statement = Statement(scope);
        if (!statement && !Match(lexical::Tokens::SCOPE_CLOSE)) {
          throw utility::Error("Scope block failed to parse a statement or a scope close token");
        }
      }
    }

    return &scope;
  }

  std::shared_ptr<node::declaration::rvalue::literal::structured::Function> Parser::FunctionLiteral(node::Scope& scope) {
    auto start = Start();

    // Create a new scope for the function
    auto* function_scope = Scope<node::scope::context::Function>(scope, start);
    auto function = Declare<node::declaration::rvalue::literal::structured::Function>(scope, start, *function_scope);

    bool has_parameters = false;
    bool has_captures = false;

    while (true) {
      if (Done()) {
        throw utility::Error("Unexpected end of tokens while parsing function literal");
      } else if (Check(lexical::Tokens::PARAMETER_OPEN) && !has_parameters) {
        ParseParameters(*function_scope, function);
        has_parameters = true;
      } else if (Check(lexical::Tokens::CAPTURE_OPEN) && !has_captures) {
        ParseCaptures(*function_scope, function);
        has_captures = true;
      } else if (Check(lexical::Tokens::SCOPE_OPEN)) {
        break;
      } else if (Check(lexical::Tokens::ARROW)) {
        break;
      } else {
        // If we reach a token that isn't PARAMETER_OPEN, CAPTURE_OPEN, or the start of the body, 
        // and we haven't set either has_parameters or has_captures, it's an error.
        if (!has_parameters && !has_captures) {
          throw utility::Error("Function literal expected either parameters or captures before the body");
        }

        // Handle unexpected token error if it's not the first iteration and has_parameters or has_captures is true.
        throw utility::Error("Unexpected token in function literal declaration");
      }
    }

    if (Match(lexical::Tokens::ARROW)) {
      function->IsArrow(true);
      Expression(*function_scope);
    } else {
      ParseScopeBlock(*function_scope);
    }

    return function;
  }

  std::shared_ptr<node::Declaration> Parser::Identifier(node::Scope& scope) {
    if (!cursor.IsIdentifiable()) return nullptr;

    auto start = Start();

    std::string_view identifier = start.View();
    Advance();

    auto declaration = Find(identifier);
    if (declaration) {
      utility::Debug("Found existing declaration for:", identifier);
      return declaration;
    }

    start.End(cbegin());

    utility::Debug("Failed to find an existing declaration for:", identifier);

    // No existing declaration was found, so create a Forward declaration
    // to act as a placeholder until later in the compilation process
    // Eventually this will either resolve to a real declaration or error
    return Declare<node::declaration::Forward>(scope, start, identifier);
  }

  std::shared_ptr<node::Declaration> Parser::Value(node::Scope& scope) {
    switch (Peek().Type()) {
      // Primitive literals
      case lexical::Tokens::_NULL: return NullLiteral(scope);
      case lexical::Tokens::UNDEFINED: return UndefinedLiteral(scope);
      case lexical::Tokens::TRUE: return TrueLiteral(scope);
      case lexical::Tokens::FALSE: return FalseLiteral(scope);

      case lexical::Tokens::OCTAL_NUMBER:
      case lexical::Tokens::HEX_NUMBER:
      case lexical::Tokens::BINARY_NUMBER:
      case lexical::Tokens::NUMBER: return NumberLiteral(scope);

      case lexical::Tokens::BINARY_INTEGER:
      case lexical::Tokens::HEX_INTEGER:
      case lexical::Tokens::OCTAL_INTEGER:
      case lexical::Tokens::INTEGER: return IntegerLiteral(scope);

      case lexical::Tokens::DECIMAL: return DecimalLiteral(scope);
      case lexical::Tokens::CHAR_OPEN: return CharLiteral(scope);
      case lexical::Tokens::STRING_OPEN: return StringLiteral(scope);

      // Structured literals
      case lexical::Tokens::TUPLE_OPEN: return TupleLiteral(scope);
      case lexical::Tokens::ARRAY_OPEN: return ArrayLiteral(scope);
      case lexical::Tokens::TEMPLATE_STRING_OPEN: return TemplateStringLiteral(scope);
      case lexical::Tokens::OBJECT_OPEN: return ObjectLiteral(scope);
      case lexical::Tokens::ENUM_OPEN: return EnumLiteral(scope);
      case lexical::Tokens::ARROW:
      case lexical::Tokens::PARAMETER_OPEN:
      case lexical::Tokens::CAPTURE_OPEN: return FunctionLiteral(scope);

      // case lexical::Tokens::IDENTIFIER: return Identifier(scope);
      default: return Identifier(scope);
    }

    return nullptr;
  }

  std::shared_ptr<node::Declaration> Parser::Expression(node::Scope& scope, int precedence) {
    utility::Indent i{};

    std::shared_ptr<node::Declaration> lhs;

    auto token = Peek();

    if (token.IsUnaryPrefix()) {
      auto span = Start();
      Advance(); // Consume the unary operator
      auto operand = Expression(scope, token.Precedence());

      if (!operand) {
        throw utility::Error("Expression expected an operand expression after unary operator.");
      }

      End(span);

      // The result of this unary operation itself becomes the initial LHS for the rest of the expression.
      auto result = Declare<node::declaration::rvalue::Temporary>(scope, span);
      node::Instruction* instruction = MakeUnaryInstruction(scope, token, span, result, operand);

      scope.Slide(result->Dtor());
      scope.Slide(operand->Dtor());

      lhs = std::move(result);

    } else {
      // Parse the initial LHS value.
      lhs = std::move(Value(scope));
      if (!lhs) return nullptr;
    }

    while (true) {
      token = Peek();

      auto token_precedence = token.Precedence();
      if (token_precedence < precedence) {
        break;
      }

      if (!token.IsImplicit()) Advance(); // Consume the operator

      auto span = Start();

      auto next_precedence = token_precedence + (token.IsRightAssociative() ? 0 : 1);
      auto rhs = Expression(scope, next_precedence);

      if (!rhs) {
        throw utility::Error("Expression expected right-hand side expression after operator.");
      }

      auto result = Declare<node::declaration::rvalue::Temporary>(scope, span);
      node::Instruction* instruction = MakeBinaryInstruction(scope, token, span, result, lhs, rhs);

      scope.Slide(result->Dtor());
      scope.Slide(rhs->Dtor());
      scope.Slide(lhs->Dtor());

      End(span);

      lhs = std::move(result); // The result becomes the LHS for the next operation.
    }

    if (lhs) {
      lhs->End(cbegin());
    }

    return lhs;
  }

  node::Scope* Parser::BreakStatement(node::Scope& scope) {
    auto start = Start();
    Expect(lexical::Tokens::BREAK);

    node::scope::conditional::Loop* target = scope.FindLoop();
    if (!target) {
      throw utility::Error("Break statement must be inside a loop scope");
    }

    while (Match(lexical::Tokens::BREAK)) {
      target = target->FindLoop();

      if (!target) {
        throw utility::Error("Chained break statement failed to find the outer loop");
      }
    }

    End(start);
    Instruct<node::instruction::control::jump::Absolute>(scope, start, &target->ExitBlock());

    Expect(lexical::Tokens::SEMICOLON);

    return &scope;
  }

  node::Scope* Parser::ContinueStatement(node::Scope& scope) {
    auto start = Start();
    Expect(lexical::Tokens::CONTINUE);

    node::scope::conditional::Loop* target = scope.FindLoop();
    if (!target) {
      throw utility::Error("Continue statement must be inside a loop scope");
    }

    while (Match(lexical::Tokens::CONTINUE)) {
      target = target->FindLoop();

      if (!target) {
        throw utility::Error("Chained continue statement failed to find the outer loop");
      }
    }

    End(start);
    Instruct<node::instruction::control::jump::Absolute>(scope, start, &target->EntryBlock());

    Expect(lexical::Tokens::SEMICOLON);

    return &scope;
  }

  std::shared_ptr<node::Declaration> Parser::Condition(node::Scope& scope) {
    Expect(lexical::Tokens::CONDITION_OPEN);

    // Parse the condition expression into the scope
    auto condition = Expression(scope);

    if (!condition) {
      throw utility::Error("Expected a condition expression after opening '('");
    }

    Expect(lexical::Tokens::CONDITION_CLOSE);

    return condition;
  }

  node::scope::conditional::If* Parser::IfStatement(node::Scope& scope) {
    auto start = Start();

    Expect(lexical::Tokens::IF);

    auto* if_scope = Scope<node::scope::conditional::If>(scope, start);

    auto condition = Condition(*if_scope);

    // Create the instruction with the condition and the jump location
    // If the condition is false (zero) it will jump to the exit block,
    // otherwise it will continue to the next block in the if_scope
    End(start);
    Instruct<node::instruction::control::jump::Zero>(*if_scope, start, condition, &if_scope->ExitBlock());

    auto statement = Statement(*if_scope);
    if (!statement) throw utility::Error("If statement expected a statement after the condition");

    if (Check(lexical::Tokens::ELSE)) {
      auto end = Start();
      // Parse the else in the parent scope, not the if's scope
      auto* else_scope = ElseStatement(scope);

      // Create an unconditional jump back to the exit block of the last in the chain
      Instruct<node::instruction::control::jump::Absolute>(*if_scope, end, &else_scope->ExitBlock());
    }

    Match(lexical::Tokens::SEMICOLON);
    End(*if_scope);

    return if_scope;
  }

  node::scope::Conditional* Parser::ElseStatement(node::Scope& scope) {
    Expect(lexical::Tokens::ELSE);

    if (Match(lexical::Tokens::IF)) {
      return IfStatement(scope);
    }

    auto* body_scope = Scope<node::scope::conditional::Else>(scope, Start());
    ParseScopeBlock(*body_scope);

    End(*body_scope);
    return body_scope;
  }

  node::scope::conditional::loop::While* Parser::WhileStatement(node::Scope& scope) {
    Expect(lexical::Tokens::WHILE);

    auto start = Start();

    // Create a new scope as a child of the current block's scope which sets up its initial blocks
    // Scopes are always created with 3 blocks: entry, exit, and 1 body block
    auto* loop_scope = Scope<node::scope::conditional::loop::While>(scope, start);
    auto target_block = std::make_unique<node::Block>();

    if (Check(lexical::Tokens::CONDITION_OPEN)) {
      // Parse the condition expression into the loop scope.
      // In this language it is valid to leave the condition blank.
      // This is treated as an unconditional loop, like `while (true) {}`
      auto condition = Condition(*loop_scope);

      // Create the instruction with the condition and the jump location
      // If the condition is false (zero) it will jump to the loop_scope's exit block,
      // otherwise it will continue to the next block in the loop_scope
      auto* result = Instruct<node::instruction::control::jump::Zero>(*loop_scope, start, condition, target_block.get());
      result->End(cbegin());
    }

    // Parse the while statement body
    auto statement = Statement(*loop_scope);
    if (!statement) throw utility::Error("While statement expected a statement after the condition");

    // Create an unconditional jump back to the first body block of the loop
    Instruct<node::instruction::control::jump::Absolute>(*loop_scope, Start(), &loop_scope->RequiredFirstBlock());
    End(*loop_scope);

    // Add the target block to the enclosing scope
    target_block->Span(Start());
    scope.Add(std::move(target_block));

    return loop_scope;
  }

  node::scope::conditional::loop::Do* Parser::DoStatement(node::Scope& scope) {
    Expect(lexical::Tokens::DO);

    // Create a new scope as a child of the current block's scope which sets up its initial blocks
    // Scopes are always created with 3 blocks: entry, exit, and 1 body block
    auto* loop_scope = Scope<node::scope::conditional::loop::Do>(scope, Start());

    auto statement = Statement(*loop_scope);
    if (!statement) throw utility::Error("Do statement expected a statement after the condition");

    Expect(lexical::Tokens::WHILE);

    auto condition_start = Start();

    // Parse the condition expression into the loop scope.
    auto condition = Condition(*loop_scope);

    // Create the instruction with the condition and the jump location
    // If the condition is NOT zero (true) it will jump to the loop_scope's entry block,
    // otherwise it will continue, exiting the loop
    auto result = Instruct<node::instruction::control::jump::NotZero>(*loop_scope, condition_start, condition, &loop_scope->EntryBlock());
    result->End(cbegin());

    // // In this language it is valid to leave the condition blank
    // if (condition) {

    // } else {
    //   // If there is no condition, create an unconditional jump to keep looping
    //   auto result = Instruct<node::instruction::control::jump::Absolute>(*loop_scope, condition_start, &loop_scope->EntryBlock());
    //   result->End(cbegin());
    // }

    Expect(lexical::Tokens::SEMICOLON);
    End(*loop_scope);

    return loop_scope;
  }

  node::scope::conditional::pattern::Match* Parser::MatchStatement(node::Scope& scope) {
    if (!Match(lexical::Tokens::MATCH) && !Match(lexical::Tokens::COVER)) {
      throw utility::Error("Expected either MATCH or COVER keyword");
    }

    // Expect(lexical::Tokens::MATCH);

    auto* match_scope = Scope<node::scope::conditional::pattern::Match>(scope, Start());

    // Parse the condition expression into the new scope
    std::shared_ptr<node::Declaration> condition = Condition(*match_scope);
    if (!condition) throw utility::Error("Match statement expected a condition");

    Expect(lexical::Tokens::SCOPE_OPEN);

    node::scope::conditional::pattern::Default* default_scope{nullptr};

    while (true) {
      if (Done()) {
        throw ErrorUnexpectedEnd("Match statement", lexical::Tokens::SCOPE_CLOSE);
      } else if (Match(lexical::Tokens::SCOPE_CLOSE)) {
        break;
      } else if (Check(lexical::Tokens::IS)) {
        node::scope::conditional::pattern::Is* is_scope = IsStatement(*match_scope);

        // Add the last instruction to jump to the end of the match scope
        Instruct<node::instruction::control::jump::Absolute>(*is_scope, Start(), &match_scope->ExitBlock());
      } else if (Check(lexical::Tokens::DEFAULT)) {
        if (default_scope) {
          throw utility::Error("A match statement cannot contain multiple default statements");
        }

        default_scope = DefaultStatement(*match_scope);

        // Add the last instruction to jump to the end of the match scope
        Instruct<node::instruction::control::jump::Absolute>(*default_scope, Start(), &match_scope->ExitBlock());
      } else {
        throw utility::Error("Unknown token in match statement of", Peek());
      }
    }

    if (default_scope) {
      // Add an unconditional jump to the default, ensuring it is last
      Instruct<node::instruction::control::jump::Absolute>(*match_scope, Start(), &default_scope->EntryBlock());
    }

    Match(lexical::Tokens::SEMICOLON);
    End(*match_scope);

    return match_scope;
  }

  node::scope::conditional::pattern::Is* Parser::IsStatement(node::Scope& scope) {
    Expect(lexical::Tokens::IS);

    auto start = Start();

    // Parse the condition into the parent scope (the match scope)
    std::shared_ptr<node::Declaration> condition = Condition(scope);
    if (!condition) throw utility::Error("Is statement expected a condition");

    auto* is_scope = Scope<node::scope::conditional::pattern::Is>(scope, Start());
    auto result = Instruct<node::instruction::control::jump::NotZero>(scope, start, condition, &is_scope->EntryBlock());
    result->End(cbegin());

    // Fallthrough, effectively making a logical OR
    while (Match(lexical::Tokens::IS)) {
      if (Done()) {
        throw ErrorUnexpectedEnd("Is statement", lexical::Tokens::IS);
      }

      auto start = Start();

      // Continue using the parent scope for the conditions
      std::shared_ptr<node::Declaration> condition = Condition(scope);
      if (!condition) throw utility::Error("Is statement expected a condition");

      auto result = Instruct<node::instruction::control::jump::NotZero>(scope, start, condition, &is_scope->EntryBlock());
      result->End(cbegin());
    }

    auto statement = Statement(*is_scope);
    if (!statement) throw utility::Error("Is statement expected a statement after the condition");

    End(*is_scope);
    return is_scope;
  }

  node::scope::conditional::pattern::Default* Parser::DefaultStatement(node::Scope& scope) {
    auto start = Start();
    Expect(lexical::Tokens::DEFAULT);
    auto* default_scope = Scope<node::scope::conditional::pattern::Default>(scope, start);

    auto statement = Statement(*default_scope);
    if (!statement) throw utility::Error("Default statement expected a statement after the condition");

    // default_scope->End(cbegin());
    End(*default_scope);

    return default_scope;
  }

  node::Scope* Parser::CoverStatement(node::Scope& scope) {
    return MatchStatement(scope); // For now this is handling both
  }

  node::scope::conditional::loop::For* Parser::ForStatement(node::Scope& scope) {
    auto start = Start();

    Expect(lexical::Tokens::FOR);

    auto* for_scope = Scope<node::scope::conditional::loop::For>(scope, start);

    Expect(lexical::Tokens::CONDITION_OPEN);

    std::shared_ptr<node::Declaration> declaration = DeclarationExpression(*for_scope);
    if (!declaration) {
      throw utility::Error("For statement expected a declaration after the opening '('");
    }

    Expect(lexical::Tokens::IN);

    auto expression = Expression(*for_scope);
    if (!expression) {
      throw utility::Error("For statement expected an expression after the 'in' keyword");
    }

    Expect(lexical::Tokens::CONDITION_CLOSE);

    auto statement = Statement(*for_scope);
    if (!statement) throw utility::Error("For statement expected a statement after the condition");

    End(*for_scope);

    return for_scope;
  }

  node::Scope* Parser::ImportStatement(node::Scope& scope) {
    Expect(lexical::Tokens::IMPORT);
    throw utility::Error("ImportStatement isn't implemented yet");

    return nullptr;
  }

  node::Scope* Parser::CaseStatement(node::Scope& scope) {
    auto start = Start();
    Expect(lexical::Tokens::CASE);

    if (!Check(lexical::Tokens::SEMICOLON)) {
      auto result = Expression(scope);
      auto instruction = Instruct<node::instruction::control::Case>(scope, start, std::move(result));
      instruction->End(cbegin());
    }

    Expect(lexical::Tokens::SEMICOLON);

    return &scope;
  }

  node::Scope* Parser::ReturnStatement(node::Scope& scope) {
    auto start = Start();
    Expect(lexical::Tokens::RETURN);

    // while (Match(lexical::Tokens::ASYNC) || Match(lexical::Tokens::CASE) || Match(lexical::Tokens::YIELD)) {
    //   // TODO: Implement
    // }

    if (!Check(lexical::Tokens::SEMICOLON)) {
      auto result = Expression(scope);
      auto instruction = Instruct<node::instruction::control::Return>(scope, start, std::move(result));
      instruction->End(cbegin());
    }

    Expect(lexical::Tokens::SEMICOLON);

    return &scope;
  }

  bool Parser::ParseDeclarationModifier(code::Modifiers& modifiers) {
    while (Valid()) {
      switch (Peek().Type()) {
        case lexical::Tokens::LET: modifiers.Let(true); return true;
        case lexical::Tokens::CONST: modifiers.Const(true); return true;
        case lexical::Tokens::MUTABLE: modifiers.Mutable(true); return true;
        case lexical::Tokens::STATIC: modifiers.Static(true); return true;
        case lexical::Tokens::PRIVATE: modifiers.Private(true); return true;
        case lexical::Tokens::PUBLIC: modifiers.Public(true); return true;
        case lexical::Tokens::PROTECTED: modifiers.Protected(true); return true;
        case lexical::Tokens::EXPORT: modifiers.Export(true); return true;
        case lexical::Tokens::COMTIME: modifiers.Comtime(true); return true;
        case lexical::Tokens::RUNTIME: modifiers.Runtime(true); return true;
        default: return false;
      }
    }

    return false;
  }

  code::Modifiers Parser::ParseDeclarationModifiers() {
    code::Modifiers modifiers;

    while (ParseDeclarationModifier(modifiers)) {
      Advance();
    }

    return modifiers;
  }

  std::shared_ptr<node::Declaration> Parser::DeclarationExpression(node::Scope& scope) {
    return nullptr;

    // if (!cursor.IsDeclarable()) {
    //   // utility::Print("Token", Peek(), "isn't declarable");
    //   return nullptr;
    // }

    // auto start = Start();

    // if (Check(lexical::Tokens::SPREAD)) {
    //   return Expression(scope);
    // }

    // auto declaration = Declare<node::declaration::Typed>(scope, start);

    // while (true) {
    //   auto modifier = ParseDeclarationModifier(scope);
    //   if (modifier == node::declaration::Modifiers::NONE) break;

    //   declaration->SetModifier(modifier);
    //   Advance();
    // }

    // auto construct_start = Start();
    // if (Match(lexical::Tokens::ENUM_OPEN)) {
    //   while (Valid()) {
    //     auto* link = Expression(scope);
    //     declaration->Link(link);

    //     if (!Match(lexical::Tokens::COMMA)) {
    //       break;
    //     }
    //   }

    //   Expect(lexical::Tokens::ENUM_CLOSE);
    // } else {
    //   auto span = Start();
    //   Expect(lexical::Tokens::IDENTIFIER);

    //   declaration->Name(span.View());
    // }

    // if (Match(lexical::Tokens::AS)) {
    //   auto span = Start();
    //   Expect(lexical::Tokens::IDENTIFIER);

    //   declaration->Name(span.View());
    // }

    // if (Match(lexical::Tokens::TYPE_START)) {
    //   declaration->Type(Expression(scope));
    // }

    // Match(lexical::Tokens::ASSIGN);

    // auto* value = Expression(scope);
    // scope.Slide(declaration->Ctor()); // Place the constructor after the RHS's constructor
    // if (value) scope.Slide(value->Dtor()); // Move the RHS's destructor down
    // declaration->Value(value);

    // End(declaration);
    // End(construct_start);
    // scope.Slide(declaration->Dtor()); // Move the destructor down after the RHS's destructor

    // Push(declaration);

    // // Instruct<node::instruction::memory::Construct>(scope, construct_start, declaration, value);
    // // Instruct<node::instruction::memory::Destruct>(scope, construct_start, declaration);
    // // scope.OffsetDeclaration(declaration);
    
    // return declaration;
  }

  std::shared_ptr<node::Declaration> Parser::DeclarationStatement(node::Scope& scope) {
    std::shared_ptr<node::Declaration> declaration = DeclarationExpression(scope);
    if (!declaration) throw utility::Error("Expected a declaration");

    Expect(lexical::Tokens::SEMICOLON);
    return declaration;
  }

  std::shared_ptr<node::Declaration> Parser::ExpressionStatement(node::Scope& scope) {
    std::shared_ptr<node::Declaration> declaration = Expression(scope);
    if (!declaration) throw utility::Error("Expected an expression");

    Expect(lexical::Tokens::SEMICOLON);
    return declaration;
  }

  Node* Parser::Statement(node::Scope& scope) {
    switch (Peek().Type()) {
      case lexical::Tokens::SCOPE_OPEN: return ScopeStatement(scope);
      case lexical::Tokens::BREAK: return BreakStatement(scope);
      case lexical::Tokens::CONTINUE: return ContinueStatement(scope);
      case lexical::Tokens::COVER: return CoverStatement(scope);
      // case lexical::Tokens::DEFAULT: return DefaultStatement(scope);
      case lexical::Tokens::DO: return DoStatement(scope);
      // case lexical::Tokens::EXPORT: return ExportStatement(scope);
      case lexical::Tokens::FOR: return ForStatement(scope);
      case lexical::Tokens::IF: return IfStatement(scope);
      // case lexical::Tokens::IMPORT: return ImportStatement(scope);
      // case lexical::Tokens::IS: return IsStatement(scope);
      case lexical::Tokens::MATCH: return MatchStatement(scope);
      case lexical::Tokens::CASE: return CaseStatement(scope);
      case lexical::Tokens::RETURN: return ReturnStatement(scope);
      case lexical::Tokens::WHILE: return WhileStatement(scope);

      case lexical::Tokens::LET:
      case lexical::Tokens::CONST:
      case lexical::Tokens::MUTABLE:
      case lexical::Tokens::STATIC:
      case lexical::Tokens::PRIVATE:
      case lexical::Tokens::PUBLIC:
      case lexical::Tokens::PROTECTED:
      case lexical::Tokens::EXPORT: return DeclarationStatement(scope).get();
      default: return ExpressionStatement(scope).get();
    }

    return nullptr;
  }
};