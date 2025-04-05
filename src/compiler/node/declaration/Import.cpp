import node.declaration._import;
import visitor;
import updater;

namespace node::declaration {
  Import::Import(const lexical::source::Span& span, node::Scope& scope, std::string&& name) noexcept
    : Declaration{span, scope}, name{std::move(name)}
  {
    // Parent(nullptr);
  }

  Import::Import(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept
    : Declaration{span, scope}, name{name}
  {
    // Parent(nullptr);
  }

  // Import::Import(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept
  //   : Declaration{span, scope}, name{name}
  // {}

  void Import::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Import::Update(Updater& updater) {}
  int32_t Import::Size() const { return 0; }
  std::string Import::Name() const { return name; }
  std::string Import::ToString() const { return std::string{Name()}; }
};