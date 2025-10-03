import compiler.ir.Symbol;
import compiler.ir.Symbols;
import compiler.program.Module;

namespace compiler::ir {
  const std::bitset<64>& Symbol::Register() { return mod.Symbols().Register(index); }
  const std::bitset<64>& Symbol::Flag() { return mod.Symbols().Flag(index); }
  uint64_t Symbol::Value() { return mod.Symbols().Value(index); }
  int32_t Symbol::Displacement() { return mod.Symbols().Displacement(index); }
  uint32_t Symbol::Size() { return mod.Symbols().Size(index); }
  ir::Symbol Symbol::Parent() { return mod.Symbols().Parent(index);}
  ir::Symbol Symbol::Name() { return mod.Symbols().Name(index); }
  ir::Symbol Symbol::Children() { return mod.Symbols().Children(index);  }
  ir::Symbol Symbol::Prev() { return mod.Symbols().Prev(index); }
  ir::Index Symbol::ParentIndex() { return mod.Symbols().ParentIndex(index);}
  ir::Index Symbol::NameIndex() { return mod.Symbols().NameIndex(index); }
  ir::Index Symbol::ChildrenIndex() { return mod.Symbols().ChildrenIndex(index);  }
  ir::Index Symbol::PrevIndex() { return mod.Symbols().PrevIndex(index); }

  void Symbol::Register(std::bitset<64> v) { mod.Symbols().Register(index, v); }
  void Symbol::Flag(std::bitset<64> v) { mod.Symbols().Flag(index, v); }
  void Symbol::Value(uint64_t v) { mod.Symbols().Value(index, v); }
  void Symbol::Value(uint32_t a, uint32_t b) { Value((static_cast<uint64_t>(a) << 32) | b); }
  void Symbol::Displacement(int32_t v) { mod.Symbols().Displacement(index, v); }
  void Symbol::Size(uint32_t v) { mod.Symbols().Size(index, v); }
  void Symbol::Parent(ir::Index v) { mod.Symbols().Parent(index, v); }
  void Symbol::Name(ir::Index v) { mod.Symbols().Name(index, v); }
  void Symbol::Children(ir::Index v) { mod.Symbols().Children(index, v); }
  void Symbol::Prev(ir::Index v) { mod.Symbols().Prev(index, v); }

  bool Symbol::IsLocal(int32_t& displacement) {
    if (IsFunction()) {
      displacement = Size() - displacement; // Make it RSP-relative
      return true; // RSP relative
    } else if (IsModule()) {
      return false; // RIP relative
    }

    displacement += Displacement();

    return Parent().IsLocal(displacement);
  }

  bool Symbol::IsLocal() {
    if (IsFunction()) return true; // RSP relative
    else if (IsModule()) return false; // RIP relative

    return Parent().IsLocal();
  }

  bool Symbol::IsResolved()  { return mod.IsResolved();             }
  bool Symbol::IsEmpty()     { return index == ir::Symbol::NO_INDEX;     }
  bool Symbol::IsValid()     { return index != ir::Symbol::NO_INDEX;     }
  bool Symbol::Is8Bit()      { return Size() == 1;   }
  bool Symbol::Is16Bit()     { return Size() == 2;   }
  bool Symbol::Is32Bit()     { return Size() == 4;   }
  bool Symbol::Is64Bit()     { return Size() == 8;   }
  bool Symbol::Is128Bit()    { return Size() == 16;  }
  bool Symbol::Is256Bit()    { return Size() == 32;  }
  bool Symbol::Is512Bit()    { return Size() == 64;  }

  bool Symbol::IsConst()    { return mod.GetFlag(s).test(kConstBit); }
  bool Symbol::IsNumber()   { return mod.GetFlag(s).test(kNumBit);   }
  bool Symbol::IsInteger()  { return mod.GetFlag(s).test(kIntBit);   }
  bool Symbol::IsDecimal()  { return mod.GetFlag(s).test(kDecBit);   }
  bool Symbol::IsString()   { return mod.GetFlag(s).test(kStrBit);   }
  bool Symbol::IsBoolean()  { return mod.GetFlag(s).test(kBoolBit);  }
  bool Symbol::IsBlock()    { return mod.GetFlag(s).test(kBlockBit); }
  bool Symbol::IsModule()   { return mod.GetFlag(s).test(kBlockBit); }
  bool Symbol::IsFunction() { return mod.GetFlag(s).test(kFuncBit);  }
};