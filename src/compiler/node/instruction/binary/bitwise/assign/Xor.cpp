import node.instruction.binary.bitwise.assign._xor;
import visitor;
import updater;

namespace node::instruction::binary::bitwise::assign {
  void Xor::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Xor::Update(Updater& updater) {}
  int32_t Xor::Size() const { return 0; }
  std::string Xor::Name() const { return "Xor"; }
  std::string Xor::ToString() const { return std::string{Name()}; }
};