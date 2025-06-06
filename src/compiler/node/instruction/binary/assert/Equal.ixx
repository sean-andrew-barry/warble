export module node.instruction.binary.assert.equal;

import node.instruction.binary.assert;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::assert {
  export class Equal : public Assert {
  private:
  public:
    using Assert::Assert;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};