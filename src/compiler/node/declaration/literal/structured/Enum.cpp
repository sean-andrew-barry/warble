import node.declaration.literal.structured._enum;
import visitor;
import updater;
import utility.print;

namespace node::declaration::literal::structured {
  Enum::Enum(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  Enum::Enum(const lexical::source::Span& span, node::Scope& scope, std::vector<node::Declaration*>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  void Enum::Value(node::Declaration* value) { values.push_back(value); }
  const std::vector<node::Declaration*>& Enum::Values() const { return values; }

  void Enum::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Enum::Update(Updater& updater) {}

  int32_t Enum::Size() const {
    int32_t size = 0;
    for (auto v : values) {
      size += v ? v->Size() : 0;
    }

    return size;
  }

  node::Declaration* Enum::Value() const { return nullptr; }

  std::string Enum::Name() const { return "Enum"; }
  std::string Enum::ToString() const { return utility::Line(Name(), " <", values, ">"); }
  std::vector<std::type_index> Enum::Signature() const { return SignatureOf(values); }
};