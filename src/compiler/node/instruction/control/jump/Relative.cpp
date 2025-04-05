import node.instruction.control.jump.relative;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void Relative::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Relative::Update(Updater& updater) {}
  int32_t Relative::Size() const { return 0; }
  std::string Relative::Name() const { return "Relative"; }
  std::string Relative::ToString() const { return std::string{Name()}; }
};