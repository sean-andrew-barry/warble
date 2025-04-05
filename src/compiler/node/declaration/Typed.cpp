import node.declaration.typed;
import utility.print;
import code.x64.encoder;
import visitor;
import updater;

namespace node::declaration {
  Typed::Typed(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept
    : Declaration{span, scope}, name{name}
  {}

  Typed::Typed(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept
    : Declaration{span, scope}, name{name}
  {}

  Typed::Typed(const lexical::source::Span& span, node::Scope& scope, node::Declaration* value) noexcept
    : Declaration{span, scope}, value{value}
  {}

  Typed::Typed(const lexical::source::Span& span, node::Scope& scope, std::string& name, node::Declaration* value) noexcept
    : Declaration{span, scope}, name{name}, value{value}
  {}

  void Typed::Name(std::string&& v) { name = std::move(v); }
  void Typed::Name(std::string_view v) { name = v; }
  void Typed::Type(node::Declaration* v) { type = v; }
  void Typed::Value(node::Declaration* v) { value = v; }
  void Typed::Link(node::Declaration* v) { link = v; }

  void Typed::SetModifier(node::declaration::Modifiers modifier) {
    modifiers.set(static_cast<uint8_t>(modifier));
  }

  bool Typed::HasModifier(node::declaration::Modifiers modifier) const {
    return modifiers[static_cast<uint8_t>(modifier)];
  }

  node::Declaration* Typed::Type() const { return type; }
  node::Declaration* Typed::Value() const { return value; }
  node::Declaration* Typed::Link() const { return link; }

  void Typed::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Typed::Update(Updater& updater) {}

  int32_t Typed::Size() const {
    // utility::Debug("Determining size of typed declaration", Name());

    if (type) {
      // utility::Debug("We do have a type for", Name());
      return type->Size();
    }

    if (value) {
      // utility::Debug("We do have a value for", Name());
      return value->Size();
    }

    return 0;
  }

  std::string Typed::Name() const { return name; }
  // std::string Typed::ToString() const { return Name(); }
  std::string Typed::ToString() const { return utility::Line("Typed(", name, ")"); }
  std::vector<std::type_index> Typed::Signature() const { return {typeid(type)}; }

  void Typed::Construct(code::x64::Encoder& encoder) {
    utility::Debug("~~~Constructing value to", Name());
    value->Assign(encoder, *this);
  }
};