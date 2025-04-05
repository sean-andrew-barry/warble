import node.declaration.literal.primitive.undefined;
import visitor;
import updater;
import utility.print;

namespace node::declaration::literal::primitive {
  void Undefined::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Undefined::Update(Updater& updater) {}
  int32_t Undefined::Size() const { return 8; }
  std::string Undefined::Name() const { return "Undefined"; }
  std::string Undefined::ToString() const { return utility::Line(Name()); }
};