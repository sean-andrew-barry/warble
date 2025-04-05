import node.instruction.unary.spread;
import visitor;
import updater;

namespace node::instruction::unary {
  void Spread::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Spread::Update(Updater& updater) {}
  int32_t Spread::Size() const { return 0; }
  std::string Spread::Name() const { return "Spread"; }
  std::string Spread::ToString() const { return std::string{Name()}; }
};