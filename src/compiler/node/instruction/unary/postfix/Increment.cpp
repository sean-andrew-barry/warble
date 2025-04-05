import node.instruction.unary.postfix.increment;
import visitor;
import updater;

namespace node::instruction::unary::postfix {
  void Increment::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Increment::Update(Updater& updater) {}
  int32_t Increment::Size() const { return 0; }
  std::string Increment::Name() const { return "Increment"; }
  std::string Increment::ToString() const { return std::string{Name()}; }
};