export module code.condition;

import code.flag;
import code.operand;
import code._register;
import code.address;
import code.immediate;

import <cstdint>;
import <limits>;
import <array>;

namespace code {
  export class Condition : public Operand {
  private:
    uint64_t value;
  public:
    constexpr Condition() = default;
    constexpr Condition(code::Flag flag) : Operand{flag} {}
    constexpr Condition(code::Flag flag, uint64_t value) : Operand{flag}, value{value} {}
    constexpr Condition(uint64_t value) : Operand{}, value{value} {}
    constexpr Condition(const Condition& other) = default;
    constexpr Condition& operator=(const Condition& other) = default;
    constexpr uint64_t Value() const { return value; }

    virtual void Encode(code::Encoder& encoder) = 0;
  };

  export class RegisterAndRegister : public Condition {
  private:
    const Register& lhs;
    const Register& rhs;
  public:
  };

  export class RegisterAndAddress : public Condition {
  private:
    const Register& lhs;
    const Address& rhs;
  public:
  };

  export class RegisterAndImmediate : public Condition {
  private:
    const Register& lhs;
    const Immediate& rhs;
  public:
  };

  export class AddressAndRegister : public Condition {
  private:
    const Address& lhs;
    const Register& rhs;
  public:
  };

  export class AddressAndAddress : public Condition {
  private:
    const Address& lhs;
    const Address& rhs;
  public:
  };

  export class AddressAndImmediate : public Condition {
  protected:
    const Address& lhs;
    const Immediate& rhs;
  public:
  };

  struct EqualAI : public AddressAndImmediate { void Encode(code::Encoder& encoder) final { encoder.Equal(lhs, rhs); } };
};