export module ir.symbol;

import <bitset>;
import <cstdint>;

import ir.index;

// ────────────────────────────────────────────────────────────────
//  Symbol view: non-owning, trivially-cheap accessor
// ────────────────────────────────────────────────────────────────
namespace ir {
  export class Module;
  
  export class Symbol {
  public:
    constexpr static uint32_t NO_SLOT = 0xFFFFFFFF;
    constexpr static uint8_t NO_REGISTER = 0xFF;
  private:
    ir::Module& mod;
    uint32_t s;
    uint8_t r;
  public:
    Symbol(ir::Module& mod, uint32_t slot = NO_SLOT, uint8_t reg = NO_REGISTER)
      : mod{mod}, s{slot}, r{reg} {}

    uint8_t Register() const;
    const std::bitset<64>& Registers() const;
    const std::bitset<64>& Flags() const;
    uint64_t Value() const;
    int32_t Displacement() const;
    uint32_t Size() const;
    ir::Symbol Parent() const;
    ir::Symbol Name() const;
    ir::Symbol Children() const;
    ir::Symbol Prev() const;
    ir::Index ParentIndex() const;
    ir::Index NameIndex() const;
    ir::Index ChildrenIndex() const;
    ir::Index PrevIndex() const;

    bool IsLocal(int32_t& displacement) const;
    bool IsLocal() const;

    bool IsResolved()  const;
    bool IsEmpty()     const;
    bool IsValid()     const;
    bool IsAllocated() const;
    bool Is8Bit()      const;
    bool Is16Bit()     const;
    bool Is32Bit()     const;
    bool Is64Bit()     const;
    bool Is128Bit()    const;
    bool Is256Bit()    const;
    bool Is512Bit()    const;

    bool IsConst()    const;
    bool IsNumber()   const;
    bool IsInteger()  const;
    bool IsDecimal()  const;
    bool IsString()   const;
    bool IsBoolean()  const;
    bool IsBlock()    const;
    bool IsModule()   const;
    bool IsFunction() const;
  };
};