export module node.instruction.binary.bitwise.assign.shift.triple.left;

import node.instruction.binary.bitwise.assign.shift.triple;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::bitwise::assign::shift::triple {
  export class Left : public Triple {
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