import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import node.declaration.lvalue;
import node.package;
import utility.print;
import visitor;
import updater;
import <memory>;

namespace node::declaration::rvalue::literal::structured {
  Object::Object(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  Object::Object(const lexical::source::Span& span, node::Scope& scope, std::vector<std::shared_ptr<node::Declaration>>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Object::TypeOf() const { return node::scope::Global::Object(); }

  void Object::Value(std::shared_ptr<node::Declaration>&& value) { values.push_back(std::move(value)); }
  const std::vector<std::shared_ptr<node::Declaration>>& Object::Values() const { return values; }
  void Object::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Object::Update(Updater& updater) {}

  int32_t Object::Size() const {
    int32_t size = 0;
    for (auto value : values) {
      size += value ? value->Size() : 0;
    }

    return size;
  }
  
  std::string Object::Name() const { return "Object"; }
  // std::string Object::ToString() const { return std::string{Name()}; }
  // std::string Object::ToString() const { return utility::Line(Name(), " {", values, "}"); }
  std::string Object::ToString() const { return utility::Line(Name(), " {", "}"); }
  // std::vector<std::type_index> Object::Signature() const { return SignatureOf(values); }
};