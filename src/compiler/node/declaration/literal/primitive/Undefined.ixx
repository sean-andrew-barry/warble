export module node.declaration.literal.primitive.undefined;

import node.declaration.literal.primitive;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration::literal::primitive {
  export class Undefined : public Primitive {
  private:
  public:
    using Primitive::Primitive;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};