export module node.declaration.rvalue.literal.structured.template_string;

import node.declaration.rvalue.literal.structured;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::declaration::rvalue::literal::structured {
  export class Object;
  
  export class TemplateString : public Structured {
  private:
    std::vector<std::shared_ptr<node::Declaration>> values;
  public:
    using Structured::Structured;

    TemplateString(const lexical::source::Span& span, node::Scope& scope);
    TemplateString(const lexical::source::Span& span, node::Scope& scope, std::vector<std::shared_ptr<node::Declaration>>&& values);

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const final;

    void Value(std::shared_ptr<node::Declaration>&& value);
    std::shared_ptr<node::Declaration> Value() const;
    const std::vector<std::shared_ptr<node::Declaration>>& Values() const;
    
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
  };
};