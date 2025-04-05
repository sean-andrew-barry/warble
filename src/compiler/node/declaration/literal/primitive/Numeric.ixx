export module node.declaration.literal.primitive.numeric;

import node.declaration.literal.primitive;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration::literal::primitive {
  export class Numeric : public Primitive {
  private:
  public:
    using Primitive::Primitive;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};