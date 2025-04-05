export module node.instruction.binary.call._enum;

import node.instruction.binary.call;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::call {
  export class Enum : public Call {
  private:
  public:
    using Call::Call;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};