export module node.declaration.rvalue.literal.primitive.numeric.number;

import node.declaration.rvalue.literal.primitive.numeric;
import <string_view>;
import <string>;
import <cstdint>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::declaration::rvalue::literal::structured {
  export class Object;
};

namespace node::declaration::rvalue::literal::primitive::numeric {
  export class Number : public Numeric {
  private:
    uint64_t value;
    // std::string name;
  public:
    using Numeric::Numeric;

    Number(const lexical::source::Span& span, node::Scope& scope, uint64_t value) noexcept;

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const final;

    void Construct(code::x64::Encoder& encoder) final;
    void Destruct(code::x64::Encoder& encoder) final;
    void Assign(code::x64::Encoder& encoder, node::Declaration& lhs) final;
    void Add(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Subtract(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Divide(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Multiply(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Modulo(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Not(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Negate(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void And(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Or(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Xor(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Compare(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Move(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Exchange(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Convert(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void RotateLeft(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void RotateRight(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void ShiftLeft(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void ShiftRight(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Increment(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Decrement(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Call(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    void Compile(code::x64::Encoder& encoder) final;

    bool IsSigned() const final { return false; }
    bool IsFloat() const final { return false; }

    uint64_t Value() const { return value; }
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
  };
};