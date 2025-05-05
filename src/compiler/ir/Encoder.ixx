export module code.encoder;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;

import ir._module;
import ir.symbols;
import ir.symbol;
import ir.instruction;
import utility.buffer;

namespace ir {
  export class Encoder {
  private:
    ir::Module& mod;
    utility::Buffer code;
  public:
    utility::Buffer& Code() { return code; }

    ir::Symbol Get(const ir::Instruction& instruction, uint8_t operand) const {
      return mod.GetSymbol(instruction, operand);
    }

    virtual void Encode(const std::vector<ir::Instruction>& instructions) = 0;
  };

  export class X64 : public Encoder {
  public:
    void SIMD();
    void OpSize();
    void AdSize();
    void Single();
    void Double();
    void CS();
    void SS();
    void DS();
    void ES();
    void FS();
    void GS();
    void BNT();
    void BT();
    void Lock();
    void REP();
    void REPNE();
    void Escape();
    void Escape(uint8_t opcode);
    void Esc();
    void Esc(uint8_t opcode);

    void REX(uint8_t rex);
    void REX();
    void REXB();
    void REXX();
    void REXXB();
    void REXR();
    void REXRB();
    void REXRX();
    void REXRXB();
    void REXW();
    void REXWB();
    void REXWX();
    void REXWXB();
    void REXWR();
    void REXWRB();
    void REXWRX();
    void REXWRXB();

    void OP(uint8_t opcode);
    void SO(uint8_t opcode);

    void Displacement(int32_t displacement, uint8_t mod, bool is_bp);

    bool IsStandard(const ir::Symbol& reg) const;
    bool IsExtended(const ir::Symbol& reg) const;

    void Lock(const ir::Symbol& symbol) { if (symbol.IsAtomic()) Lock(); }

    void IMM8(const ir::Symbol& symbol) { Codes().Emit8(symbol.Value()); }
    void IMM16(const ir::Symbol& symbol) { Codes().Emit16(symbol.Value()); }
    void IMM32(const ir::Symbol& symbol) { Codes().Emit32(symbol.Value()); }
    void IMM64(const ir::Symbol& symbol) { Codes().Emit64(symbol.Value()); }

    inline bool IsExtended(const ir::Symbol& s) {
      return s.IsAllocated() && s.Register() >= 8;
    }

    // Return 1 when the register number is r8–r15, else 0.
    // Works for   reg ∈ {0 … 15, 255}.  Any other value is UB.
    static inline unsigned RexHi(uint8_t reg) {
      // bit 3 tells us whether the register is in the upper bank.
      // The ~reg>>5 term zeroes the result when reg == 255. 
      return ((~reg >> 5) & (reg >> 3)) & 1;
    }

    inline void REX(bool w, uint8_t r, uint8_t x, uint8_t b) {
      uint8_t rex = 0x40 | (w << 3) | (r << 2) | (x << 1) | b;
      if (rex != 0x40) Code().Emit8(rex);
    }

    inline void REX_W(const ir::Symbol& rm) {
      REX(true, 0, 0, RexHi(rm));
    }

    inline void REX_WRB(const ir::Symbol& reg, const ir::Symbol& rm, bool w) {
      REX(w, RexHi(reg), 0, RexHi(rm));
    }

    inline void REX_WRXB(const ir::Symbol& reg, const ir::Symbol& base, const ir::Symbol& index, bool w) {
      REX(w, RexHi(reg), RexHi(index), RexHi(base));
    }

    template<typename T>
    requires (std::same_as<T, uint8_t> || std::same_as<T, ir::Symbol>)
    void ModRM(const T& reg, const ir::Symbol& rm) {
      uint8_t modrm  = 0;
      uint8_t reg_id = 0;
    
      // 1. Encode the REG field
      if constexpr (std::same_as<T, uint8_t>) {
        reg_id = reg & 0x07; // Extension opcode in REG
      } else {
        // ir::Symbol in REG field
        if (!reg.IsAllocated()) {
          throw std::runtime_error("REG symbol must be allocated to a register.");
        }

        reg_id = reg.Register() & 0x07;
      }
    
      // 2. Is the RM operand in a register?
      if (rm.IsAllocated()) {
        modrm = 0xC0 | (reg_id << 3) | (rm.Register() & 0x07);
        Instructions().Emit8(modrm);
        return;
      }

      // 3. Decide base and displacement
      int32_t  displacement = 0;
      bool     is_local     = rm.IsLocal(displacement); // true -> RSP-rel
      uint8_t  base_id      = is_local ? 0x04 : 0x05;   // RSP or RIP
      uint8_t  mod          = 0x00;                     // default Mod
      bool     need_disp    = false;                    // emit disp?

      if (is_local) {
        // Adjust by any current offset, caused by things like PUSH/POP
        displacement -= static_cast<int32_t>(RSP());

        // Choose 0, 8 or 32-bit disp for [RSP + disp]
        if (displacement == 0) {
          mod = 0x00;
        } else if (displacement >= -128 && displacement <= 127) {
          mod = 0x40; // 8-bit
          need_disp = true;
        } else {
          mod = 0x80; // 32-bit
          need_disp = true;
        }
      } else {
        // RIP-relative always uses 32-bit disp
        mod       = 0x00;
        need_disp = true;
      }

      // 4. Emit ModR/M
      modrm = mod | (reg_id << 3) | base_id;
      Instructions().Emit8(modrm);
      
      if (is_local) {
        // 5a. Emit mandatory SIB for RSP base
        Instructions().Emit8(0x24); // scale = 00, index = 100 (none), base = 100 (RSP)
      } else if (!rm.IsResolved()) {
        // 5b. Unresolved global -> 6-byte placeholder
        Patches().emplace_back(rm, RIP(), 4, 0);
        Instructions().Emit32(displacement); // Write the bytes from the start of the module's data
        return;
      }
    
      // 6. Emit displacement if required
      if (need_disp) {
        if (!is_local) {
          // Adjust displacement for RIP-relative addressing
          displacement -= static_cast<int32_t>(RIP()) + 4;
        }
    
        if (mod == 0x40) {
          Instructions().Emit8(static_cast<int8_t>(displacement));
        } else {
          Instructions().Emit32(displacement);
        }
      }
    }

    bool AL_IMM8(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool AX_IMM16(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool EAX_IMM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool RAX_IMM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool R8_IMM8(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool R32_IMM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool R64_IMM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool R8_IMM8s(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool R16_IMM8s(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool R32_IMM8s(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool R64_IMM8s(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm);
    bool RM8_IMM8(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm);
    bool RM16_IMM8(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm);
    bool RM32_IMM8(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm);
    bool RM64_IMM8(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm);
    bool RM16_IMM16(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm);
    bool RM32_IMM32(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm);
    bool RM64_IMM32(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm);
    bool R8_RM8(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rm);
    bool R16_RM16(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rm);
    bool R32_RM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rm);
    bool R64_RM64(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rm);
    bool RM8(uint8_t op, uint8_t ext, const ir::Symbol& rm);
    bool RM16(uint8_t op, uint8_t ext, const ir::Symbol& rm);
    bool RM32(uint8_t op, uint8_t ext, const ir::Symbol& rm);
    bool RM64(uint8_t op, uint8_t ext, const ir::Symbol& rm);
    bool IMM8(uint8_t op, const ir::Symbol& imm);
    bool IMM16(uint8_t op, const ir::Symbol& imm);
    bool IMM32(uint8_t op, const ir::Symbol& imm);
    void REL8(const ir::Symbol& imm);
    void REL32(const ir::Symbol& imm);
    bool RMx_1();
    bool RMx_CL();
    bool RMx_IMM8();
    bool RDX_RAX_RM8(uint8_t op, uint8_t ext, const ir::Symbol& rdx, const ir::Symbol& rax, const ir::Symbol& rm);
    bool RDX_RAX_RM16(uint8_t op, uint8_t ext, const ir::Symbol& rdx, const ir::Symbol& rax, const ir::Symbol& rm);
    bool RDX_RAX_RM32(uint8_t op, uint8_t ext, const ir::Symbol& rdx, const ir::Symbol& rax, const ir::Symbol& rm);
    bool RDX_RAX_RM64(uint8_t op, uint8_t ext, const ir::Symbol& rdx, const ir::Symbol& rax, const ir::Symbol& rm);
    bool RAX_RM64();
    bool R16_AX(uint8_t op, const ir::Symbol& reg, const ir::Symbol& ax);
    bool R32_EAX(uint8_t op, const ir::Symbol& reg, const ir::Symbol& eax);
    bool R64_RAX(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rax);
    bool MOFFS32_RAX();
    bool RAX_MOFFS32();

    bool ADD(const ir::Symbol& d, const ir::Symbol& s) {
      return AL_IMM8(0x04, d, s)
          || AX_IMM16(0x05, d, s)
          || EAX_IMM32(0x05, d, s)
          || RAX_IMM32(0x05, d, s)
          || R32_IMM8(0x83, d, s)
          || R64_IMM8(0x83, d, s)
          || RM8_IMM8(0x80, 0x00, d, s)
          || RM32_IMM8(0x83, 0x00, d, s)
          || RM64_IMM8(0x83, 0x00, d, s)
          || RM32_IMM32(0x81, 0x00, d, s)
          || RM64_IMM32(0x81, 0x00, d, s)
          || RM16_IMM8(0x83, 0x00, d, s)
          || R8_RM8(0x02, d, s)
          || R32_RM32(0x03, d, s)
          || R64_RM64(0x03, d, s)
          || R16_RM16(0x03, d, s)
          || RM8_R8(0x00, d, s)
          || RM32_R32(0x01, d, s)
          || RM64_R64(0x01, d, s)
          || RM16_R16(0x01, d, s)
          || RM16_IMM16(0x81, 0x00, d, s);
    }

    bool Add(const ir::Symbol& res, const ir::Symbol& lhs, const ir::Symbol& rhs) {
      // Integer addition (signed and unsigned)
      if (res == lhs) {
        return ADD(res, rhs); // Directly add rhs to res
      } else if (res == rhs) {
        return ADD(res, lhs); // Directly add lhs to res
      } else {
        return MOV(res, lhs)
            && ADD(res, rhs);
      }
    }

    bool AddFloat(const ir::Symbol& res, const ir::Symbol& lhs, const ir::Symbol& rhs) {
      // // Floating-point addition
      // if (res == lhs) {
      //   return ADDPS(res, rhs); // Directly add rhs to res (packed single-precision)
      // } else {
      //   return MOVAPS(res, lhs)
      //       && ADDPS(res, rhs);
      // }

      return false;
    }

    bool Process(const ir::Instruction& i) {
      auto res = Get(i, 0);
      auto lhs = Get(i, 1);
      auto rhs = Get(i, 2);

      switch (i.opcode) {
        case ir::OpCode::Jump: return Jump(res);
        case ir::OpCode::Add: return Add(res, lhs, rhs);
        default: return false;
      }
    }

    virtual void Encode(const std::vector<ir::Instruction>& instructions) final {
      for (auto i : instructions) {
        bool result = Process(i);
        // if it fails, do something like error or log it or whatever
      }
    }
  };
};