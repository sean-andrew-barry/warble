import node.declaration.rvalue.literal.structured.template_string;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import visitor;
import updater;
import utility.print;
import <memory>;

namespace node::declaration::rvalue::literal::structured {
  TemplateString::TemplateString(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  TemplateString::TemplateString(const lexical::source::Span& span, node::Scope& scope, std::vector<std::shared_ptr<node::Declaration>>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TemplateString::TypeOf() const { return node::scope::Global::TemplateString(); }

  void TemplateString::Value(std::shared_ptr<node::Declaration>&& value) { values.push_back(std::move(value)); }
  const std::vector<std::shared_ptr<node::Declaration>>& TemplateString::Values() const { return values; }

  void TemplateString::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void TemplateString::Update(Updater& updater) {}

  int32_t TemplateString::Size() const {
    int32_t size = 0;
    for (auto v : values) {
      size += v ? v->Size() : 0;
    }

    return size;
  }

  std::string TemplateString::Name() const { return "TemplateString"; }
  // std::string TemplateString::ToString() const { return utility::Line(Name(), " (", values, ")"); }
  std::string TemplateString::ToString() const { return utility::Line(Name(), " (", ")"); }
  std::vector<std::type_index> TemplateString::Signature() const { return SignatureOf(values); }
  std::shared_ptr<node::Declaration> TemplateString::Value() const { return nullptr; }
};