import node.scope.conditional;
import visitor;
import updater;

namespace node::scope {
  Conditional::Conditional(const lexical::source::Span& span, node::Scope& parent)
    : Scope{span}
    , parent{parent}
  {
  }

  void Conditional::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Conditional::Update(Updater& updater) {}
  int32_t Conditional::Size() const { return 0; }
  std::string Conditional::Name() const { return "Conditional"; }
  std::string Conditional::ToString() const { return std::string{Name()}; }
};