export module node.instruction.control.jump.not_sign;

import node.instruction.control.jump;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::control::jump {
  export class NotSign : public Jump {
  private:
  public:
    using Jump::Jump;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};