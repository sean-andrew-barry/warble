import node.instruction.unary.unexpected;
import visitor;
import updater;

namespace node::instruction::unary {
  void Unexpected::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Unexpected::Update(Updater& updater) {}
  int32_t Unexpected::Size() const { return 0; }
  std::string Unexpected::Name() const { return "Unexpected"; }
  std::string Unexpected::ToString() const { return std::string{Name()}; }
};