export module lexical.parser;

import utility.print;
import lexical.source;
import lexical.source.span;
import lexical.tokens;
import lexical.token;
import lexical.cursor.token;
import lexical.text;

import code.modifiers;

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
// import node._module;
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
import node.instruction.binary.nullish;
import node.instruction.binary.assert;
import node.instruction.binary.assert.equal;
import node.instruction.binary.assert.not_equal;
import node.instruction.binary.assert.greater_or_equal;
import node.instruction.binary.assert.lesser_or_equal;
import node.instruction.binary.logical._and;
import node.instruction.binary.logical._or;
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
  export class Parser {
  private:
    node::scope::context::Module& mod;
    lexical::Source& source;
    lexical::cursor::Token cursor;
    // std::unordered_map<node::Declaration*, node::Declaration*> roots;
    std::unordered_map<std::string_view, std::string> identifiers;
    std::vector<std::shared_ptr<node::Declaration>> declarations;
    std::vector<node::instruction::memory::Destruct*> destructors;
  public:
    constexpr bool Done() const { return cursor.Done(); }
    constexpr bool Valid() const { return cursor.Valid(); }
  protected:
    void Advance(size_t n = 1);
    // constexpr void Retreat(size_t n = 1) { cursor.Retreat(n); }
    // constexpr void Retreat(std::vector<lexical::Token>::const_iterator iter) { cursor.Retreat(iter); }
    // constexpr decltype(auto) Peek() const { return cursor.Peek(); }
    decltype(auto) Peek(size_t n = 0) const { return cursor.Peek(n); }
    constexpr bool Check(const lexical::Tokens type) const { return cursor.Check(type); }
    constexpr bool Match(const lexical::Tokens type) { return cursor.Match(type); }
    constexpr decltype(auto) cbegin() const { return cursor.cbegin(); }
    constexpr decltype(auto) cend() const { return cursor.cend(); }

    void Push(std::shared_ptr<node::Declaration> declaration, node::instruction::memory::Destruct* destructor);
    void Push(std::shared_ptr<node::Declaration>&& declaration, node::instruction::memory::Destruct* destructor);

    lexical::source::Span Start();
    void End(lexical::source::Span& span);
    void End(Node& node);
    void End(Node* node);
    template<typename T>
    void End(std::shared_ptr<T>& node) { End(node.get()); }
    void End(node::Scope* scope);

    void End(node::Scope& scope);

    template<typename T, typename... Args>
    T* Scope(node::Scope& scope, const lexical::source::Span& span, Args&&... args) {
      // utility::Print("Adding declaration", ::GetName<T>(), args...);

      auto uptr = std::make_unique<T>(span, scope, std::forward<Args>(args)...);
      T* ptr = uptr.get();
      scope.Add(std::move(uptr));

      // // Save all the function scopes
      // if constexpr (std::same_as<T, node::scope::context::Function>) {
      //   mod.AddFunction(ptr);
      // }

      return ptr;
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> Declare(node::Scope& scope, const lexical::source::Span& span, Args&&... args) {
      // utility::Print("Adding declaration", typeid(T), args...);

      auto sptr = std::make_shared<T>(span, scope, std::forward<Args>(args)...);
      // stack.push_back(sptr);
      // T* ptr = sptr.get();
      // scope.Add(sptr);

      auto* ctor = Instruct<node::instruction::memory::Construct>(scope, span, sptr, nullptr);
      auto* dtor = Instruct<node::instruction::memory::Destruct>(scope, span, sptr);
      sptr->Ctor(ctor);
      sptr->Dtor(dtor);

      return sptr;
    }

    template<typename T, typename... Args>
    T* Instruct(node::Scope& scope, const lexical::source::Span& span, Args&&... args) {
      node::Block& last_block = scope.RequiredLastBlock();

      if (last_block.IsTerminated()) {
        last_block.End(cbegin()); // End the last block where the next one begins
        auto new_block_uptr = std::make_unique<node::Block>(span);
        node::Block& new_block = *new_block_uptr;
        scope.Add(std::move(new_block_uptr));

        return (new_block.Instruct<T>(span, std::forward<Args>(args)...));
      } else {
        return (last_block.Instruct<T>(span, std::forward<Args>(args)...));
      }
    }

    std::string_view Normalized(const std::string_view view);
    std::shared_ptr<node::Declaration> Find(const std::string_view name);
    // std::shared_ptr<node::Declaration> Find(std::shared_ptr<node::Declaration> parent);
    // std::shared_ptr<node::Declaration> Find(const std::string_view name, std::shared_ptr<node::Declaration> parent);
  public:
    Parser(node::scope::context::Module& mod, lexical::Source& source);

    // auto Error(std::string message) const {
    //   return std::runtime_error{message};
    // }

    std::runtime_error ErrorUnexpectedEnd(std::string_view message, lexical::Token token = {}) const;
    std::runtime_error ErrorUnexpectedToken(std::string_view message, lexical::Token token = {}) const;
    std::runtime_error ErrorExpectedToken(std::string_view message, lexical::Token token) const;

    void Expect(lexical::Tokens type);
    std::shared_ptr<node::declaration::rvalue::literal::primitive::Null> NullLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::primitive::Undefined> UndefinedLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::primitive::Boolean> TrueLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::primitive::Boolean> FalseLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::primitive::numeric::Number> NumberLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::primitive::numeric::Integer> IntegerLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::primitive::numeric::Decimal> DecimalLiteral(node::Scope& scope);
    std::string ParseText(node::Scope& scope, std::string_view caller, lexical::Tokens end1, lexical::Tokens end2 = lexical::Tokens::NONE);
    std::shared_ptr<node::declaration::rvalue::literal::primitive::Character> CharLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::structured::String> StringLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::structured::Tuple> TupleLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::structured::TemplateString> TemplateStringLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::structured::Array> ArrayLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> ObjectLiteral(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::structured::Enum> EnumLiteral(node::Scope& scope);
    void ParseParameters(node::Scope& scope, std::shared_ptr<node::declaration::rvalue::literal::structured::Function>& function);
    void ParseCaptures(node::Scope& scope, std::shared_ptr<node::declaration::rvalue::literal::structured::Function>& function);
    std::vector<Node*> ParseScopeBlock(node::Scope& scope);
    node::Scope* ScopeStatement(node::Scope& scope);
    std::shared_ptr<node::declaration::rvalue::literal::structured::Function> FunctionLiteral(node::Scope& scope);
    std::shared_ptr<node::Declaration> Identifier(node::Scope& scope);
    std::shared_ptr<node::Declaration> Value(node::Scope& scope);

    node::instruction::Unary* MakeUnaryInstruction(node::Scope& scope, const Token& token, auto&&... args) {
      using enum lexical::Tokens;

      switch (token.Type()) {
        case NOT: return Instruct<node::instruction::unary::Not>(scope, std::forward<decltype(args)>(args)...);
        case INCREMENT: return Instruct<node::instruction::unary::Increment>(scope, std::forward<decltype(args)>(args)...);
        case DECREMENT: return Instruct<node::instruction::unary::Decrement>(scope, std::forward<decltype(args)>(args)...);
        case POSITIVE: return Instruct<node::instruction::unary::Positive>(scope, std::forward<decltype(args)>(args)...);
        case NEGATIVE: return Instruct<node::instruction::unary::Negative>(scope, std::forward<decltype(args)>(args)...);
        case COPY: return Instruct<node::instruction::unary::Copy>(scope, std::forward<decltype(args)>(args)...);
        case REFERENCE: return Instruct<node::instruction::unary::Reference>(scope, std::forward<decltype(args)>(args)...);
        // case OPTIONAL_REFERENCE: return Instruct<node::instruction::unary::OptionalReference>(scope, std::forward<decltype(args)>(args)...);
        // case BORROW: return Instruct<node::instruction::unary::Borrow>(scope, std::forward<decltype(args)>(args)...);
        case SPREAD: return Instruct<node::instruction::unary::Spread>(scope, std::forward<decltype(args)>(args)...);
        case OPTIONAL: return Instruct<node::instruction::unary::Optional>(scope, std::forward<decltype(args)>(args)...);
        case SYMBOL: return Instruct<node::instruction::unary::Symbol>(scope, std::forward<decltype(args)>(args)...);
        case VIRTUAL: return Instruct<node::instruction::unary::Virtual>(scope, std::forward<decltype(args)>(args)...);
        // case MOVE: return Instruct<node::instruction::unary::Move>(scope, std::forward<decltype(args)>(args)...);

        default: throw std::runtime_error("Failed to generate a unary instruction from token " + token.ToString());
      }

      return nullptr;
    }

    node::instruction::Binary* MakeBinaryInstruction(node::Scope& scope, const Token& token, auto&&... args) {
      using enum lexical::Tokens;

      switch (token.Type()) {
        case ASSERT_EQUAL: return Instruct<node::instruction::binary::assert::Equal>(scope, args...);
        case ASSERT_NOT_EQUAL: return Instruct<node::instruction::binary::assert::NotEqual>(scope, args...);
        case ASSERT_GREATER_OR_EQUAL: return Instruct<node::instruction::binary::assert::GreaterOrEqual>(scope, args...);
        case ASSERT_LESSER_OR_EQUAL: return Instruct<node::instruction::binary::assert::LesserOrEqual>(scope, args...);
        case AND: return Instruct<node::instruction::binary::logical::And>(scope, args...);
        case OR: return Instruct<node::instruction::binary::logical::Or>(scope, args...);
        case EQUAL: return Instruct<node::instruction::binary::logical::Equal>(scope, args...);
        case NOT_EQUAL: return Instruct<node::instruction::binary::logical::NotEqual>(scope, args...);
        case GREATER: return Instruct<node::instruction::binary::logical::Greater>(scope, args...);
        case GREATER_OR_EQUAL: return Instruct<node::instruction::binary::logical::GreaterOrEqual>(scope, args...);
        case LESSER: return Instruct<node::instruction::binary::logical::Lesser>(scope, args...);
        case LESSER_OR_EQUAL: return Instruct<node::instruction::binary::logical::LesserOrEqual>(scope, args...);

        case ASSIGN: return Instruct<node::instruction::binary::Assign>(scope, args...);
        case ASSIGN_ADD: return Instruct<node::instruction::binary::assign::Add>(scope, args...);
        case ASSIGN_SUBTRACT: return Instruct<node::instruction::binary::assign::Subtract>(scope, args...);
        case ASSIGN_MULTIPLY: return Instruct<node::instruction::binary::assign::Multiply>(scope, args...);
        case ASSIGN_DIVIDE: return Instruct<node::instruction::binary::assign::Divide>(scope, args...);
        case ASSIGN_MODULO: return Instruct<node::instruction::binary::assign::Modulo>(scope, args...);
        case ASSIGN_EXPONENT: return Instruct<node::instruction::binary::assign::Exponent>(scope, args...);
        case ADD: return Instruct<node::instruction::binary::math::Add>(scope, args...);
        case SUBTRACT: return Instruct<node::instruction::binary::math::Subtract>(scope, args...);
        case MULTIPLY: return Instruct<node::instruction::binary::math::Multiply>(scope, args...);
        case DIVIDE: return Instruct<node::instruction::binary::math::Divide>(scope, args...);
        case MODULO: return Instruct<node::instruction::binary::math::Modulo>(scope, args...);
        case EXPONENT: return Instruct<node::instruction::binary::math::Exponent>(scope, args...);
        case USE: return Instruct<node::instruction::binary::Use>(scope, args...);

        case MEMBER_ACCESS: return Instruct<node::instruction::binary::access::Member>(scope, args...);
        case STATIC_MEMBER_ACCESS: return Instruct<node::instruction::binary::access::member::Static>(scope, args...);

        case TUPLE_OPEN: return Instruct<node::instruction::binary::call::Tuple>(scope, args...);
        case ARRAY_OPEN: return Instruct<node::instruction::binary::call::Array>(scope, args...);
        case OBJECT_OPEN: return Instruct<node::instruction::binary::call::Object>(scope, args...);
        case ENUM_OPEN: return Instruct<node::instruction::binary::call::Enum>(scope, args...);
        case TEMPLATE_STRING_OPEN: return Instruct<node::instruction::binary::call::TemplateString>(scope, args...);

        default: throw std::runtime_error("Failed to generate a binary instruction from token");
      }

      return nullptr;
    }

    std::shared_ptr<node::Declaration> Expression(node::Scope& scope, int precedence = 0);
    node::Scope* BreakStatement(node::Scope& scope);
    node::Scope* ContinueStatement(node::Scope& scope);
    std::shared_ptr<node::Declaration> Condition(node::Scope& scope);
    node::scope::conditional::If* IfStatement(node::Scope& scope);
    node::scope::Conditional* ElseStatement(node::Scope& scope);
    node::scope::conditional::loop::While* WhileStatement(node::Scope& scope);
    node::scope::conditional::loop::Do* DoStatement(node::Scope& scope);
    node::scope::conditional::pattern::Match* MatchStatement(node::Scope& scope);
    node::scope::conditional::pattern::Is* IsStatement(node::Scope& scope);
    node::scope::conditional::pattern::Default* DefaultStatement(node::Scope& scope);
    node::Scope* CoverStatement(node::Scope& scope);
    node::scope::conditional::loop::For* ForStatement(node::Scope& scope);
    node::Scope* ImportStatement(node::Scope& scope);
    node::Scope* CaseStatement(node::Scope& scope);
    node::Scope* ReturnStatement(node::Scope& scope);
    bool ParseDeclarationModifier(code::Modifiers& modifiers);
    code::Modifiers ParseDeclarationModifiers();
    std::shared_ptr<node::Declaration> DeclarationExpression(node::Scope& scope);
    std::shared_ptr<node::Declaration> DeclarationStatement(node::Scope& scope);
    std::shared_ptr<node::Declaration> ExpressionStatement(node::Scope& scope);
    Node* Statement(node::Scope& scope);
  };
};