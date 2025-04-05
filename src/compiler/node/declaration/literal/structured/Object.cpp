import node.declaration.literal.structured.object;
import visitor;
import updater;
import utility.print;

namespace node::declaration::literal::structured {
  Object::Object(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  Object::Object(const lexical::source::Span& span, node::Scope& scope, std::vector<node::Declaration*>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  void Object::Value(node::Declaration* value) { values.push_back(value); }
  const std::vector<node::Declaration*>& Object::Values() const { return values; }
  void Object::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Object::Update(Updater& updater) {}
  node::Declaration* Object::Value() const { return nullptr; }

  int32_t Object::Size() const {
    int32_t size = 0;
    for (auto value : values) {
      size += value ? value->Size() : 0;
    }

    return size;
  }
  
  std::string Object::Name() const { return "Object"; }
  // std::string Object::ToString() const { return std::string{Name()}; }
  std::string Object::ToString() const { return utility::Line(Name(), " {", values, "}"); }
  std::vector<std::type_index> Object::Signature() const { return SignatureOf(values); }
};