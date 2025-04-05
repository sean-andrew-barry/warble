export module node.instruction.control._break;

import node.instruction.control;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::control {
  export class Break : public Control {
  private:
  public:
    using Control::Control;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};