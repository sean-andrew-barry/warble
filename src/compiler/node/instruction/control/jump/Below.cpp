import node.instruction.control.jump.below;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void Below::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Below::Update(Updater& updater) {}
  int32_t Below::Size() const { return 0; }
  std::string Below::Name() const { return "Below"; }
  std::string Below::ToString() const { return std::string{Name()}; }
};