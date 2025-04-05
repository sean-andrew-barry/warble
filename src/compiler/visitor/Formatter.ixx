export module visitor.formatter;

import visitor;
import utility.to_string;
import <string>;
import <memory>;
import <vector>;
import <ranges>;
import <cstdint>;

namespace visitor {
  export class Formatter : public Visitor {
  private:
    std::string output;
    int64_t indent = 0;
    int64_t index = 0;
  public:
    std::string ToString() const { return output; }

    void In(const std::string_view view = "{") {
      if (!view.empty()) {
        Add(view);
      }

      indent += 1;
    }

    void Out(const std::string_view view = "}") {
      indent -= 1;

      if (!view.empty()) {
        Line(view);
      }
    }

    void Line(auto&&... args) {
      if (!output.empty()) {
        output += "\n";

        auto count = indent;
        while (count > 0) {
          output += "  ";
          --count;
        }
      }

      Add(std::forward<decltype(args)>(args)...);
    }

    void Add(auto&&... args) {
      // int64_t remaining = sizeof...(args);
      // (output.append(utility::ToString(std::forward<decltype(args)>(args)) + (--remaining > 0 ? " " : "")), ...);
      (output.append(utility::ToString(std::forward<decltype(args)>(args))), ...);
    }

    void List(std::string_view format, auto&&... args) {
      auto separator = std::string_view{format.cbegin() + 1, format.cend() - 1};
      output += format.front();

      int64_t remaining = sizeof...(args);
      (output.append(utility::ToString(std::forward<decltype(args)>(args)) + (--remaining > 0 ? separator : "")), ...);
      
      output += format.back();
    }

    void Each(std::string_view format, std::ranges::range auto const& nodes) {
      auto separator = std::string_view{format.cbegin() + 1, format.cend() - 1};
      output += format.front();

      auto range = std::ranges::subrange(nodes);
      Next(range.front());
      range.advance(1);

      for (auto& node : range) {
        Next(node);
        output += separator;
      }

      output += format.back();
    }

    void Array(auto&&... args) { List("[, ]", std::forward<decltype(args)>(args)...); }
    void Object(auto&&... args) { List("{, }", std::forward<decltype(args)>(args)...); }
    void Enum(auto&&... args) { List("<, >", std::forward<decltype(args)>(args)...); }

    void Visit(node::Block& block);
    void Visit(node::Compiler& compiler);
    void Visit(node::Scope& scope);
    void Visit(node::scope::Conditional& conditional);
    void Visit(node::scope::conditional::If& _if);
    void Visit(node::scope::conditional::Else& _else);
    void Visit(node::scope::conditional::ElseIf& _else_if);
    void Visit(node::scope::conditional::Loop& loop);
    void Visit(node::scope::conditional::loop::Do& _do);
    void Visit(node::scope::conditional::loop::While& _while);
    void Visit(node::scope::conditional::loop::For& _for);
    void Visit(node::scope::conditional::Pattern& pattern);
    void Visit(node::scope::conditional::pattern::Cover& cover);
    void Visit(node::scope::conditional::pattern::Match& match);
    void Visit(node::scope::conditional::pattern::Default& _default);
    void Visit(node::scope::conditional::pattern::Is& is);
    void Visit(node::scope::Context& context);
    void Visit(node::scope::context::Module& _module);
    void Visit(node::scope::context::Function& function);
    void Visit(node::scope::context::function::Inline& inline_function);
    void Visit(node::Declaration& declaration);
    void Visit(node::Instruction& instruction);
    void Visit(node::declaration::Automatic& automatic);
    void Visit(node::declaration::Forward& forward);
    void Visit(node::declaration::Spread& spread);
    void Visit(node::declaration::Typed& typed);
    void Visit(node::declaration::Literal& literal);
    void Visit(node::declaration::literal::Primitive& primitive);
    void Visit(node::declaration::literal::Structured& structured);
    void Visit(node::declaration::literal::primitive::Boolean& boolean);
    void Visit(node::declaration::literal::primitive::Character& character);
    void Visit(node::declaration::literal::primitive::Null& null);
    void Visit(node::declaration::literal::primitive::Undefined& undefined);
    void Visit(node::declaration::literal::primitive::Numeric& numeric);
    void Visit(node::declaration::literal::primitive::numeric::Decimal& decimal);
    void Visit(node::declaration::literal::primitive::numeric::Integer& integer);
    void Visit(node::declaration::literal::primitive::numeric::Number& number);
    void Visit(node::declaration::literal::structured::Array& array);
    void Visit(node::declaration::literal::structured::Enum& _enum);
    void Visit(node::declaration::literal::structured::Object& object);
    void Visit(node::declaration::literal::structured::String& string);
    void Visit(node::declaration::literal::structured::TemplateString& template_string);
    void Visit(node::declaration::literal::structured::Tuple& tuple);
    void Visit(node::declaration::literal::structured::Function& function);
    void Visit(node::instruction::Memory& memory);
    void Visit(node::instruction::Control& control);
    void Visit(node::instruction::Unary& unary);
    void Visit(node::instruction::Binary& binary);
    void Visit(node::instruction::memory::Construct& construct);
    void Visit(node::instruction::memory::Destruct& destruct);
    void Visit(node::instruction::memory::Move& move);
    void Visit(node::instruction::memory::Store& store);
    void Visit(node::instruction::memory::Load& load);
    void Visit(node::instruction::control::Return& _return);
    void Visit(node::instruction::control::Continue& _continue);
    void Visit(node::instruction::control::Break& _break);
    void Visit(node::instruction::control::Jump& jump);
    void Visit(node::instruction::control::jump::Absolute& absolute);
    void Visit(node::instruction::control::jump::Relative& relative);
    void Visit(node::instruction::control::jump::Equal& equal);
    void Visit(node::instruction::control::jump::Zero& zero);
    void Visit(node::instruction::control::jump::NotEqual& not_equal);
    void Visit(node::instruction::control::jump::NotZero& not_zero);
    void Visit(node::instruction::control::jump::Greater& greater);
    void Visit(node::instruction::control::jump::GreaterOrEqual& greater_or_equal);
    void Visit(node::instruction::control::jump::Less& less);
    void Visit(node::instruction::control::jump::LessOrEqual& less_or_equal);
    void Visit(node::instruction::control::jump::Sign& sign);
    void Visit(node::instruction::control::jump::NotSign& not_sign);
    void Visit(node::instruction::control::jump::Above& above);
    void Visit(node::instruction::control::jump::AboveOrEqual& above_or_equal);
    void Visit(node::instruction::control::jump::Below& below);
    void Visit(node::instruction::control::jump::BelowOrEqual& below_or_equal);
    void Visit(node::instruction::unary::BitwiseNot& bitwise_not);
    void Visit(node::instruction::unary::Copy& copy);
    void Visit(node::instruction::unary::Reference& reference);
    void Visit(node::instruction::unary::Spread& spread);
    void Visit(node::instruction::unary::Optional& optional);
    void Visit(node::instruction::unary::Symbol& symbol);
    void Visit(node::instruction::unary::Length& length);
    void Visit(node::instruction::unary::Virtual& _virtual);
    void Visit(node::instruction::unary::Not& _not);
    void Visit(node::instruction::unary::Increment& increment);
    void Visit(node::instruction::unary::Decrement& decrement);
    void Visit(node::instruction::unary::Positive& positive);
    void Visit(node::instruction::unary::Negative& negative);
    void Visit(node::instruction::unary::Await& await);
    void Visit(node::instruction::unary::Expected& expected);
    void Visit(node::instruction::unary::Unexpected& unexpected);
    void Visit(node::instruction::unary::Yield& yield);
    void Visit(node::instruction::unary::Postfix& postfix);
    void Visit(node::instruction::unary::postfix::Increment& increment);
    void Visit(node::instruction::unary::postfix::Decrement& decrement);
    void Visit(node::instruction::binary::And& _and);
    void Visit(node::instruction::binary::Or& _or);
    void Visit(node::instruction::binary::Nullish& nullish);
    void Visit(node::instruction::binary::Equal& equal);
    void Visit(node::instruction::binary::NotEqual& not_equal);
    void Visit(node::instruction::binary::Assert& assert);
    void Visit(node::instruction::binary::Greater& greater);
    void Visit(node::instruction::binary::GreaterOrEqual& greater_or_equal);
    void Visit(node::instruction::binary::Lesser& lesser);
    void Visit(node::instruction::binary::LesserOrEqual& lesser_or_equal);
    void Visit(node::instruction::binary::Add& add);
    void Visit(node::instruction::binary::Subtract& subtract);
    void Visit(node::instruction::binary::Multiply& multiply);
    void Visit(node::instruction::binary::Divide& divide);
    void Visit(node::instruction::binary::Modulo& modulo);
    void Visit(node::instruction::binary::Exponent& exponent);
    void Visit(node::instruction::binary::Assign& assign);
    void Visit(node::instruction::binary::Access& access);
    void Visit(node::instruction::binary::Bitwise& bitwise);
    void Visit(node::instruction::binary::Range& range);
    void Visit(node::instruction::binary::Call& call);
    void Visit(node::instruction::binary::assert::Equal& equal);
    void Visit(node::instruction::binary::assert::NotEqual& not_equal);
    void Visit(node::instruction::binary::assert::GreaterOrEqual& greater_or_equal);
    void Visit(node::instruction::binary::assert::LesserOrEqual& lesser_or_equal);
    void Visit(node::instruction::binary::assign::Add& add);
    void Visit(node::instruction::binary::assign::Subtract& subtract);
    void Visit(node::instruction::binary::assign::Multiply& multiply);
    void Visit(node::instruction::binary::assign::Divide& divide);
    void Visit(node::instruction::binary::assign::Modulo& modulo);
    void Visit(node::instruction::binary::assign::Exponent& exponent);
    void Visit(node::instruction::binary::access::Member& member);
    void Visit(node::instruction::binary::access::member::Static& _static);
    void Visit(node::instruction::binary::access::member::Optional& optional);
    void Visit(node::instruction::binary::bitwise::And& _and);
    void Visit(node::instruction::binary::bitwise::Or& _or);
    void Visit(node::instruction::binary::bitwise::Xor& _xor);
    void Visit(node::instruction::binary::bitwise::Shift& shift);
    void Visit(node::instruction::binary::bitwise::Assign& assign);
    void Visit(node::instruction::binary::bitwise::shift::Left& left);
    void Visit(node::instruction::binary::bitwise::shift::Right& right);
    void Visit(node::instruction::binary::bitwise::shift::Triple& triple);
    void Visit(node::instruction::binary::bitwise::shift::triple::Left& left);
    void Visit(node::instruction::binary::bitwise::shift::triple::Right& right);
    void Visit(node::instruction::binary::bitwise::assign::And& _and);
    void Visit(node::instruction::binary::bitwise::assign::Xor& _xor);
    void Visit(node::instruction::binary::bitwise::assign::Or& _or);
    void Visit(node::instruction::binary::bitwise::assign::Shift& shift);
    void Visit(node::instruction::binary::bitwise::assign::shift::Left& left);
    void Visit(node::instruction::binary::bitwise::assign::shift::Right& right);
    void Visit(node::instruction::binary::bitwise::assign::shift::Triple& triple);
    void Visit(node::instruction::binary::bitwise::assign::shift::triple::Left& left);
    void Visit(node::instruction::binary::bitwise::assign::shift::triple::Right& right);
    void Visit(node::instruction::binary::range::Exclusive& exclusive);
    void Visit(node::instruction::binary::range::Inclusive& inclusive);
    void Visit(node::instruction::binary::call::Character& character);
    void Visit(node::instruction::binary::call::Decimal& decimal);
    void Visit(node::instruction::binary::call::Integer& integer);
    void Visit(node::instruction::binary::call::Number& number);
    void Visit(node::instruction::binary::call::Array& array);
    void Visit(node::instruction::binary::call::Enum& _enum);
    void Visit(node::instruction::binary::call::Object& object);
    void Visit(node::instruction::binary::call::String& string);
    void Visit(node::instruction::binary::call::TemplateString& template_string);
    void Visit(node::instruction::binary::call::Tuple& tuple);
    void Visit(node::instruction::binary::call::Function& function);
    void Visit(node::instruction::TernaryCondition& ternary_condition);
    void Visit(node::instruction::Phi& phi);
  };
};