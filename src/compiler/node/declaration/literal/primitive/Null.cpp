import node.declaration.literal.primitive.null;
import visitor;
import updater;
import utility.print;

namespace node::declaration::literal::primitive {
  void Null::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Null::Update(Updater& updater) {}
  int32_t Null::Size() const { return 8; }
  std::string Null::Name() const { return "Null"; }
  std::string Null::ToString() const { return utility::Line(Name()); }
};