export module code._register;

import code.flag;
import code.operand;
import utility.print;

import <cstdint>;
import <limits>;
import <string>;

namespace code {
  export class Register : public OperandTemplate<Register> {
  private:
    // The register identifier, only meaningful to the hardware specific encoders
    uint8_t id{std::numeric_limits<uint8_t>::max()};
    uint8_t score{0};
    float heat{1.0};
  public:
    constexpr Register() = default;
    constexpr Register(code::Flag flags) : Super{flags} {}
    constexpr Register(uint8_t id) : id{id} {}
    constexpr Register(const Register& other) = default;

    constexpr Register(const Register& other, code::Flag f)
      : Super{f}, id{other.id}, score{other.score}, heat{other.heat}
    {
    }

    constexpr Register& operator=(const Register& other) = default;

    bool Resolved() const { return id != std::numeric_limits<uint8_t>::max(); }
    explicit operator bool() const { return Resolved(); }

    // constexpr bool operator==(const Register& other) const { return id == other.id; }
    // constexpr bool operator!=(const Register& other) const { return id != other.id; }

    constexpr bool operator==(const Register& other) const { return this == &other; }
    constexpr bool operator!=(const Register& other) const { return this != &other; }

    constexpr void ID(uint8_t v) { id = v; }
    constexpr uint8_t ID() const { return id; }

    constexpr void Value(uint8_t v) { id = v; }
    constexpr uint8_t Value() const { return id; }

    constexpr bool IsHotterThan(const Register& other) const { return heat > other.heat; }

    constexpr void Reset() {
      id = std::numeric_limits<uint8_t>::max();
    }

    constexpr void AddHeat(float h) {
      heat += h;
    }

    constexpr void UpdateHeat(bool is_used, uint32_t depth) {
      float depth_factor = 1.0f + (0.1f * depth); // Increase effect based on scope depth

      if (is_used) {
        heat *= 1.1f * depth_factor; // Heat up the register when used
      } else {
        heat *= 0.9f; // Cool down the register when not used
      }
    }

    // constexpr bool IsLogical() const {
    //   return Any(Flag::EQUAL() | Flag::GREATER() | Flag::LESSER() | Flag::AND() | Flag::OR() | Flag::XOR() | Flag::NOT());
    // }

    // constexpr bool IsArithmetic() const {
    //   return Any(Flag::ADD() | Flag::SUBTRACT() | Flag::MULTIPLY() | Flag::DIVIDE());
    // }
    
    // constexpr bool IsControl() const {
    //   return Any(Flag::JUMP() | Flag::CALL() | Flag::NEAR() | Flag::FAR() | Flag::PREDICATE());
    // }

    // constexpr bool IsStack() const {
    //   return Any(Flag::PUSH() | Flag::POP());
    // }

    constexpr bool IsSIMD() const { return Any(Flag::SCALAR() | Flag::VECTOR()); }

    uint64_t Alignment() const {
      if (Is8BitAligned()) return 1;
      if (Is16BitAligned()) return 2;
      if (Is32BitAligned()) return 4;
      if (Is64BitAligned()) return 8;
      if (Is128BitAligned()) return 16;
      if (Is256BitAligned()) return 32;
      if (Is512BitAligned()) return 64;
    }

    uint64_t Size() const {
      if (Is8Bit()) return 1;
      if (Is16Bit()) return 2;
      if (Is32Bit()) return 4;
      if (Is64Bit()) return 8;
      if (Is128Bit()) return 16;
      if (Is256Bit()) return 32;
      if (Is512Bit()) return 64;
    }

    constexpr Register R8() const { return Register{*this, (flags & ~Flag::Sizes()) | Flag::SIZE8()}; }
    constexpr Register R16() const { return Register{*this, (flags & ~Flag::Sizes()) | Flag::SIZE16()}; }
    constexpr Register R32() const { return Register{*this, (flags & ~Flag::Sizes()) | Flag::SIZE32()}; }
    constexpr Register R64() const { return Register{*this, (flags & ~Flag::Sizes()) | Flag::SIZE64()}; }
    constexpr Register R128() const { return Register{*this, (flags & ~Flag::Sizes()) | Flag::SIZE128()}; }
    constexpr Register R256() const { return Register{*this, (flags & ~Flag::Sizes()) | Flag::SIZE256()}; }
    constexpr Register R512() const { return Register{*this, (flags & ~Flag::Sizes()) | Flag::SIZE512()}; }

    constexpr Register To8Bit() const {
      Register reg{*this};
      reg.Resize8();
      return reg;
    }

    constexpr Register To16Bit() const {
      Register reg{*this};
      reg.Resize16();
      return reg;
    }

    constexpr Register To32Bit() const {
      Register reg{*this};
      reg.Resize32();
      return reg;
    }

    constexpr Register To64Bit() const {
      Register reg{*this};
      reg.Resize64();
      return reg;
    }

    constexpr Register To128Bit() const {
      Register reg{*this};
      reg.Resize128();
      return reg;
    }

    constexpr Register To256Bit() const {
      Register reg{*this};
      reg.Resize256();
      return reg;
    }

    constexpr Register To512Bit() const {
      Register reg{*this};
      reg.Resize512();
      return reg;
    }

    std::string ToString() const { return utility::Join("register(", id, ")"); }
  };
};