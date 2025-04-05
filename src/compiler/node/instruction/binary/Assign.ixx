export module node.instruction.binary.assign;

import node.instruction.binary;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary {
  export class Assign : public Binary {
  private:
  public:
    using Binary::Binary;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;

    bool IsAssignment() const final { return true; }
  };
};