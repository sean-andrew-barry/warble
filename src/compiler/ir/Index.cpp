import compiler.ir.Index;
import compiler.ir.Symbols;
import compiler.ir.symbol.Flag;
import compiler.ir.symbol.Type;
import compiler.program.Module;

namespace compiler::ir {
  const std::bitset<64>& Index::Register(program::Module& mod) const { return mod.Symbols().Registers(*this); }
  const std::bitset<64>& Index::Flag(program::Module& mod) const { return mod.Symbols().Flags(*this); }

  ir::symbol::Type Index::Type(program::Module& mod) const {
    // First byte of flags stores the Type enum value; mask low 8 bits
    auto v = mod.Symbols().Flags(*this).to_ullong();
    return static_cast<ir::symbol::Type>(static_cast<uint8_t>(v & 0xFFull));
  }

  uint64_t Index::Value(program::Module& mod) const { return mod.Symbols().Value(*this); }
  int32_t Index::Displacement(program::Module& mod) const { return mod.Symbols().Displacement(*this); }
  uint32_t Index::Size(program::Module& mod) const { return static_cast<uint32_t>(mod.Symbols().Value(*this) & 0xFFFFFFFFu); }
  ir::Index Index::Parent(program::Module& mod) const { return mod.Symbols().Parent(*this); }
  ir::Index Index::Name(program::Module& mod) const { return mod.Symbols().Name(*this); }

  ir::Index Index::FirstChild(program::Module& mod) const {
    if (!IsStructured(mod)) return ir::Index{};

    ir::Index next{index + 1};

    if (!next.IsValid(mod) || next.Parent(mod) != *this) return ir::Index{};

    return next;
  }

  ir::Index Index::LastChild(program::Module& mod) const {
    if (!IsStructured(mod)) return ir::Index{};
    uint64_t value = Value(mod);
    uint32_t end = static_cast<uint32_t>(value >> 32);

    return ir::Index{end};
  }

  ir::Index Index::Scope(program::Module& mod) const {
    if (IsScope(mod)) return *this;

    auto parent = Parent(mod);
    if (parent.IsValid(mod)) return parent.Scope(mod);

    return ir::Index{};
  }

  ir::Index Index::Loop(program::Module& mod) const {
    if (IsLoop(mod)) return *this;

    auto parent = Parent(mod);
    if (parent.IsValid(mod)) return parent.Loop(mod);

    return ir::Index{};
  }

  ir::Index Index::Context(program::Module& mod) const {
    if (IsContext(mod)) return *this;

    auto parent = Parent(mod);
    if (parent.IsValid(mod)) return parent.Context(mod);

    return ir::Index{};
  }

  void Index::Register(program::Module& mod, std::bitset<64> v) { mod.Symbols().Registers(*this, v); }
  void Index::Flag(program::Module& mod, std::bitset<64> v) { mod.Symbols().Flags(*this, v); }

  void Index::Type(program::Module& mod, ir::symbol::Type v) {
    // Replace the first byte of flags with the Type
    auto current = mod.Symbols().Flags(*this).to_ullong();
    auto updated = (current & ~0xFFull) | static_cast<uint8_t>(v);
    std::bitset<64> new_flags{updated};
    mod.Symbols().Flags(*this, new_flags);
  }

  void Index::Value(program::Module& mod, uint64_t v) { mod.Symbols().Value(*this, v); }
  void Index::Value(program::Module& mod, uint32_t a, uint32_t b) { Value(mod, (static_cast<uint64_t>(a) << 32) | b); }
  void Index::Displacement(program::Module& mod, int32_t v) { mod.Symbols().Displacement(*this, v); }
  void Index::Size(program::Module& mod, uint32_t v) { Value(mod, static_cast<uint32_t>(Value(mod) >> 32), static_cast<uint32_t>(v)); }
  void Index::Name(program::Module& mod, ir::Index v) { mod.Symbols().Name(*this, v); }
  void Index::Parent(program::Module& mod, ir::Index v) { mod.Symbols().Parent(*this, v); }

  bool Index::IsValid(program::Module& mod) const {
    return index != 0 && index < mod.Symbols().Count();
  }

  bool Index::IsScope(program::Module& mod) const {
    switch (Type(mod)) {
      case ir::symbol::Type::Function:
      case ir::symbol::Type::Module:
      case ir::symbol::Type::If:
      case ir::symbol::Type::ElseIf:
      case ir::symbol::Type::Else:
      case ir::symbol::Type::Match:
      case ir::symbol::Type::For:
      case ir::symbol::Type::Repeat:
      case ir::symbol::Type::While:
      case ir::symbol::Type::Do:
        return true;
      default:
        return false;
    }
  }

  bool Index::IsLoop(program::Module& mod) const {
    switch (Type(mod)) {
      case ir::symbol::Type::For:
      case ir::symbol::Type::While:
      case ir::symbol::Type::Repeat:
        return true;
      default:
        return false;
    }
  }

  bool Index::IsContext(program::Module& mod) const {
    switch (Type(mod)) {
      case ir::symbol::Type::Function:
      case ir::symbol::Type::Module:
        return true;
      default:
        return false;
    }
  }

  bool Index::IsStructured(program::Module& mod) const {
    switch (Type(mod)) {
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
      case ir::symbol::Type::Match:
      case ir::symbol::Type::For:
      case ir::symbol::Type::Repeat:
      case ir::symbol::Type::While:
      case ir::symbol::Type::Do:
        return true;
      default:
        return false;
    }
  }
};