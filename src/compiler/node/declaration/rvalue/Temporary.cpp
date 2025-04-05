import node.declaration.rvalue.temporary;
import node.declaration.rvalue.literal.structured.object;
import utility.print;
import visitor;
import updater;

namespace node::declaration::rvalue {
  Temporary::Temporary(const lexical::source::Span& span, node::Scope& scope, std::shared_ptr<node::declaration::rvalue::literal::structured::Object>&& type) noexcept
    : RValue{span, scope}, type{std::move(type)}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Temporary::TypeOf() const {
    return type ? type->TypeOf() : nullptr;
  }

  void Temporary::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Temporary::Update(Updater& updater) {}
  int32_t Temporary::Size() const { return type->Size(); }
  std::string Temporary::Name() const { return "Temporary"; }
  std::string Temporary::ToString() const { return utility::Line("Temporary"); }
};