import node.instruction.unary.positive;
import visitor;
import updater;

namespace node::instruction::unary {
  void Positive::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Positive::Update(Updater& updater) {}
  int32_t Positive::Size() const { return 0; }
  std::string Positive::Name() const { return "Positive"; }
  std::string Positive::ToString() const { return std::string{Name()}; }
};