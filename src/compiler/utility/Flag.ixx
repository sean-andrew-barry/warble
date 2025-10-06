export module compiler.utility.Flag;

import <cstdint>;
import <bit>;
import <limits>;
import <type_traits>;

namespace compiler::utility {
  export template<typename T, typename Flags>
  requires std::is_unsigned_v<T> && std::is_enum_v<Flags> && (std::numeric_limits<std::underlying_type_t<Flags>>::digits <= std::numeric_limits<T>::digits)
  class Flag {
  private:
  protected:
    T bits{};
    
    constexpr Flag(T bits) : bits{bits} {}

    static consteval Flag Cast(Flags f) {
      Flag flag{0};
      flag.Set(static_cast<size_t>(f));
      return flag;
    }
  public:
    static constexpr size_t Width = std::numeric_limits<T>::digits;

    constexpr Flag() = default;
    constexpr Flag(Flags index) : bits{T(1) << static_cast<size_t>(index)} {}
    constexpr Flag(const Flag& other) = default;
    constexpr Flag& operator=(const Flag& other) = default;

    constexpr Flag operator~() const { return Flag{~bits}; }
    constexpr Flag operator|(Flag rhs) const { return Flag{bits | rhs.bits}; }
    constexpr Flag operator&(Flag rhs) const { return Flag{bits & rhs.bits}; }
    constexpr Flag operator^(Flag rhs) const { return Flag{bits ^ rhs.bits}; }
    constexpr Flag operator<<(size_t shift) const { return Flag{bits << shift}; }
    constexpr Flag operator>>(size_t shift) const { return Flag{bits >> shift}; }

    // Mixed Flag <op> Flags (single-bit) for ergonomics
    constexpr Flag operator|(Flags rhs) const { return Flag{bits | (T(1) << static_cast<size_t>(rhs))}; }
    constexpr Flag operator&(Flags rhs) const { return Flag{bits & (T(1) << static_cast<size_t>(rhs))}; }
    constexpr Flag operator^(Flags rhs) const { return Flag{bits ^ (T(1) << static_cast<size_t>(rhs))}; }

    constexpr Flag& operator|=(Flag rhs) { bits |= rhs.bits; return *this; }
    constexpr Flag& operator&=(Flag rhs) { bits &= rhs.bits; return *this; }
    constexpr Flag& operator^=(Flag rhs) { bits ^= rhs.bits; return *this; }
    constexpr Flag& operator<<=(size_t shift) { bits <<= shift; return *this; }
    constexpr Flag& operator>>=(size_t shift) { bits >>= shift; return *this; }

    // Mixed compound ops with enum bit
    constexpr Flag& operator|=(Flags rhs) { bits |= (T(1) << static_cast<size_t>(rhs)); return *this; }
    constexpr Flag& operator&=(Flags rhs) { bits &= (T(1) << static_cast<size_t>(rhs)); return *this; }
    constexpr Flag& operator^=(Flags rhs) { bits ^= (T(1) << static_cast<size_t>(rhs)); return *this; }

    constexpr bool operator==(Flag rhs) const { return bits == rhs.bits; }
    constexpr bool operator!=(Flag rhs) const { return bits != rhs.bits; }

    constexpr explicit operator bool() const { return Any(); }

    [[nodiscard]] constexpr int32_t CountTrue() const { return std::popcount(bits); }
    [[nodiscard]] constexpr int32_t CountConsecutiveFalseFromLeft() const { return std::countl_zero(bits); }
    [[nodiscard]] constexpr int32_t CountConsecutiveTrueFromLeft() const { return std::countl_one(bits); }
    [[nodiscard]] constexpr int32_t CountConsecutiveFalseFromRight() const { return std::countr_zero(bits); }
    [[nodiscard]] constexpr int32_t CountConsecutiveTrueFromRight() const { return std::countr_one(bits); }

    [[nodiscard]] constexpr bool Any() const { return bits != 0; }
    [[nodiscard]] constexpr bool One() const { return std::popcount(bits) == 1; }
    [[nodiscard]] constexpr bool None() const { return bits == 0; } // No bits are set
    [[nodiscard]] constexpr bool All() const { return bits == std::numeric_limits<decltype(bits)>::max(); } // All bits are set

    [[nodiscard]] constexpr bool Test(size_t index) const { return (bits & (T(1) << index)) != 0; }
    constexpr void Set(size_t index) { bits |= (T(1) << index); }
    constexpr void Reset(size_t index) { bits &= ~(T(1) << index); }
    constexpr void Flip(size_t index) { bits ^= (T(1) << index); }

    [[nodiscard]] constexpr bool Test(Flags index) const { return (bits & (T(1) << static_cast<size_t>(index))) != 0; }
    constexpr void Set(Flags index) { bits |= (T(1) << static_cast<size_t>(index)); }
    constexpr void Reset(Flags index) { bits &= ~(T(1) << static_cast<size_t>(index)); }
    constexpr void Flip(Flags index) { bits ^= (T(1) << static_cast<size_t>(index)); }

    [[nodiscard]] constexpr bool Any(Flag flag) const { return (bits & flag.bits) != 0; }
    [[nodiscard]] constexpr bool One(Flag flag) const { return std::popcount(bits & flag.bits) == 1; }
    [[nodiscard]] constexpr bool Has(Flag flag) const { return (bits & flag.bits) == flag.bits; }
    [[nodiscard]] constexpr bool None(Flag flag) const { return (bits & flag.bits) == 0; }

    // Mask-based mutators for convenience
    constexpr void Set(Flag mask) { bits |= mask.bits; }
    constexpr void Reset(Flag mask) { bits &= ~mask.bits; }
    constexpr void Flip(Flag mask) { bits ^= mask.bits; }

    constexpr void RotateLeft(int32_t shift) { bits = std::rotl(bits, shift); }
    constexpr void RotateRight(int32_t shift) { bits = std::rotr(bits, shift); }
    constexpr Flag RotateLeft(int32_t shift) const { return std::rotl(bits, shift); }
    constexpr Flag RotateRight(int32_t shift) const { return std::rotr(bits, shift); }

    // Raw accessors
    [[nodiscard]] constexpr T Bits() const { return bits; }
    static constexpr Flag FromBits(T value) { return Flag{value}; }

    // Symmetric Flags <op> Flag via friends (single-bit on Flags side)
    friend constexpr Flag operator|(Flags lhs, Flag rhs) { return Flag{(T(1) << static_cast<size_t>(lhs)) | rhs.bits}; }
    friend constexpr Flag operator&(Flags lhs, Flag rhs) { return Flag{(T(1) << static_cast<size_t>(lhs)) & rhs.bits}; }
    friend constexpr Flag operator^(Flags lhs, Flag rhs) { return Flag{(T(1) << static_cast<size_t>(lhs)) ^ rhs.bits}; }
  };
};
