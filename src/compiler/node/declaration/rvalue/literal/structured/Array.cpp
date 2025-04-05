import node.declaration.rvalue.literal.structured.array;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import visitor;
import updater;
import utility.print;
import <memory>;

namespace node::declaration::rvalue::literal::structured {
  Array::Array(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  Array::Array(const lexical::source::Span& span, node::Scope& scope, std::vector<std::shared_ptr<node::Declaration>>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Array::TypeOf() const { return node::scope::Global::Array(); }

  void Array::Value(std::shared_ptr<node::Declaration>&& value) { values.push_back(std::move(value)); }
  const std::vector<std::shared_ptr<node::Declaration>>& Array::Values() const { return values; }

  void Array::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Array::Update(Updater& updater) {}

  int32_t Array::Size() const {
    int32_t size = 0;
    for (auto v : values) {
      size += v ? v->Size() : 0;
    }

    return size;
  }

  std::shared_ptr<node::Declaration> Array::Value() const { return nullptr; }

  std::string Array::Name() const { return "Array"; }
  // std::string Array::ToString() const { return utility::Line(Name(), " [", values, "]"); }
  std::string Array::ToString() const { return utility::Line(Name(), " [", "]"); }
  std::vector<std::type_index> Array::Signature() const { return SignatureOf(values); }
};