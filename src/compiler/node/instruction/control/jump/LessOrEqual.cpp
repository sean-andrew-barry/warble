import node.instruction.control.jump.less_or_equal;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void LessOrEqual::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void LessOrEqual::Update(Updater& updater) {}
  int32_t LessOrEqual::Size() const { return 0; }
  std::string LessOrEqual::Name() const { return "LessOrEqual"; }
  std::string LessOrEqual::ToString() const { return std::string{Name()}; }
};