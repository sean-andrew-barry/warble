export module node.instruction.binary.math.multiply;

import node.instruction.binary.math;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::math {
  export class Multiply : public Math {
  private:
  public:
    using Math::Math;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};