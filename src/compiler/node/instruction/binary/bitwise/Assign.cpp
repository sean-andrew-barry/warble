import node.instruction.binary.bitwise.assign;
import visitor;
import updater;

namespace node::instruction::binary::bitwise {
  void Assign::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Assign::Update(Updater& updater) {}
  int32_t Assign::Size() const { return 0; }
  std::string Assign::Name() const { return "Assign"; }
  std::string Assign::ToString() const { return std::string{Name()}; }
};