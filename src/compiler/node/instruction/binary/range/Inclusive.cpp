import node.instruction.binary.range.inclusive;
import visitor;
import updater;

namespace node::instruction::binary::range {
  void Inclusive::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Inclusive::Update(Updater& updater) {}
  int32_t Inclusive::Size() const { return 0; }
  std::string Inclusive::Name() const { return "Inclusive"; }
  std::string Inclusive::ToString() const { return std::string{Name()}; }
};