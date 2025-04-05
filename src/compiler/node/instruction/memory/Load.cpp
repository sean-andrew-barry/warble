import node.instruction.memory.load;
import visitor;
import updater;

namespace node::instruction::memory {
  void Load::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Load::Update(Updater& updater) {}
  int32_t Load::Size() const { return 0; }
  std::string Load::Name() const { return "Load"; }
  std::string Load::ToString() const { return std::string{Name()}; }
};