export module ir.symbol.Flags;

import ir.symbol.Type;
import ir.symbol.Flag;

import <bitset>;
import <cstdint>;
import <initializer_list>;
import <limits>;
import <bit>;

namespace ir::symbol {

  // Assumptions this module relies on:
  static_assert(std::numeric_limits<unsigned long long>::digits >= 64,
                "Need 64-bit unsigned long long for bitset<64>::to_ullong().");

  // Layout:
  //   bits [0..7]   : Type (uint8_t) -- exactly one type value (exclusive)
  //   bits [8..63]  : Flags (independent)
  //
  // Flag values are masks with a single bit set at positions >= 8.
  // Example: Flag::Mutable = (1ULL << 8), Flag::Async = (1ULL << 9), etc.
  //
  // Irrespective of how ir.symbol.flag is defined, this class assumes:
  //   static_cast<uint64_t>(Flag::X) yields a single-bit mask (1ULL << k), k ∈ [8,63].

  export class Flags {
  private:
    // --- constants describing the layout ---
    static constexpr uint64_t kTypeMask       = 0xFFull;     // low byte
    static constexpr int      kTypeBits       = 8;
    static constexpr int      kFirstFlagBit   = 8;           // first flag bit index
    static constexpr uint64_t kFlagsMask      = ~kTypeMask;  // high 56 bits

    // Keep std::bitset<64> as the single data member (compact, ABI-stable).
    std::bitset<64> bits_;

    // Access low byte quickly without touching std::bitset internals
    uint64_t u64() const noexcept { return bits_.to_ullong(); }

    // Cache-friendly low byte view (avoids re-masking at callsite)
    uint8_t low_byte_unsafe() const noexcept {
      return static_cast<uint8_t>(u64() & kTypeMask);
    }

    static constexpr uint64_t mask(ir::symbol::Flag f) noexcept {
      return static_cast<uint64_t>(f);
    }
  public:
    // --- ctors ---
    Flags() noexcept : bits_{0} {} // Type::Undefined, no flags
    explicit Flags(ir::symbol::Type t) noexcept : bits_{0} { set_type(t); }
    explicit Flags(uint64_t raw) noexcept : bits_{raw} {} // raw bit pattern


    // --- type access ---
    ir::symbol::Type Type() const noexcept {
      return static_cast<ir::symbol::Type>(low_byte_unsafe());
    }

    void Type(ir::symbol::Type t) noexcept {
      uint64_t u = u64();
      u = (u & ~kTypeMask) | static_cast<uint8_t>(t);
      bits_ = u;
    }

    // Resets type to Undefined (preserving flags)
    void ClearType() noexcept { bits_ &= kFlagsMask; }

    // --- single-flag ops ---
    bool Has(ir::symbol::Flag f) const noexcept { return (u64() & mask(f)) != 0; }
    void Set(ir::symbol::Flag f)       noexcept { bits_ = u64() |  mask(f); }
    void Reset(ir::symbol::Flag f)     noexcept { bits_ = u64() & ~mask(f); }
    void Toggle(ir::symbol::Flag f)    noexcept { bits_ = u64() ^  mask(f); }

    // --- convenience helpers for common single-flag predicates ---
    bool IsMutable()     const noexcept { return has(Flag::Mutable); }
    bool IsAsync()       const noexcept { return has(Flag::Async); }
    bool IsSigned()      const noexcept { return has(Flag::Signed); }
    bool IsInternal()    const noexcept { return has(Flag::Internal); }
    bool IsComtime()     const noexcept { return has(Flag::Comtime); }
    bool IsRuntime()     const noexcept { return has(Flag::Runtime); }
  };
} // namespace ir::symbol
