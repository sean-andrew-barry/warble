import node.instruction.binary.access;
import visitor;
import updater;

namespace node::instruction::binary {
  void Access::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Access::Update(Updater& updater) {}
  int32_t Access::Size() const { return 0; }
  std::string Access::Name() const { return "Access"; }
  std::string Access::ToString() const { return std::string{Name()}; }
};