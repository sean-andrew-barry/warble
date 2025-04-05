export module node.instruction.binary.assign.subtract;

import node.instruction.binary.assign;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::assign {
  export class Subtract : public Assign {
  private:
  public:
    using Assign::Assign;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};