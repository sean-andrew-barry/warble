export module node.instruction.binary.access.member;

import node.instruction.binary.access;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::access {
  export class Member : public Access {
  private:
  public:
    using Access::Access;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};