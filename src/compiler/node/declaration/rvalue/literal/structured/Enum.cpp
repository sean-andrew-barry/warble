import node.declaration.rvalue.literal.structured._enum;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import visitor;
import updater;
import utility.print;
import <memory>;

namespace node::declaration::rvalue::literal::structured {
  Enum::Enum(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  Enum::Enum(const lexical::source::Span& span, node::Scope& scope, std::vector<std::shared_ptr<node::Declaration>>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Enum::TypeOf() const { return node::scope::Global::Enum(); }

  // void Enum::Value(std::shared_ptr<node::Declaration> value) { values.push_back(std::move(value)); }
  void Enum::Value(std::shared_ptr<node::Declaration> value) { values.push_back(std::move(value)); }
  const std::vector<std::shared_ptr<node::Declaration>>& Enum::Values() const { return values; }

  void Enum::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Enum::Update(Updater& updater) {}

  int32_t Enum::Size() const {
    int32_t size = 0;
    for (auto v : values) {
      size += v ? v->Size() : 0;
    }

    return size;
  }

  // node::Declaration* Enum::Value() const { return nullptr; }

  std::string Enum::Name() const { return "Enum"; }
  // std::string Enum::ToString() const { return utility::Line(Name(), " <", values, ">"); }
  std::string Enum::ToString() const { return utility::Line(Name(), " <", ">"); }
  std::vector<std::type_index> Enum::Signature() const { return SignatureOf(values); }
};