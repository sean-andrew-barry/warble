export module node.instruction.binary.logical.greater_or_equal;

import node.instruction.binary.logical;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::logical {
  export class GreaterOrEqual : public Logical {
  private:
  public:
    using Logical::Logical;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};