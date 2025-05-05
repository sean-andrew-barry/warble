import code.x64.encoder;
import utility.os;
import utility.print;
import utility.to_string;
import <iostream>;
import <vector>;
import <limits>;
import <concepts>;

namespace code::x64 {
  Encoder::Encoder() : code::Encoder{std::endian::little} {}

  void Encoder::SIMD() { Code().Emit8(0x66); }
  void Encoder::OpSize() { Code().Emit8(0x66); }
  void Encoder::AdSize() { Code().Emit8(0x67); }
  void Encoder::Single() { Code().Emit8(0xF3); } // Single precision floating point prefix in SSE
  void Encoder::Double() { Code().Emit8(0xF2); } // Double precision floating point prefix in SSE
  void Encoder::CS() { Code().Emit8(0x2E); }
  void Encoder::SS() { Code().Emit8(0x36); }
  void Encoder::DS() { Code().Emit8(0x3E); }
  void Encoder::ES() { Code().Emit8(0x26); }
  void Encoder::FS() { Code().Emit8(0x64); }
  void Encoder::GS() { Code().Emit8(0x65); }
  void Encoder::BNT() { Code().Emit8(0x2E); } // Branch not taken, same code as CS
  void Encoder::BT() { Code().Emit8(0x3E); } // Branch taken, same code as DS
  void Encoder::Lock() { Code().Emit8(0xF0); }
  void Encoder::REP() { Code().Emit8(0xF3); } // Repeat while equal
  void Encoder::REPNE() { Code().Emit8(0xF2); } // Repeat while not equal
  void Encoder::Escape() { Code().Emit8(0x0F); }
  void Encoder::Escape(uint8_t opcode) { Code().Emit8(0x0F); Code().Emit8(opcode); }
  void Encoder::Esc() { Code().Emit8(0x0F); }
  void Encoder::Esc(uint8_t opcode) { Code().Emit8(0x0F); Code().Emit8(opcode); }

  void Encoder::REX(uint8_t rex) { Code().Emit8(rex); }
  void Encoder::REX() { Code().Emit8(0x40); }
  void Encoder::REXB() { Code().Emit8(0x41); }
  void Encoder::REXX() { Code().Emit8(0x42); }
  void Encoder::REXXB() { Code().Emit8(0x43); }
  void Encoder::REXR() { Code().Emit8(0x44); }
  void Encoder::REXRB() { Code().Emit8(0x45); }
  void Encoder::REXRX() { Code().Emit8(0x46); }
  void Encoder::REXRXB() { Code().Emit8(0x47); }
  void Encoder::REXW() { Code().Emit8(0x48); }
  void Encoder::REXWB() { Code().Emit8(0x49); }
  void Encoder::REXWX() { Code().Emit8(0x4A); }
  void Encoder::REXWXB() { Code().Emit8(0x4B); }
  void Encoder::REXWR() { Code().Emit8(0x4C); }
  void Encoder::REXWRB() { Code().Emit8(0x4D); }
  void Encoder::REXWRX() { Code().Emit8(0x4E); }
  void Encoder::REXWRXB() { Code().Emit8(0x4F); }

  void Encoder::OP(uint8_t primary_opcode) { Code().Emit8(primary_opcode); }
  void Encoder::SO(uint8_t secondary_opcode) { Code().Emit8(secondary_opcode); }

  void Encoder::Displacement(int32_t displacement, uint8_t mod, bool is_bp) {
    // Emit displacement based on mod bits and base register
    if (mod == 0x80) { // 32-bit displacement
      Code().Emit32(displacement);
    } else if (mod == 0x40) { // 8-bit displacement
      Code().Emit8(static_cast<int8_t>(displacement));
    } else if (is_bp) { // Special case for BP with no displacement
      Code().Emit32(displacement);
    }
  }

  bool Encoder::IsStandard(const ir::Symbol& reg) { return (reg.Register() & 0b11101000) == 0b00000000; }
  bool Encoder::IsExtended(const ir::Symbol& reg) { return (reg.Register() & 0b11101000) != 0b00001000; }

  void Encoder::REL8(const ir::Symbol& imm) {
    // TODO: Address calculation needs to be made recursive
    Code().Emit8(imm.Displacement());
  }

  void Encoder::REL32(const ir::Symbol& imm) {
    // TODO: Address calculation needs to be made recursive
    Code().Emit32(imm.Displacement());
  }

  bool Encoder::AL_IMM8(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    if (reg.Register() != 0 || !reg.Is8Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

    OP(op);
    IMM8(imm);

    return true;
  }

  bool Encoder::AX_IMM16(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    if (reg.Register() != 0 || !reg.Is16Bit() || !imm.Is16Bit() || !imm.IsImmediate()) return false;

    OP(op);
    IMM16(imm);

    return true;
  }

  bool Encoder::EAX_IMM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    if (reg.Register() != 0 || !reg.Is32Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

    OP(op);
    IMM32(imm);

    return true;
  }

  bool Encoder::RAX_IMM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    if (reg.Register() != 0 || !reg.Is32Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

    REXW();
    OP(op);
    IMM32(imm);

    return true;
  }

  bool Encoder::R8_IMM8(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    if (reg.Register() != 0 || !reg.Is8Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

    OP(op);
    IMM8(imm);

    return true;
  }

  bool Encoder::R32_IMM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    if (reg.Register() != 0 || !reg.Is32Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

    OP(op);
    IMM32(imm);

    return true;
  }

  bool Encoder::R64_IMM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    if (reg.Register() != 0 || !reg.Is64Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

    REXW();
    OP(op);
    IMM32(imm);

    return true;
  }

  bool Encoder::R8_IMM8s(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    return true;
  }

  bool Encoder::R16_IMM8s(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    return true;
  }

  bool Encoder::R32_IMM8s(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    return true;
  }

  bool Encoder::R64_IMM8s(uint8_t op, const ir::Symbol& reg, const ir::Symbol& imm) {
    return true;
  }

  bool Encoder::RM8_IMM8(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm) {
    if (!rm.Is8Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

    REX(rm);
    OP(op);
    ModRM(ext, rm);
    IMM8(imm);

    return true;
  }

  bool Encoder::RM16_IMM8(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm) {
    if (!rm.Is16Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

    OpSize();
    REX(rm);
    OP(op);
    ModRM(ext, rm);
    IMM8(imm);

    return true;
  }

  bool Encoder::RM32_IMM8(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm) {
    if (!rm.Is32Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

    REX(rm);
    OP(op);
    ModRM(ext, rm);
    IMM8(imm);

    return true;
  }

  bool Encoder::RM64_IMM8(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm) {
    if (!rm.Is64Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

    REX64(rm);
    OP(op);
    ModRM(ext, rm);
    IMM8(imm);

    return true;
  }

  bool Encoder::RM16_IMM16(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm) {
    if (!rm.Is16Bit() || !imm.Is16Bit() || !imm.IsImmediate()) return false;

    OpSize();
    REX(rm);
    OP(op);
    ModRM(ext, rm);
    IMM16(imm);

    return true;
  }

  bool Encoder::RM32_IMM32(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm) {
    if (!rm.Is32Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

    REX(rm);
    OP(op);
    ModRM(ext, rm);
    IMM32(imm);

    return true;
  }

  bool Encoder::RM64_IMM32(uint8_t op, uint8_t ext, const ir::Symbol& rm, const ir::Symbol& imm) {
    if (!rm.Is64Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

    REX64(rm);
    OP(op);
    ModRM(ext, rm);
    IMM32(imm);

    return true;
  }

  bool Encoder::R8_RM8(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rm) {
    if (!reg.Is8Bit() || !rm.Is8Bit() || !reg.IsAllocated()) return false;

    REX(reg, rm);
    OP(op);
    ModRM(reg, rm);

    return true;
  }

  bool Encoder::R16_RM16(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rm) {
    if (!reg.Is16Bit() || !rm.Is16Bit() || !reg.IsAllocated()) return false;

    OpSize();
    REX(reg, rm);
    OP(op);
    ModRM(reg, rm);

    return true;
  }

  bool Encoder::R32_RM32(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rm) {
    if (!reg.Is32Bit() || !rm.Is32Bit() || !reg.IsAllocated()) return false;

    REX(reg, rm);
    OP(op);
    ModRM(reg, rm);

    return true;
  }

  bool Encoder::R64_RM64(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rm) {
    if (!reg.Is64Bit() || !rm.Is64Bit() || !reg.IsAllocated()) return false;

    REX64(reg, rm); // Use the 64 bit version of REX
    OP(op);
    ModRM(reg, rm);

    return true;
  }

  bool Encoder::RM8(uint8_t op, uint8_t ext, const ir::Symbol& rm) {
    if (!rm.Is8Bit()) return false;

    REX(rm);
    OP(op);
    ModRM(ext, rm);

    return true;
  }

  bool Encoder::RM16(uint8_t op, uint8_t ext, const ir::Symbol& rm) {
    if (!rm.Is16Bit()) return false;

    OpSize();
    REX(rm);
    OP(op);
    ModRM(ext, rm);

    return true;
  }

  bool Encoder::RM32(uint8_t op, uint8_t ext, const ir::Symbol& rm) {
    if (!rm.Is32Bit()) return false;

    REX(rm);
    OP(op);
    ModRM(ext, rm);

    return true;
  }

  bool Encoder::RM64(uint8_t op, uint8_t ext, const ir::Symbol& rm) {
    if (!rm.Is64Bit()) return false;

    REX64(rm);
    OP(op);
    ModRM(ext, rm);

    return true;
  }

  bool Encoder::IMM8(uint8_t op, const ir::Symbol& imm) {
    if (!imm.Is8Bit() || !imm.IsImmediate()) return false;

    OP(op);
    IMM8(imm);

    return true;
  }

  bool Encoder::IMM16(uint8_t op, const ir::Symbol& imm) {
    if (!imm.Is16Bit() || !imm.IsImmediate()) return false;

    OpSize();
    OP(op);
    IMM16(imm);

    return true;
  }

  bool Encoder::IMM32(uint8_t op, const ir::Symbol& imm) {
    if (!imm.Is32Bit() || !imm.IsImmediate()) return false;

    OP(op);
    IMM32(imm);

    return true;
  }

  bool Encoder::RMx_1() {
    return false; // IDK how this encodes
  }

  bool Encoder::RMx_CL() {
    return false; // IDK how this encodes
  }

  bool Encoder::RMx_IMM8() {
    return false; // IDK how this encodes
  }

  bool Encoder::RDX_RAX_RM8(uint8_t op, uint8_t ext, const ir::Symbol& rdx, const ir::Symbol& rax, const ir::Symbol& rm) {
    if (!rdx.Is64Bit() || !rax.Is64Bit() || !rm.Is8Bit() || !rdx.IsAllocated() || !rax.IsAllocated()) return false;

    REX(rm);
    OP(op);
    ModRM(ext, rm);

    return true;
  }

  bool Encoder::RDX_RAX_RM16(uint8_t op, uint8_t ext, const ir::Symbol& rdx, const ir::Symbol& rax, const ir::Symbol& rm) {
    if (!rdx.Is64Bit() || !rax.Is64Bit() || !rm.Is16Bit() || !rdx.IsAllocated() || !rax.IsAllocated()) return false;

    OpSize();
    REX(rm);
    OP(op);
    ModRM(ext, rm);

    return true;
  }

  bool Encoder::RDX_RAX_RM32(uint8_t op, uint8_t ext, const ir::Symbol& rdx, const ir::Symbol& rax, const ir::Symbol& rm) {
    if (!rdx.Is64Bit() || !rax.Is64Bit() || !rm.Is32Bit() || !rdx.IsAllocated() || !rax.IsAllocated()) return false;

    REX(rm);
    OP(op);
    ModRM(ext, rm);

    return true;
  }

  bool Encoder::RDX_RAX_RM64(uint8_t op, uint8_t ext, const ir::Symbol& rdx, const ir::Symbol& rax, const ir::Symbol& rm) {
    if (!rdx.Is64Bit() || !rax.Is64Bit() || !rm.Is64Bit() || !r1.IsAllocated() || !rax.IsAllocated()) return false;

    REX64(rm);
    OP(op);
    ModRM(ext, rm);

    return true;
  }

  bool Encoder::RAX_RM64() {
    return false; // TODO
  }

  bool Encoder::R16_AX(uint8_t op, const ir::Symbol& reg, const ir::Symbol& ax) {
    if (!reg.IsAllocated() || !reg.Is16Bit() || !ax.IsAllocated() || !ax.Is16Bit() || ax.Register() != 0) return false;

    OpSize();
    OP(op, reg);

    return true;
  }

  bool Encoder::R32_EAX(uint8_t op, const ir::Symbol& reg, const ir::Symbol& eax) {
    if (!reg.IsAllocated() || !reg.Is32Bit() || !eax.IsAllocated() || !eax.Is32Bit() || eax.Register() != 0) return false;

    OP(op, reg);

    return true;
  }

  bool Encoder::R64_RAX(uint8_t op, const ir::Symbol& reg, const ir::Symbol& rax) {
    if (!reg.IsAllocated() || !reg.Is64Bit() || !rax.IsAllocated() || !rax.Is64Bit() || rax.Register() != 0) return false;

    REX(reg);
    OP(op, reg);

    return true;
  }

  bool Encoder::MOFFS32_RAX() {
    return false; // IDK how this encodes
  }

  bool Encoder::RAX_MOFFS32() {
    return false; // IDK how this encodes
  }
};