import node.instruction.memory.move;
import visitor;
import updater;

namespace node::instruction::memory {
  void Move::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Move::Update(Updater& updater) {}
  int32_t Move::Size() const { return 0; }
  std::string Move::Name() const { return "Move"; }
  std::string Move::ToString() const { return std::string{Name()}; }
};