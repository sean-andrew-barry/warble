import node.declaration.literal.structured.tuple;
import visitor;
import updater;
import utility.print;

namespace node::declaration::literal::structured {
  Tuple::Tuple(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  Tuple::Tuple(const lexical::source::Span& span, node::Scope& scope, std::vector<node::Declaration*>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  void Tuple::Value(node::Declaration* value) { values.push_back(value); }
  const std::vector<node::Declaration*>& Tuple::Values() const { return values; }

  void Tuple::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Tuple::Update(Updater& updater) {}

  int32_t Tuple::Size() const { return SizeOf(values); }

  std::string Tuple::Name() const { return "Tuple"; }
  std::string Tuple::ToString() const { return utility::Line(Name(), " (", values, ")"); }

  std::vector<std::type_index> Tuple::Signature() const { return SignatureOf(values); }
  node::Declaration* Tuple::Value() const { return nullptr; }
};