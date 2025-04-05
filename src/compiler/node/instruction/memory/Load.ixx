export module node.instruction.memory.load;

import node.instruction.memory;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::memory {
  export class Load : public Memory {
  private:
  public:
    using Memory::Memory;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};