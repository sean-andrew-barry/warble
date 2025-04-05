import node.instruction.memory;
import visitor;
import updater;

namespace node::instruction {
  void Memory::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Memory::Update(Updater& updater) {}
  int32_t Memory::Size() const { return 0; }
  std::string Memory::Name() const { return "Memory"; }
  std::string Memory::ToString() const { return std::string{Name()}; }
};