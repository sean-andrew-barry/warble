import node.declaration.temporary;
import utility.print;
import code.x64.encoder;
import code.x64._register;
import visitor;
import updater;

namespace node::declaration {
  Temporary::Temporary(const lexical::source::Span& span, node::Scope& scope, node::Declaration* value) noexcept
    : Declaration{span, scope}, value{value}
  {}

  void Temporary::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Temporary::Update(Updater& updater) {}
  int32_t Temporary::Size() const { return value->Size(); }
  std::string Temporary::Name() const { return "Temporary"; }
  // std::string Temporary::ToString() const { return Name(); }
  std::string Temporary::ToString() const { return utility::Line("Temporary"); }
};