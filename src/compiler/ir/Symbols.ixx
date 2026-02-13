export module compiler.ir.Symbols;

import <cstdint>;
import <cstddef>;
import <bitset>;
import <vector>;
import compiler.ir.Symbol;
import compiler.ir.symbol.Kind;
import compiler.ir.symbol.Flag;

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
    std::vector<ir::symbol::Kind> kinds; // Symbol kinds
    std::vector<uint64_t> flags; // Modifiers, size, etc
    std::vector<uint64_t> payloads; // A generic typeless field: immediate literal, pointer/index, etc
    std::vector<uint32_t> offsets; // Byte offset from the base for runtime memory layout

    std::vector<ir::Symbol> parents;
    std::vector<ir::Symbol> names; // A string or enum literal symbol or 0 for undefined

    std::vector<uint32_t> tokens; // The index of the first token that defined this symbol
  public:
    Symbols() = default;
    Symbols(Symbols&&) noexcept = default;
    Symbols& operator=(Symbols&&) noexcept = default;
    Symbols(const Symbols&) = delete;
    Symbols& operator=(const Symbols&) = delete;

    ir::symbol::Kind Kind(ir::Symbol i) const { return kinds[i.Row()]; }
    uint64_t Flags(ir::Symbol i) const { return flags[i.Row()]; }
    uint64_t Payload(ir::Symbol i) const { return payloads[i.Row()]; }
    uint32_t PayloadLow(ir::Symbol i) const { return static_cast<uint32_t>(Payload(i) & 0xFFFFFFFFu); }
    uint32_t PayloadHigh(ir::Symbol i) const { return static_cast<uint32_t>(Payload(i) >> 32); }
    uint32_t Offset(ir::Symbol i) const { return offsets[i.Row()]; }
    ir::Symbol Parent(ir::Symbol i) const { return parents[i.Row()]; }
    ir::Symbol Name(ir::Symbol i) const { return names[i.Row()]; }
    uint32_t Token(ir::Symbol i) const { return tokens[i.Row()]; }
    bool Flag(ir::Symbol i, ir::symbol::Flag f) const { return (flags[i.Row()] & (1ull << static_cast<uint64_t>(f))) != 0; }

    void Kind(ir::Symbol i, ir::symbol::Kind v) { kinds[i.Row()] = v; }
    void Flags(ir::Symbol i, uint64_t v) { flags[i.Row()] = v; }
    void Payload(ir::Symbol i, uint64_t v) { payloads[i.Row()] = v; }
    void Payload(ir::Symbol i, uint32_t low, uint32_t high) { Payload(i, (static_cast<uint64_t>(high) << 32) | low); }
    void PayloadLow(ir::Symbol i, uint32_t v) { Payload(i, v, PayloadHigh(i)); }
    void PayloadHigh(ir::Symbol i, uint32_t v) { Payload(i, PayloadLow(i), v); }
    void Offset(ir::Symbol i, uint32_t v) { offsets[i.Row()] = v; }
    void Parent(ir::Symbol i, ir::Symbol v) { parents[i.Row()] = v; }
    void Name(ir::Symbol i, ir::Symbol v) { names[i.Row()] = v; }
    void Token(ir::Symbol i, uint32_t v) { tokens[i.Row()] = v; }
    void Flag(ir::Symbol i, ir::symbol::Flag f) { flags[i.Row()] |= (1ull << static_cast<uint64_t>(f)); }

    bool IsValid(ir::Symbol i) const {
      return i.Row() < Count();
    }

    bool IsScope(ir::Symbol i) const {
      switch (Kind(i)) {
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

    bool IsLoop(ir::Symbol i) const {
      switch (Kind(i)) {
        case ir::symbol::Kind::For:
        case ir::symbol::Kind::While:
        case ir::symbol::Kind::Repeat:
          return true;
        default:
          return false;
      }
    }

    bool IsContext(ir::Symbol i) const {
      switch (Kind(i)) {
        case ir::symbol::Kind::Function:
        case ir::symbol::Kind::Module:
          return true;
        default:
          return false;
      }
    }

    bool IsStructured(ir::Symbol i) const {
      switch (Kind(i)) {
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

    ir::Symbol FirstChild(ir::Symbol i) const {
      if (!IsStructured(i)) return ir::Symbol{};

      ir::Symbol next{i.Row() + 1};
      if (IsValid(next)) return next;

      return ir::Symbol{};
    }

    ir::Symbol LastChild(ir::Symbol i) const {
      if (!IsStructured(i)) return ir::Symbol{};

      uint64_t value = Payload(i);
      uint32_t end = static_cast<uint32_t>(value >> 32);

      return ir::Symbol{end};
    }

    ir::Symbol Scope(ir::Symbol i) const {
      if (IsScope(i)) return i;

      ir::Symbol parent = Parent(i);
      if (parent) return Scope(parent);

      return ir::Symbol{};
    }

    ir::Symbol Loop(ir::Symbol i) const {
      if (IsLoop(i)) return i;

      auto parent = Parent(i);
      if (parent) return Loop(parent);

      return ir::Symbol{};
    }

    ir::Symbol Context(ir::Symbol i) const {
      if (IsContext(i)) return i;

      auto parent = Parent(i);
      if (parent) return Context(parent);

      return ir::Symbol{};
    }

    ir::Symbol Add(ir::symbol::Kind type) {
      ir::Symbol index{static_cast<uint32_t>(payloads.size())}; // Take the size of any column, they should all be the same

      kinds.emplace_back(type);
      flags.emplace_back(0);
      payloads.push_back(0);
      offsets.push_back(0);
      parents.push_back({});
      names.push_back({});
      tokens.push_back(0);

      return index;
    }

    // Create a copy of an existing symbol
    ir::Symbol Copy(ir::Symbol source) {
      ir::Symbol index{static_cast<uint32_t>(payloads.size())}; // Take the size of any column, they should all be the same

      kinds.emplace_back(kinds[source.Row()]);
      flags.emplace_back(flags[source.Row()]);
      payloads.push_back(payloads[source.Row()]);
      offsets.push_back(offsets[source.Row()]);
      
      // These columns are not copied
      parents.push_back({});
      names.push_back({});
      tokens.push_back(0);

      return index;
    }

    size_t Count() const { return payloads.size(); }

    void Resize(size_t n) {
      kinds.resize(n);
      flags.resize(n);
      payloads.resize(n);
      offsets.resize(n);
      parents.resize(n);
      names.resize(n);
      tokens.resize(n);
    }
  };
};
