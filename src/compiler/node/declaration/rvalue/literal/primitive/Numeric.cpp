import node.declaration.rvalue.literal.primitive.numeric;
import visitor;
import updater;

namespace node::declaration::rvalue::literal::primitive {
  void Numeric::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Numeric::Update(Updater& updater) {}
  int32_t Numeric::Size() const { return 0; }
  std::string Numeric::Name() const { return "Numeric"; }
  std::string Numeric::ToString() const { return std::string{Name()}; }
};