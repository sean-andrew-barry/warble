export module visitor;

import <memory>;
import <vector>;
import <ranges>;

export class Node;

export namespace node {
  class Block;
  class Compiler;
  class Scope;
  class Module;
  class Declaration;
  class Instruction;
}

namespace node {
  namespace scope {
    export class Conditional;
    export class Context;

    namespace conditional {
      export class If;
      export class Else;
      export class ElseIf;
      export class Loop;
      export class Pattern;

      namespace loop {
        export class Do;
        export class While;
        export class For;
      };

      namespace pattern {
        export class Cover;
        export class Match;
        export class Default;
        export class Is;
      };
    };

    namespace context {
      export class Module;
      export class Function;

      namespace function {
        export class Inline;
      };
    };
  };
};

export namespace node::_module {
  class Importer;
  class Transformer;
}

export namespace node::declaration {
  class Automatic;
  class Forward;
  class Spread;
  class Typed;
  class RValue;
  class LValue;
}

export namespace node::declaration::rvalue {
  class Literal;
  class Temporary;
}

export namespace node::declaration::lvalue {
  class Named;
  class Linked;
  class Phi;
}

export namespace node::declaration::rvalue::literal {
  class Primitive;
  class Structured;
}

export namespace node::declaration::rvalue::literal::primitive {
  class Boolean;
  class Character;
  class Null;
  class Undefined;
  class Numeric;
}

export namespace node::declaration::rvalue::literal::primitive::numeric {
  class Decimal;
  class Integer;
  class Number;
}

export namespace node::declaration::rvalue::literal::structured {
  class Array;
  class Enum;
  class Object;
  class String;
  class TemplateString;
  class Tuple;
  class Function;
}

export namespace node::instruction {
  class Memory;
  class Control;
  class Unary;
  class Binary;
  class TernaryCondition;
  class Phi;
}

export namespace node::instruction::memory {
  class Construct;
  class Destruct;
  class Move;
  class Store;
  class Load;
}

export namespace node::instruction::control {
  class Return;
  class Continue;
  class Break;
  class Jump;
}

export namespace node::instruction::control::jump {
  class Absolute;
  class Relative;
  class Equal;
  class Zero;
  class NotEqual;
  class NotZero;
  class Greater;
  class GreaterOrEqual;
  class Less;
  class LessOrEqual;
  class Sign;
  class NotSign;
  class Above;
  class AboveOrEqual;
  class Below;
  class BelowOrEqual;
}

export namespace node::instruction::unary {
  class BitwiseNot;
  class Copy;
  class Reference;
  class Spread;
  class Optional;
  class Symbol;
  class Length;
  class Virtual;
  class Not;
  class Increment;
  class Decrement;
  class Positive;
  class Negative;
  class Await;
  class Expected;
  class Unexpected;
  class Yield;
  class Postfix;
}

export namespace node::instruction::unary::postfix {
  class Increment;
  class Decrement;
}

export namespace node::instruction::binary {
  class Assert;
  class Assign;
  class Access;
  class Bitwise;
  class Range;
  class Call;
  class Nullish;
}

export namespace node::instruction::binary::logical {
  class And;
  class Or;
  class Equal;
  class NotEqual;
  class Assert;
  class Greater;
  class GreaterOrEqual;
  class Lesser;
  class LesserOrEqual;
}

export namespace node::instruction::binary::math {
  class Add;
  class Subtract;
  class Multiply;
  class Divide;
  class Modulo;
  class Exponent;
}

export namespace node::instruction::binary::assert {
  class Equal;
  class NotEqual;
  class GreaterOrEqual;
  class LesserOrEqual;
}

export namespace node::instruction::binary::assign {
  class Add;
  class Subtract;
  class Multiply;
  class Divide;
  class Modulo;
  class Exponent;
}

export namespace node::instruction::binary::access {
  class Member;
}

export namespace node::instruction::binary::access::member {
  class Static;
  class Optional;
}

export namespace node::instruction::binary::bitwise {
  class And;
  class Or;
  class Xor;
  class Shift;
  class Assign;
}

export namespace node::instruction::binary::bitwise::shift {
  class Left;
  class Right;
  class Triple;
}

export namespace node::instruction::binary::bitwise::shift::triple {
  class Left;
  class Right;
}

export namespace node::instruction::binary::bitwise::assign {
  class And;
  class Xor;
  class Or;
  class Shift;
}

export namespace node::instruction::binary::bitwise::assign::shift {
  class Left;
  class Right;
  class Triple;
}

export namespace node::instruction::binary::bitwise::assign::shift::triple {
  class Left;
  class Right;
}

export namespace node::instruction::binary::range {
  class Exclusive;
  class Inclusive;
}

export namespace node::instruction::binary::call {
  class Character;
  class Decimal;
  class Integer;
  class Number;
  class Array;
  class Enum;
  class Object;
  class String;
  class TemplateString;
  class Tuple;
  class Function;
}

export class Visitor {
private:
public:
  virtual ~Visitor() = default;

  void Next(std::ranges::range auto const& nodes) {
    for (auto& node : nodes) {
      node->Accept(*this);
    }
  }

  void Next(std::unique_ptr<Node>& node);
  void Next(Node& node);
  void Next(Node* node);
  void Next(std::vector<std::unique_ptr<Node>>& nodes);
  void Next(std::vector<Node*>& nodes);

  virtual void Visit(node::Block& block);
  virtual void Visit(node::Compiler& compiler);
  virtual void Visit(node::Scope& scope);
  virtual void Visit(node::scope::Conditional& conditional);
  virtual void Visit(node::scope::conditional::If& _if);
  virtual void Visit(node::scope::conditional::Else& _else);
  virtual void Visit(node::scope::conditional::ElseIf& _else_if);
  virtual void Visit(node::scope::conditional::Loop& loop);
  virtual void Visit(node::scope::conditional::loop::Do& _do);
  virtual void Visit(node::scope::conditional::loop::While& _while);
  virtual void Visit(node::scope::conditional::loop::For& _for);
  virtual void Visit(node::scope::conditional::Pattern& pattern);
  virtual void Visit(node::scope::conditional::pattern::Cover& cover);
  virtual void Visit(node::scope::conditional::pattern::Match& match);
  virtual void Visit(node::scope::conditional::pattern::Default& _default);
  virtual void Visit(node::scope::conditional::pattern::Is& is);
  virtual void Visit(node::scope::Context& context);
  virtual void Visit(node::scope::context::Module& _module);
  virtual void Visit(node::scope::context::Function& function);
  virtual void Visit(node::scope::context::function::Inline& inline_function);
  virtual void Visit(node::Module& _module);
  virtual void Visit(node::Declaration& declaration);
  virtual void Visit(node::Instruction& instruction);
  virtual void Visit(node::_module::Importer& importer);
  virtual void Visit(node::_module::Transformer& transformer);
  virtual void Visit(node::declaration::Automatic& automatic);
  virtual void Visit(node::declaration::Forward& forward);
  virtual void Visit(node::declaration::Spread& spread);
  virtual void Visit(node::declaration::Typed& typed);
  virtual void Visit(node::declaration::rvalue::Literal& literal);
  virtual void Visit(node::declaration::rvalue::literal::Primitive& primitive);
  virtual void Visit(node::declaration::rvalue::literal::Structured& structured);
  virtual void Visit(node::declaration::rvalue::literal::primitive::Boolean& boolean);
  virtual void Visit(node::declaration::rvalue::literal::primitive::Character& character);
  virtual void Visit(node::declaration::rvalue::literal::primitive::Null& null);
  virtual void Visit(node::declaration::rvalue::literal::primitive::Undefined& undefined);
  virtual void Visit(node::declaration::rvalue::literal::primitive::Numeric& numeric);
  virtual void Visit(node::declaration::rvalue::literal::primitive::numeric::Decimal& decimal);
  virtual void Visit(node::declaration::rvalue::literal::primitive::numeric::Integer& integer);
  virtual void Visit(node::declaration::rvalue::literal::primitive::numeric::Number& number);
  virtual void Visit(node::declaration::rvalue::literal::structured::Array& array);
  virtual void Visit(node::declaration::rvalue::literal::structured::Enum& _enum);
  virtual void Visit(node::declaration::rvalue::literal::structured::Object& object);
  virtual void Visit(node::declaration::rvalue::literal::structured::String& string);
  virtual void Visit(node::declaration::rvalue::literal::structured::TemplateString& template_string);
  virtual void Visit(node::declaration::rvalue::literal::structured::Tuple& tuple);
  virtual void Visit(node::declaration::rvalue::literal::structured::Function& function);
  virtual void Visit(node::instruction::Memory& memory);
  virtual void Visit(node::instruction::Control& control);
  virtual void Visit(node::instruction::Unary& unary);
  virtual void Visit(node::instruction::Binary& binary);
  virtual void Visit(node::instruction::memory::Construct& construct);
  virtual void Visit(node::instruction::memory::Destruct& destruct);
  virtual void Visit(node::instruction::memory::Move& move);
  virtual void Visit(node::instruction::memory::Store& store);
  virtual void Visit(node::instruction::memory::Load& load);
  virtual void Visit(node::instruction::control::Return& _return);
  virtual void Visit(node::instruction::control::Continue& _continue);
  virtual void Visit(node::instruction::control::Break& _break);
  virtual void Visit(node::instruction::control::Jump& jump);
  virtual void Visit(node::instruction::control::jump::Absolute& absolute);
  virtual void Visit(node::instruction::control::jump::Relative& relative);
  virtual void Visit(node::instruction::control::jump::Equal& equal);
  virtual void Visit(node::instruction::control::jump::Zero& zero);
  virtual void Visit(node::instruction::control::jump::NotEqual& not_equal);
  virtual void Visit(node::instruction::control::jump::NotZero& not_zero);
  virtual void Visit(node::instruction::control::jump::Greater& greater);
  virtual void Visit(node::instruction::control::jump::GreaterOrEqual& greater_or_equal);
  virtual void Visit(node::instruction::control::jump::Less& less);
  virtual void Visit(node::instruction::control::jump::LessOrEqual& less_or_equal);
  virtual void Visit(node::instruction::control::jump::Sign& sign);
  virtual void Visit(node::instruction::control::jump::NotSign& not_sign);
  virtual void Visit(node::instruction::control::jump::Above& above);
  virtual void Visit(node::instruction::control::jump::AboveOrEqual& above_or_equal);
  virtual void Visit(node::instruction::control::jump::Below& below);
  virtual void Visit(node::instruction::control::jump::BelowOrEqual& below_or_equal);
  virtual void Visit(node::instruction::unary::BitwiseNot& bitwise_not);
  virtual void Visit(node::instruction::unary::Copy& copy);
  virtual void Visit(node::instruction::unary::Reference& reference);
  virtual void Visit(node::instruction::unary::Spread& spread);
  virtual void Visit(node::instruction::unary::Optional& optional);
  virtual void Visit(node::instruction::unary::Symbol& symbol);
  virtual void Visit(node::instruction::unary::Length& length);
  virtual void Visit(node::instruction::unary::Virtual& _virtual);
  virtual void Visit(node::instruction::unary::Not& _not);
  virtual void Visit(node::instruction::unary::Increment& increment);
  virtual void Visit(node::instruction::unary::Decrement& decrement);
  virtual void Visit(node::instruction::unary::Positive& positive);
  virtual void Visit(node::instruction::unary::Negative& negative);
  virtual void Visit(node::instruction::unary::Await& await);
  virtual void Visit(node::instruction::unary::Expected& expected);
  virtual void Visit(node::instruction::unary::Unexpected& unexpected);
  virtual void Visit(node::instruction::unary::Yield& yield);
  virtual void Visit(node::instruction::unary::Postfix& postfix);
  virtual void Visit(node::instruction::unary::postfix::Increment& increment);
  virtual void Visit(node::instruction::unary::postfix::Decrement& decrement);
  virtual void Visit(node::instruction::binary::Nullish& nullish);
  virtual void Visit(node::instruction::binary::logical::And& _and);
  virtual void Visit(node::instruction::binary::logical::Or& _or);
  virtual void Visit(node::instruction::binary::logical::Equal& equal);
  virtual void Visit(node::instruction::binary::logical::NotEqual& not_equal);
  virtual void Visit(node::instruction::binary::logical::Greater& greater);
  virtual void Visit(node::instruction::binary::logical::GreaterOrEqual& greater_or_equal);
  virtual void Visit(node::instruction::binary::logical::Lesser& lesser);
  virtual void Visit(node::instruction::binary::logical::LesserOrEqual& lesser_or_equal);
  virtual void Visit(node::instruction::binary::math::Add& add);
  virtual void Visit(node::instruction::binary::math::Subtract& subtract);
  virtual void Visit(node::instruction::binary::math::Multiply& multiply);
  virtual void Visit(node::instruction::binary::math::Divide& divide);
  virtual void Visit(node::instruction::binary::math::Modulo& modulo);
  virtual void Visit(node::instruction::binary::math::Exponent& exponent);
  virtual void Visit(node::instruction::binary::Access& access);
  virtual void Visit(node::instruction::binary::Bitwise& bitwise);

  virtual void Visit(node::instruction::binary::Assert& assert);
  virtual void Visit(node::instruction::binary::assert::Equal& equal);
  virtual void Visit(node::instruction::binary::assert::NotEqual& not_equal);
  virtual void Visit(node::instruction::binary::assert::GreaterOrEqual& greater_or_equal);
  virtual void Visit(node::instruction::binary::assert::LesserOrEqual& lesser_or_equal);

  virtual void Visit(node::instruction::binary::Assign& assign);
  virtual void Visit(node::instruction::binary::assign::Add& add);
  virtual void Visit(node::instruction::binary::assign::Subtract& subtract);
  virtual void Visit(node::instruction::binary::assign::Multiply& multiply);
  virtual void Visit(node::instruction::binary::assign::Divide& divide);
  virtual void Visit(node::instruction::binary::assign::Modulo& modulo);
  virtual void Visit(node::instruction::binary::assign::Exponent& exponent);
  virtual void Visit(node::instruction::binary::access::Member& member);
  virtual void Visit(node::instruction::binary::access::member::Static& _static);
  virtual void Visit(node::instruction::binary::access::member::Optional& optional);
  virtual void Visit(node::instruction::binary::bitwise::And& _and);
  virtual void Visit(node::instruction::binary::bitwise::Or& _or);
  virtual void Visit(node::instruction::binary::bitwise::Xor& _xor);
  virtual void Visit(node::instruction::binary::bitwise::Shift& shift);
  virtual void Visit(node::instruction::binary::bitwise::Assign& assign);
  virtual void Visit(node::instruction::binary::bitwise::shift::Left& left);
  virtual void Visit(node::instruction::binary::bitwise::shift::Right& right);
  virtual void Visit(node::instruction::binary::bitwise::shift::Triple& triple);
  virtual void Visit(node::instruction::binary::bitwise::shift::triple::Left& left);
  virtual void Visit(node::instruction::binary::bitwise::shift::triple::Right& right);
  virtual void Visit(node::instruction::binary::bitwise::assign::And& _and);
  virtual void Visit(node::instruction::binary::bitwise::assign::Xor& _xor);
  virtual void Visit(node::instruction::binary::bitwise::assign::Or& _or);
  virtual void Visit(node::instruction::binary::bitwise::assign::Shift& shift);
  virtual void Visit(node::instruction::binary::bitwise::assign::shift::Left& left);
  virtual void Visit(node::instruction::binary::bitwise::assign::shift::Right& right);
  virtual void Visit(node::instruction::binary::bitwise::assign::shift::Triple& triple);
  virtual void Visit(node::instruction::binary::bitwise::assign::shift::triple::Left& left);
  virtual void Visit(node::instruction::binary::bitwise::assign::shift::triple::Right& right);

  virtual void Visit(node::instruction::binary::Range& range);
  virtual void Visit(node::instruction::binary::range::Exclusive& exclusive);
  virtual void Visit(node::instruction::binary::range::Inclusive& inclusive);

  virtual void Visit(node::instruction::binary::Call& call);
  virtual void Visit(node::instruction::binary::call::Character& character);
  virtual void Visit(node::instruction::binary::call::Decimal& decimal);
  virtual void Visit(node::instruction::binary::call::Integer& integer);
  virtual void Visit(node::instruction::binary::call::Number& number);
  virtual void Visit(node::instruction::binary::call::Array& array);
  virtual void Visit(node::instruction::binary::call::Enum& _enum);
  virtual void Visit(node::instruction::binary::call::Object& object);
  virtual void Visit(node::instruction::binary::call::String& string);
  virtual void Visit(node::instruction::binary::call::TemplateString& template_string);
  virtual void Visit(node::instruction::binary::call::Tuple& tuple);
  virtual void Visit(node::instruction::binary::call::Function& function);
  virtual void Visit(node::instruction::TernaryCondition& ternary_condition);
};
