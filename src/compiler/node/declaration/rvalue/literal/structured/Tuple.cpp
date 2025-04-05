import node.declaration.rvalue.literal.structured.tuple;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import visitor;
import updater;
import utility.print;
import <memory>;

namespace node::declaration::rvalue::literal::structured {
  Tuple::Tuple(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  Tuple::Tuple(const lexical::source::Span& span, node::Scope& scope, std::vector<std::shared_ptr<node::Declaration>>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Tuple::TypeOf() const { return node::scope::Global::Tuple(); }

  void Tuple::Value(std::shared_ptr<node::Declaration>&& value) { values.push_back(std::move(value)); }
  const std::vector<std::shared_ptr<node::Declaration>>& Tuple::Values() const { return values; }

  void Tuple::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Tuple::Update(Updater& updater) {}

  // int32_t Tuple::Size() const { return SizeOf(values); }
  int32_t Tuple::Size() const { return 0; }

  std::string Tuple::Name() const { return "Tuple"; }
  // std::string Tuple::ToString() const { return utility::Line(Name(), " (", values, ")"); }
  std::string Tuple::ToString() const { return utility::Line(Name(), " (", ")"); }

  std::vector<std::type_index> Tuple::Signature() const { return SignatureOf(values); }
  std::shared_ptr<node::Declaration> Tuple::Value() const { return nullptr; }
};