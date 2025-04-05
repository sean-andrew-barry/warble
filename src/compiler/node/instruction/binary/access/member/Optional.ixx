export module node.instruction.binary.access.member.optional;

import node.instruction.binary.access.member;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::access::member {
  export class Optional : public Member {
  private:
  public:
    using Member::Member;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};