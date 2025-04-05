export module node.declaration.literal.primitive.character;

import node.declaration.literal.primitive;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration::literal::primitive {
  export class Character : public Primitive {
  private:
    std::string value;
    char32_t raw; // TODO
  public:
    using Primitive::Primitive;

    Character(const lexical::source::Span& span, node::Scope& scope, std::string&& value);
    Character(const lexical::source::Span& span, node::Scope& scope, std::string&& value, char32_t raw);

    const std::string& Value() const { return value; }
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
  };
};