import node.instruction.control;
import visitor;
import updater;

namespace node::instruction {
  void Control::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Control::Update(Updater& updater) {}
  int32_t Control::Size() const { return 0; }
  std::string Control::Name() const { return "Control"; }
  std::string Control::ToString() const { return std::string{Name()}; }
};