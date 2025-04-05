import node.instruction.binary.assign.exponent;
import visitor;
import updater;

namespace node::instruction::binary::assign {
  void Exponent::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Exponent::Update(Updater& updater) {}
  int32_t Exponent::Size() const { return 0; }
  std::string Exponent::Name() const { return "Exponent"; }
  std::string Exponent::ToString() const { return std::string{Name()}; }
};