import node.instruction.binary.assign.multiply;
import visitor;
import updater;

namespace node::instruction::binary::assign {
  void Multiply::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Multiply::Update(Updater& updater) {}
  int32_t Multiply::Size() const { return 0; }
  std::string Multiply::Name() const { return "Multiply"; }
  std::string Multiply::ToString() const { return std::string{Name()}; }
};