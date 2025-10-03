export module compiler.ir.Symbol;

import <bitset>;
import <cstdint>;

import compiler.ir.Index;

namespace compiler::program {
  export class Module;
};

// ────────────────────────────────────────────────────────────────
//  Symbol view: non-owning, trivially-cheap accessor
// ────────────────────────────────────────────────────────────────
namespace compiler::ir {
  export class Symbol {
  public:
    constexpr static uint32_t NO_INDEX = 0xFFFFFFFF;
  private:
    program::Module& mod;
    ir::Index index;
  public:
    Symbol(program::Module& mod, ir::Index index = NO_INDEX)
      : mod{mod}, index{index} {}

    const std::bitset<64>& Register() const;
    const std::bitset<64>& Flag() const;
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

    void Register(std::bitset<64> v);
    void Flag(std::bitset<64> v);
    void Value(uint64_t v);
    void Value(uint32_t a, uint32_t b);
    void Displacement(int32_t v);
    void Size(uint32_t v);
    void Parent(ir::Index v);
    void Name(ir::Index v);
    void Children(ir::Index v);
    void Prev(ir::Index v);

    bool IsLocal(int32_t& displacement) const;
    bool IsLocal() const;

    bool IsResolved()  const;
    bool IsEmpty()     const;
    bool IsValid()     const;
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