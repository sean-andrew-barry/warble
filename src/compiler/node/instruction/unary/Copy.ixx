export module node.instruction.unary.copy;

import node.instruction.unary;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::unary {
  export class Copy : public Unary {
  private:
  public:
    using Unary::Unary;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};