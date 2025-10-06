export module compiler.ir.Symbols;

import <cstdint>;
import <bitset>;
import <vector>;
import compiler.ir.Index;

// ────────────────────────────────────────────────────────────────
//  Symbols table: columnar (SoA) style representation of Symbol data
// ────────────────────────────────────────────────────────────────

// Tree topology: Preorder layout with subtree interval (DFS numbering) — nodes are stored in DFS-preorder; each node stores parent and an inclusive subtree end index to hop over whole subtrees in O(1) per child.
namespace compiler::ir {
  export class Symbols {
  private:
    std::vector<std::bitset<64>> registers; // Live-range bitset
    std::vector<std::bitset<64>> flags; // Modifiers, type information, etc
    std::vector<uint64_t> values; // A generic typeless field: immediate literal, pointer/index, etc
    std::vector<int32_t> displacements; // Byte offset from the parent for runtime memory layout

    // 2. Hierarchy / metadata
    std::vector<ir::Index> parents;
    std::vector<ir::Index> names; // A string or enum literal symbol or 0 for undefined

    // 3. Source mapping
    std::vector<uint32_t> tokens; // The index of the first token that defined this symbol
  public:
    const std::bitset<64>& Registers(ir::Index i) const { return registers[i.Value()]; }
    const std::bitset<64>& Flags(ir::Index i) const { return flags[i.Value()]; }
    uint64_t Value(ir::Index i) const { return values[i.Value()]; }
    int32_t Displacement(ir::Index i) const { return displacements[i.Value()]; }
    ir::Index Parent(ir::Index i) const { return parents[i.Value()]; }
    ir::Index Name(ir::Index i) const { return names[i.Value()]; }

    void Registers(ir::Index i, std::bitset<64> v) { registers[i.Value()] = v; }
    void Flags(ir::Index i, std::bitset<64> v) { flags[i.Value()] = v; }
    void Value(ir::Index i, uint64_t v) { values[i.Value()] = v; }
    void Displacement(ir::Index i, int32_t v) { displacements[i.Value()] = v; }
    void Parent(ir::Index i, ir::Index v) { parents[i.Value()] = v; }
    void Name(ir::Index i, ir::Index v) { names[i.Value()] = v; }

    ir::Index Add(ir::symbol::Type type) {
      ir::Index index = static_cast<uint32_t>(values.size()); // Take the size of any column, they should all be the same

      registers.emplace_back();
      flags.emplace_back(static_cast<uint64_t>(type));
      values.push_back(0);
      displacements.push_back(0);
      parents.push_back({});
      names.push_back({});
      tokens.push_back(0);

      return index;
    }

    size_t Count() const { return values.size(); }

    void Resize(size_t n) {
      registers.resize(n);
      flags.resize(n);
      values.resize(n);
      displacements.resize(n);
      parents.resize(n);
      names.resize(n);
      tokens.resize(n);
    }
  };
};