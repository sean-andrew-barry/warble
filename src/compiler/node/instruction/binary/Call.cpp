import node.instruction.binary.call;
import visitor;
import updater;

namespace node::instruction::binary {
  void Call::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Call::Update(Updater& updater) {}
  int32_t Call::Size() const { return 0; }
  std::string Call::Name() const { return "Call"; }
  std::string Call::ToString() const { return std::string{Name()}; }
};