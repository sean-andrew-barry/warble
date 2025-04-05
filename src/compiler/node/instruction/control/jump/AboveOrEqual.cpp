import node.instruction.control.jump.above_or_equal;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void AboveOrEqual::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void AboveOrEqual::Update(Updater& updater) {}
  int32_t AboveOrEqual::Size() const { return 0; }
  std::string AboveOrEqual::Name() const { return "AboveOrEqual"; }
  std::string AboveOrEqual::ToString() const { return std::string{Name()}; }
};