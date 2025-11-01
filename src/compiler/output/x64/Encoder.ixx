export module compiler.output.x64.Encoder;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.Encoder;
import compiler.ir.Symbols;
import compiler.ir.Index;
import compiler.ir.Instruction;
import compiler.utility.Buffer;

namespace compiler::output::x64 {
  export class Encoder : public compiler::Encoder {
  public:
    virtual ~Encoder() final = default;

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
    void OP(uint8_t op, const ir::Index reg); // Embeds the register in the opcode

    void Displacement(int32_t displacement, uint8_t mod, bool is_bp);

    bool IsStandard(const ir::Index reg) const;
    bool IsExtended(const ir::Index reg) const;

    void Lock(const ir::Index symbol) { if (symbol.IsAtomic()) Lock(); }

    void IMM8(const ir::Index symbol) { Codes().Emit8(symbol.Value()); }
    void IMM16(const ir::Index symbol) { Codes().Emit16(symbol.Value()); }
    void IMM32(const ir::Index symbol) { Codes().Emit32(symbol.Value()); }
    void IMM64(const ir::Index symbol) { Codes().Emit64(symbol.Value()); }

    inline uint8_t Ext(const ir::Index reg) {
      // return (reg.Register() >> 3) & 1u;
      return !!(reg.Register() & 0x08);
    }

    void REX(const ir::Index r);
    void REX(const ir::Index r, const ir::Index rm);
    void REX(const ir::Index r, const ir::Index base, const ir::Index index);
    void REXW(const ir::Index r);
    void REXW(const ir::Index r, const ir::Index rm);
    void REXW(const ir::Index r, const ir::Index base, const ir::Index index);

    template<typename T>
    requires (std::same_as<T, uint8_t> || std::same_as<T, ir::Symbol>)
    void ModRM(const T& reg, const ir::Index rm) {
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

    bool AL_IMM8(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool AX_IMM16(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool EAX_IMM32(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool RAX_IMM32(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool R8_IMM8(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool R32_IMM32(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool R64_IMM32(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool R8_IMM8s(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool R16_IMM8s(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool R32_IMM8s(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool R64_IMM8s(uint8_t op, const ir::Index reg, const ir::Index imm);
    bool RM8_IMM8(uint8_t op, uint8_t ext, const ir::Index rm, const ir::Index imm);
    bool RM16_IMM8(uint8_t op, uint8_t ext, const ir::Index rm, const ir::Index imm);
    bool RM32_IMM8(uint8_t op, uint8_t ext, const ir::Index rm, const ir::Index imm);
    bool RM64_IMM8(uint8_t op, uint8_t ext, const ir::Index rm, const ir::Index imm);
    bool RM16_IMM16(uint8_t op, uint8_t ext, const ir::Index rm, const ir::Index imm);
    bool RM32_IMM32(uint8_t op, uint8_t ext, const ir::Index rm, const ir::Index imm);
    bool RM64_IMM32(uint8_t op, uint8_t ext, const ir::Index rm, const ir::Index imm);
    bool R8_RM8(uint8_t op, const ir::Index reg, const ir::Index rm);
    bool R16_RM16(uint8_t op, const ir::Index reg, const ir::Index rm);
    bool R32_RM32(uint8_t op, const ir::Index reg, const ir::Index rm);
    bool R64_RM64(uint8_t op, const ir::Index reg, const ir::Index rm);
    bool R16(uint8_t op, const ir::Index reg);
    bool R32(uint8_t op, const ir::Index reg);
    bool R64(uint8_t op, const ir::Index reg);
    bool RM8(uint8_t op, uint8_t ext, const ir::Index rm);
    bool RM16(uint8_t op, uint8_t ext, const ir::Index rm);
    bool RM32(uint8_t op, uint8_t ext, const ir::Index rm);
    bool RM64(uint8_t op, uint8_t ext, const ir::Index rm);
    bool IMM8(uint8_t op, const ir::Index imm);
    bool IMM16(uint8_t op, const ir::Index imm);
    bool IMM32(uint8_t op, const ir::Index imm);
    void REL8(const ir::Index imm);
    void REL32(const ir::Index imm);
    bool RMx_1();
    bool RMx_CL();
    bool RMx_IMM8();
    bool RDX_RAX_RM8(uint8_t op, uint8_t ext, const ir::Index rdx, const ir::Index rax, const ir::Index rm);
    bool RDX_RAX_RM16(uint8_t op, uint8_t ext, const ir::Index rdx, const ir::Index rax, const ir::Index rm);
    bool RDX_RAX_RM32(uint8_t op, uint8_t ext, const ir::Index rdx, const ir::Index rax, const ir::Index rm);
    bool RDX_RAX_RM64(uint8_t op, uint8_t ext, const ir::Index rdx, const ir::Index rax, const ir::Index rm);
    bool RAX_RM64();
    bool R16_AX(uint8_t op, const ir::Index reg, const ir::Index ax);
    bool R32_EAX(uint8_t op, const ir::Index reg, const ir::Index eax);
    bool R64_RAX(uint8_t op, const ir::Index reg, const ir::Index rax);
    bool MOFFS32_RAX();
    bool RAX_MOFFS32();

    bool ADD(const ir::Index d, const ir::Index s);
    bool ADC(const ir::Index d, const ir::Index s);
    bool XADD(const ir::Index d, const ir::Index s);
    bool SUB(const ir::Index d, const ir::Index s);
    bool SBB(const ir::Index d, const ir::Index s);
    bool AND(const ir::Index d, const ir::Index s);
    bool OR(const ir::Index d, const ir::Index s);
    bool XOR(const ir::Index d, const ir::Index s);
    bool CMP(const ir::Index d, const ir::Index s);
    bool TEST(const ir::Index d, const ir::Index s);
    bool MOV(const ir::Index d, const ir::Index s);
    bool MOVSX(const ir::Index d, const ir::Index s);
    bool MOVSXD(const ir::Index d, const ir::Index s);
    bool MOVZX(const ir::Index d, const ir::Index s);
    
    bool LEA(const ir::Index d, const ir::Index s);
    bool XCHG(const ir::Index d, const ir::Index s);
    bool CMPXCHG(const ir::Index d, const ir::Index s);
    bool PUSH(const ir::Index v);
    bool POP(const ir::Index d);

    bool MUL(const ir::Index d, const ir::Index s);
    bool IMUL(const ir::Index d, const ir::Index s);
    bool IMUL(const ir::Index d, const ir::Index s, const ir::Index i);
    bool CWDE();
    bool CDQE();
    bool CDQ();
    bool CQO();
    bool DIV(const ir::Index d, const ir::Index s);
    bool IDIV(const ir::Index d, const ir::Index s);

    bool INC(const ir::Index d);
    bool DEC(const ir::Index d);
    bool NEG(const ir::Index d);
    bool NOT(const ir::Index d);

    bool Shift(uint8_t ext, const ir::Index d);
    bool ROL(const ir::Index d);
    bool ROR(const ir::Index d);
    bool RCL(const ir::Index d);
    bool RCR(const ir::Index d);
    bool SHL(const ir::Index d);
    bool SHR(const ir::Index d);
    bool SAL(const ir::Index d);
    bool SAR(const ir::Index d);

    bool Shift(uint8_t ext, const ir::Index d, const ir::Index s);
    bool ROL(const ir::Index d, const ir::Index s);
    bool ROR(const ir::Index d, const ir::Index s);
    bool RCL(const ir::Index d, const ir::Index s);
    bool RCR(const ir::Index d, const ir::Index s);
    bool SHL(const ir::Index d, const ir::Index s);
    bool SHR(const ir::Index d, const ir::Index s);
    bool SAL(const ir::Index d, const ir::Index s);
    bool SAR(const ir::Index d, const ir::Index s);

    bool CALL(const ir::Index d);
    bool RET();
    bool RET(const ir::Index bytes);
    bool NOP();
    bool JMP(const ir::Index d);

    bool Jcc(uint8_t op8, uint8_t op, const ir::Index d);
    bool JO(const ir::Index d);
    bool JNO(const ir::Index d);
    bool JB(const ir::Index d);
    bool JNAE(const ir::Index d);
    bool JC(const ir::Index d);
    bool JNB(const ir::Index d);
    bool JAE(const ir::Index d);
    bool JNC(const ir::Index d);
    bool JZ(const ir::Index d);
    bool JE(const ir::Index d);
    bool JNZ(const ir::Index d);
    bool JNE(const ir::Index d);
    bool JBE(const ir::Index d);
    bool JNA(const ir::Index d);
    bool JNBE(const ir::Index d);
    bool JA(const ir::Index d);
    bool JS(const ir::Index d);
    bool JNS(const ir::Index d);
    bool JP(const ir::Index d);
    bool JPE(const ir::Index d);
    bool JNP(const ir::Index d);
    bool JPO(const ir::Index d);
    bool JL(const ir::Index d);
    bool JNGE(const ir::Index d);
    bool JNL(const ir::Index d);
    bool JGE(const ir::Index d);
    bool JLE(const ir::Index d);
    bool JNG(const ir::Index d);
    bool JNLE(const ir::Index d);
    bool JG(const ir::Index d);

    bool SETcc(uint8_t op, const ir::Index d);
    bool SETO(const ir::Index d);
    bool SETNO(const ir::Index d);
    bool SETB(const ir::Index d);
    bool SETNAE(const ir::Index d);
    bool SETC(const ir::Index d);
    bool SETNB(const ir::Index d);
    bool SETAE(const ir::Index d);
    bool SETNC(const ir::Index d);
    bool SETZ(const ir::Index d);
    bool SETE(const ir::Index d);
    bool SETNZ(const ir::Index d);
    bool SETNE(const ir::Index d);
    bool SETBE(const ir::Index d);
    bool SETNA(const ir::Index d);
    bool SETNBE(const ir::Index d);
    bool SETA(const ir::Index d);
    bool SETS(const ir::Index d);
    bool SETNS(const ir::Index d);
    bool SETP(const ir::Index d);
    bool SETPE(const ir::Index d);
    bool SETNP(const ir::Index d);
    bool SETPO(const ir::Index d);
    bool SETL(const ir::Index d);
    bool SETNGE(const ir::Index d);
    bool SETNL(const ir::Index d);
    bool SETGE(const ir::Index d);
    bool SETLE(const ir::Index d);
    bool SETNG(const ir::Index d);
    bool SETNLE(const ir::Index d);
    bool SETG(const ir::Index d);

    bool CMOVcc(uint8_t op, const ir::Index d, const ir::Index s);
    bool CMOVO(const ir::Index d, const ir::Index s);
    bool CMOVNO(const ir::Index d, const ir::Index s);
    bool CMOVB(const ir::Index d, const ir::Index s);
    bool CMOVNAE(const ir::Index d, const ir::Index s);
    bool CMOVC(const ir::Index d, const ir::Index s);
    bool CMOVNB(const ir::Index d, const ir::Index s);
    bool CMOVAE(const ir::Index d, const ir::Index s);
    bool CMOVNC(const ir::Index d, const ir::Index s);
    bool CMOVZ(const ir::Index d, const ir::Index s);
    bool CMOVE(const ir::Index d, const ir::Index s);
    bool CMOVNZ(const ir::Index d, const ir::Index s);
    bool CMOVNE(const ir::Index d, const ir::Index s);
    bool CMOVBE(const ir::Index d, const ir::Index s);
    bool CMOVNA(const ir::Index d, const ir::Index s);
    bool CMOVNBE(const ir::Index d, const ir::Index s);
    bool CMOVA(const ir::Index d, const ir::Index s);
    bool CMOVS(const ir::Index d, const ir::Index s);
    bool CMOVNS(const ir::Index d, const ir::Index s);
    bool CMOVP(const ir::Index d, const ir::Index s);
    bool CMOVPE(const ir::Index d, const ir::Index s);
    bool CMOVNP(const ir::Index d, const ir::Index s);
    bool CMOVPO(const ir::Index d, const ir::Index s);
    bool CMOVL(const ir::Index d, const ir::Index s);
    bool CMOVNGE(const ir::Index d, const ir::Index s);
    bool CMOVNL(const ir::Index d, const ir::Index s);
    bool CMOVGE(const ir::Index d, const ir::Index s);
    bool CMOVLE(const ir::Index d, const ir::Index s);
    bool CMOVNG(const ir::Index d, const ir::Index s);
    bool CMOVNLE(const ir::Index d, const ir::Index s);
    bool CMOVG(const ir::Index d, const ir::Index s);

    bool Add(const ir::Index res, const ir::Index lhs, const ir::Index rhs) {
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

    bool AddFloat(const ir::Index res, const ir::Index lhs, const ir::Index rhs) {
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