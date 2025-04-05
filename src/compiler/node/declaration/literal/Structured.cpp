import node.declaration.literal.structured;
import visitor;
import updater;

namespace node::declaration::literal {
  void Structured::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Structured::Update(Updater& updater) {}
  int32_t Structured::Size() const { return 0; }
  std::string Structured::Name() const { return "Structured"; }
  std::string Structured::ToString() const { return std::string{Name()}; }
};