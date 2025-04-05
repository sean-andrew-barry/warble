export module ir.symbol;

import <bitset>;

namespace ir {
  export struct Symbol {
    std::bitset<64> registers; // Marks exactly which registers this symbol potentially uses.
    std::bitset<64> flags;     // Modifiers, type information, etc.
    uint64_t value;            // A generic typeless field: immediate literal, pointer/index, etc.
    uint32_t size;             // Memory footprint in bytes.
    uint32_t displacement;     // Offset relative to the parent's runtime location.

    // Indexes into the symbol table.
    uint32_t name;             // Index for the string literal symbol representing the name.
    uint32_t prev;             // Index of the previous sibling symbol.
    uint32_t parent;           // Index of the parent symbol.
    uint32_t children;         // Index of the enum literal symbol for children.

    // Source mapping.
    uint32_t index;            // Start character index in the source file.
    uint32_t length;           // Number of characters for this symbol's source.
  };
};