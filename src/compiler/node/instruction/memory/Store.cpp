import node.instruction.memory.store;
import visitor;
import updater;

namespace node::instruction::memory {
  void Store::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Store::Update(Updater& updater) {}
  int32_t Store::Size() const { return 0; }
  std::string Store::Name() const { return "Store"; }
  std::string Store::ToString() const { return std::string{Name()}; }
};