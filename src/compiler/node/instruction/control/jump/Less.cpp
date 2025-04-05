import node.instruction.control.jump.less;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void Less::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Less::Update(Updater& updater) {}
  int32_t Less::Size() const { return 0; }
  std::string Less::Name() const { return "Less"; }
  std::string Less::ToString() const { return std::string{Name()}; }
};