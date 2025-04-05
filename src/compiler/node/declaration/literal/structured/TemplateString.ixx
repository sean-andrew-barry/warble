export module node.declaration.literal.structured.template_string;

import node.declaration.literal.structured;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration::literal::structured {
  export class TemplateString : public Structured {
  private:
    std::vector<node::Declaration*> values;
  public:
    using Structured::Structured;

    TemplateString(const lexical::source::Span& span, node::Scope& scope);
    TemplateString(const lexical::source::Span& span, node::Scope& scope, std::vector<node::Declaration*>&& values);

    void Value(node::Declaration* value);
    node::Declaration* Value() const;
    const std::vector<node::Declaration*>& Values() const;
    
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
  };
};