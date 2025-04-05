import node.instruction.binary.use;
import visitor;
import updater;

namespace node::instruction::binary {
  void Use::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Use::Update(Updater& updater) {}
  int32_t Use::Size() const { return 0; }
  std::string Use::Name() const { return "Use"; }
  std::string Use::ToString() const { return std::string{Name()}; }
};