export module ir.index;

import <stdexcept>;
import <cstdint>;

namespace ir {
  export class Index {
  private:
    static constexpr uint32_t SYMBOL_MASK = 0x3FFFF;         // Lower 18 bits.
    static constexpr uint32_t MAX_DEPENDENCY = (1u << 14);   // 16384.
    static constexpr uint32_t MAX_SYMBOL = (1u << 18);       // 262144.
  private:
    uint32_t index;
  public:
    Index() : index{0} {}

    // Constructs an Index by packing dependency and symbol.
    Index(uint32_t dependency, uint32_t symbol) {
      if (dependency >= MAX_DEPENDENCY) throw std::out_of_range("Dependency index too large");
      if (symbol >= MAX_SYMBOL) throw std::out_of_range("Symbol index too large");

      index = (dependency << 18) | (symbol & SYMBOL_MASK);
    }

    void Dependency(uint32_t dependency) {
      if (dependency >= MAX_DEPENDENCY) throw std::out_of_range("Dependency index too large");

      // Clear the current dependency bits and set the new ones.
      index = (dependency << 18) | (index & SYMBOL_MASK);
    }

    void Symbol(uint32_t symbol) {
      if (symbol >= MAX_SYMBOL) throw std::out_of_range("Symbol index too large");

      // Clear the current symbol bits and set the new ones.
      index = (index & ~SYMBOL_MASK) | (symbol & SYMBOL_MASK);
    }

    uint32_t Dependency() { return index >> 18; } // Shift off the lowest 18 bits, leaving only the dependency index
    uint32_t Symbol() { return index & SYMBOL_MASK; } // Remove the highest 14 bits, leaving only the symbol index
  };
};