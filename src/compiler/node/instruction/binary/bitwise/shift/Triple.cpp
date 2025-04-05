import node.instruction.binary.bitwise.shift.triple;
import visitor;
import updater;

namespace node::instruction::binary::bitwise::shift {
  void Triple::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Triple::Update(Updater& updater) {}
  int32_t Triple::Size() const { return 0; }
  std::string Triple::Name() const { return "Triple"; }
  std::string Triple::ToString() const { return std::string{Name()}; }
};