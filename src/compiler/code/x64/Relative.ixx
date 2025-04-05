export module code.x64.relative;

import code.x64.operand;
import <cstdint>;
import <stdexcept>;
import <algorithm>;
import <concepts>;

export namespace code::x64 {
  template<typename Derived, typename T>
  class Relative : public Operand {
    T value;
  public:
    Relative(T val) : value{val} {}

    T Value() const { return value; }

    // Operator overloads
    Derived operator+(const Derived& other) const { return Derived(value + other.value); }
    Derived operator-(const Derived& other) const { return Derived(value - other.value); }
    Derived operator*(const Derived& other) const { return Derived(value * other.value); }
    Derived operator/(const Derived& other) const { return Derived(value / other.value); }
    Derived operator%(const Derived& other) const { return Derived(value % other.value); }

    // Operator overloads for arithmetic with raw values
    Derived operator+(T other) const { return Derived(value + other); }
    Derived operator-(T other) const { return Derived(value - other); }
    Derived operator*(T other) const { return Derived(value * other); }
    Derived operator/(T other) const { return Derived(value / other); }
    Derived operator%(T other) const { return Derived(value % other); }

    // Compound assignment operators
    Derived& operator+=(const Derived& other) { value += other.value; return *this; }
    Derived& operator-=(const Derived& other) { value -= other.value; return *this; }
    Derived& operator*=(const Derived& other) { value *= other.value; return *this; }
    Derived& operator/=(const Derived& other) { value /= other.value; return *this; }
    Derived& operator%=(const Derived& other) { value %= other.value; return *this; }

    // Compound assignment operators for arithmetic with raw values
    Derived& operator+=(T other) { value += other; return *this; }
    Derived& operator-=(T other) { value -= other; return *this; }
    Derived& operator*=(T other) { value *= other; return *this; }
    Derived& operator/=(T other) { value /= other; return *this; }
    Derived& operator%=(T other) { value %= other; return *this; }
  };

  struct Relative8 : public Relative<Relative8, int8_t> {
    using Relative::Relative;
  };

  struct Relative16 : public Relative<Relative16, int16_t> {
    using Relative::Relative;
  };
  
  struct Relative32 : public Relative<Relative32, int32_t> {
    using Relative::Relative;
  };

  template<typename T>
  concept IsRelative = std::same_as<Relative8, T> || std::same_as<Relative16, T> || std::same_as<Relative32, T>;
};