import node.instruction.unary.bitwise_not;
import visitor;
import updater;

namespace node::instruction::unary {
  void BitwiseNot::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void BitwiseNot::Update(Updater& updater) {}
  int32_t BitwiseNot::Size() const { return 0; }
  std::string BitwiseNot::Name() const { return "BitwiseNot"; }
  std::string BitwiseNot::ToString() const { return std::string{Name()}; }
};