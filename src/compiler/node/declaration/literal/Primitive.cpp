import node.declaration.literal.primitive;
import node.scope;
import code.x64.encoder;
import visitor;
import updater;
import <cstdint>;

namespace node::declaration::literal {
  void Primitive::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Primitive::Update(Updater& updater) {}
  int32_t Primitive::Size() const { return 0; }
  std::string Primitive::Name() const { return "Primitive"; }
  std::string Primitive::ToString() const { return std::string{Name()}; }
};