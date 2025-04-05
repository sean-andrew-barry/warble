export module node.declaration.rvalue.literal.primitive.character;

import node.declaration.rvalue.literal.primitive;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::declaration::rvalue::literal::structured {
  export class Object;
};

namespace node::declaration::rvalue::literal::primitive {
  export class Character : public Primitive {
  private:
    std::string value;
    char32_t raw; // TODO
  public:
    using Primitive::Primitive;

    Character(const lexical::source::Span& span, node::Scope& scope, std::string&& value);
    Character(const lexical::source::Span& span, node::Scope& scope, std::string&& value, char32_t raw);

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const final;

    const std::string& Value() const { return value; }
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
  };
};