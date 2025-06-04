export module code.flag;

import <cstdint>;
import <bitset>;
import <bit>;
import <limits>;
import <string>;

export namespace code {
  // These will be used as indices on a bitset.
  enum class Flags : uint8_t {
    // Most instructions have different versions based on size.
    // These flags help the encoder select the best one.
    // For example, on x86-64 if an immediate in an ADD is flagged SIZE8 it'll prefer 0x83 over 0x81.
    SIZE8,
    SIZE16,
    SIZE32,
    SIZE64,
    SIZE128,
    SIZE256,
    SIZE512,

    // Used by addresses to specify a scale factor.
    // Architectures that don't support a scale factor must implement it manually.
    SCALE2,
    SCALE4,
    SCALE8,

    // Alignment is specified in bits, not bytes, in order to maintain symmetry with the SIZE flags.
    // Any operand without an alignment is assumed to be unaligned.
    ALIGN8,
    ALIGN16,
    ALIGN32,
    ALIGN64,
    ALIGN128,
    ALIGN256,
    ALIGN512,

    // These are used to specify addressing modes. While I'm using the x86-64 naming convention,
    // these concepts are quite generic across all hardware.
    RIP, // Relative to the instruction pointer (AKA program counter)
    RAP, // Relative to the absolute pointer (AKA an absolute address, a positive displacement from 0)
    RSP, // Relative to the stack pointer
    RBP, // Relative to the base pointer
    RTP, // Relative to the `this` pointer (AKA object pointer)
    RDP, // Relative to the data pointer for static data access

    // RMP, // Relative to the virtual memory pointer (AKA an absolute address, a positive displacement from 0)
    // RPP, // Relative to the parameter pointer
    // RHP, // Relative to a heap pointer for dynamic data access

    // // These indicate which pointer is used to perform an address lookup
    // INSTRUCTION, // In the instruction buffer, accessed via the instruction pointer/program counter
    // DATA, // In the static data buffer, accessed as a known offset from the IP/PC
    // STACK, // Accessed via the stack pointer
    // MEMBER, // Accessed via a compile-time offset from a base address
    // HEAP, // Accessed via a runtime pointer that is in the stack

    // Many instructions have different versions based on if the operand(s) are signed/unsigned/decimal.
    NUMBER, // Unsigned integer
    INTEGER, // Signed integer
    DECIMAL, // Floating point
    BOOL,
    _NULL,
    UNDEFINED,
    BITSET, // Not a Number
    POINTER, // The operand is a pointer to a memory location

    // These specify that SIMD instructions should be used. This has some overlap with DECIMAL, which
    // typically also results in SIMD instructions being used, but they are not the same.
    // The operands in `1.0 + 5.0` should be both DECIMAL and SCALAR.
    VECTOR,
    SCALAR,

    MUTABLE, // Overrides the default immutability
    CONST, // A more powerful form of immutability that overrides MUTABLE
    ATOMIC, // The access should be controlled via locks
    IMMEDIATE, // Indicates an address's displacement should be encoded as an immediate
    INVERTED, // Indicates an address should use inverted (negative) displacement

    INLINE_PREFERRED,
    INLINE_AVOIDED,

    PREFETCH,
    UNCACHED, // Operand should not be cached
    MASKED, // Useful in SIMD instructions
    SIMD, // 
    STEP, // 

    // The endian flags are useful for instructions like BSWAP. The encoder knows its endianness, so it knows what to expect.
    LITTLE_ENDIAN,
    BIG_ENDIAN,

    BUSY, // Indicates that this register operand is already in use and must be spilled

    LOGICAL_RESULT, // Indicates this operand is exclusively used as a boolean result of logical operations
    LOGICAL_NEGATE, // Indicates that the operand should be inverted in logical operations

    SHIFT,
    RUNTIME,
    COMTIME,
    RETURNED, // Marks the value as being the return of the function
    LVALUE,
    RVALUE,
    ACCUMULATOR,
    ARITHMETIC,
    COUNTER, // Used for counting, often in loops
    VIRTUAL, // Indicates a register is virtual and isn't mapped to real hardware register

    // These represent the hardware flag register on architectures that use them, like x86-64
    CARRY_FLAG,
    PARITY_FLAG,
    ZERO_FLAG,
    SIGN_FLAG,
    DIRECTION_FLAG,
    OVERFLOW_FLAG,

    SPREAD, // Indicates a symbol is spread into another container, such as `[a, ...arr]`
    REPEAT, // Indicates a symbol is repeated in another container, such as `[0; 10]`
  };

  class Flag {
  private:
    uint64_t bits;

    constexpr Flag(uint64_t bits) : bits{bits} {}

    static consteval Flag Cast(Flags f) {
      Flag flag{0};
      flag.Set(static_cast<size_t>(f));
      return flag;
    }
  public:
    constexpr Flag() = default;
    constexpr Flag(Flags index) : bits{uint64_t(1) << static_cast<size_t>(index)} {}
    constexpr Flag(const Flag& other) = default;
    constexpr Flag& operator=(const Flag& other) = default;

    constexpr Flag operator~() const { return Flag{~bits}; }
    constexpr Flag operator|(Flag rhs) const { return Flag{bits | rhs.bits}; }
    constexpr Flag operator&(Flag rhs) const { return Flag{bits & rhs.bits}; }
    constexpr Flag operator^(Flag rhs) const { return Flag{bits ^ rhs.bits}; }
    constexpr Flag operator<<(size_t shift) const { return Flag{bits << shift}; }
    constexpr Flag operator>>(size_t shift) const { return Flag{bits >> shift}; }

    constexpr Flag& operator|=(Flag rhs) { bits |= rhs.bits; return *this; }
    constexpr Flag& operator&=(Flag rhs) { bits &= rhs.bits; return *this; }
    constexpr Flag& operator^=(Flag rhs) { bits ^= rhs.bits; return *this; }
    constexpr Flag& operator<<=(size_t shift) { bits <<= shift; return *this; }
    constexpr Flag& operator>>=(size_t shift) { bits >>= shift; return *this; }

    constexpr bool operator==(Flag rhs) const { return bits == rhs.bits; }
    constexpr bool operator!=(Flag rhs) const { return bits != rhs.bits; }

    constexpr explicit operator bool() const { return Any(); }

    constexpr int32_t CountTrue() const { return std::popcount(bits); }
    constexpr int32_t CountConsecutiveFalseFromLeft() const { return std::countl_zero(bits); }
    constexpr int32_t CountConsecutiveTrueFromLeft() const { return std::countl_one(bits); }
    constexpr int32_t CountConsecutiveFalseFromRight() const { return std::countr_zero(bits); }
    constexpr int32_t CountConsecutiveTrueFromRight() const { return std::countr_one(bits); }

    constexpr bool Any() const { return bits != 0; }
    constexpr bool One() const { return std::popcount(bits) == 1; }
    constexpr bool None() const { return bits == 0; }
    constexpr bool All() const { return bits == std::numeric_limits<decltype(bits)>::max(); }

    constexpr bool Test(size_t index) const { return (bits & (uint64_t(1) << index)) != 0; }
    constexpr void Set(size_t index) { bits |= (uint64_t(1) << index); }
    constexpr void Reset(size_t index) { bits &= ~(uint64_t(1) << index); }
    constexpr void Flip(size_t index) { bits ^= (uint64_t(1) << index); }

    constexpr bool Test(Flags index) const { return (bits & (uint64_t(1) << static_cast<size_t>(index))) != 0; }
    constexpr void Set(Flags index) { bits |= (uint64_t(1) << static_cast<size_t>(index)); }
    constexpr void Reset(Flags index) { bits &= ~(uint64_t(1) << static_cast<size_t>(index)); }
    constexpr void Flip(Flags index) { bits ^= (uint64_t(1) << static_cast<size_t>(index)); }

    constexpr bool Any(Flag flag) const { return (bits & flag.bits) != 0; }
    constexpr bool One(Flag flag) const { return std::popcount(bits & flag.bits) == 1; }
    constexpr bool Has(Flag flag) const { return (bits & flag.bits) == flag.bits; }
    constexpr bool None(Flag flag) const { return (bits & flag.bits) == 0; }
    constexpr bool All(Flag flag) const { return (bits & flag.bits) == std::numeric_limits<decltype(bits)>::max(); }

    constexpr void RotateLeft(int32_t shift) { bits = std::rotl(bits, shift); }
    constexpr void RotateRight(int32_t shift) { bits = std::rotr(bits, shift); }
    constexpr Flag RotateLeft(int32_t shift) const { return std::rotl(bits, shift); }
    constexpr Flag RotateRight(int32_t shift) const { return std::rotr(bits, shift); }

    std::string ToString() { return "Flag TODO"; }

    static consteval Flag SIZE8() { return Cast(Flags::SIZE8); }
    static consteval Flag SIZE16() { return Cast(Flags::SIZE16); }
    static consteval Flag SIZE32() { return Cast(Flags::SIZE32); }
    static consteval Flag SIZE64() { return Cast(Flags::SIZE64); }
    static consteval Flag SIZE128() { return Cast(Flags::SIZE128); }
    static consteval Flag SIZE256() { return Cast(Flags::SIZE256); }
    static consteval Flag SIZE512() { return Cast(Flags::SIZE512); }
    static consteval Flag SCALE2() { return Cast(Flags::SCALE2); }
    static consteval Flag SCALE4() { return Cast(Flags::SCALE4); }
    static consteval Flag SCALE8() { return Cast(Flags::SCALE8); }
    static consteval Flag ALIGN8() { return Cast(Flags::ALIGN8); }
    static consteval Flag ALIGN16() { return Cast(Flags::ALIGN16); }
    static consteval Flag ALIGN32() { return Cast(Flags::ALIGN32); }
    static consteval Flag ALIGN64() { return Cast(Flags::ALIGN64); }
    static consteval Flag ALIGN128() { return Cast(Flags::ALIGN128); }
    static consteval Flag ALIGN256() { return Cast(Flags::ALIGN256); }
    static consteval Flag ALIGN512() { return Cast(Flags::ALIGN512); }
    static consteval Flag RIP() { return Cast(Flags::RIP); }
    static consteval Flag RSP() { return Cast(Flags::RSP); }
    static consteval Flag RBP() { return Cast(Flags::RBP); }
    // static consteval Flag RPP() { return Cast(Flags::RPP); }
    static consteval Flag RTP() { return Cast(Flags::RTP); }
    static consteval Flag RDP() { return Cast(Flags::RDP); }
    // static consteval Flag RMP() { return Cast(Flags::RMP); }
    static consteval Flag RAP() { return Cast(Flags::RAP); }
    static consteval Flag NUMBER() { return Cast(Flags::NUMBER); }
    static consteval Flag INTEGER() { return Cast(Flags::INTEGER); }
    static consteval Flag DECIMAL() { return Cast(Flags::DECIMAL); }
    static consteval Flag BOOL() { return Cast(Flags::BOOL); }
    static consteval Flag _NULL() { return Cast(Flags::_NULL); }
    static consteval Flag UNDEFINED() { return Cast(Flags::UNDEFINED); }
    static consteval Flag BITSET() { return Cast(Flags::BITSET); }
    static consteval Flag POINTER() { return Cast(Flags::POINTER); }
    static consteval Flag VECTOR() { return Cast(Flags::VECTOR); }
    static consteval Flag SCALAR() { return Cast(Flags::SCALAR); }
    static consteval Flag MUTABLE() { return Cast(Flags::MUTABLE); }
    static consteval Flag CONST() { return Cast(Flags::CONST); }
    static consteval Flag ATOMIC() { return Cast(Flags::ATOMIC); }
    static consteval Flag IMMEDIATE() { return Cast(Flags::IMMEDIATE); }
    static consteval Flag INVERTED() { return Cast(Flags::INVERTED); }
    static consteval Flag INLINE_PREFERRED() { return Cast(Flags::INLINE_PREFERRED); }
    static consteval Flag INLINE_AVOIDED() { return Cast(Flags::INLINE_AVOIDED); }
    static consteval Flag PREFETCH() { return Cast(Flags::PREFETCH); }
    static consteval Flag UNCACHED() { return Cast(Flags::UNCACHED); }
    static consteval Flag MASKED() { return Cast(Flags::MASKED); }
    static consteval Flag SIMD() { return Cast(Flags::SIMD); }
    static consteval Flag STEP() { return Cast(Flags::STEP); }
    static consteval Flag LITTLE_ENDIAN() { return Cast(Flags::LITTLE_ENDIAN); }
    static consteval Flag BIG_ENDIAN() { return Cast(Flags::BIG_ENDIAN); }
    static consteval Flag BUSY() { return Cast(Flags::BUSY); }
    static consteval Flag LOGICAL_RESULT() { return Cast(Flags::LOGICAL_RESULT); }
    static consteval Flag LOGICAL_NEGATE() { return Cast(Flags::LOGICAL_NEGATE); }
    static consteval Flag SHIFT() { return Cast(Flags::SHIFT); }
    static consteval Flag RUNTIME() { return Cast(Flags::RUNTIME); }
    static consteval Flag COMTIME() { return Cast(Flags::COMTIME); }
    static consteval Flag RETURNED() { return Cast(Flags::RETURNED); }
    static consteval Flag LVALUE() { return Cast(Flags::LVALUE); }
    static consteval Flag RVALUE() { return Cast(Flags::RVALUE); }
    static consteval Flag ACCUMULATOR() { return Cast(Flags::ACCUMULATOR); }
    static consteval Flag ARITHMETIC() { return Cast(Flags::ARITHMETIC); }
    static consteval Flag COUNTER() { return Cast(Flags::COUNTER); }
    static consteval Flag VIRTUAL() { return Cast(Flags::VIRTUAL); }
    static consteval Flag CARRY_FLAG() { return Cast(Flags::CARRY_FLAG); }
    static consteval Flag PARITY_FLAG() { return Cast(Flags::PARITY_FLAG); }
    static consteval Flag ZERO_FLAG() { return Cast(Flags::ZERO_FLAG); }
    static consteval Flag SIGN_FLAG() { return Cast(Flags::SIGN_FLAG); }
    static consteval Flag DIRECTION_FLAG() { return Cast(Flags::DIRECTION_FLAG); }
    static consteval Flag OVERFLOW_FLAG() { return Cast(Flags::OVERFLOW_FLAG); }

    static consteval Flag Sizes() { return Flag::SIZE8() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64() | Flag::SIZE128() | Flag::SIZE256() | Flag::SIZE512(); }
    static consteval Flag Alignments() { return Flag::ALIGN8() | Flag::ALIGN16() | Flag::ALIGN32() | Flag::ALIGN64() | Flag::ALIGN128() | Flag::ALIGN256() | Flag::ALIGN512(); }

    static consteval Flag StandardSizes() { return Flag::SIZE8() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64(); }
    static consteval Flag StandardAlignments() { return Flag::ALIGN8() | Flag::ALIGN16() | Flag::ALIGN32() | Flag::ALIGN64(); }

    constexpr bool Is8BitAligned() const { return Has(Flag::ALIGN8()); }
    constexpr bool Is16BitAligned() const { return Has(Flag::ALIGN16()); }
    constexpr bool Is32BitAligned() const { return Has(Flag::ALIGN32()); }
    constexpr bool Is64BitAligned() const { return Has(Flag::ALIGN64()); }
    constexpr bool Is128BitAligned() const { return Has(Flag::ALIGN128()); }
    constexpr bool Is256BitAligned() const { return Has(Flag::ALIGN256()); }
    constexpr bool Is512BitAligned() const { return Has(Flag::ALIGN512()); }
    constexpr bool Is8Bit() const { return Has(Flag::SIZE8()); }
    constexpr bool Is16Bit() const { return Has(Flag::SIZE16()); }
    constexpr bool Is32Bit() const { return Has(Flag::SIZE32()); }
    constexpr bool Is64Bit() const { return Has(Flag::SIZE64()); }
    constexpr bool Is128Bit() const { return Has(Flag::SIZE128()); }
    constexpr bool Is256Bit() const { return Has(Flag::SIZE256()); }
    constexpr bool Is512Bit() const { return Has(Flag::SIZE512()); }
    constexpr bool IsByte() const { return Has(Flag::SIZE8()); }
    constexpr bool IsHalfWord() const { return Has(Flag::SIZE16()); }
    constexpr bool IsWord() const { return Has(Flag::SIZE32()); }
    constexpr bool IsDoubleWord() const { return Has(Flag::SIZE64()); }
    constexpr bool IsSmallWord() const { return Any(Flag::SIZE16() | Flag::SIZE32()); }
    constexpr bool IsAnyWord() const { return Any(Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64()); }
    constexpr bool IsBigWord() const { return Any(Flag::SIZE32() | Flag::SIZE64()); }
    constexpr bool IsOversized() const { return Any(Flag::SIZE128() | Flag::SIZE256() | Flag::SIZE512()); }
    constexpr bool IsUnsized() const { return None(Sizes()); }
    constexpr bool IsUnaligned() const { return None(Flag::ALIGN8() | Flag::ALIGN16() | Flag::ALIGN32() | Flag::ALIGN64() | Flag::ALIGN128() | Flag::ALIGN256() | Flag::ALIGN512()); }
    constexpr bool IsRIP() const { return Has(Flag::RIP()); }
    constexpr bool IsRAP() const { return Has(Flag::RAP()); }
    constexpr bool IsRSP() const { return Has(Flag::RSP()); }
    constexpr bool IsRBP() const { return Has(Flag::RBP()); }
    // constexpr bool IsRPP() const { return Has(Flag::RPP()); }
    constexpr bool IsRDP() const { return Has(Flag::RDP()); }
  };

  consteval Flag operator|(Flags lhs, Flags rhs) { return Flag{lhs} | Flag{rhs}; }
  consteval Flag operator&(Flags lhs, Flags rhs) { return Flag{lhs} & Flag{rhs}; }
  consteval Flag operator^(Flags lhs, Flags rhs) { return Flag{lhs} ^ Flag{rhs}; }
};
