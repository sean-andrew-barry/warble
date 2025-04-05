import node.instruction.unary.symbol;
import visitor;
import updater;

namespace node::instruction::unary {
  void Symbol::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Symbol::Update(Updater& updater) {}
  int32_t Symbol::Size() const { return 0; }
  std::string Symbol::Name() const { return "Symbol"; }
  std::string Symbol::ToString() const { return std::string{Name()}; }
};