export module node.declaration.literal.primitive.numeric.decimal;

import node.declaration.literal.primitive.numeric;
import <string_view>;
import <string>;
import <cstdint>;
import <cmath>;

export class Visitor;
export class Updater;

namespace node::declaration::literal::primitive::numeric {
  export class Decimal : public Numeric {
  private:
    double value;
  public:
    using Numeric::Numeric;

    Decimal(const lexical::source::Span& span, node::Scope& scope, double value) noexcept;

    bool IsSigned() const final { return false; }
    bool IsFloat() const final { return true; }

    double Value() const { return value; }
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
    void Construct(code::x64::Encoder& encoder) final;
  };
};