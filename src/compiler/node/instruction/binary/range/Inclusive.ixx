export module node.instruction.binary.range.inclusive;

import node.instruction.binary.range;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary::range {
  export class Inclusive : public Range {
  private:
  public:
    using Range::Range;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};