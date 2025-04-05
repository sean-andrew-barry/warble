import node.declaration.automatic;
import utility.print;
import code.x64.encoder;
import code.x64._register;
import visitor;
import updater;

namespace node::declaration {
  Automatic::Automatic(const lexical::source::Span& span, node::Scope& scope, node::Declaration* value) noexcept
    : Declaration{span, scope}, value{value}
  {}

  void Automatic::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Automatic::Update(Updater& updater) {}
  int32_t Automatic::Size() const { return value->Size(); }
  std::string Automatic::Name() const { return "Automatic"; }
  // std::string Automatic::ToString() const { return Name(); }
  std::string Automatic::ToString() const { return utility::Line("Automatic"); }

  void Automatic::Add(code::x64::Encoder& encoder, node::Declaration& lhs, node::Declaration& rhs) {

  }
};