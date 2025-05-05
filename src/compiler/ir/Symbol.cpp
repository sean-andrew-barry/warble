import ir.symbol;
import ir._module;

namespace ir {
  uint8_t Symbol::Register() { return r; }
  const std::bitset<64>& Symbol::Registers() { return mod.GetRegister(s); }
  const std::bitset<64>& Symbol::Flags() { return mod.GetFlag(s); }
  uint64_t Symbol::Value() { return mod.GetValue(s); }
  int32_t Symbol::Displacement() { return mod.GetDisplacement(s); }
  uint32_t Symbol::Size() { return mod.GetSize(s); }
  ir::Symbol Symbol::Parent() { return mod.GetParent(s);}
  ir::Symbol Symbol::Name() { return mod.GetName(s); }
  ir::Symbol Symbol::Children() { return mod.GetChildren(s);  }
  ir::Symbol Symbol::Prev() { return mod.GetPrev(s); }
  ir::Index Symbol::ParentIndex() { return mod.GetParentIndex(s);}
  ir::Index Symbol::NameIndex() { return mod.GetNameIndex(s); }
  ir::Index Symbol::ChildrenIndex() { return mod.GetChildrenIndex(s);  }
  ir::Index Symbol::PrevIndex() { return mod.GetPrevIndex(s); }

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
  bool Symbol::IsEmpty()     { return s == ir::Symbol::NO_SLOT;     }
  bool Symbol::IsValid()     { return s != ir::Symbol::NO_SLOT;     }
  bool Symbol::IsAllocated() { return r != ir::Symbol::NO_REGISTER; }
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