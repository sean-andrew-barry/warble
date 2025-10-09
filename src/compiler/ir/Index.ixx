export module compiler.ir.Index;

import <cstdint>;
import <bitset>;
import compiler.ir.symbol.Flag;
import compiler.ir.symbol.Type;

namespace compiler::program {
  export class Module;
}

namespace compiler::ir {
  export class Index {
  private:
    uint32_t index;
  public:
    Index() : index{0} {}
    Index(uint32_t v) : index{v} {}

    const std::bitset<64>& Register(program::Module& mod) const;
    const std::bitset<64>& Flag(program::Module& mod) const;
    ir::symbol::Type Type(program::Module& mod) const;
    uint64_t Value(program::Module& mod) const;
    int32_t Displacement(program::Module& mod) const;
    uint32_t Size(program::Module& mod) const;
    ir::Index Name(program::Module& mod) const;
    ir::Index Parent(program::Module& mod) const;
    ir::Index FirstChild(program::Module& mod) const;
    ir::Index LastChild(program::Module& mod) const;
    ir::Index Scope(program::Module& mod) const;
    ir::Index Loop(program::Module& mod) const;
    ir::Index Context(program::Module& mod) const;

    void Register(program::Module& mod, std::bitset<64> v);
    void Flag(program::Module& mod, std::bitset<64> v);
    void Type(program::Module& mod, ir::symbol::Type v);
    void Value(program::Module& mod, uint64_t v);
    void Value(program::Module& mod, uint32_t a, uint32_t b);
    void Displacement(program::Module& mod, int32_t v);
    void Size(program::Module& mod, uint32_t v);
    void Name(program::Module& mod, ir::Index v);
    void Parent(program::Module& mod, ir::Index v);

    bool IsValid(program::Module& mod) const;
    bool IsScope(program::Module& mod) const; // Scope symbols are any that form a scope block
    bool IsLoop(program::Module& mod) const;
    bool IsContext(program::Module& mod) const; // Context symbols are callable scopes
    bool IsStructured(program::Module& mod) const; // Structured symbols can have children

    uint32_t Value() const { return index; }
    explicit operator bool() const;
  };
};