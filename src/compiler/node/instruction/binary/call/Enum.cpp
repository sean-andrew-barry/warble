import node.instruction.binary.call._enum;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void Enum::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Enum::Update(Updater& updater) {}
  int32_t Enum::Size() const { return 0; }
  std::string Enum::Name() const { return "Enum"; }
  std::string Enum::ToString() const { return std::string{Name()}; }
};