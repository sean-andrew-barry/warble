export module encoder.x64.encoder;

import encoder.encoder;
import encoder._register;
import encoder.address;
import encoder.flag;
import <string>;
import <cstdint>;
import <utility>;

namespace encoder::x64 {
  export class Encoder : public encoder::Template<Encoder> {
  private:
    static constexpr uint8_t REX_W = 0x08;
    static constexpr uint8_t REX_R = 0x04;
    static constexpr uint8_t REX_X = 0x02;
    static constexpr uint8_t REX_B = 0x01;
    static constexpr uint8_t REGISTER_COUNT = 16;

    struct RegisterWrapper {
      Register reg;
      int8_t value;
      bool spilled;
    };

    struct AddressWrapper {
      Address address;
      int32_t displacement;
      bool resolved;
    };

    std::vector<RegisterWrapper> registers;
    std::vector<AddressWrapper> address;
  public:
  protected:
    void Lock(const encoder::Register& reg) { if (reg.IsLocked()) Emit8(0x00); }
    void OpSize(const encoder::Register& reg) { if (reg.IsHalfWord()) Emit8(0x66); }

    void REX(const encoder::Register& reg, const encoder::Register& index, const encoder::Register& base) {
      uint8_t rex = 0x40;

      // Handle reg register
      if (reg.IsValid()) {
        if (reg.IsExtended()) rex |= REX_R;
        if (reg.Is64Bit()) rex |= REX_W;
      }

      // Handle index register
      if (index.IsValid()) {
        if (index.IsExtended()) rex |= REX_X;
      }

      // Handle base register
      if (base.IsValid()) {
        if (base.IsExtended()) rex |= REX_B;
      }

      // Emit REX prefix if needed
      if (rex != 0x40) Emit8(rex);
    }

    void REX(const encoder::Register& reg) {
      uint8_t rex = 0x40;

      if (reg.IsValid()) {
        if (reg.IsExtended()) rex |= REX_R;
        if (reg.Is64Bit()) rex |= REX_W;  // 64-bit operand size
      }

      // Emit REX prefix if needed
      if (rex != 0x40) Emit8(rex);
    }

    void REX(const encoder::Register& reg1, const encoder::Register&reg2) {
      uint8_t rex = 0x40;

      // Handle reg1 register
      if (reg1.IsValid()) {
        if (reg1.IsExtended()) rex |= REX_R;
        if (reg1.Is64Bit()) rex |= REX_W;  // 64-bit operand size
      }

      // Handle reg2 register
      if (reg2.IsValid()) {
        if (reg2.IsExtended()) rex |= REX_B;
      }

      // Emit REX prefix if needed
      if (rex != 0x40) Emit8(rex);
    }

    void REX(const encoder::Register& a, const encoder::Address& b) { REX(a, b.Index(), b.Base()); }
    void REX(const encoder::Address& a, const encoder::Register& b) { REX(b, a.Index(), a.Base()); }

    // virtual encoder::Register& Bind(encoder::Register& r) final {
    //   // Each frame needs to track what registers it changes
    //   // That way frames that jump to that frame can know what to expect
    //   registers.emplace_back(r, 0, false);
    //   return r;
    // }

    // virtual encoder::Address& Bind(encoder::Address& a) final {
    //   return a;
    // }

    bool Unresolved(encoder::Address& address) const { return addresses[address.ID()].resolved; }

    void Assign(node::Block& block, node::Declaration& declaration) {
      auto& registers = block.Registers();
      auto& reg = declaration.Register();
      
      // Find a new register for this
      for (size_t i = 0; i < TotalRegisters(); ++i) {
        if (!registers.Test(i)) {
          registers.Set(i);
          reg.Set(i);
          return;
        }
      }

      // If no available register is found, mark the declaration as spilled
      reg.Set(0); // Give it RAX, IDK if this matters, it's going to be saved/restored anyway
      reg.Add(Flags::SPILLED);
    }
  public:
    virtual size_t TotalGPR() const final { return 16; }
    virtual size_t TotalSIMD() const final { return 16; }
    virtual size_t TotalRegisters() const final { return TotalGPR() + TotalSIMD(); }

    virtual void Declare(node::Block& block, node::Declaration& declaration) final {
      auto& context = block.Context();

      // Here we can use the register's flags to influence register selection,
      // preferring registers that enable more optimized instructions
      if (declaration.Register().Any(Flags::ADD | Flags::SUBTRACT | Flags::AND | Flags::OR | Flags::XOR | Flags::EQUAL)) {
        // Possibly prefer RAX, for example
      }

      Assign(block, declaration);
    }

    virtual void Collide(node::Block& block, node::Block& previous) final {
      auto& context = block.Context();
      auto& registers = block.Registers();
      auto& prev_registers = previous.Registers();

      // Early exit if no collisions
      if (!registers.Overlaps(prev_registers)) return;

      // Examine each collision and resolve it if necessary
      for (node::Declaration* declaration : context.LiveStack()) {
        auto& reg = declaration->Register();
        auto index = reg.Value();

        // If there's a conflict with a register not in the previous block's declarations
        if (prev_registers.Test(index) && !previous.Has(node)) {
          Assign(block, declaration);
        }
      }
    }

    virtual void Add(encoder::Register a, encoder::Address b) final {
      if (b.Unresolved()) Placeholder([=](){ Add(a, b); });

      if (a.Is8Bit()) a.IsLocked() ? ADD_M8_R8_LOCK(b, a) : ADD_M8_R8(b, a);
      else if (a.Is16Bit()) a.IsLocked() ? ADD_M16_R16_LOCK(b, a) : ADD_M16_R16(b, a);
      else if (a.Is32Bit()) a.IsLocked() ? ADD_M32_R32_LOCK(b, a) : ADD_M32_R32(b, a);
      else if (a.Is64Bit()) a.IsLocked() ? ADD_M64_R64_LOCK(b, a) : ADD_M64_R64(b, a);
    }

    // virtual void Add(const encoder::Register& a, const encoder::Address& b) final {
    //   if (!b.Resolved()) Placeholder([=](){ Add(a, b); });

    //   // Lock(a);
    //   // OpSize(a);
    //   // REX(a, b);
    //   // OP(a.IsAnyWord() ? 0x01 : 0x00);
    //   // ModRM(a, b);

    //   if (a.Is8Bit()) a.IsLocked() ? ADD_M8_R8_LOCK(b, a) : ADD_M8_R8(b, a);
    //   else if (a.Is16Bit()) a.IsLocked() ? ADD_M16_R16_LOCK(b, a) : ADD_M16_R16(b, a);
    //   else if (a.Is32Bit()) a.IsLocked() ? ADD_M32_R32_LOCK(b, a) : ADD_M32_R32(b, a);
    //   else if (a.Is64Bit()) a.IsLocked() ? ADD_M64_R64_LOCK(b, a) : ADD_M64_R64(b, a);
    // }

    // Opcode 0x00: ADD r/m8, r8
    void ADD_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x00); ModRM(r1, r2); }
    void ADD_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x00); ModRM(m, r); }
    void ADD_R8_R8_LOCK(IsGPR8 auto r1, IsGPR8 auto r2) { Lock(); REX(r1, r2); OP(0x00); ModRM(r1, r2); }
    void ADD_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x00); ModRM(m, r); }

    // Opcode 0x01: ADD r/m16/32/64, r16/32/64
    void ADD_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    void ADD_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x01); ModRM(m, r); }
    void ADD_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    void ADD_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x01); ModRM(m, r); }
    void ADD_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    void ADD_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x01); ModRM(m, r); }
  };
};