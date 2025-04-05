import node.declaration.rvalue.literal.primitive.boolean;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
// import node.compiler;
import visitor;
import updater;
import utility.print;

namespace node::declaration::rvalue::literal::primitive {
  Boolean::Boolean(const lexical::source::Span& span, node::Scope& scope, bool value)
    : Primitive{span, scope}, value{value}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Boolean::TypeOf() const { return node::scope::Global::Boolean(); }

  void Boolean::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Boolean::Update(Updater& updater) {}
  int32_t Boolean::Size() const { return 1; }
  std::string Boolean::Name() const { return utility::Join("Boolean: ", value, ""); }
  std::string Boolean::ToString() const { return utility::Line(Name()); }
  std::vector<std::type_index> Boolean::Signature() const { return {typeid(*this)}; }
};