import node.instruction.binary.call.decimal;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void Decimal::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Decimal::Update(Updater& updater) {}
  int32_t Decimal::Size() const { return 0; }
  std::string Decimal::Name() const { return "Decimal"; }
  std::string Decimal::ToString() const { return std::string{Name()}; }
};