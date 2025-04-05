import node.instruction.control.jump.sign;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void Sign::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Sign::Update(Updater& updater) {}
  int32_t Sign::Size() const { return 0; }
  std::string Sign::Name() const { return "Sign"; }
  std::string Sign::ToString() const { return std::string{Name()}; }
};