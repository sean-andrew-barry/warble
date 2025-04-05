import node.instruction.control.jump.above;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void Above::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Above::Update(Updater& updater) {}
  int32_t Above::Size() const { return 0; }
  std::string Above::Name() const { return "Above"; }
  std::string Above::ToString() const { return std::string{Name()}; }
};