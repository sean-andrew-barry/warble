import node.instruction.control.jump.not_zero;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void NotZero::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void NotZero::Update(Updater& updater) {}
  int32_t NotZero::Size() const { return 0; }
  std::string NotZero::Name() const { return "NotZero"; }
  std::string NotZero::ToString() const { return std::string{Name()}; }
};