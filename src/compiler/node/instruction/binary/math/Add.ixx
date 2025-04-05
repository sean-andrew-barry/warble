export module node.instruction.binary.math.add;

import node.instruction.binary.math;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::math {
  export class Add : public Math {
  private:
  public:
    using Math::Math;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    void Declare(code::Encoder& encoder) final;
    void Compile(code::Encoder& encoder) final;
    void Compile(code::x64::Encoder& encoder) final;
  };
};