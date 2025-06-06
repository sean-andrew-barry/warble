export module node.instruction.binary.bitwise.shift.triple.right;

import node.instruction.binary.bitwise.shift.triple;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::bitwise::shift::triple {
  export class Right : public Triple {
  private:
  public:
    using Triple::Triple;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};