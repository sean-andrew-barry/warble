export module node.instruction.binary.math;

import node.instruction.binary;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::instruction::binary {
  export class Math : public Binary {
  private:
  public:
    using Binary::Binary;

    Math(const lexical::source::Span& span, std::shared_ptr<node::declaration::rvalue::Temporary> result, std::shared_ptr<node::Declaration> lhs, std::shared_ptr<node::Declaration> rhs) noexcept;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};