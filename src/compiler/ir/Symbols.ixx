export module ir.symbols;

import <cstdint>;
import <bitset>;
import <vector>;
import ir.index;

// ────────────────────────────────────────────────────────────────
//  Symbols table: columnar (SoA) style representation of Symbol data
// ────────────────────────────────────────────────────────────────
namespace ir {
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
    const std::bitset<64>& Registers(size_t i) const { return registers[i]; }
    const std::bitset<64>& Flags(size_t i) const { return flags[i]; }
    uint64_t Value(size_t i) const { return values[i]; }
    int32_t Displacement(size_t i) const { return displacements[i]; }
    uint32_t Size(size_t i) const { return sizes[i]; }
    ir::Index Parent(size_t i) const { return parents[i];}
    ir::Index Name(size_t i) const { return names[i]; }
    ir::Index Children(size_t i) const { return children[i];  }
    ir::Index Prev(size_t i) const { return prevs[i]; }

    void Registers(size_t i, std::bitset<64> v) { registers[i] = v; }
    void Flags(size_t i, std::bitset<64> v) { flags[i] = v; }
    void Value(size_t i, uint64_t v) { values[i] = v; }
    void Displacement(size_t i, int32_t v) { displacements[i] = v; }
    void Size(size_t i, uint32_t v) { sizes[i] = v; }
    void Parent(size_t i, ir::Index v) { parents[i] = v;}
    void Name(size_t i, ir::Index v) { names[i] = v; }
    void Children(size_t i, ir::Index v) { children[i] = v; }
    void Prev(size_t i, ir::Index v) { prevs[i] = v; }

    uint32_t Emplace() {
      const uint32_t slot = static_cast<uint32_t>(registers.size());

      registers.emplace_back();
      flags.emplace_back();
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

      return slot;
    }
  };
};