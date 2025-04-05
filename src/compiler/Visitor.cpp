import visitor;

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
import node._module;
import node._module.importer;
import node._module.transformer;
import node.declaration;
import node.declaration.automatic;
import node.declaration.forward;
import node.declaration.spread;
import node.declaration.typed;
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
import node.instruction.binary.logical._and;
import node.instruction.binary.logical._or;
import node.instruction.binary.logical.equal;
import node.instruction.binary.logical.not_equal;
import node.instruction.binary.logical.greater;
import node.instruction.binary.logical.greater_or_equal;
import node.instruction.binary.logical.lesser;
import node.instruction.binary.logical.lesser_or_equal;
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

void Visitor::Next(std::unique_ptr<Node>& node) { if (node) node->Accept(*this); }
void Visitor::Next(Node& node) { node.Accept(*this); }
void Visitor::Next(Node* node) { if (node) node->Accept(*this); }

void Visitor::Next(std::vector<std::unique_ptr<Node>>& nodes) {
  for (auto& node : nodes) {
    Next(node);
  }
}

void Visitor::Next(std::vector<Node*>& nodes) {
  for (auto* node : nodes) {
    Next(node);
  }
}

void Visitor::Visit(node::Block& block) {}
void Visitor::Visit(node::Compiler& compiler) {}
void Visitor::Visit(node::Scope& scope) {}
void Visitor::Visit(node::scope::Conditional& conditional) {}
void Visitor::Visit(node::scope::conditional::If& _if) {}
void Visitor::Visit(node::scope::conditional::Else& _else) {}
void Visitor::Visit(node::scope::conditional::ElseIf& _else_if) {}
void Visitor::Visit(node::scope::conditional::Loop& loop) {}
void Visitor::Visit(node::scope::conditional::loop::Do& _do) {}
void Visitor::Visit(node::scope::conditional::loop::While& _while) {}
void Visitor::Visit(node::scope::conditional::loop::For& _for) {}
void Visitor::Visit(node::scope::conditional::Pattern& pattern) {}
void Visitor::Visit(node::scope::conditional::pattern::Cover& cover) {}
void Visitor::Visit(node::scope::conditional::pattern::Match& match) {}
void Visitor::Visit(node::scope::conditional::pattern::Default& _default) {}
void Visitor::Visit(node::scope::conditional::pattern::Is& is) {}
void Visitor::Visit(node::scope::Context& context) {}
void Visitor::Visit(node::scope::context::Module& _module) {}
void Visitor::Visit(node::scope::context::Function& function) {}
void Visitor::Visit(node::scope::context::function::Inline& inline_function) {}
void Visitor::Visit(node::Module& _module) {}
void Visitor::Visit(node::_module::Importer& importer) {}
void Visitor::Visit(node::_module::Transformer& transformer) {}
void Visitor::Visit(node::Declaration& declaration) {}
void Visitor::Visit(node::declaration::Automatic& automatic) {}
void Visitor::Visit(node::declaration::Forward& forward) {}
void Visitor::Visit(node::declaration::Spread& spread) {}
void Visitor::Visit(node::declaration::Typed& typed) {}
void Visitor::Visit(node::declaration::rvalue::Literal& literal) {}
void Visitor::Visit(node::declaration::rvalue::literal::Primitive& primitive) {}
void Visitor::Visit(node::declaration::rvalue::literal::primitive::Boolean& boolean) {}
void Visitor::Visit(node::declaration::rvalue::literal::primitive::Character& character) {}
void Visitor::Visit(node::declaration::rvalue::literal::primitive::Null& null) {}
void Visitor::Visit(node::declaration::rvalue::literal::primitive::Undefined& undefined) {}
void Visitor::Visit(node::declaration::rvalue::literal::primitive::Numeric& numeric) {}
void Visitor::Visit(node::declaration::rvalue::literal::primitive::numeric::Decimal& decimal) {}
void Visitor::Visit(node::declaration::rvalue::literal::primitive::numeric::Integer& integer) {}
void Visitor::Visit(node::declaration::rvalue::literal::primitive::numeric::Number& number) {}
void Visitor::Visit(node::declaration::rvalue::literal::Structured& structured) {}
void Visitor::Visit(node::declaration::rvalue::literal::structured::Array& array) {}
void Visitor::Visit(node::declaration::rvalue::literal::structured::Enum& _enum) {}
void Visitor::Visit(node::declaration::rvalue::literal::structured::Object& object) {}
void Visitor::Visit(node::declaration::rvalue::literal::structured::String& string) {}
void Visitor::Visit(node::declaration::rvalue::literal::structured::TemplateString& template_string) {}
void Visitor::Visit(node::declaration::rvalue::literal::structured::Tuple& tuple) {}
void Visitor::Visit(node::declaration::rvalue::literal::structured::Function& function) {}
void Visitor::Visit(node::Instruction& instruction) {}
void Visitor::Visit(node::instruction::Memory& memory) {}
void Visitor::Visit(node::instruction::memory::Construct& construct) {}
void Visitor::Visit(node::instruction::memory::Destruct& destruct) {}
void Visitor::Visit(node::instruction::memory::Move& move) {}
void Visitor::Visit(node::instruction::memory::Store& store) {}
void Visitor::Visit(node::instruction::memory::Load& load) {}
void Visitor::Visit(node::instruction::Control& control) {}
void Visitor::Visit(node::instruction::control::Return& _return) {}
void Visitor::Visit(node::instruction::control::Continue& _continue) {}
void Visitor::Visit(node::instruction::control::Break& _break) {}
void Visitor::Visit(node::instruction::control::Jump& jump) {}
void Visitor::Visit(node::instruction::control::jump::Absolute& absolute) {}
void Visitor::Visit(node::instruction::control::jump::Relative& relative) {}
void Visitor::Visit(node::instruction::control::jump::Equal& equal) {}
void Visitor::Visit(node::instruction::control::jump::Zero& zero) {}
void Visitor::Visit(node::instruction::control::jump::NotEqual& not_equal) {}
void Visitor::Visit(node::instruction::control::jump::NotZero& not_zero) {}
void Visitor::Visit(node::instruction::control::jump::Greater& greater) {}
void Visitor::Visit(node::instruction::control::jump::GreaterOrEqual& greater_or_equal) {}
void Visitor::Visit(node::instruction::control::jump::Less& less) {}
void Visitor::Visit(node::instruction::control::jump::LessOrEqual& less_or_equal) {}
void Visitor::Visit(node::instruction::control::jump::Sign& sign) {}
void Visitor::Visit(node::instruction::control::jump::NotSign& not_sign) {}
void Visitor::Visit(node::instruction::control::jump::Above& above) {}
void Visitor::Visit(node::instruction::control::jump::AboveOrEqual& above_or_equal) {}
void Visitor::Visit(node::instruction::control::jump::Below& below) {}
void Visitor::Visit(node::instruction::control::jump::BelowOrEqual& below_or_equal) {}
void Visitor::Visit(node::instruction::Unary& unary) {}
void Visitor::Visit(node::instruction::unary::BitwiseNot& bitwise_not) {}
void Visitor::Visit(node::instruction::unary::Copy& copy) {}
void Visitor::Visit(node::instruction::unary::Reference& reference) {}
void Visitor::Visit(node::instruction::unary::Spread& spread) {}
void Visitor::Visit(node::instruction::unary::Optional& optional) {}
void Visitor::Visit(node::instruction::unary::Symbol& symbol) {}
void Visitor::Visit(node::instruction::unary::Length& length) {}
void Visitor::Visit(node::instruction::unary::Virtual& _virtual) {}
void Visitor::Visit(node::instruction::unary::Not& _not) {}
void Visitor::Visit(node::instruction::unary::Increment& increment) {}
void Visitor::Visit(node::instruction::unary::Decrement& decrement) {}
void Visitor::Visit(node::instruction::unary::Positive& positive) {}
void Visitor::Visit(node::instruction::unary::Negative& negative) {}
void Visitor::Visit(node::instruction::unary::Await& await) {}
void Visitor::Visit(node::instruction::unary::Expected& expected) {}
void Visitor::Visit(node::instruction::unary::Unexpected& unexpected) {}
void Visitor::Visit(node::instruction::unary::Yield& yield) {}
void Visitor::Visit(node::instruction::unary::Postfix& postfix) {}
void Visitor::Visit(node::instruction::unary::postfix::Increment& increment) {}
void Visitor::Visit(node::instruction::unary::postfix::Decrement& decrement) {}

void Visitor::Visit(node::instruction::Binary& binary) {}

void Visitor::Visit(node::instruction::binary::Assert& assert) {}
void Visitor::Visit(node::instruction::binary::assert::Equal& equal) {}
void Visitor::Visit(node::instruction::binary::assert::NotEqual& not_equal) {}
void Visitor::Visit(node::instruction::binary::assert::GreaterOrEqual& greater_or_equal) {}
void Visitor::Visit(node::instruction::binary::assert::LesserOrEqual& lesser_or_equal) {}

void Visitor::Visit(node::instruction::binary::Nullish& nullish) {}

void Visitor::Visit(node::instruction::binary::logical::And& _and) {}
void Visitor::Visit(node::instruction::binary::logical::Or& _or) {}
void Visitor::Visit(node::instruction::binary::logical::Equal& equal) {}
void Visitor::Visit(node::instruction::binary::logical::NotEqual& not_equal) {}
void Visitor::Visit(node::instruction::binary::logical::Greater& greater) {}
void Visitor::Visit(node::instruction::binary::logical::GreaterOrEqual& greater_or_equal) {}
void Visitor::Visit(node::instruction::binary::logical::Lesser& lesser) {}
void Visitor::Visit(node::instruction::binary::logical::LesserOrEqual& lesser_or_equal) {}

void Visitor::Visit(node::instruction::binary::math::Add& add) {}
void Visitor::Visit(node::instruction::binary::math::Subtract& subtract) {}
void Visitor::Visit(node::instruction::binary::math::Multiply& multiply) {}
void Visitor::Visit(node::instruction::binary::math::Divide& divide) {}
void Visitor::Visit(node::instruction::binary::math::Modulo& modulo) {}
void Visitor::Visit(node::instruction::binary::math::Exponent& exponent) {}

void Visitor::Visit(node::instruction::binary::Assign& assign) {}
void Visitor::Visit(node::instruction::binary::assign::Add& add) {}
void Visitor::Visit(node::instruction::binary::assign::Subtract& subtract) {}
void Visitor::Visit(node::instruction::binary::assign::Multiply& multiply) {}
void Visitor::Visit(node::instruction::binary::assign::Divide& divide) {}
void Visitor::Visit(node::instruction::binary::assign::Modulo& modulo) {}
void Visitor::Visit(node::instruction::binary::assign::Exponent& exponent) {}

void Visitor::Visit(node::instruction::binary::Access& access) {}
void Visitor::Visit(node::instruction::binary::access::Member& member) {}
void Visitor::Visit(node::instruction::binary::access::member::Static& _static) {}
void Visitor::Visit(node::instruction::binary::access::member::Optional& optional) {}

void Visitor::Visit(node::instruction::binary::Bitwise& bitwise) {}
void Visitor::Visit(node::instruction::binary::bitwise::And& _and) {}
void Visitor::Visit(node::instruction::binary::bitwise::Or& _or) {}
void Visitor::Visit(node::instruction::binary::bitwise::Xor& _xor) {}
void Visitor::Visit(node::instruction::binary::bitwise::Shift& shift) {}
void Visitor::Visit(node::instruction::binary::bitwise::shift::Left& left) {}
void Visitor::Visit(node::instruction::binary::bitwise::shift::Right& right) {}
void Visitor::Visit(node::instruction::binary::bitwise::shift::Triple& triple) {}
void Visitor::Visit(node::instruction::binary::bitwise::shift::triple::Left& left) {}
void Visitor::Visit(node::instruction::binary::bitwise::shift::triple::Right& right) {}
void Visitor::Visit(node::instruction::binary::bitwise::Assign& assign) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::And& _and) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::Xor& _xor) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::Or& _or) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::Shift& shift) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::shift::Left& left) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::shift::Right& right) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::shift::Triple& triple) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::shift::triple::Left& left) {}
void Visitor::Visit(node::instruction::binary::bitwise::assign::shift::triple::Right& right) {}

void Visitor::Visit(node::instruction::binary::Range& range) {}
void Visitor::Visit(node::instruction::binary::range::Exclusive& exclusive) {}
void Visitor::Visit(node::instruction::binary::range::Inclusive& inclusive) {}

void Visitor::Visit(node::instruction::binary::Call& call) {}
void Visitor::Visit(node::instruction::binary::call::Character& character) {}
void Visitor::Visit(node::instruction::binary::call::Decimal& decimal) {}
void Visitor::Visit(node::instruction::binary::call::Integer& integer) {}
void Visitor::Visit(node::instruction::binary::call::Number& number) {}
void Visitor::Visit(node::instruction::binary::call::Array& array) {}
void Visitor::Visit(node::instruction::binary::call::Enum& _enum) {}
void Visitor::Visit(node::instruction::binary::call::Object& object) {}
void Visitor::Visit(node::instruction::binary::call::String& string) {}
void Visitor::Visit(node::instruction::binary::call::TemplateString& template_string) {}
void Visitor::Visit(node::instruction::binary::call::Tuple& tuple) {}
void Visitor::Visit(node::instruction::binary::call::Function& function) {}
void Visitor::Visit(node::instruction::TernaryCondition& ternary_condition) {}
