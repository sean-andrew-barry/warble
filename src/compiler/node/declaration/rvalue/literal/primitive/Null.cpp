import node.declaration.rvalue.literal.primitive.null;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import visitor;
import updater;
import utility.print;
import <memory>;

namespace node::declaration::rvalue::literal::primitive {
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Null::TypeOf() const { return node::scope::Global::Null(); }

  void Null::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Null::Update(Updater& updater) {}
  int32_t Null::Size() const { return 8; }
  std::string Null::Name() const { return "Null"; }
  std::string Null::ToString() const { return utility::Line(Name()); }
};