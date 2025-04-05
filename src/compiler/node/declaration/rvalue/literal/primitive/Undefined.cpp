import node.declaration.rvalue.literal.primitive.undefined;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import visitor;
import updater;
import utility.print;
import <memory>;

namespace node::declaration::rvalue::literal::primitive {
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Undefined::TypeOf() const { return node::scope::Global::Undefined(); }

  void Undefined::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Undefined::Update(Updater& updater) {}
  int32_t Undefined::Size() const { return 8; }
  std::string Undefined::Name() const { return "Undefined"; }
  std::string Undefined::ToString() const { return utility::Line(Name()); }
};