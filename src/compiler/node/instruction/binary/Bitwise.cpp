import node.instruction.binary.bitwise;
import visitor;
import updater;

namespace node::instruction::binary {
  void Bitwise::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Bitwise::Update(Updater& updater) {}
  int32_t Bitwise::Size() const { return 0; }
  std::string Bitwise::Name() const { return "Bitwise"; }
  std::string Bitwise::ToString() const { return std::string{Name()}; }
};