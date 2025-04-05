export module node.declaration.rvalue.literal.structured;

import node.declaration.literal;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration::rvalue::literal {
  export class Structured : public Literal {
  private:
  public:
    using Literal::Literal;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};