import node.instruction.control.jump.below_or_equal;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void BelowOrEqual::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void BelowOrEqual::Update(Updater& updater) {}
  int32_t BelowOrEqual::Size() const { return 0; }
  std::string BelowOrEqual::Name() const { return "BelowOrEqual"; }
  std::string BelowOrEqual::ToString() const { return std::string{Name()}; }
};