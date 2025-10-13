export module compiler.ir.Symbols;

import <cstdint>;
import <bitset>;
import <vector>;
import compiler.ir.Index;
import compiler.ir.symbol.Type;

// ────────────────────────────────────────────────────────────────
//  Symbols table: columnar (SoA) style representation of Symbol data
// ────────────────────────────────────────────────────────────────

// Tree topology: Preorder layout with subtree interval (DFS numbering) — nodes are stored in DFS-preorder; each node stores parent and an inclusive subtree end index to hop over whole subtrees in O(1) per child.
namespace compiler::ir {
  export class Symbols {
  private:
    std::vector<uint64_t> registers; // Live-range bitset
    std::vector<uint64_t> flags; // Modifiers, type information, etc
    std::vector<uint64_t> values; // A generic typeless field: immediate literal, pointer/index, etc
    std::vector<int32_t> displacements; // Byte offset from the parent for runtime memory layout

    std::vector<ir::Index> parents;
    std::vector<ir::Index> names; // A string or enum literal symbol or 0 for undefined

    std::vector<uint32_t> first_tokens; // The index of the first token that defined this symbol
    std::vector<uint32_t> last_tokens; // The index of the last token that defined this symbol
  public:
    Symbols() = default;
    Symbols(Symbols&&) noexcept = default;
    Symbols& operator=(Symbols&&) noexcept = default;
    Symbols(const Symbols&) = delete;
    Symbols& operator=(const Symbols&) = delete;

    uint64_t Registers(ir::Index i) const { return registers[i.Row()]; }
    uint64_t Flags(ir::Index i) const { return flags[i.Row()]; }
    uint64_t Value(ir::Index i) const { return values[i.Row()]; }
    uint32_t Size(ir::Index i) const { return static_cast<uint32_t>(Value(i) & 0xFFFFFFFFu); }
    int32_t Displacement(ir::Index i) const { return displacements[i.Row()]; }
    ir::Index Parent(ir::Index i) const { return parents[i.Row()]; }
    ir::Index Name(ir::Index i) const { return names[i.Row()]; }

    ir::symbol::Type Type(ir::Index i) const {
      // First byte of flags stores the Type enum value; mask low 8 bits
      auto v = Flags(i);
      return static_cast<ir::symbol::Type>(static_cast<uint8_t>(v & 0xFFull));
    }

    void Registers(ir::Index i, uint64_t v) { registers[i.Row()] = v; }
    void Flags(ir::Index i, uint64_t v) { flags[i.Row()] = v; }
    void Value(ir::Index i, uint64_t v) { values[i.Row()] = v; }
    void Value(ir::Index i, uint32_t a, uint32_t b) { Value(i, (static_cast<uint64_t>(a) << 32) | b); }
    void Size(ir::Index i, uint32_t v) { Value(i, static_cast<uint32_t>(Value(i) >> 32), static_cast<uint32_t>(v)); }
    void Displacement(ir::Index i, int32_t v) { displacements[i.Row()] = v; }
    void Parent(ir::Index i, ir::Index v) { parents[i.Row()] = v; }
    void Name(ir::Index i, ir::Index v) { names[i.Row()] = v; }

    void Type(ir::Index i, ir::symbol::Type v) {
      // Replace the first byte of flags with the Type
      auto current = Flags(i);
      auto updated = (current & ~0xFFull) | static_cast<uint8_t>(v);
      Flags(i, updated);
    }

    bool IsValid(ir::Index i) const {
      return i != 0 && i < Count();
    }

    bool IsScope(ir::Index i) const {
      switch (Type(i)) {
        case ir::symbol::Type::Function:
        case ir::symbol::Type::Module:
        case ir::symbol::Type::If:
        case ir::symbol::Type::ElseIf:
        case ir::symbol::Type::Else:
        case ir::symbol::Type::When:
        case ir::symbol::Type::For:
        case ir::symbol::Type::Repeat:
        case ir::symbol::Type::While:
        case ir::symbol::Type::Do:
          return true;
        default:
          return false;
      }
    }

    bool IsLoop(ir::Index i) const {
      switch (Type(i)) {
        case ir::symbol::Type::For:
        case ir::symbol::Type::While:
        case ir::symbol::Type::Repeat:
          return true;
        default:
          return false;
      }
    }

    bool IsContext(ir::Index i) const {
      switch (Type(i)) {
        case ir::symbol::Type::Function:
        case ir::symbol::Type::Module:
          return true;
        default:
          return false;
      }
    }

    bool IsStructured(ir::Index i) const {
      switch (Type(i)) {
        case ir::symbol::Type::Function:
        case ir::symbol::Type::Module:
        case ir::symbol::Type::Object:
        case ir::symbol::Type::Array:
        case ir::symbol::Type::Enum:
        case ir::symbol::Type::Tuple:
        case ir::symbol::Type::TemplateString:
        case ir::symbol::Type::If:
        case ir::symbol::Type::ElseIf:
        case ir::symbol::Type::Else:
        case ir::symbol::Type::When:
        case ir::symbol::Type::For:
        case ir::symbol::Type::Repeat:
        case ir::symbol::Type::While:
        case ir::symbol::Type::Do:
          return true;
        default:
          return false;
      }
    }

    ir::Index FirstChild(ir::Index i) const {
      if (!IsStructured(i)) return ir::Index{};

      ir::Index next{index + 1};

      if (!next.IsValid(i) || next.Parent(i) != *this) return ir::Index{};

      return next;
    }

    ir::Index LastChild(ir::Index i) const {
      if (!IsStructured(i)) return ir::Index{};
      uint64_t value = Value(i);
      uint32_t end = static_cast<uint32_t>(value >> 32);

      return ir::Index{end};
    }

    ir::Index Scope(ir::Index i) const {
      if (IsScope(i)) return *this;

      auto parent = Parent(i);
      if (parent.IsValid(i)) return parent.Scope(i);

      return ir::Index{};
    }

    ir::Index Loop(ir::Index i) const {
      if (IsLoop(i)) return *this;

      auto parent = Parent(i);
      if (parent.IsValid(i)) return parent.Loop(i);

      return ir::Index{};
    }

    ir::Index Context(ir::Index i) const {
      if (IsContext(i)) return *this;

      auto parent = Parent(i);
      if (parent.IsValid(i)) return parent.Context(i);

      return ir::Index{};
    }

    ir::Index Add(ir::symbol::Type type) {
      ir::Index index = static_cast<uint32_t>(values.size()); // Take the size of any column, they should all be the same

      registers.emplace_back(0);
      flags.emplace_back(static_cast<uint64_t>(type));
      values.push_back(0);
      displacements.push_back(0);
      parents.push_back({});
      names.push_back({});
      first_tokens.push_back(0);
      last_tokens.push_back(0);

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
      first_tokens.resize(n);
      last_tokens.resize(n);
    }
  };
};