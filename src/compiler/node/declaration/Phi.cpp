import node.declaration.phi;
import node.declaration;
import visitor;
import updater;

namespace node::declaration {
  void Phi::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Phi::Update(Updater& updater) {}
  int32_t Phi::Size() const { return 0; }
  std::string Phi::Name() const { return "Phi"; }
  std::string Phi::ToString() const { return Name(); }
};