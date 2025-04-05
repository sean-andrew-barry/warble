import node.instruction.binary.assign.modulo;
import visitor;
import updater;

namespace node::instruction::binary::assign {
  void Modulo::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Modulo::Update(Updater& updater) {}
  int32_t Modulo::Size() const { return 0; }
  std::string Modulo::Name() const { return "Modulo"; }
  std::string Modulo::ToString() const { return std::string{Name()}; }
};