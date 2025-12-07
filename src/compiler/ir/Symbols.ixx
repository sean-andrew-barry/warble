export module compiler.ir.Symbols;

import <cstdint>;
import <cstddef>;
import <bitset>;
import <vector>;
import compiler.ir.Index;
import compiler.ir.symbol.Kind;

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
  export class Symbols {
  private:
    std::vector<uint64_t> registers; // Live-range bitset
    std::vector<ir::symbol::Kind> types; // Symbol types
    std::vector<uint64_t> flags; // Modifiers, type information, etc
    std::vector<uint64_t> payloads; // A generic typeless field: immediate literal, pointer/index, etc
    std::vector<uint32_t> offsets; // Byte offset from the parent for runtime memory layout
    std::vector<uint32_t> displacements; // Byte offset from the stack pointer for runtime memory layout

    std::vector<ir::Index> parents;
    std::vector<ir::Index> names; // A string or enum literal symbol or 0 for undefined

    std::vector<uint32_t> tokens; // The index of the first token that defined this symbol
  public:
    Symbols() = default;
    Symbols(Symbols&&) noexcept = default;
    Symbols& operator=(Symbols&&) noexcept = default;
    Symbols(const Symbols&) = delete;
    Symbols& operator=(const Symbols&) = delete;

    uint64_t Registers(ir::Index i) const { return registers[i.Row()]; }
    ir::symbol::Kind Type(ir::Index i) const { return types[i.Row()]; }
    uint64_t Flags(ir::Index i) const { return flags[i.Row()]; }
    uint64_t Payload(ir::Index i) const { return payloads[i.Row()]; }
    uint32_t Size(ir::Index i) const { return static_cast<uint32_t>(Payload(i) & 0xFFFFFFFFu); }
    uint32_t Offset(ir::Index i) const { return offsets[i.Row()]; }
    uint32_t Displacement(ir::Index i) const { return displacements[i.Row()]; }
    ir::Index Parent(ir::Index i) const { return parents[i.Row()]; }
    ir::Index Name(ir::Index i) const { return names[i.Row()]; }
    uint32_t Token(ir::Index i) const { return tokens[i.Row()]; }

    void Registers(ir::Index i, uint64_t v) { registers[i.Row()] = v; }
    void Type(ir::Index i, ir::symbol::Kind v) { types[i.Row()] = v; }
    void Flags(ir::Index i, uint64_t v) { flags[i.Row()] = v; }
    void Payload(ir::Index i, uint64_t v) { payloads[i.Row()] = v; }
    void Payload(ir::Index i, uint32_t a, uint32_t b) { Payload(i, (static_cast<uint64_t>(a) << 32) | b); }
    void Size(ir::Index i, uint32_t v) { Payload(i, static_cast<uint32_t>(Payload(i) >> 32), static_cast<uint32_t>(v)); }
    void Offset(ir::Index i, uint32_t v) { offsets[i.Row()] = v; }
    void Displacement(ir::Index i, uint32_t v) { displacements[i.Row()] = v; }
    void Parent(ir::Index i, ir::Index v) { parents[i.Row()] = v; }
    void Name(ir::Index i, ir::Index v) { names[i.Row()] = v; }
    void Token(ir::Index i, uint32_t v) { tokens[i.Row()] = v; }

    bool IsValid(ir::Index i) const {
      return i.Row() < Count();
    }

    bool IsScope(ir::Index i) const {
      switch (Type(i)) {
        case ir::symbol::Kind::Function:
        case ir::symbol::Kind::Module:
        case ir::symbol::Kind::If:
        case ir::symbol::Kind::ElseIf:
        case ir::symbol::Kind::Else:
        case ir::symbol::Kind::When:
        case ir::symbol::Kind::For:
        case ir::symbol::Kind::Repeat:
        case ir::symbol::Kind::While:
        case ir::symbol::Kind::Do:
          return true;
        default:
          return false;
      }
    }

    bool IsLoop(ir::Index i) const {
      switch (Type(i)) {
        case ir::symbol::Kind::For:
        case ir::symbol::Kind::While:
        case ir::symbol::Kind::Repeat:
          return true;
        default:
          return false;
      }
    }

    bool IsContext(ir::Index i) const {
      switch (Type(i)) {
        case ir::symbol::Kind::Function:
        case ir::symbol::Kind::Module:
          return true;
        default:
          return false;
      }
    }

    bool IsStructured(ir::Index i) const {
      switch (Type(i)) {
        case ir::symbol::Kind::Function:
        case ir::symbol::Kind::Module:
        case ir::symbol::Kind::Object:
        case ir::symbol::Kind::Array:
        case ir::symbol::Kind::Enum:
        case ir::symbol::Kind::Tuple:
        case ir::symbol::Kind::TemplateString:
        case ir::symbol::Kind::Error:
        case ir::symbol::Kind::If:
        case ir::symbol::Kind::ElseIf:
        case ir::symbol::Kind::Else:
        case ir::symbol::Kind::When:
        case ir::symbol::Kind::For:
        case ir::symbol::Kind::Repeat:
        case ir::symbol::Kind::While:
        case ir::symbol::Kind::Do:
          return true;
        default:
          return false;
      }
    }

    ir::Index FirstChild(ir::Index i) const {
      if (!IsStructured(i)) return ir::Index{};

      ir::Index next{i.Row() + 1};
      if (IsValid(next)) return next;

      return ir::Index{};
    }

    ir::Index LastChild(ir::Index i) const {
      if (!IsStructured(i)) return ir::Index{};

      uint64_t value = Payload(i);
      uint32_t end = static_cast<uint32_t>(value >> 32);

      return ir::Index{end};
    }

    ir::Index Scope(ir::Index i) const {
      if (IsScope(i)) return i;

      ir::Index parent = Parent(i);
      if (parent) return Scope(parent);

      return ir::Index{};
    }

    ir::Index Loop(ir::Index i) const {
      if (IsLoop(i)) return i;

      auto parent = Parent(i);
      if (parent) return Loop(parent);

      return ir::Index{};
    }

    ir::Index Context(ir::Index i) const {
      if (IsContext(i)) return i;

      auto parent = Parent(i);
      if (parent) return Context(parent);

      return ir::Index{};
    }

    ir::Index Add(ir::symbol::Kind type) {
      ir::Index index{static_cast<uint32_t>(payloads.size())}; // Take the size of any column, they should all be the same

      registers.emplace_back(0);
      types.emplace_back(type);
      flags.emplace_back(0);
      payloads.push_back(0);
      offsets.push_back(0);
      displacements.push_back(0);
      parents.push_back({});
      names.push_back({});
      tokens.push_back(0);

      return index;
    }

    size_t Count() const { return payloads.size(); }

    void Resize(size_t n) {
      registers.resize(n);
      types.resize(n);
      flags.resize(n);
      payloads.resize(n);
      offsets.resize(n);
      displacements.resize(n);
      parents.resize(n);
      names.resize(n);
      tokens.resize(n);
    }
  };
};
