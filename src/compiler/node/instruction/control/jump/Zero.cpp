import node.instruction.control.jump.zero;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void Zero::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Zero::Update(Updater& updater) {}
  int32_t Zero::Size() const { return 0; }
  std::string Zero::Name() const { return "Zero"; }
  std::string Zero::ToString() const { return std::string{Name()}; }
};