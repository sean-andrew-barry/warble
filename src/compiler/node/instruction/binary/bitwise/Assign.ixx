export module node.instruction.binary.bitwise.assign;

import node.instruction.binary.bitwise;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::bitwise {
  export class Assign : public Bitwise {
  private:
  public:
    using Bitwise::Bitwise;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};