import node.instruction.binary.bitwise.shift.right;
import visitor;
import updater;

namespace node::instruction::binary::bitwise::shift {
  void Right::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Right::Update(Updater& updater) {}
  int32_t Right::Size() const { return 0; }
  std::string Right::Name() const { return "Right"; }
  std::string Right::ToString() const { return std::string{Name()}; }
};