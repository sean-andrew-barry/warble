export module node.instruction.memory.construct;

import node.instruction.memory;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node {
  export class Declaration;
};

// namespace node::declaration {
//   export class LValue;
// };

namespace node::instruction::memory {
  export class Construct : public Memory {
  private:
    std::shared_ptr<node::Declaration> target;
    std::shared_ptr<node::Declaration> value;
  public:
    using Memory::Memory;

    Construct(const lexical::source::Span& span, std::shared_ptr<node::Declaration> target, std::shared_ptr<node::Declaration> value) noexcept;

    void Target(std::shared_ptr<node::Declaration> v) { target = v; }
    std::shared_ptr<node::Declaration> Target() { return target; }

    void Value(std::shared_ptr<node::Declaration> v) { value = v; }
    std::shared_ptr<node::Declaration> Value() { return value; }

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const final;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    virtual void Declare(code::Encoder& encoder) final;
    virtual void Compile(code::Encoder& encoder) final;
    void Compile(code::x64::Encoder& encoder) final;
  };
};