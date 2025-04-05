import node.declaration.literal.structured.template_string;
import visitor;
import updater;
import utility.print;

namespace node::declaration::literal::structured {
  TemplateString::TemplateString(const lexical::source::Span& span, node::Scope& scope)
    : Structured{span, scope}
  {}

  TemplateString::TemplateString(const lexical::source::Span& span, node::Scope& scope, std::vector<node::Declaration*>&& values)
    : Structured{span, scope}, values{std::move(values)}
  {}

  void TemplateString::Value(node::Declaration* value) { values.push_back(value); }
  const std::vector<node::Declaration*>& TemplateString::Values() const { return values; }

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
  std::string TemplateString::ToString() const { return utility::Line(Name(), " (", values, ")"); }
  std::vector<std::type_index> TemplateString::Signature() const { return SignatureOf(values); }
  node::Declaration* TemplateString::Value() const { return nullptr; }
};