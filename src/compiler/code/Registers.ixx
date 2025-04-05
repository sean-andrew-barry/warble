export module code.registers;

import utility.print;
import utility.random;
import <bitset>;
import <limits>;
import <random>;

namespace code {
  export class Registers {
  private:
    static constexpr auto SIZE = 64;
    std::bitset<SIZE> registers;
  protected:
    constexpr Registers(std::bitset<SIZE> registers) : registers{registers} {}
  public:
    constexpr Registers() = default;
    constexpr Registers(const Registers& other) = default;
    constexpr Registers& operator=(const Registers& other) = default;

    constexpr void Set(size_t index) { registers.set(index); }
    constexpr void Set() { registers.set(); }
    constexpr void Reset(size_t index) { registers.reset(index); }
    constexpr void Reset() { registers.reset(); }
    constexpr void Flip(size_t index) { registers.flip(index); }
    constexpr void Flip() { registers.flip(); }
    constexpr bool Test(size_t index) const { return registers.test(index); }
    constexpr bool Any() const { return registers.any(); }
    constexpr bool None() const { return registers.none(); }
    constexpr bool Compatible(Registers other) const { return (registers & other.registers).none(); }
    constexpr bool Overlaps(Registers other) const { return (registers & other.registers).any(); }
    static constexpr size_t Size() { return SIZE; }

    constexpr Registers operator&(const Registers& other) const { return Registers{registers & other.registers}; }
    constexpr Registers operator|(const Registers& other) const { return Registers{registers | other.registers}; }

    constexpr Registers& operator&=(const Registers& other) { registers &= other.registers; return *this; }
    constexpr Registers& operator|=(const Registers& other) { registers |= other.registers; return *this; }

    constexpr bool operator==(const Registers& other) const { return registers == other.registers; }
    constexpr bool operator!=(const Registers& other) const { return registers != other.registers; }
    constexpr explicit operator uint8_t() const { return static_cast<uint8_t>(registers.to_ulong()); }
    constexpr explicit operator uint16_t() const { return static_cast<uint16_t>(registers.to_ulong()); }
    constexpr explicit operator uint32_t() const { return static_cast<uint32_t>(registers.to_ulong()); }
    constexpr explicit operator uint64_t() const { return static_cast<uint64_t>(registers.to_ullong()); }

    template<uint8_t Min, uint8_t Max>
    bool Full() const {
      constexpr auto mask = Mask<Min, Max>();
      return (registers & mask) == mask;
    }

    // Create a mask for the range [Min, Max)
    // TODO: Double check that this is right
    // template<uint8_t Min, uint8_t Max>
    // consteval std::bitset<SIZE> Mask() {
    //   static_assert(Min < Max && Max <= SIZE, "Invalid range");
    //   return ((std::bitset<SIZE>{1} << Max) - 1) ^ ((std::bitset<SIZE>{1} << Min) - 1);
    // }

    template<uint8_t Min, uint8_t Max>
    static consteval std::bitset<SIZE> Mask() {
      static_assert(Min < Max && Max <= SIZE, "Invalid range");
      return (std::bitset<SIZE>((1ULL << Max) - 1) & ~std::bitset<SIZE>((1ULL << Min) - 1));
    }

    template<uint8_t Min, uint8_t Max>
    constexpr std::bitset<Max - Min> Subset() const {
      static_assert(Min < Max && Max <= SIZE, "Invalid range");

      constexpr auto mask = Mask<Min, Max>();

      // Apply the mask and shift the bits down to the least significant position
      auto subset = (registers & mask) >> Min;

      // Convert to the result bitset size
      return std::bitset<Max - Min>{subset.to_ullong()};
    }

    template<uint8_t Min, uint8_t Max>
    constexpr uint8_t Random() {
      static_assert(Min < Max, "Min must be less than Max");
      static_assert(Max < SIZE, "Max must be less than the size of the bitset");

      // constexpr uint8_t MIN = 0;
      // constexpr uint8_t MAX = 16;
      constexpr auto mask = Mask<0, 16>();
      // constexpr auto mask = Mask<Min, Max>();
      auto masked = registers & mask;

      if (masked == mask) {
        // All bits are already set within that range
        return std::numeric_limits<uint8_t>::max();
      }

      auto unset_bits = ~masked & mask;
      uint8_t count{0};
      for (uint8_t i{Min}; i <= Max; ++i) {
        if (unset_bits.test(i)) {
          ++count;
        }
      }

      // std::uniform_int_distribution<uint32_t> dist{0, count - 1};
      uint8_t target = static_cast<uint8_t>(utility::Random::XORSHIFT(0u, count - 1));
      uint8_t matched{0};

      for (uint8_t i{Min}; i <= Max; ++i) {
        if (!registers.test(i)) {
          if (matched == target) {
            return i;
          }
          ++matched;
        }
      }

      throw utility::Error("Somehow failed to select a random bit index. This should never happen.");
    }
  };
};
