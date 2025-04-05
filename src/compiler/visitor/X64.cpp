import visitor.x64;
// import code.x64.encoder;
import utility.print;

import node;
import node.block;
import node.compiler;
import node.scope;
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
import node.declaration;
import node.declaration.automatic;
import node.declaration.forward;
import node.declaration.spread;
import node.declaration.typed;
import node.declaration.literal;
import node.declaration.literal.primitive;
import node.declaration.literal.primitive.boolean;
import node.declaration.literal.primitive.character;
import node.declaration.literal.primitive.null;
import node.declaration.literal.primitive.undefined;
import node.declaration.literal.primitive.numeric;
import node.declaration.literal.primitive.numeric.decimal;
import node.declaration.literal.primitive.numeric.integer;
import node.declaration.literal.primitive.numeric.number;
import node.declaration.literal.structured;
import node.declaration.literal.structured.array;
import node.declaration.literal.structured._enum;
import node.declaration.literal.structured.object;
import node.declaration.literal.structured.string;
import node.declaration.literal.structured.template_string;
import node.declaration.literal.structured.tuple;
import node.declaration.literal.structured.function;
import node.instruction;
import node.instruction.memory;
import node.instruction.memory.construct;
import node.instruction.memory.destruct;
import node.instruction.memory.move;
import node.instruction.memory.store;
import node.instruction.memory.load;
import node.instruction.control;
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
import node.instruction.binary._and;
import node.instruction.binary._or;
import node.instruction.binary.nullish;
import node.instruction.binary.equal;
import node.instruction.binary.not_equal;
import node.instruction.binary.assert;
import node.instruction.binary.assert.equal;
import node.instruction.binary.assert.not_equal;
import node.instruction.binary.assert.greater_or_equal;
import node.instruction.binary.assert.lesser_or_equal;
import node.instruction.binary.greater;
import node.instruction.binary.greater_or_equal;
import node.instruction.binary.lesser;
import node.instruction.binary.lesser_or_equal;
import node.instruction.binary.add;
import node.instruction.binary.subtract;
import node.instruction.binary.multiply;
import node.instruction.binary.divide;
import node.instruction.binary.modulo;
import node.instruction.binary.exponent;
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
import node.instruction.phi;

import <typeinfo>;

using NUMBER_T = node::declaration::literal::primitive::numeric::Number;
using INTEGER_T = node::declaration::literal::primitive::numeric::Integer;
using DECIMAL_T = node::declaration::literal::primitive::numeric::Decimal;
using BOOL_T = node::declaration::literal::primitive::Boolean;
using CHARACTER_T = node::declaration::literal::primitive::Character;
using _NULL_T = node::declaration::literal::primitive::Null;
using UNDEFINED_T = node::declaration::literal::primitive::Undefined;

constexpr auto& NUMBER = typeid(NUMBER_T);
constexpr auto& INTEGER = typeid(INTEGER_T);
constexpr auto& DECIMAL = typeid(DECIMAL_T);
constexpr auto& BOOL = typeid(BOOL_T);
constexpr auto& CHARACTER = typeid(CHARACTER_T);
constexpr auto& _NULL = typeid(_NULL_T);
constexpr auto& UNDEFINED = typeid(UNDEFINED_T);

namespace visitor {
  // code::x64::Encoder& X64::Encoder() { return encoder; }

  void X64::Visit(node::Block& n) {
    // for (auto& uptr : n.Instructions()) {
    //   uptr->Accept(*this);
    // }
  }

  void X64::Visit(node::Compiler& compiler) {}

  void X64::Visit(node::Scope& scope) {
    // for (auto& uptr : scope.Nodes()) {
    //   if (uptr && typeid(uptr) == typeid(node::Block*)) {
    //     uptr->Accept(*this);
    //   }
    // }
  }

  void X64::Visit(node::scope::Conditional& conditional) {}
  void X64::Visit(node::scope::conditional::If& _if) {}
  void X64::Visit(node::scope::conditional::Else& _else) {}
  void X64::Visit(node::scope::conditional::ElseIf& _else_if) {}
  void X64::Visit(node::scope::conditional::Loop& loop) {}
  void X64::Visit(node::scope::conditional::loop::Do& _do) {}
  void X64::Visit(node::scope::conditional::loop::While& _while) {}
  void X64::Visit(node::scope::conditional::loop::For& _for) {}
  void X64::Visit(node::scope::conditional::Pattern& pattern) {}
  void X64::Visit(node::scope::conditional::pattern::Cover& cover) {}
  void X64::Visit(node::scope::conditional::pattern::Match& match) {}
  void X64::Visit(node::scope::conditional::pattern::Default& _default) {}
  void X64::Visit(node::scope::conditional::pattern::Is& is) {}
  void X64::Visit(node::scope::Context& context) {}
  void X64::Visit(node::scope::context::Module& n) { X64::Visit(static_cast<node::Scope&>(n)); }
  void X64::Visit(node::scope::context::Function& n) { X64::Visit(static_cast<node::Scope&>(n)); }
  void X64::Visit(node::scope::context::function::Inline& n) { X64::Visit(static_cast<node::Scope&>(n)); }
  void X64::Visit(node::Declaration& declaration) {}
  
  void X64::Visit(node::declaration::Automatic& n) {
  }

  void X64::Visit(node::declaration::Forward& n) {}
  void X64::Visit(node::declaration::Spread& n) {}

  void X64::Visit(node::declaration::Typed& n) {
    auto* type = n.Type();
    auto* parent = n.Parent();
    auto* value = n.Value();
  }

  void X64::Visit(node::declaration::Literal& n) {}
  void X64::Visit(node::declaration::literal::Primitive& n) {}
  void X64::Visit(node::declaration::literal::primitive::Boolean& n) {}
  void X64::Visit(node::declaration::literal::primitive::Character& n) {}
  void X64::Visit(node::declaration::literal::primitive::Null& n) {}
  void X64::Visit(node::declaration::literal::primitive::Undefined& n) {}
  void X64::Visit(node::declaration::literal::primitive::Numeric& numeric) {}
  void X64::Visit(node::declaration::literal::primitive::numeric::Decimal& n) {}
  void X64::Visit(node::declaration::literal::primitive::numeric::Integer& n) {}
  void X64::Visit(node::declaration::literal::primitive::numeric::Number& n) {}
  void X64::Visit(node::declaration::literal::Structured& n) {}
  void X64::Visit(node::declaration::literal::structured::Array& n) {}
  void X64::Visit(node::declaration::literal::structured::Enum& n) {}
  void X64::Visit(node::declaration::literal::structured::Object& n) {}
  void X64::Visit(node::declaration::literal::structured::String& n) {}
  void X64::Visit(node::declaration::literal::structured::TemplateString& n) {}
  void X64::Visit(node::declaration::literal::structured::Tuple& n) {}
  void X64::Visit(node::declaration::literal::structured::Function& n) {}
  void X64::Visit(node::Instruction& n) {}
  void X64::Visit(node::instruction::Memory& n) {}

  void X64::Visit(node::instruction::memory::Construct& n) {
    // auto* value = n.Value();
    // if (!value) throw utility::Error("Construct instruction was expected to have a value");

    // auto& type = typeid(*value);
    // if (type == NUMBER) {
    //   encoder.MOV_RAX_IMM64(static_cast<NUMBER_T*>(value)->Value());
    //   encoder.PUSH_RAX(); // Store it on the stack, this is now our "constructed" value
    // } else if (type == INTEGER) {
    //   // TODO
    // } else if (type == DECIMAL) {
    //   // TODO
    // }
  }

  void X64::Visit(node::instruction::memory::Destruct& n) {
    auto size = n.Size();
  }

  // void X64::Visit(node::instruction::memory::Destruct& n) {
  //   auto size = n.Size();
  //   // encoder.
  //   // Line("Destruct");

  //   // auto* target = n.Target();
  //   // if (target) Add(" ", target->Name());
  // }

  void X64::Visit(node::instruction::memory::Move& n) {}
  void X64::Visit(node::instruction::memory::Store& n) {}
  void X64::Visit(node::instruction::memory::Load& n) {}
  void X64::Visit(node::instruction::Control& control) {}
  void X64::Visit(node::instruction::control::Return& n) {}
  void X64::Visit(node::instruction::control::Continue& n) {}
  void X64::Visit(node::instruction::control::Break& n) {}
  void X64::Visit(node::instruction::control::Jump& n) {}
  void X64::Visit(node::instruction::control::jump::Absolute& n) {}
  void X64::Visit(node::instruction::control::jump::Relative& n) {}
  void X64::Visit(node::instruction::control::jump::Equal& n) {}
  void X64::Visit(node::instruction::control::jump::Zero& n) {}
  void X64::Visit(node::instruction::control::jump::NotEqual& n) {}
  void X64::Visit(node::instruction::control::jump::NotZero& n) {}
  void X64::Visit(node::instruction::control::jump::Greater& n) {}
  void X64::Visit(node::instruction::control::jump::GreaterOrEqual& n) {}
  void X64::Visit(node::instruction::control::jump::Less& n) {}
  void X64::Visit(node::instruction::control::jump::LessOrEqual& n) {}
  void X64::Visit(node::instruction::control::jump::Sign& n) {}
  void X64::Visit(node::instruction::control::jump::NotSign& n) {}
  void X64::Visit(node::instruction::control::jump::Above& n) {}
  void X64::Visit(node::instruction::control::jump::AboveOrEqual& n) {}
  void X64::Visit(node::instruction::control::jump::Below& n) {}
  void X64::Visit(node::instruction::control::jump::BelowOrEqual& n) {}
  void X64::Visit(node::instruction::Unary& n) {}
  void X64::Visit(node::instruction::unary::BitwiseNot& n) {}
  void X64::Visit(node::instruction::unary::Copy& n) {}
  void X64::Visit(node::instruction::unary::Reference& n) {}
  void X64::Visit(node::instruction::unary::Spread& n) {}
  void X64::Visit(node::instruction::unary::Optional& n) {}
  void X64::Visit(node::instruction::unary::Symbol& n) {}
  void X64::Visit(node::instruction::unary::Length& n) {}
  void X64::Visit(node::instruction::unary::Virtual& n) {}
  void X64::Visit(node::instruction::unary::Not& n) {}
  void X64::Visit(node::instruction::unary::Increment& n) {}
  void X64::Visit(node::instruction::unary::Decrement& n) {}
  void X64::Visit(node::instruction::unary::Positive& n) {}
  void X64::Visit(node::instruction::unary::Negative& n) {}
  void X64::Visit(node::instruction::unary::Await& n) {}
  void X64::Visit(node::instruction::unary::Expected& n) {}
  void X64::Visit(node::instruction::unary::Unexpected& n) {}
  void X64::Visit(node::instruction::unary::Yield& n) {}
  void X64::Visit(node::instruction::unary::Postfix& n) {}
  void X64::Visit(node::instruction::unary::postfix::Increment& n) {}
  void X64::Visit(node::instruction::unary::postfix::Decrement& n) {}
  void X64::Visit(node::instruction::Binary& n) {}
  void X64::Visit(node::instruction::binary::And& n) {}
  void X64::Visit(node::instruction::binary::Or& n) {}
  void X64::Visit(node::instruction::binary::Nullish& n) {}
  void X64::Visit(node::instruction::binary::Equal& n) {}
  void X64::Visit(node::instruction::binary::NotEqual& n) {}
  void X64::Visit(node::instruction::binary::Assert& n) {}
  void X64::Visit(node::instruction::binary::assert::Equal& n) {}
  void X64::Visit(node::instruction::binary::assert::NotEqual& n) {}
  void X64::Visit(node::instruction::binary::assert::GreaterOrEqual& n) {}
  void X64::Visit(node::instruction::binary::assert::LesserOrEqual& n) {}
  void X64::Visit(node::instruction::binary::Greater& n) {}
  void X64::Visit(node::instruction::binary::GreaterOrEqual& n) {}
  void X64::Visit(node::instruction::binary::Lesser& n) {}
  void X64::Visit(node::instruction::binary::LesserOrEqual& n) {}

  void X64::Visit(node::instruction::binary::Add& n) {
    // auto* lhs = n.LHS();
    // auto* rhs = n.RHS();

    // if (!lhs) throw utility::Error("Add instruction was expected to have a LHS");
    // if (!rhs) throw utility::Error("Add instruction was expected to have a RHS");

    // auto& a = typeid(*lhs);
    // auto& b = typeid(*rhs);

    // if (a == NUMBER && b == NUMBER) {

    // }
  }

  void X64::Visit(node::instruction::binary::Subtract& n) {}
  void X64::Visit(node::instruction::binary::Multiply& n) {}
  void X64::Visit(node::instruction::binary::Divide& n) {}
  void X64::Visit(node::instruction::binary::Modulo& n) {}
  void X64::Visit(node::instruction::binary::Exponent& n) {}
  void X64::Visit(node::instruction::binary::Assign& n) {}
  void X64::Visit(node::instruction::binary::assign::Add& n) {}
  void X64::Visit(node::instruction::binary::assign::Subtract& n) {}
  void X64::Visit(node::instruction::binary::assign::Multiply& n) {}
  void X64::Visit(node::instruction::binary::assign::Divide& n) {}
  void X64::Visit(node::instruction::binary::assign::Modulo& n) {}
  void X64::Visit(node::instruction::binary::assign::Exponent& n) {}
  void X64::Visit(node::instruction::binary::Access& n) {}
  void X64::Visit(node::instruction::binary::access::Member& n) {}
  void X64::Visit(node::instruction::binary::access::member::Static& n) {}
  void X64::Visit(node::instruction::binary::access::member::Optional& n) {}
  void X64::Visit(node::instruction::binary::Bitwise& n) {}
  void X64::Visit(node::instruction::binary::bitwise::And& n) {}
  void X64::Visit(node::instruction::binary::bitwise::Or& n) {}
  void X64::Visit(node::instruction::binary::bitwise::Xor& n) {}
  void X64::Visit(node::instruction::binary::bitwise::Shift& n) {}
  void X64::Visit(node::instruction::binary::bitwise::shift::Left& n) {}
  void X64::Visit(node::instruction::binary::bitwise::shift::Right& n) {}
  void X64::Visit(node::instruction::binary::bitwise::shift::Triple& n) {}
  void X64::Visit(node::instruction::binary::bitwise::shift::triple::Left& n) {}
  void X64::Visit(node::instruction::binary::bitwise::shift::triple::Right& n) {}
  void X64::Visit(node::instruction::binary::bitwise::Assign& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::And& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::Xor& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::Or& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::Shift& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::shift::Left& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::shift::Right& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::shift::Triple& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::shift::triple::Left& n) {}
  void X64::Visit(node::instruction::binary::bitwise::assign::shift::triple::Right& n) {}
  void X64::Visit(node::instruction::binary::Range& n) {}
  void X64::Visit(node::instruction::binary::range::Exclusive& n) {}
  void X64::Visit(node::instruction::binary::range::Inclusive& n) {}
  void X64::Visit(node::instruction::binary::Call& n) {}
  void X64::Visit(node::instruction::binary::call::Character& n) {}
  void X64::Visit(node::instruction::binary::call::Decimal& n) {}
  void X64::Visit(node::instruction::binary::call::Integer& n) {}
  void X64::Visit(node::instruction::binary::call::Number& n) {}
  void X64::Visit(node::instruction::binary::call::Array& n) {}
  void X64::Visit(node::instruction::binary::call::Enum& n) {}
  void X64::Visit(node::instruction::binary::call::Object& n) {}
  void X64::Visit(node::instruction::binary::call::String& n) {}
  void X64::Visit(node::instruction::binary::call::TemplateString& n) {}
  void X64::Visit(node::instruction::binary::call::Tuple& n) {}
  void X64::Visit(node::instruction::binary::call::Function& n) {}
  void X64::Visit(node::instruction::TernaryCondition& n) {}
  void X64::Visit(node::instruction::Phi& n) {}
}