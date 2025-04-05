import node.instruction.control.jump.not_sign;
import visitor;
import updater;

namespace node::instruction::control::jump {
  void NotSign::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void NotSign::Update(Updater& updater) {}
  int32_t NotSign::Size() const { return 0; }
  std::string NotSign::Name() const { return "NotSign"; }
  std::string NotSign::ToString() const { return std::string{Name()}; }
};