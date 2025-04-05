export module node.declaration.literal.primitive.numeric.integer;

import node.declaration.literal.primitive.numeric;
import <string_view>;
import <string>;
import <cstdint>;

export class Visitor;
export class Updater;

namespace node::declaration::literal::primitive::numeric {
  export class Integer : public Numeric {
  private:
    int64_t value;
  public:
    using Numeric::Numeric;

    Integer(const lexical::source::Span& span, node::Scope& scope, int64_t value) noexcept;

    bool IsSigned() const final { return true; }
    bool IsFloat() const final { return false; }

    int64_t Value() const { return value; }
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
    void Construct(code::x64::Encoder& encoder) final;
    void Compile(code::x64::Encoder& encoder) final;
  };
};