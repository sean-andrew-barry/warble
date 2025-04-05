export module node.instruction.unary;

import node.instruction;
import <string_view>;
import <string>;
import <memory>;

namespace node {
  export class Declaration;
};

namespace node::declaration::rvalue {
  export class Temporary;
};

export class Visitor;
export class Updater;

namespace node::instruction {
  export class Unary : public Instruction {
  private:
    std::shared_ptr<node::declaration::rvalue::Temporary> result;
    std::shared_ptr<node::Declaration> value;
  public:
    using Instruction::Instruction;

    Unary(const lexical::source::Span& span, std::shared_ptr<node::declaration::rvalue::Temporary> result, std::shared_ptr<node::Declaration> value) noexcept;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;

    void Result(std::shared_ptr<node::declaration::rvalue::Temporary> v) { result = std::move(v); }
    std::shared_ptr<node::declaration::rvalue::Temporary>& Result() { return result; }
    const std::shared_ptr<node::declaration::rvalue::Temporary>& Result() const { return result; }

    void Value(std::shared_ptr<node::Declaration> v) { value = std::move(v); }
    std::shared_ptr<node::Declaration>& Value() { return value; }
    const std::shared_ptr<node::Declaration>& Value() const { return value; }
  };
};