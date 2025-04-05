export module node.declaration.rvalue.literal.primitive.boolean;

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
  export class Boolean : public Primitive {
  private:
    bool value;
  public:
    // using Primitive::Primitive;
    
    Boolean(const lexical::source::Span& span, node::Scope& scope, bool value);

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const final;

    bool Value() const { return value; }
    bool IsSigned() const final { return false; }
    bool IsFloat() const final { return false; }

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
  };
};