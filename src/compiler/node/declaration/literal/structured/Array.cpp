import node.declaration.literal.structured.array;
import visitor;
import updater;
import utility.print;

namespace node::declaration::literal::structured {
  Array::Array(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  Array::Array(const lexical::source::Span& span, node::Scope& scope, std::vector<node::Declaration*>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  void Array::Value(node::Declaration* value) { values.push_back(value); }
  const std::vector<node::Declaration*>& Array::Values() const { return values; }

  void Array::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Array::Update(Updater& updater) {}

  int32_t Array::Size() const {
    int32_t size = 0;
    for (auto v : values) {
      size += v ? v->Size() : 0;
    }

    return size;
  }

  node::Declaration* Array::Value() const { return nullptr; }

  std::string Array::Name() const { return "Array"; }
  std::string Array::ToString() const { return utility::Line(Name(), " [", values, "]"); }
  std::vector<std::type_index> Array::Signature() const { return SignatureOf(values); }
};