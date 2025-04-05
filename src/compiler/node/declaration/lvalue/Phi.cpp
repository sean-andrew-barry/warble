import node.declaration.lvalue.phi;
import node.declaration.lvalue;
import node.declaration;
import visitor;
import updater;
import <memory>;

namespace node::declaration::lvalue {
  Phi::Phi(const lexical::source::Span& span, node::Scope& scope, std::shared_ptr<node::declaration::LValue>&& source) noexcept
    : LValue{span, scope}, source{std::move(source)}
  {}

  void Phi::Add(std::shared_ptr<node::declaration::LValue>&& v) {
    declarations.push_back(std::move(v));
  }

  bool Phi::Match(const std::string_view v) const { return source->Match(v); }
  bool Phi::Match(const std::string& v) const { return source->Match(v); }
  bool Phi::Match(const node::declaration::rvalue::literal::structured::Enum& v) const { return source->Match(v); }

  void Phi::Declarations(std::vector<std::shared_ptr<node::declaration::LValue>>&& v) { declarations = std::move(v); }
  const std::vector<std::shared_ptr<node::declaration::LValue>>& Phi::Declarations() const { return declarations; }

  void Phi::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Phi::Update(Updater& updater) {}
  int32_t Phi::Size() const { return 0; }
  std::string Phi::Name() const { return "Phi"; }
  std::string Phi::ToString() const { return Name(); }
};