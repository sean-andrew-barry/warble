import node.instruction.binary.bitwise.assign.shift.triple.left;
import visitor;
import updater;

namespace node::instruction::binary::bitwise::assign::shift::triple {
  void Left::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Left::Update(Updater& updater) {}
  int32_t Left::Size() const { return 0; }
  std::string Left::Name() const { return "Left"; }
  std::string Left::ToString() const { return std::string{Name()}; }
};