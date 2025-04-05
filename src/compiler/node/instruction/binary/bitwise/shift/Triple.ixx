export module node.instruction.binary.bitwise.shift.triple;

import node.instruction.binary.bitwise.shift;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::bitwise::shift {
  export class Triple : public Shift {
  private:
  public:
    using Shift::Shift;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};