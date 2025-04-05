import node.instruction.control.jump.absolute;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void Absolute::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Absolute::Update(Updater& updater) {}
  int32_t Absolute::Size() const { return 0; }
  std::string Absolute::Name() const { return "Absolute"; }
  std::string Absolute::ToString() const { return std::string{Name()}; }
};