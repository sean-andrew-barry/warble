export module code.encoder;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

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
    void Prefix(uint8_t opcode);
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
    void OP(uint8_t op, const ir::Symbol& reg); // Embeds the register in the opcode

    void Displacement(int32_t displacement, uint8_t mod, bool is_bp);

    bool IsStandard(const ir::Symbol& reg) const;
    bool IsExtended(const ir::Symbol& reg) const;

    void Lock(const ir::Symbol& symbol) { if (symbol.IsAtomic()) Lock(); }

    void IMM8(const ir::Symbol& symbol) { Codes().Emit8(symbol.Value()); }
    void IMM16(const ir::Symbol& symbol) { Codes().Emit16(symbol.Value()); }
    void IMM32(const ir::Symbol& symbol) { Codes().Emit32(symbol.Value()); }
    void IMM64(const ir::Symbol& symbol) { Codes().Emit64(symbol.Value()); }

    inline uint8_t Ext(const ir::Symbol& reg) {
      // return (reg.Register() >> 3) & 1u;
      return !!(reg.Register() & 0x08);
    }

    void REX(const ir::Symbol& r);
    void REX(const ir::Symbol& r, const ir::Symbol& rm);
    void REX(const ir::Symbol& r, const ir::Symbol& base, const ir::Symbol& index);
    void REXW(const ir::Symbol& r);
    void REXW(const ir::Symbol& r, const ir::Symbol& rm);
    void REXW(const ir::Symbol& r, const ir::Symbol& base, const ir::Symbol& index);

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
    bool R16(uint8_t op, const code::Address& reg);
    bool R32(uint8_t op, const code::Address& reg);
    bool R64(uint8_t op, const code::Address& reg);
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

    bool ADD(const ir::Symbol& d, const ir::Symbol& s);
    bool ADC(const ir::Symbol& d, const ir::Symbol& s);
    bool XADD(const ir::Symbol& d, const ir::Symbol& s);
    bool SUB(const ir::Symbol& d, const ir::Symbol& s);
    bool SBB(const ir::Symbol& d, const ir::Symbol& s);
    bool AND(const ir::Symbol& d, const ir::Symbol& s);
    bool OR(const ir::Symbol& d, const ir::Symbol& s);
    bool XOR(const ir::Symbol& d, const ir::Symbol& s);
    bool CMP(const ir::Symbol& d, const ir::Symbol& s);
    bool TEST(const ir::Symbol& d, const ir::Symbol& s);
    bool MOV(const ir::Symbol& d, const ir::Symbol& s);
    bool MOVSX(const ir::Symbol& d, const ir::Symbol& s);
    bool MOVSXD(const ir::Symbol& d, const ir::Symbol& s);
    bool MOVZX(const ir::Symbol& d, const ir::Symbol& s);
    
    bool LEA(const ir::Symbol& d, const ir::Symbol& s);
    bool XCHG(const ir::Symbol& d, const ir::Symbol& s);
    bool CMPXCHG(const ir::Symbol& d, const ir::Symbol& s);
    bool PUSH(const ir::Symbol& v);
    bool POP(const ir::Symbol& d);

    bool MUL(const ir::Symbol& d, const ir::Symbol& s);
    bool IMUL(const ir::Symbol& d, const ir::Symbol& s);
    bool IMUL(const ir::Symbol& d, const ir::Symbol& s, const ir::Symbol& i);
    bool CWDE();
    bool CDQE();
    bool CDQ();
    bool CQO();
    bool DIV(const ir::Symbol& d, const ir::Symbol& s);
    bool IDIV(const ir::Symbol& d, const ir::Symbol& s);

    bool INC(const ir::Symbol& d);
    bool DEC(const ir::Symbol& d);
    bool NEG(const ir::Symbol& d);
    bool NOT(const ir::Symbol& d);

    bool Shift(uint8_t ext, const ir::Symbol& d);
    bool ROL(const ir::Symbol& d);
    bool ROR(const ir::Symbol& d);
    bool RCL(const ir::Symbol& d);
    bool RCR(const ir::Symbol& d);
    bool SHL(const ir::Symbol& d);
    bool SHR(const ir::Symbol& d);
    bool SAL(const ir::Symbol& d);
    bool SAR(const ir::Symbol& d);

    bool Shift(uint8_t ext, const ir::Symbol& d, const ir::Symbol& s);
    bool ROL(const ir::Symbol& d, const ir::Symbol& s);
    bool ROR(const ir::Symbol& d, const ir::Symbol& s);
    bool RCL(const ir::Symbol& d, const ir::Symbol& s);
    bool RCR(const ir::Symbol& d, const ir::Symbol& s);
    bool SHL(const ir::Symbol& d, const ir::Symbol& s);
    bool SHR(const ir::Symbol& d, const ir::Symbol& s);
    bool SAL(const ir::Symbol& d, const ir::Symbol& s);
    bool SAR(const ir::Symbol& d, const ir::Symbol& s);

    bool CALL(const ir::Symbol& d);
    bool RET();
    bool RET(const ir::Symbol& bytes);
    bool NOP();
    bool JMP(const ir::Symbol& d);

    bool Jcc(uint8_t op8, uint8_t op, const ir::Symbol& d);
    bool JO(const ir::Symbol& d);
    bool JNO(const ir::Symbol& d);
    bool JB(const ir::Symbol& d);
    bool JNAE(const ir::Symbol& d);
    bool JC(const ir::Symbol& d);
    bool JNB(const ir::Symbol& d);
    bool JAE(const ir::Symbol& d);
    bool JNC(const ir::Symbol& d);
    bool JZ(const ir::Symbol& d);
    bool JE(const ir::Symbol& d);
    bool JNZ(const ir::Symbol& d);
    bool JNE(const ir::Symbol& d);
    bool JBE(const ir::Symbol& d);
    bool JNA(const ir::Symbol& d);
    bool JNBE(const ir::Symbol& d);
    bool JA(const ir::Symbol& d);
    bool JS(const ir::Symbol& d);
    bool JNS(const ir::Symbol& d);
    bool JP(const ir::Symbol& d);
    bool JPE(const ir::Symbol& d);
    bool JNP(const ir::Symbol& d);
    bool JPO(const ir::Symbol& d);
    bool JL(const ir::Symbol& d);
    bool JNGE(const ir::Symbol& d);
    bool JNL(const ir::Symbol& d);
    bool JGE(const ir::Symbol& d);
    bool JLE(const ir::Symbol& d);
    bool JNG(const ir::Symbol& d);
    bool JNLE(const ir::Symbol& d);
    bool JG(const ir::Symbol& d);

    bool SETcc(uint8_t op, const ir::Symbol& d);
    bool SETO(const ir::Symbol& d);
    bool SETNO(const ir::Symbol& d);
    bool SETB(const ir::Symbol& d);
    bool SETNAE(const ir::Symbol& d);
    bool SETC(const ir::Symbol& d);
    bool SETNB(const ir::Symbol& d);
    bool SETAE(const ir::Symbol& d);
    bool SETNC(const ir::Symbol& d);
    bool SETZ(const ir::Symbol& d);
    bool SETE(const ir::Symbol& d);
    bool SETNZ(const ir::Symbol& d);
    bool SETNE(const ir::Symbol& d);
    bool SETBE(const ir::Symbol& d);
    bool SETNA(const ir::Symbol& d);
    bool SETNBE(const ir::Symbol& d);
    bool SETA(const ir::Symbol& d);
    bool SETS(const ir::Symbol& d);
    bool SETNS(const ir::Symbol& d);
    bool SETP(const ir::Symbol& d);
    bool SETPE(const ir::Symbol& d);
    bool SETNP(const ir::Symbol& d);
    bool SETPO(const ir::Symbol& d);
    bool SETL(const ir::Symbol& d);
    bool SETNGE(const ir::Symbol& d);
    bool SETNL(const ir::Symbol& d);
    bool SETGE(const ir::Symbol& d);
    bool SETLE(const ir::Symbol& d);
    bool SETNG(const ir::Symbol& d);
    bool SETNLE(const ir::Symbol& d);
    bool SETG(const ir::Symbol& d);

    bool CMOVcc(uint8_t op, const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVO(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNO(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVB(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNAE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVC(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNB(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVAE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNC(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVZ(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNZ(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVBE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNA(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNBE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVA(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVS(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNS(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVP(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVPE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNP(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVPO(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVL(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNGE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNL(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVGE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVLE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNG(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVNLE(const ir::Symbol& d, const ir::Symbol& s);
    bool CMOVG(const ir::Symbol& d, const ir::Symbol& s);

    bool Add(const ir::Symbol& res, const ir::Symbol& lhs, const ir::Symbol& rhs) {
      // Integer addition (signed and unsigned)
      if (res == lhs) {
        return ADD(res, rhs); // Directly add rhs to res
      } else if (res == rhs) {
        return ADD(res, lhs); // Directly add lhs to res, addition doesn't care about order
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