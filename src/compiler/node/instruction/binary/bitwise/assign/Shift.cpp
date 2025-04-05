import node.instruction.binary.bitwise.assign.shift;
import visitor;
import updater;

namespace node::instruction::binary::bitwise::assign {
  void Shift::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Shift::Update(Updater& updater) {}
  int32_t Shift::Size() const { return 0; }
  std::string Shift::Name() const { return "Shift"; }
  std::string Shift::ToString() const { return std::string{Name()}; }
};