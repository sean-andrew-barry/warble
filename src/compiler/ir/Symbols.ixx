export module compiler.ir.Symbols;

import <cstdint>;
import <bitset>;
import <vector>;
import compiler.ir.Index;

// ────────────────────────────────────────────────────────────────
//  Symbols table: columnar (SoA) style representation of Symbol data
// ────────────────────────────────────────────────────────────────
namespace compiler::ir {
  export class Symbols {
  private:
    std::vector<std::bitset<64>> registers; // Live-range bitset
    std::vector<std::bitset<64>> flags; // Modifiers, type information, etc
    std::vector<uint64_t> values; // A generic typeless field: immediate literal, pointer/index, etc
    std::vector<int32_t> displacements; // Byte offset from the parent
    std::vector<uint32_t> sizes; // Memory footprint in bytes

    // 2. Hierarchy / metadata
    std::vector<ir::Index> parents;
    std::vector<ir::Index> prevs;
    std::vector<ir::Index> children; // An enum literal symbol
    std::vector<ir::Index> names; // A string literal symbol

    // 3. Source mapping
    std::vector<uint32_t> token_start;
    std::vector<uint32_t> token_end;
    std::vector<uint32_t> character_start;
  public:
    // -------- raw fields --------
    const std::bitset<64>& Registers(ir::Index i) const { return registers[i.Value()]; }
    const std::bitset<64>& Flags(ir::Index i) const { return flags[i.Value()]; }
    uint64_t Value(ir::Index i) const { return values[i.Value()]; }
    int32_t Displacement(ir::Index i) const { return displacements[i.Value()]; }
    uint32_t Size(ir::Index i) const { return sizes[i.Value()]; }
    ir::Index Parent(ir::Index i) const { return parents[i.Value()]; }
    ir::Index Name(ir::Index i) const { return names[i.Value()]; }
    ir::Index Children(ir::Index i) const { return children[i.Value()]; }
    ir::Index Prev(ir::Index i) const { return prevs[i.Value()]; }

    void Registers(ir::Index i, std::bitset<64> v) { registers[i.Value()] = v; }
    void Flags(ir::Index i, std::bitset<64> v) { flags[i.Value()] = v; }
    void Value(ir::Index i, uint64_t v) { values[i.Value()] = v; }
    void Displacement(ir::Index i, int32_t v) { displacements[i.Value()] = v; }
    void Size(ir::Index i, uint32_t v) { sizes[i.Value()] = v; }
    void Parent(ir::Index i, ir::Index v) { parents[i.Value()] = v; }
    void Name(ir::Index i, ir::Index v) { names[i.Value()] = v; }
    void Children(ir::Index i, ir::Index v) { children[i.Value()] = v; }
    void Prev(ir::Index i, ir::Index v) { prevs[i.Value()] = v; }

    ir::Index Add(ir::symbol::Type type) {
      ir::Index index = static_cast<uint32_t>(registers.size()); // Take the size of any column, they should all be the same

      registers.emplace_back();
      flags.emplace_back(static_cast<uint64_t>(type));
      values.push_back(0);
      displacements.push_back(0);
      sizes.push_back(0);
      parents.push_back({});
      prevs.push_back({});
      children.push_back({});
      names.push_back({});
      token_start.push_back(0);
      token_end.push_back(0);
      character_start.push_back(0);

      return index;
    }
  };
};