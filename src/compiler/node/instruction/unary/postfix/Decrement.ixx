export module node.instruction.unary.postfix.decrement;

import node.instruction.unary.postfix;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::unary::postfix {
  export class Decrement : public Postfix {
  private:
  public:
    using Postfix::Postfix;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};