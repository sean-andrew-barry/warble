import node.declaration.forward;
import visitor;
import updater;
import utility.print;

namespace node::declaration {
  Forward::Forward(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept
    : Declaration{span, scope}, name{name}
  {}

  Forward::Forward(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept
    : Declaration{span, scope}, name{name}
  {}

  void Forward::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Forward::Update(Updater& updater) {}
  int32_t Forward::Size() const { return 0; }
  std::string Forward::Name() const { return name; }
  std::string Forward::ToString() const { return utility::Line("Forward(", name, ")"); }
};