export module node.instruction.binary;

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
  export class Binary : public Instruction {
  private:
    std::shared_ptr<node::declaration::rvalue::Temporary> result;
    std::shared_ptr<node::Declaration> lhs;
    std::shared_ptr<node::Declaration> rhs;
  public:
    using Instruction::Instruction;

    Binary(const lexical::source::Span& span, std::shared_ptr<node::declaration::rvalue::Temporary> result, std::shared_ptr<node::Declaration> lhs, std::shared_ptr<node::Declaration> rhs) noexcept;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;

    void Declare(code::Encoder& encoder) override;
    void Compile(code::Encoder& encoder) override;

    void Result(std::shared_ptr<node::declaration::rvalue::Temporary> v) { result = std::move(v); }
    std::shared_ptr<node::declaration::rvalue::Temporary>& Result() { return result; }
    const std::shared_ptr<node::declaration::rvalue::Temporary>& Result() const { return result; }

    void LHS(std::shared_ptr<node::Declaration> v) { lhs = std::move(v); }
    std::shared_ptr<node::Declaration>& LHS() { return lhs; }
    const std::shared_ptr<node::Declaration>& LHS() const { return lhs; }

    void RHS(std::shared_ptr<node::Declaration> v) { rhs = std::move(v); }
    std::shared_ptr<node::Declaration>& RHS() { return rhs; }
    const std::shared_ptr<node::Declaration>& RHS() const { return rhs; }

    // void Compile(code::x64::Encoder& encoder) override;
  };
};