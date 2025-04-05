import node.instruction.unary.postfix.decrement;
import visitor;
import updater;

namespace node::instruction::unary::postfix {
  void Decrement::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Decrement::Update(Updater& updater) {}
  int32_t Decrement::Size() const { return 0; }
  std::string Decrement::Name() const { return "Decrement"; }
  std::string Decrement::ToString() const { return std::string{Name()}; }
};