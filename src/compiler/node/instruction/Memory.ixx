export module node.instruction.memory;

import node.instruction;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction {
  export class Memory : public Instruction {
  private:
  public:
    using Instruction::Instruction;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};