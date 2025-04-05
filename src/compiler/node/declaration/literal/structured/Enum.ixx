export module node.declaration.literal.structured._enum;

import node.declaration.literal.structured;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration::literal::structured {
  export class Enum : public Structured {
  private:
    std::vector<node::Declaration*> values;
  public:
    using Structured::Structured;

    Enum(const lexical::source::Span& span, node::Scope& scope);
    Enum(const lexical::source::Span& span, node::Scope& scope, std::vector<node::Declaration*>&& values);

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