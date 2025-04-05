import visitor.formatter;
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

import <memory>;

namespace visitor {
  void Formatter::Visit(node::Block& n) {
    Line("block");

    auto& instructions = n.Instructions();
    if (!instructions.empty()) {
      In(" {");
      Next(instructions);
      Out();
    }
  }

  void Formatter::Visit(node::Compiler& compiler) {}
  void Formatter::Visit(node::Scope& scope) {}
  void Formatter::Visit(node::scope::Conditional& conditional) {}
  void Formatter::Visit(node::scope::conditional::If& _if) {}
  void Formatter::Visit(node::scope::conditional::Else& _else) {}
  void Formatter::Visit(node::scope::conditional::ElseIf& _else_if) {}
  void Formatter::Visit(node::scope::conditional::Loop& loop) {}
  void Formatter::Visit(node::scope::conditional::loop::Do& _do) {}
  void Formatter::Visit(node::scope::conditional::loop::While& _while) {}
  void Formatter::Visit(node::scope::conditional::loop::For& _for) {}
  void Formatter::Visit(node::scope::conditional::Pattern& pattern) {}
  void Formatter::Visit(node::scope::conditional::pattern::Cover& cover) {}
  void Formatter::Visit(node::scope::conditional::pattern::Match& match) {}
  void Formatter::Visit(node::scope::conditional::pattern::Default& _default) {}
  void Formatter::Visit(node::scope::conditional::pattern::Is& is) {}
  void Formatter::Visit(node::scope::Context& context) {}
  void Formatter::Visit(node::scope::context::Module& n) { Line("module ", n.Specifier(), " "); In(); Next(n.Nodes()); Out(); }
  void Formatter::Visit(node::scope::context::Function& n) { Line("function "); Next(n.Nodes()); }
  void Formatter::Visit(node::scope::context::function::Inline& n) { Line("inline function "); Next(n.Nodes()); }
  void Formatter::Visit(node::Declaration& declaration) {}
  
  void Formatter::Visit(node::declaration::Automatic& n) {
    // auto* value = n.Value();
    // if (value) {
    //   Add("auto ", value->Name());
    //   // Next(value);
    // }
  }

  void Formatter::Visit(node::declaration::Forward& n) {}
  void Formatter::Visit(node::declaration::Spread& n) {}

  void Formatter::Visit(node::declaration::Typed& n) {
    auto* type = n.Type();
    auto* parent = n.Parent();
    auto* value = n.Value();

    Line("Typed: ");

    if (type) Add(type->Name(), " ");
    // else Add("auto ");

    auto name = n.Name();

    if (parent) {
      Add("[", parent->Name(), "] ");
      if (!name.empty()) {
        Add("as ", name, " ");
      }
    }
    else if (!name.empty()) {
      Add(name, " ");
    }
    else {
      Add("(anonymous) ");
    }

    if (value) {
      Add("= ");
      // Next(n.Value());
    }
  }

  void Formatter::Visit(node::declaration::Literal& n) {}
  void Formatter::Visit(node::declaration::literal::Primitive& n) {}
  void Formatter::Visit(node::declaration::literal::primitive::Boolean& n) { Add(n.Value()); }
  void Formatter::Visit(node::declaration::literal::primitive::Character& n) { Add(n.Value()); }
  void Formatter::Visit(node::declaration::literal::primitive::Null& n) { Add("null"); }
  void Formatter::Visit(node::declaration::literal::primitive::Undefined& n) { Add("undefined"); }
  void Formatter::Visit(node::declaration::literal::primitive::Numeric& numeric) {}
  void Formatter::Visit(node::declaration::literal::primitive::numeric::Decimal& n) { Add(n.Value()); }
  void Formatter::Visit(node::declaration::literal::primitive::numeric::Integer& n) { Add(n.Value()); }
  void Formatter::Visit(node::declaration::literal::primitive::numeric::Number& n) { Add(n.Value()); }
  void Formatter::Visit(node::declaration::literal::Structured& n) {}
  void Formatter::Visit(node::declaration::literal::structured::Array& n) { Each("[, ]", n.Values()); }
  void Formatter::Visit(node::declaration::literal::structured::Enum& n) { Each("<, >", n.Values()); }
  void Formatter::Visit(node::declaration::literal::structured::Object& n) { Each("{, }", n.Values()); }
  void Formatter::Visit(node::declaration::literal::structured::String& n) { Add('"', n.Value(), '"'); }
  void Formatter::Visit(node::declaration::literal::structured::TemplateString& n) {}
  void Formatter::Visit(node::declaration::literal::structured::Tuple& n) { Each("(, )", n.Values()); }
  void Formatter::Visit(node::declaration::literal::structured::Function& n) {  }
  void Formatter::Visit(node::Instruction& n) {}
  void Formatter::Visit(node::instruction::Memory& n) {}

  void Formatter::Visit(node::instruction::memory::Construct& n) {
    Line("Construct");

    auto* target = n.Target();
    auto* value = n.Value();
    if (target) Add(" ", target->Name());
    if (value) Next(value);
  }

  void Formatter::Visit(node::instruction::memory::Destruct& n) {
    Line("Destruct");

    auto* target = n.Target();
    if (target) Add(" ", target->Name());
  }

  void Formatter::Visit(node::instruction::memory::Move& n) { Line("Move"); }
  void Formatter::Visit(node::instruction::memory::Store& n) { Line("Store"); }
  void Formatter::Visit(node::instruction::memory::Load& n) { Line("Load"); }
  void Formatter::Visit(node::instruction::Control& control) {}
  void Formatter::Visit(node::instruction::control::Return& n) { Line("Return"); }
  void Formatter::Visit(node::instruction::control::Continue& n) { Line("Continue"); }
  void Formatter::Visit(node::instruction::control::Break& n) { Line("Break"); }
  void Formatter::Visit(node::instruction::control::Jump& n) { Line("Jump"); }
  void Formatter::Visit(node::instruction::control::jump::Absolute& n) { Line("Absolute"); }
  void Formatter::Visit(node::instruction::control::jump::Relative& n) { Line("Relative"); }
  void Formatter::Visit(node::instruction::control::jump::Equal& n) { Line("Equal"); }
  void Formatter::Visit(node::instruction::control::jump::Zero& n) { Line("Zero"); }
  void Formatter::Visit(node::instruction::control::jump::NotEqual& n) { Line("NotEqual"); }
  void Formatter::Visit(node::instruction::control::jump::NotZero& n) { Line("NotZero"); }
  void Formatter::Visit(node::instruction::control::jump::Greater& n) { Line("Greater"); }
  void Formatter::Visit(node::instruction::control::jump::GreaterOrEqual& n) { Line("GreaterOrEqual"); }
  void Formatter::Visit(node::instruction::control::jump::Less& n) { Line("Less"); }
  void Formatter::Visit(node::instruction::control::jump::LessOrEqual& n) { Line("LessOrEqual"); }
  void Formatter::Visit(node::instruction::control::jump::Sign& n) { Line("Sign"); }
  void Formatter::Visit(node::instruction::control::jump::NotSign& n) { Line("NotSign"); }
  void Formatter::Visit(node::instruction::control::jump::Above& n) { Line("Above"); }
  void Formatter::Visit(node::instruction::control::jump::AboveOrEqual& n) { Line("AboveOrEqual"); }
  void Formatter::Visit(node::instruction::control::jump::Below& n) { Line("Below"); }
  void Formatter::Visit(node::instruction::control::jump::BelowOrEqual& n) { Line("BelowOrEqual"); }
  void Formatter::Visit(node::instruction::Unary& n) {}
  void Formatter::Visit(node::instruction::unary::BitwiseNot& n) {}
  void Formatter::Visit(node::instruction::unary::Copy& n) {}
  void Formatter::Visit(node::instruction::unary::Reference& n) {}
  void Formatter::Visit(node::instruction::unary::Spread& n) {}
  void Formatter::Visit(node::instruction::unary::Optional& n) {}
  void Formatter::Visit(node::instruction::unary::Symbol& n) {}
  void Formatter::Visit(node::instruction::unary::Length& n) {}
  void Formatter::Visit(node::instruction::unary::Virtual& n) {}
  void Formatter::Visit(node::instruction::unary::Not& n) {}
  void Formatter::Visit(node::instruction::unary::Increment& n) {}
  void Formatter::Visit(node::instruction::unary::Decrement& n) {}
  void Formatter::Visit(node::instruction::unary::Positive& n) {}
  void Formatter::Visit(node::instruction::unary::Negative& n) {}
  void Formatter::Visit(node::instruction::unary::Await& n) {}
  void Formatter::Visit(node::instruction::unary::Expected& n) {}
  void Formatter::Visit(node::instruction::unary::Unexpected& n) {}
  void Formatter::Visit(node::instruction::unary::Yield& n) {}
  void Formatter::Visit(node::instruction::unary::Postfix& n) {}
  void Formatter::Visit(node::instruction::unary::postfix::Increment& n) {}
  void Formatter::Visit(node::instruction::unary::postfix::Decrement& n) {}
  void Formatter::Visit(node::instruction::Binary& n) {}
  void Formatter::Visit(node::instruction::binary::And& n) {}
  void Formatter::Visit(node::instruction::binary::Or& n) {}
  void Formatter::Visit(node::instruction::binary::Nullish& n) { Line("Nullish"); }
  void Formatter::Visit(node::instruction::binary::Equal& n) { Line("Equal"); }
  void Formatter::Visit(node::instruction::binary::NotEqual& n) { Line("NotEqual"); }
  void Formatter::Visit(node::instruction::binary::Assert& n) { Line("Assert"); }
  void Formatter::Visit(node::instruction::binary::assert::Equal& n) {}
  void Formatter::Visit(node::instruction::binary::assert::NotEqual& n) {}
  void Formatter::Visit(node::instruction::binary::assert::GreaterOrEqual& n) {}
  void Formatter::Visit(node::instruction::binary::assert::LesserOrEqual& n) {}
  void Formatter::Visit(node::instruction::binary::Greater& n) { Line("Greater"); }
  void Formatter::Visit(node::instruction::binary::GreaterOrEqual& n) { Line("GreaterOrEqual"); }
  void Formatter::Visit(node::instruction::binary::Lesser& n) { Line("Lesser"); }
  void Formatter::Visit(node::instruction::binary::LesserOrEqual& n) { Line("LesserOrEqual"); }
  void Formatter::Visit(node::instruction::binary::Add& n) { Line("Add"); }
  void Formatter::Visit(node::instruction::binary::Subtract& n) { Line("Subtract"); }
  void Formatter::Visit(node::instruction::binary::Multiply& n) { Line("Multiply"); }
  void Formatter::Visit(node::instruction::binary::Divide& n) { Line("Divide"); }
  void Formatter::Visit(node::instruction::binary::Modulo& n) { Line("Modulo"); }
  void Formatter::Visit(node::instruction::binary::Exponent& n) { Line("Exponent"); }
  void Formatter::Visit(node::instruction::binary::Assign& n) { Line("Assign"); }
  void Formatter::Visit(node::instruction::binary::assign::Add& n) {}
  void Formatter::Visit(node::instruction::binary::assign::Subtract& n) {}
  void Formatter::Visit(node::instruction::binary::assign::Multiply& n) {}
  void Formatter::Visit(node::instruction::binary::assign::Divide& n) {}
  void Formatter::Visit(node::instruction::binary::assign::Modulo& n) {}
  void Formatter::Visit(node::instruction::binary::assign::Exponent& n) {}
  void Formatter::Visit(node::instruction::binary::Access& n) {}
  void Formatter::Visit(node::instruction::binary::access::Member& n) {}
  void Formatter::Visit(node::instruction::binary::access::member::Static& n) {}
  void Formatter::Visit(node::instruction::binary::access::member::Optional& n) {}
  void Formatter::Visit(node::instruction::binary::Bitwise& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::And& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::Or& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::Xor& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::Shift& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::shift::Left& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::shift::Right& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::shift::Triple& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::shift::triple::Left& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::shift::triple::Right& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::Assign& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::And& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::Xor& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::Or& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::Shift& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::shift::Left& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::shift::Right& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::shift::Triple& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::shift::triple::Left& n) {}
  void Formatter::Visit(node::instruction::binary::bitwise::assign::shift::triple::Right& n) {}
  void Formatter::Visit(node::instruction::binary::Range& n) {}
  void Formatter::Visit(node::instruction::binary::range::Exclusive& n) {}
  void Formatter::Visit(node::instruction::binary::range::Inclusive& n) {}
  void Formatter::Visit(node::instruction::binary::Call& n) {}
  void Formatter::Visit(node::instruction::binary::call::Character& n) {}
  void Formatter::Visit(node::instruction::binary::call::Decimal& n) {}
  void Formatter::Visit(node::instruction::binary::call::Integer& n) {}
  void Formatter::Visit(node::instruction::binary::call::Number& n) {}
  void Formatter::Visit(node::instruction::binary::call::Array& n) {}
  void Formatter::Visit(node::instruction::binary::call::Enum& n) {}
  void Formatter::Visit(node::instruction::binary::call::Object& n) {}
  void Formatter::Visit(node::instruction::binary::call::String& n) {}
  void Formatter::Visit(node::instruction::binary::call::TemplateString& n) {}
  void Formatter::Visit(node::instruction::binary::call::Tuple& n) {}
  void Formatter::Visit(node::instruction::binary::call::Function& n) {}
  void Formatter::Visit(node::instruction::TernaryCondition& n) {}
  void Formatter::Visit(node::instruction::Phi& n) {}
};