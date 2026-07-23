#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include "compiler/ir/Symbol.hpp"
#include "compiler/ir/symbol/Kind.hpp"
#include "compiler/ir/symbol/Flag.hpp"
// ────────────────────────────────────────────────────────────────
//  Symbols table: columnar (SoA) style representation of Symbol data
// ────────────────────────────────────────────────────────────────

// Tree topology: Preorder layout with subtree interval (DFS numbering) — nodes are stored in DFS-preorder; each node stores parent and an inclusive subtree end index to hop over whole subtrees in O(1) per child.

// Notes:
// - Symbols can only be appended; no deletions or reordering is supported.
// - Since symbols are created depth-first, parents always have lower indexes than their children.
// - Not all symbols are structured (i.e., have children); only those that do will have subtree end indexes.
// - The parent of a symbol can be found via a backwards scan to the first symbol whose subtree end index is > the current symbol's index.
// - The next symbol after any given symbol is always either a child of that symbol or a younger sibling.

// TODO:
// - Subdivide `flags` into multiple columns for better cache locality
// - Get rid of `names`, only declarations need names, and they can be stored in the payload
// - The `parents` could be shrunk to be a `uint8_t` if `255` means it's out of range, fall back to a search.
namespace compiler::ir {
  class Symbols {
  private:
    std::vector<ir::symbol::Kind> kinds; // Symbol kinds
    std::vector<uint64_t> flags; // Modifiers, size, etc
    std::vector<uint64_t> payloads; // A generic typeless field: immediate literal, pointer/index, etc
    std::vector<uint32_t> offsets; // Byte offset from the base for runtime memory layout

    std::vector<ir::Symbol> parents;
    std::vector<ir::Symbol> names; // A string or enum literal symbol or 0 for undefined

    std::vector<uint32_t> tokens; // The index of the first token that defined this symbol
  public:
    Symbols();
    Symbols(Symbols&&) noexcept;
    Symbols& operator=(Symbols&&) noexcept;
    Symbols(const Symbols&) = delete;
    Symbols& operator=(const Symbols&) = delete;

    ir::symbol::Kind Kind(ir::Symbol i) const;
    uint64_t Flags(ir::Symbol i) const;
    uint64_t Payload(ir::Symbol i) const;
    uint32_t PayloadLow(ir::Symbol i) const;
    uint32_t PayloadHigh(ir::Symbol i) const;
    uint32_t Offset(ir::Symbol i) const;
    ir::Symbol Parent(ir::Symbol i) const;
    ir::Symbol Name(ir::Symbol i) const;
    uint32_t Token(ir::Symbol i) const;
    bool Flag(ir::Symbol i, ir::symbol::Flag f) const;

    void Kind(ir::Symbol i, ir::symbol::Kind v);
    void Flags(ir::Symbol i, uint64_t v);
    void Payload(ir::Symbol i, uint64_t v);
    void Payload(ir::Symbol i, uint32_t low, uint32_t high);
    void PayloadLow(ir::Symbol i, uint32_t v);
    void PayloadHigh(ir::Symbol i, uint32_t v);
    void Offset(ir::Symbol i, uint32_t v);
    void Parent(ir::Symbol i, ir::Symbol v);
    void Name(ir::Symbol i, ir::Symbol v);
    void Token(ir::Symbol i, uint32_t v);
    void Flag(ir::Symbol i, ir::symbol::Flag f);

    bool IsValid(ir::Symbol i) const;
    bool IsScope(ir::Symbol i) const;
    bool IsLoop(ir::Symbol i) const;
    bool IsContext(ir::Symbol i) const;
    bool IsStructured(ir::Symbol i) const;
    ir::Symbol FirstChild(ir::Symbol i) const;
    ir::Symbol LastChild(ir::Symbol i) const;
    ir::Symbol Scope(ir::Symbol i) const;
    ir::Symbol Loop(ir::Symbol i) const;
    ir::Symbol Context(ir::Symbol i) const;
    ir::Symbol Add(ir::symbol::Kind type);
    ir::Symbol Copy(ir::Symbol source);
    size_t Count() const;
    void Resize(size_t n);
  };
}
