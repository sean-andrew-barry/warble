import node.instruction.unary.negative;
import visitor;
import updater;

namespace node::instruction::unary {
  void Negative::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Negative::Update(Updater& updater) {}
  int32_t Negative::Size() const { return 0; }
  std::string Negative::Name() const { return "Negative"; }
  std::string Negative::ToString() const { return std::string{Name()}; }
};