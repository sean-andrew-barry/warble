export module code.immediate;

import code.operand;

import <cstdint>;
import <limits>;
import <cmath>;
// import <cstdlib>;
import <string>;

namespace code {
  export class Immediate : public OperandTemplate<Immediate> {
  private:
    // Seems that MSVC doesn't yet support std::abs/std::fabs as constexpr
    constexpr double fabs(double x) {
      return (x < 0) ? -x : x;
    }

    constexpr float fabs(float x) {
      return (x < 0) ? -x : x;
    }
  private:
    size_t index{std::numeric_limits<size_t>::max()};
    uint64_t value;
  public:
    constexpr Immediate() = default;
    constexpr Immediate(code::Flag f) : Super{f} {}
    constexpr Immediate(code::Flag f, uint64_t v) : Super{f} { Value(v); }
    constexpr Immediate(code::Flag f, int64_t v) : Super{f} { Value(v); }
    constexpr Immediate(code::Flag f, double v) : Super{f} { Value(v); }

    constexpr Immediate(uint64_t v) { Value(v); }
    constexpr Immediate(int64_t v) { Value(v); }
    constexpr Immediate(double v) { Value(v); }

    constexpr Immediate(const Immediate& other) = default;
    constexpr Immediate(const Immediate& other, code::Flag f) : Super{f}, index{other.index}, value{other.value} {}

    constexpr void Index(size_t v) { index = v; }
    constexpr size_t Index() const { return index; }

    constexpr void Value(uint64_t v) {
      Clear(Flag::NUMBER() | Flag::INTEGER() | Flag::DECIMAL() | Flag::Sizes());
      
      value = v;

      if (v <= std::numeric_limits<uint8_t>::max()) {
        *this |= Flag::NUMBER() | Flag::SIZE8() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64();
      } else if (v <= std::numeric_limits<uint16_t>::max()) {
        *this |= Flag::NUMBER() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64();
      } else if (v <= std::numeric_limits<uint32_t>::max()) {
        *this |= Flag::NUMBER() | Flag::SIZE32() | Flag::SIZE64();
      } else {
        *this |= Flag::NUMBER() | Flag::SIZE64();
      }
    }

    constexpr void Value(int64_t v) {
      union {
        int64_t i;
        uint64_t u;
      } converter;
      converter.i = v;

      value = converter.u; // Copy the binary representation to uint64_t

      Clear(Flag::NUMBER() | Flag::INTEGER() | Flag::DECIMAL() | Flag::Sizes());

      if (v >= std::numeric_limits<int8_t>::min() && v <= std::numeric_limits<int8_t>::max()) {
        *this |= Flag::INTEGER() | Flag::SIZE8() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64();
      } else if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max()) {
        *this |= Flag::INTEGER() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64();
      } else if (v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max()) {
        *this |= Flag::INTEGER() | Flag::SIZE32() | Flag::SIZE64();
      } else {
        *this |= Flag::INTEGER() | Flag::SIZE64();
      }
    }

    constexpr void Value(double v) {
      union {
        double d;
        uint64_t u;
      } converter;
      converter.d = v;

      value = converter.u; // Copy the binary representation to uint64_t

      Clear(Flag::NUMBER() | Flag::INTEGER() | Flag::DECIMAL() | Flag::Sizes());

      // Check if the value can be represented as a float without losing precision
      float f = static_cast<float>(v);

      if (fabs(f - v) <= std::numeric_limits<float>::epsilon() * fabs(f)) {
        *this |= Flag::DECIMAL() | Flag::SIZE32() | Flag::SIZE64();
      } else {
        *this |= Flag::DECIMAL() | Flag::SIZE64();
      }
    }

    constexpr void Value(bool v) {
      value = v ? 1 : 0; // Copy the binary representation to uint64_t

      Clear(Flag::NUMBER() | Flag::INTEGER() | Flag::DECIMAL() | Flag::Sizes());
      *this |= Flag::NUMBER() | Flag::SIZE8() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64();
    }

    constexpr uint64_t Value() const { return value; }

    std::string ToString() { return "Immediate"; }

    // constexpr explicit operator bool() const { return index != std::numeric_limits<size_t>::max(); }

    static Immediate ZERO;
    static Immediate ONE;
  };

  Immediate Immediate::ZERO{0ull};
  Immediate Immediate::ONE{1ull};

  // export struct Immediate8 : public Immediate<int8_t> {
  //   using Immediate::Immediate;

  //   // static consteval const Immediate8& Zero() {
  //   //   static constexpr Immediate8 zero{0};
  //   //   return zero;
  //   // }

  //   static Immediate8 ZERO;
  //   static Immediate8 ONE;
  // };

  // export struct Immediate16 : public Immediate<int16_t> {
  //   using Immediate::Immediate;

  //   static Immediate16 ZERO;
  //   static Immediate16 ONE;
  // };

  // export struct Immediate32 : public Immediate<int32_t> {
  //   using Immediate::Immediate;

  //   static Immediate32 ZERO;
  //   static Immediate32 ONE;
  // };

  // export struct Immediate64 : public Immediate<int64_t> {
  //   using Immediate::Immediate;

  //   static Immediate64 ZERO;
  //   static Immediate64 ONE;
  // };

  // Immediate8 Immediate8::ZERO{0};
  // Immediate8 Immediate8::ONE{1};
  // Immediate16 Immediate16::ZERO{0};
  // Immediate16 Immediate16::ONE{1};
  // Immediate32 Immediate32::ZERO{0};
  // Immediate32 Immediate32::ONE{1};
  // Immediate64 Immediate64::ZERO{0};
  // Immediate64 Immediate64::ONE{1};
};