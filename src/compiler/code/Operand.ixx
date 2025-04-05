export module code.operand;

import code.flag;

import <cstdint>;

namespace code {
  export class Operand {
  private:
    static constexpr Flag SIZE_MASK = Flag::SIZE8() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64() | Flag::SIZE128() | Flag::SIZE256() | Flag::SIZE512();
    static constexpr Flag ALIGNMENT_MASK = Flag::ALIGN8() | Flag::ALIGN16() | Flag::ALIGN32() | Flag::ALIGN64() | Flag::ALIGN128() | Flag::ALIGN256() | Flag::ALIGN512();
  protected:
    code::Flag flags;

    // template<typename Derived>
    // friend class OperandTemplate;
  public:
    constexpr Operand() = default;
    constexpr Operand(code::Flag flags) : flags{flags} {}
    constexpr Operand(const Operand& other) = default;
    constexpr Operand& operator=(const Operand& other) = default;

    constexpr bool operator==(const Operand& other) const { return this == &other; }
    constexpr bool operator!=(const Operand& other) const { return this != &other; }
    constexpr explicit operator bool() const { return IsValid(); }

    constexpr void Add(code::Flag flag) { flags |= flag; }
    constexpr bool Has(code::Flag flag) const { return flags.Has(flag); }
    // constexpr bool Any(size_t mask) const { return flags.Any(mask); }
    constexpr bool Any(code::Flag flag) const { return flags.Any(flag); }
    constexpr bool None(code::Flag flag) const { return flags.None(flag); }

    constexpr void Clear(Flag flag) { flags &= ~flag; }
    constexpr void Set(Flag flag) { flags |= flag; }

    constexpr void ClearSize() { Clear(SIZE_MASK); }
    constexpr void Resize(Flag flag) { Clear(SIZE_MASK); Set(flag); }

    constexpr void Resize8() { flags = (flags & ~SIZE_MASK) | code::Flag::SIZE8(); }
    constexpr void Resize16() { flags = (flags & ~SIZE_MASK) | code::Flag::SIZE16(); }
    constexpr void Resize32() { flags = (flags & ~SIZE_MASK) | code::Flag::SIZE32(); }
    constexpr void Resize64() { flags = (flags & ~SIZE_MASK) | code::Flag::SIZE64(); }
    constexpr void Resize128() { flags = (flags & ~SIZE_MASK) | code::Flag::SIZE128(); }
    constexpr void Resize256() { flags = (flags & ~SIZE_MASK) | code::Flag::SIZE256(); }
    constexpr void Resize512() { flags = (flags & ~SIZE_MASK) | code::Flag::SIZE512(); }
    
    constexpr bool IsValid() const { return !!flags; }
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

    constexpr bool IsScale2() const { return Has(Flag::SCALE2()); }
    constexpr bool IsScale4() const { return Has(Flag::SCALE4()); }
    constexpr bool IsScale8() const { return Has(Flag::SCALE8()); }
    
    constexpr bool IsByte() const { return Has(Flag::SIZE8()); }
    constexpr bool IsHalfWord() const { return Has(Flag::SIZE16()); }
    constexpr bool IsWord() const { return Has(Flag::SIZE32()); }
    constexpr bool IsDoubleWord() const { return Has(Flag::SIZE64()); }
    constexpr bool IsSmallWord() const { return Any(Flag::SIZE16() | Flag::SIZE32()); }
    constexpr bool IsAnyWord() const { return Any(Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64()); }
    constexpr bool IsBigWord() const { return Any(Flag::SIZE32() | Flag::SIZE64()); }
    constexpr bool IsOversized() const { return Any(Flag::SIZE128() | Flag::SIZE256() | Flag::SIZE512()); }
    constexpr bool IsUnsized() const { return None(Flag::SIZE8() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64() | Flag::SIZE128() | Flag::SIZE256() | Flag::SIZE512()); }
    constexpr bool IsUnaligned() const { return None(Flag::ALIGN8() | Flag::ALIGN16() | Flag::ALIGN32() | Flag::ALIGN64() | Flag::ALIGN128() | Flag::ALIGN256() | Flag::ALIGN512()); }
    constexpr bool IsRIP() const { return Has(Flag::RIP()); }
    constexpr bool IsRAP() const { return Has(Flag::RAP()); }
    constexpr bool IsRDP() const { return Has(Flag::RDP()); }
    constexpr bool IsRSP() const { return Has(Flag::RSP()); }
    constexpr bool IsRBP() const { return Has(Flag::RBP()); }
    // constexpr bool IsRPP() const { return Has(Flag::RPP()); }
    // constexpr bool IsRMP() const { return Has(Flag::RMP()); }
    // constexpr bool IsROP() const { return Has(Flag::ROP()); }
    constexpr bool IsInstructionRelative() const { return Any(Flag::RIP() | Flag::RAP() | Flag::RDP()); }
    constexpr bool IsStackRelative() const { return Any(Flag::RSP() | Flag::RBP()); }
    constexpr bool IsRelative() const { return Any(Flag::RIP() | Flag::RAP() | Flag::RDP() | Flag::RSP() | Flag::RBP()); }
    constexpr bool IsScaled() const { return Any(Flag::SCALE2() | Flag::SCALE4() | Flag::SCALE8()); }
    // constexpr bool IsLoad() const { return Has(Flag::LOAD()); }
    // constexpr bool IsStore() const { return Has(Flag::STORE()); }
    // constexpr bool IsExchange() const { return Has(Flag::EXCHANGE()); }
    // constexpr bool IsMove() const { return Has(Flag::LOAD() | Flag::STORE() | Flag::EXCHANGE()); }
    // constexpr bool IsJump() const { return Has(Flag::JUMP()); }
    // constexpr bool IsCall() const { return Has(Flag::CALL()); }
    // constexpr bool IsNear() const { return Has(Flag::NEAR()); }
    // constexpr bool IsFar() const { return Has(Flag::FAR()); }
    // constexpr bool IsPredicate() const { return Has(Flag::PREDICATE()); }
    constexpr bool IsMutable() const { return Has(Flag::MUTABLE()); }
    constexpr bool IsConst() const { return Has(Flag::CONST()); }
    constexpr bool IsSigned() const { return Has(Flag::INTEGER()); }
    constexpr bool IsUnsigned() const { return Has(Flag::NUMBER()); }
    constexpr bool IsFloating() const { return Has(Flag::DECIMAL()); }
    // constexpr bool IsAligned() const { return Has(Flag::ALIGNED()); }
    // constexpr bool IsUnaligned() const { return Has(Flag::UNALIGNED()); }
    constexpr bool IsImmediate() const { return Has(Flag::IMMEDIATE()); }
    constexpr bool IsStep() const { return Has(Flag::STEP()); }
    constexpr bool IsResult() const { return Has(Flag::LOGICAL_RESULT()); }
    constexpr bool IsNegated() const { return Has(Flag::LOGICAL_NEGATE()); }
    // constexpr bool IsExtended() const { return Has(Flag::EXTENDED()); }
    // constexpr bool IsReadOnly() const { return Has(Flag::READ_ONLY()); }
    // constexpr bool IsWriteOnly() const { return Has(Flag::WRITE_ONLY()); }
    // constexpr bool IsPush() const { return Has(Flag::PUSH()); }
    constexpr bool IsSIMD() const { return Has(Flag::SIMD()); }
    constexpr bool IsSpilled() const { return Has(Flag::BUSY()); }
    constexpr bool IsBusy() const { return Has(Flag::BUSY()); }
    constexpr bool IsAtomic() const { return Has(Flag::ATOMIC()); }
    constexpr bool IsLocked() const { return Has(Flag::ATOMIC()); }
    constexpr bool IsSingle() const { return Has(Flag::DECIMAL() | Flag::SIZE32()); }
    constexpr bool IsDouble() const { return Has(Flag::DECIMAL() | Flag::SIZE64()); }
    constexpr bool IsArithmetic() const { return Has(Flag::ARITHMETIC()); }

    constexpr bool IsSameSize(const Operand& other) const { return false; } // TODO

    constexpr bool SharesAny(const Operand& other, Flag flag) const { return (flags & other.flags).Any(flag); }
    constexpr bool SharesAll(const Operand& other, Flag flag) const { return (flags & other.flags).Has(flag); }
    constexpr bool SharesOne(const Operand& other, Flag flag) const { return (flags & other.flags).One(flag); }
    constexpr bool SharesNone(const Operand& other, Flag flag) const { return (flags & other.flags).None(flag); }

    constexpr bool IsF32() const { return Has(Flag::DECIMAL() | Flag::SIZE32()); }
    constexpr bool IsF64() const { return Has(Flag::DECIMAL() | Flag::SIZE64()); }
    constexpr bool IsI8 () const { return Has(Flag::INTEGER() | Flag::SIZE8 ()); }
    constexpr bool IsI16() const { return Has(Flag::INTEGER() | Flag::SIZE16()); }
    constexpr bool IsI32() const { return Has(Flag::INTEGER() | Flag::SIZE32()); }
    constexpr bool IsI64() const { return Has(Flag::INTEGER() | Flag::SIZE64()); }
    constexpr bool IsU8 () const { return Has(Flag::NUMBER () | Flag::SIZE8 ()); }
    constexpr bool IsU16() const { return Has(Flag::NUMBER () | Flag::SIZE16()); }
    constexpr bool IsU32() const { return Has(Flag::NUMBER () | Flag::SIZE32()); }
    constexpr bool IsU64() const { return Has(Flag::NUMBER () | Flag::SIZE64()); }

    constexpr bool IsStandardSize() const { return Has(Flag::SIZE8() | Flag::SIZE16() | Flag::SIZE32() | Flag::SIZE64()); }
    // constexpr bool IsSameSize(const Operand& other) const { return false; } // TODO
  };

  export template<typename Derived>
  class OperandTemplate : public Operand {
  private:
  protected:
    using Super = OperandTemplate<Derived>;
  public:
    using Operand::Operand;

    constexpr Derived operator~() const { return Derived{static_cast<Derived&>(*this), ~flags}; }
    constexpr Derived operator|(Flag rhs) const { return Derived{static_cast<Derived&>(*this), flags | rhs}; }
    constexpr Derived operator&(Flag rhs) const { return Derived{static_cast<Derived&>(*this), flags & rhs}; }
    constexpr Derived operator^(Flag rhs) const { return Derived{static_cast<Derived&>(*this), flags ^ rhs}; }

    constexpr Derived& operator|=(Flag rhs) {
      flags |= rhs;
      return static_cast<Derived&>(*this);
    }
    
    constexpr Derived& operator&=(Flag rhs) {
      flags &= rhs;
      return static_cast<Derived&>(*this);
    }
    
    constexpr Derived& operator^=(Flag rhs) {
      flags ^= rhs;
      return static_cast<Derived&>(*this);
    }

    constexpr Derived Clear(Flag flag) const { return {static_cast<const Derived&>(*this), flags & ~flag}; }
    constexpr Derived Set(Flag flag) const { return {static_cast<const Derived&>(*this), flags | flag}; }
  };
};