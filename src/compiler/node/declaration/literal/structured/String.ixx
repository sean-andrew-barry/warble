export module node.declaration.literal.structured.string;

import node.declaration.literal.structured;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration::literal::structured {
  export class String : public Structured {
  private:
    std::string value;
  public:
    using Structured::Structured;

    String(const lexical::source::Span& span, node::Scope& scope, std::string&& value)
      : Structured{span, scope}, value{std::move(value)}
    {}

    const std::string& Value() const { return value; }
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
  };
};