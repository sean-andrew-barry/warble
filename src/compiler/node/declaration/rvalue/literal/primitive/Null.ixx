export module node.declaration.rvalue.literal.primitive.null;

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
  export class Null : public Primitive {
  private:
  public:
    using Primitive::Primitive;

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const final;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};