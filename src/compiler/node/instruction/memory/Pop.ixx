export module node.instruction.memory.pop;

import node.instruction.memory;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node {
  export class Declaration;
};

namespace node::instruction::memory {
  export class Pop : public Memory {
  private:
    node::Declaration* target;
    node::Declaration* value;
  public:
    using Memory::Memory;

    Pop(const lexical::source::Span& span, node::Declaration* target, node::Declaration* value) noexcept;

    void Target(node::Declaration* v) { target = v; }
    node::Declaration* Target() { return target; }

    void Value(node::Declaration* v) { value = v; }
    node::Declaration* Value() { return value; }

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