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
  void Encoder::Prefix(uint8_t opcode) { if (opcode) Code().Emit8(opcode); }
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

  void Encoder::OP(uint8_t op, const ir::Symbol& reg) {
    OP(op + (reg.Register() & 0x07));
  }

  void Encoder::WOP(uint8_t opcode) {
    REXW();
    OP(opcode);
  }

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

  bool Encoder::R16(uint8_t op, const ir::Symbol& reg) {
    if (!reg.IsAllocated() || !reg.Is16Bit()) return false;

    OpSize();
    REX(reg);
    OP(op, reg);

    return true;
  }

  bool Encoder::R32(uint8_t op, const ir::Symbol& reg) {
    if (!reg.IsAllocated() || !reg.Is32Bit()) return false;

    REX(reg);
    OP(op, reg);

    return true;
  }

  bool Encoder::R64(uint8_t op, const ir::Symbol& reg) {
    if (!reg.IsAllocated() || !reg.Is64Bit()) return false;

    REX64(reg);
    OP(op, reg);

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

  bool Encoder::ADD(const ir::Symbol& d, const ir::Symbol& s) {
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

  bool Encoder::ADC(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8  (0x14,           d, s)  // ADC AL,  imm8
        || AX_IMM16 (0x15,           d, s)  // ADC AX,  imm16
        || EAX_IMM32(0x15,           d, s)  // ADC EAX, imm32
        || RAX_IMM32(0x15,           d, s)  // ADC RAX, imm32 (sign-ext)
        || R32_IMM8 (0x83,           d, s)  // ADC r32, imm8  (sign-ext)
        || R64_IMM8 (0x83,           d, s)  // ADC r64, imm8  (sign-ext)
        || RM8_IMM8 (0x80, 0x02,     d, s)  // ADC r/m8,  imm8
        || RM32_IMM8(0x83, 0x02,     d, s)  // ADC r/m32, imm8 (sign-ext)
        || RM64_IMM8(0x83, 0x02,     d, s)  // ADC r/m64, imm8 (sign-ext)
        || RM32_IMM32(0x81, 0x02,    d, s)  // ADC r/m32, imm32
        || RM64_IMM32(0x81, 0x02,    d, s)  // ADC r/m64, imm32
        || RM16_IMM8(0x83, 0x02,     d, s)  // ADC r/m16, imm8 (sign-ext)
        || R8_RM8   (0x12,           d, s)  // ADC r8,  r/m8
        || R32_RM32 (0x13,           d, s)  // ADC r32, r/m32
        || R64_RM64 (0x13,           d, s)  // ADC r64, r/m64
        || R16_RM16 (0x13,           d, s)  // ADC r16, r/m16
        || RM8_R8   (0x10,           d, s)  // ADC r/m8,  r8
        || RM32_R32 (0x11,           d, s)  // ADC r/m32, r32
        || RM64_R64 (0x11,           d, s)  // ADC r/m64, r64
        || RM16_R16 (0x11,           d, s)  // ADC r/m16, r16
        || RM16_IMM16(0x81, 0x02,    d, s); // ADC r/m16, imm16
  }

  bool Encoder::XADD(const ir::Symbol& d, const ir::Symbol& s) {
    return RM8_R8  (0xC0, d, s, true)   // XADD r/m8 ,  r8
        || RM16_R16(0xC1, d, s, true)   // XADD r/m16, r16
        || RM32_R32(0xC1, d, s, true)   // XADD r/m32, r32
        || RM64_R64(0xC1, d, s, true);  // XADD r/m64, r64   (REX.W)
  }

  bool Encoder::SUB(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8 (0x2C,            d, s)  // SUB AL,  imm8
        || AX_IMM16 (0x2D,           d, s)  // SUB AX,  imm16
        || EAX_IMM32(0x2D,           d, s)  // SUB EAX, imm32
        || RAX_IMM32(0x2D,           d, s)  // SUB RAX, imm32 (sign-ext)
        || R32_IMM8 (0x83,           d, s)  // SUB r32, imm8  (sign-ext)
        || R64_IMM8 (0x83,           d, s)  // SUB r64, imm8  (sign-ext)
        || RM8_IMM8 (0x80, 0x05,     d, s)  // SUB r/m8,  imm8
        || RM32_IMM8(0x83, 0x05,     d, s)  // SUB r/m32, imm8 (sign-ext)
        || RM64_IMM8(0x83, 0x05,     d, s)  // SUB r/m64, imm8 (sign-ext)
        || RM32_IMM32(0x81, 0x05,    d, s)  // SUB r/m32, imm32
        || RM64_IMM32(0x81, 0x05,    d, s)  // SUB r/m64, imm32
        || RM16_IMM8(0x83, 0x05,     d, s)  // SUB r/m16, imm8 (sign-ext)
        || R8_RM8   (0x2A,           d, s)  // SUB r8,  r/m8
        || R32_RM32 (0x2B,           d, s)  // SUB r32, r/m32
        || R64_RM64 (0x2B,           d, s)  // SUB r64, r/m64
        || R16_RM16 (0x2B,           d, s)  // SUB r16, r/m16
        || RM8_R8   (0x28,           d, s)  // SUB r/m8,  r8
        || RM32_R32 (0x29,           d, s)  // SUB r/m32, r32
        || RM64_R64 (0x29,           d, s)  // SUB r/m64, r64
        || RM16_R16 (0x29,           d, s)  // SUB r/m16, r16
        || RM16_IMM16(0x81, 0x05,    d, s); // SUB r/m16, imm16
  }

  bool Encoder::SBB(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8  (0x1C,           d, s)  // SBB AL,  imm8
        || AX_IMM16 (0x1D,           d, s)  // SBB AX,  imm16
        || EAX_IMM32(0x1D,           d, s)  // SBB EAX, imm32
        || RAX_IMM32(0x1D,           d, s)  // SBB RAX, imm32 (sign-ext)
        || R32_IMM8 (0x83,           d, s)  // SBB r32, imm8  (sign-ext)
        || R64_IMM8 (0x83,           d, s)  // SBB r64, imm8  (sign-ext)
        || RM8_IMM8 (0x80, 0x03,     d, s)  // SBB r/m8,  imm8
        || RM32_IMM8(0x83, 0x03,     d, s)  // SBB r/m32, imm8 (sign-ext)
        || RM64_IMM8(0x83, 0x03,     d, s)  // SBB r/m64, imm8 (sign-ext)
        || RM32_IMM32(0x81, 0x03,    d, s)  // SBB r/m32, imm32
        || RM64_IMM32(0x81, 0x03,    d, s)  // SBB r/m64, imm32
        || RM16_IMM8(0x83, 0x03,     d, s)  // SBB r/m16, imm8 (sign-ext)
        || R8_RM8   (0x1A,           d, s)  // SBB r8,  r/m8
        || R32_RM32 (0x1B,           d, s)  // SBB r32, r/m32
        || R64_RM64 (0x1B,           d, s)  // SBB r64, r/m64
        || R16_RM16 (0x1B,           d, s)  // SBB r16, r/m16
        || RM8_R8   (0x18,           d, s)  // SBB r/m8,  r8
        || RM32_R32 (0x19,           d, s)  // SBB r/m32, r32
        || RM64_R64 (0x19,           d, s)  // SBB r/m64, r64
        || RM16_R16 (0x19,           d, s)  // SBB r/m16, r16
        || RM16_IMM16(0x81, 0x03,    d, s); // SBB r/m16, imm16
  }

  bool Encoder::AND(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8(0x24, d, s)
        || AX_IMM16(0x25, d, s)
        || EAX_IMM32(0x25, d, s)
        || RAX_IMM32(0x25, d, s)
        || RM8_IMM8(0x80, 0x04, d, s)
        || RM32_IMM8(0x83, 0x04, d, s)
        || RM64_IMM8(0x83, 0x04, d, s)
        || RM16_IMM8(0x83, 0x04, d, s)
        || RM32_IMM32(0x81, 0x04, d, s)
        || RM64_IMM32(0x81, 0x04, d, s)
        || RM16_IMM16(0x81, 0x04, d, s)
        || R8_RM8(0x22, d, s)
        || R32_RM32(0x23, d, s)
        || R64_RM64(0x23, d, s)
        || R16_RM16(0x23, d, s)
        || RM8_R8(0x20, d, s)
        || RM32_R32(0x21, d, s)
        || RM64_R64(0x21, d, s)
        || RM16_R16(0x21, d, s);
  }

  bool Encoder::OR(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8(0x0C, d, s)
        || AX_IMM16(0x0D, d, s)
        || EAX_IMM32(0x0D, d, s)
        || RAX_IMM32(0x0D, d, s)
        || RM8_IMM8(0x80, 0x01, d, s)
        || RM32_IMM8(0x83, 0x01, d, s)
        || RM64_IMM8(0x83, 0x01, d, s)
        || RM16_IMM8(0x83, 0x01, d, s)
        || RM32_IMM32(0x81, 0x01, d, s)
        || RM64_IMM32(0x81, 0x01, d, s)
        || RM16_IMM16(0x81, 0x01, d, s)
        || R8_RM8(0x0A, d, s)
        || R32_RM32(0x0B, d, s)
        || R64_RM64(0x0B, d, s)
        || R16_RM16(0x0B, d, s)
        || RM8_R8(0x08, d, s)
        || RM32_R32(0x09, d, s)
        || RM64_R64(0x09, d, s)
        || RM16_R16(0x09, d, s);
  }

  bool Encoder::XOR(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8(0x34, d, s)
        || AX_IMM16(0x35, d, s)
        || EAX_IMM32(0x35, d, s)
        || RAX_IMM32(0x35, d, s)
        || RM8_IMM8(0x80, 0x06, d, s)
        || RM32_IMM8(0x83, 0x06, d, s)
        || RM64_IMM8(0x83, 0x06, d, s)
        || RM16_IMM8(0x83, 0x06, d, s)
        || RM32_IMM32(0x81, 0x06, d, s)
        || RM64_IMM32(0x81, 0x06, d, s)
        || RM16_IMM16(0x81, 0x06, d, s)
        || R8_RM8(0x32, d, s)
        || R32_RM32(0x33, d, s)
        || R64_RM64(0x33, d, s)
        || R16_RM16(0x33, d, s)
        || RM8_R8(0x30, d, s)
        || RM32_R32(0x31, d, s)
        || RM64_R64(0x31, d, s)
        || RM16_R16(0x31, d, s);
  }

  bool Encoder::CMP(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8(0x3C, d, s)
        || AX_IMM16(0x3D, d, s)
        || EAX_IMM32(0x3D, d, s)
        || RAX_IMM32(0x3D, d, s)
        || RM8_IMM8(0x80, 0x07, d, s)
        || RM16_IMM8(0x83, 0x07, d, s)
        || RM32_IMM8(0x83, 0x07, d, s)
        || RM64_IMM8(0x83, 0x07, d, s)
        || RM16_IMM16(0x81, 0x07, d, s)
        || RM32_IMM32(0x81, 0x07, d, s)
        || RM64_IMM32(0x81, 0x07, d, s)
        || R8_RM8(0x3A, d, s)
        || R16_RM16(0x3B, d, s)
        || R32_RM32(0x3B, d, s)
        || R64_RM64(0x3B, d, s)
        || RM8_R8(0x38, d, s)
        || RM16_R16(0x39, d, s)
        || RM32_R32(0x39, d, s)
        || RM64_R64(0x39, d, s);
  }

  bool Encoder::TEST(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8(0xA8, d, s)
        || AX_IMM16(0xA9, d, s)
        || EAX_IMM32(0xA9, d, s)
        || RAX_IMM32(0xA9, d, s)
        || RM8_IMM8(0xF6, 0x00, d, s)
        || RM16_IMM16(0xF7, 0x00, d, s)
        || RM32_IMM32(0xF7, 0x00, d, s)
        || RM64_IMM32(0xF7, 0x00, d, s)
        || RM8_R8(0x84, d, s)
        || RM16_R16(0x85, d, s)
        || RM32_R32(0x85, d, s)
        || RM64_R64(0x85, d, s);
  }

  bool Encoder::MOV(const ir::Symbol& d, const ir::Symbol& s) {
    return AL_IMM8  (0xB0,           d, s)  // MOV  AL,  imm8
        || R8_IMM8  (0xB0,           d, s)  // MOV  r8,  imm8   (0xB0+rb)
        || AX_IMM16 (0xB8,           d, s)  // MOV  AX,  imm16
        || EAX_IMM32(0xB8,           d, s)  // MOV  EAX, imm32
        || RAX_IMM32(0xB8,           d, s)  // MOV  RAX, imm32 (sign-ext)
        || R32_IMM32(0xB8,           d, s)  // MOV  r32, imm32
        || R64_IMM64(0xB8,           d, s)  // MOV  r64, imm64
        || RM8_IMM8 (0xC6, 0x00,     d, s)  // MOV  r/m8,  imm8
        || RM16_IMM16(0xC7, 0x00,    d, s)  // MOV  r/m16, imm16
        || RM32_IMM32(0xC7, 0x00,    d, s)  // MOV  r/m32, imm32
        || RM64_IMM32(0xC7, 0x00,    d, s)  // MOV  r/m64, imm32 (sign-ext)
        || R8_RM8   (0x8A,           d, s)  // MOV  r8,  r/m8
        || R16_RM16 (0x8B,           d, s)  // MOV  r16, r/m16
        || R32_RM32 (0x8B,           d, s)  // MOV  r32, r/m32
        || R64_RM64 (0x8B,           d, s)  // MOV  r64, r/m64
        || RM8_R8   (0x88,           d, s)  // MOV  r/m8,  r8
        || RM16_R16 (0x89,           d, s)  // MOV  r/m16, r16
        || RM32_R32 (0x89,           d, s)  // MOV  r/m32, r32
        || RM64_R64 (0x89,           d, s); // MOV  r/m64, r64
  }

  bool Encoder::MOVSX(const ir::Symbol& d, const ir::Symbol& s) {
    return R16_RM8 (0xBE, d, s, true)  // MOVSX r16, r/m8
        || R32_RM8 (0xBE, d, s, true)  // MOVSX r32, r/m8
        || R64_RM8 (0xBE, d, s, true)  // MOVSX r64, r/m8
        || R32_RM16(0xBF, d, s, true)  // MOVSX r32, r/m16
        || R64_RM16(0xBF, d, s, true); // MOVSX r64, r/m16
  }

  bool Encoder::MOVSXD(const ir::Symbol& d, const ir::Symbol& s) {
    return R64_RM32(0x63, d, s, true); // MOVSXD r64, r/m32
  }

  bool Encoder::MOVZX(const ir::Symbol& d, const ir::Symbol& s) {
    return R16_RM8 (0xB6, d, s, true)  // MOVZX r16, r/m8
        || R32_RM8 (0xB6, d, s, true)  // MOVZX r32, r/m8
        || R64_RM8 (0xB6, d, s, true)  // MOVZX r64, r/m8
        || R32_RM16(0xB7, d, s, true)  // MOVZX r32, r/m16
        || R64_RM16(0xB7, d, s, true); // MOVZX r64, r/m16
  }

  bool Encoder::LEA(const ir::Symbol& d, const ir::Symbol& s) {
    // Try widest first (typical for 64-bit builds), then fall back.
    return R64_RM64(0x8D, d, s)  // LEA r64,  m
        || R32_RM32(0x8D, d, s)  // LEA r32,  m   (zero-extended in 64-bit mode)
        || R16_RM16(0x8D, d, s); // LEA r16,  m   (requires 0x66 prefix)
  }

  bool Encoder::XCHG(const ir::Symbol& d, const ir::Symbol& s) {
    return R8_RM8  (0x86, d, s) // r8  ↔ r/m8
        || R16_RM16(0x87, d, s) // r16 ↔ r/m16
        || R32_RM32(0x87, d, s) // r32 ↔ r/m32
        || R64_RM64(0x87, d, s) // r64 ↔ r/m64
        || RM8_R8  (0x86, d, s) // r/m8 ↔ r8
        || RM16_R16(0x87, d, s) // r/m16 ↔ r16
        || RM32_R32(0x87, d, s) // r/m32 ↔ r32
        || RM64_R64(0x87, d, s) // r/m64 ↔ r64

        // Accumulator-special encodings (90+rd)
        || AX_R16O (0x90, d, s) || AX_R16O (0x90, s, d)  // XCHG AX, r16
        || EAX_R32O(0x90, d, s) || EAX_R32O(0x90, s, d)  // XCHG EAX, r32
        || RAX_R64O(0x90, d, s) || RAX_R64O(0x90, s, d); // XCHG RAX, r64
  }

  bool Encoder::CMPXCHG(const ir::Symbol& d, const ir::Symbol& s) {
    return RM8_R8  (0xB0, d, s, true)  // CMPXCHG r/m8 ,  r8
        || RM16_R16(0xB1, d, s, true)  // CMPXCHG r/m16, r16
        || RM32_R32(0xB1, d, s, true)  // CMPXCHG r/m32, r32
        || RM64_R64(0xB1, d, s, true); // CMPXCHG r/m64, r64
  }

  bool Encoder::PUSH(const ir::Symbol& v) {
    return R64O (0x50,     v)   // PUSH r64        (50+rd, REX as needed)
        || R32O (0x50,     v)   // PUSH r32        (32-bit mode or no REX.W)
        || R16O (0x50,     v)   // PUSH r16 (requires 0x66 prefix in 64-bit)
        || IMM8 (0x6A,     v)   // PUSH imm8       (sign-extended)
        || IMM32(0x68,     v)   // PUSH imm16/32   (sign-extended to 64 in 64-bit)
        || RM64(0xFF, 0x06, v)  // PUSH r/m64      (FF /6)
        || RM32(0xFF, 0x06, v)  // PUSH r/m32
        || RM16(0xFF, 0x06, v); // PUSH r/m16
  }

  bool Encoder::POP(const ir::Symbol& d) {
    return R64O (0x58,      d)   // POP r64         (58+rd)
        || R32O (0x58,      d)   // POP r32
        || R16O (0x58,      d)   // POP r16 (needs 0x66 in 64-bit)
        || RM64(0x8F, 0x00, d)  // POP r/m64       (8F /0)
        || RM32(0x8F, 0x00, d)  // POP r/m32
        || RM16(0x8F, 0x00, d); // POP r/m16
  }

  bool Encoder::MUL(const ir::Symbol& d, const ir::Symbol& s) {
    return AX_RM8  (0xF6, 0x04, d, s)  // MUL r/m8   → AX
        || AX_RM16 (0xF7, 0x04, d, s)  // MUL r/m16  → DX:AX
        || EAX_RM32(0xF7, 0x04, d, s)  // MUL r/m32  → EDX:EAX
        || RAX_RM64(0xF7, 0x04, d, s); // MUL r/m64  → RDX:RAX
  }

  bool Encoder::IMUL(const ir::Symbol& d, const ir::Symbol& s) {
    // one-operand (widening, same encodings as MUL but /5)
    return AX_RM8   (0xF6, 0x05, d, s) // IMUL r/m8   → AX
        || AX_RM16  (0xF7, 0x05, d, s) // IMUL r/m16  → DX:AX
        || EAX_RM32 (0xF7, 0x05, d, s) // IMUL r/m32  → EDX:EAX
        || RAX_RM64 (0xF7, 0x05, d, s) // IMUL r/m64  → RDX:RAX
        // two-operand non-widening (sign-preserving)
        || R16_RM16 (0xAF, d, s, true)  // IMUL r16, r/m16
        || R32_RM32 (0xAF, d, s, true)  // IMUL r32, r/m32
        || R64_RM64 (0xAF, d, s, true); // IMUL r64, r/m64
  }

  bool Encoder::IMUL(const ir::Symbol& d, const ir::Symbol& s, const ir::Symbol& i) {
    return R16_RM16_IMM8 (0x6B, d, s, i)  // IMUL r16, r/m16, imm8  (sign-ext)
        || R32_RM32_IMM8 (0x6B, d, s, i)  // IMUL r32, r/m32, imm8
        || R64_RM64_IMM8 (0x6B, d, s, i)  // IMUL r64, r/m64, imm8
        || R16_RM16_IMM16(0x69, d, s, i)  // IMUL r16, r/m16, imm16
        || R32_RM32_IMM32(0x69, d, s, i)  // IMUL r32, r/m32, imm32
        || R64_RM64_IMM32(0x69, d, s, i); // IMUL r64, r/m64, imm32
  }

  bool Encoder::CWDE() {
    // AX -> EAX   (32-bit operand size)
    OP(0x98);
    return true;
  }

  bool Encoder::CDQE() {
    // AX -> RAX   (64-bit operand size, REX.W)
    WOP(0x98);
    return true;
  }

  bool Encoder::CDQ() {
    // EAX -> EDX:EAX
    OP(0x99);
    return true;
  }

  bool Encoder::CQO() {
    // RAX -> RDX:RAX
    WOP(0x99);
    return true;
  }

  bool Encoder::DIV(const ir::Symbol& d, const ir::Symbol& s) {
    return AX_RM8  (0xF6, 0x06, d, s)  // DIV r/m8   : AX ÷ r/m8
        || AX_RM16 (0xF7, 0x06, d, s)  // DIV r/m16  : DX:AX ÷ r/m16
        || EAX_RM32(0xF7, 0x06, d, s)  // DIV r/m32  : EDX:EAX ÷ r/m32
        || RAX_RM64(0xF7, 0x06, d, s); // DIV r/m64  : RDX:RAX ÷ r/m64
  }

  bool Encoder::IDIV(const ir::Symbol& d, const ir::Symbol& s) {
    return AX_RM8  (0xF6, 0x07, d, s)  // IDIV r/m8   : AX ÷ r/m8
        || AX_RM16 (0xF7, 0x07, d, s)  // IDIV r/m16  : DX:AX ÷ r/m16
        || EAX_RM32(0xF7, 0x07, d, s)  // IDIV r/m32  : EDX:EAX ÷ r/m32
        || RAX_RM64(0xF7, 0x07, d, s); // IDIV r/m64  : RDX:RAX ÷ r/m64
  }

  bool Encoder::INC(const ir::Symbol& d) {
    return RM8 (0xFE, 0x00, d)
        || RM64(0xFF, 0x00, d)
        || RM32(0xFF, 0x00, d)
        || RM16(0xFF, 0x00, d);
  }

  bool Encoder::DEC(const ir::Symbol& d) {
    return RM8 (0xFE, 0x01, d)
        || RM64(0xFF, 0x01, d)
        || RM32(0xFF, 0x01, d)
        || RM16(0xFF, 0x01, d);
  }

  bool Encoder::NEG(const ir::Symbol& d) {
    return RM8(0xF6, 0x03, d)
        || RM16(0xF7, 0x03, d)
        || RM32(0xF7, 0x03, d)
        || RM64(0xF7, 0x03, d);
  }

  bool Encoder::NOT(const ir::Symbol& d) {
    return RM8(0xF6, 0x02, d)
        || RM16(0xF7, 0x02, d)
        || RM32(0xF7, 0x02, d)
        || RM64(0xF7, 0x02, d);
  }

  bool Encoder::Shift(uint8_t ext, const ir::Symbol& d) {
    return RM8 (0xD0, ext, d)
        || RM16(0xD1, ext, d)
        || RM32(0xD1, ext, d)
        || RM64(0xD1, ext, d);
  }

  bool Encoder::ROL(const ir::Symbol& d) { return Shift(0x00, d); }
  bool Encoder::ROR(const ir::Symbol& d) { return Shift(0x01, d); }
  bool Encoder::RCL(const ir::Symbol& d) { return Shift(0x02, d); }
  bool Encoder::RCR(const ir::Symbol& d) { return Shift(0x03, d); }
  bool Encoder::SHL(const ir::Symbol& d) { return Shift(0x04, d); }
  bool Encoder::SAL(const ir::Symbol& d) { return SHL(d); }
  bool Encoder::SHR(const ir::Symbol& d) { return Shift(0x05, d); }
  bool Encoder::SAR(const ir::Symbol& d) { return Shift(0x07, d); }

  bool Encoder::Shift(uint8_t ext, const ir::Symbol& d, const ir::Symbol& s) {
    return RM8_CL (0xD2, ext, d, s)
        || RM16_CL(0xD3, ext, d, s)
        || RM32_CL(0xD3, ext, d, s)
        || RM64_CL(0xD3, ext, d, s)
        || RM8_IMM8 (0xC0, ext, d, s)
        || RM16_IMM8(0xC1, ext, d, s)
        || RM32_IMM8(0xC1, ext, d, s)
        || RM64_IMM8(0xC1, ext, d, s);
  }

  bool Encoder::ROL(const ir::Symbol& d, const ir::Symbol& s) { return Shift(0x00, d, s); }
  bool Encoder::ROR(const ir::Symbol& d, const ir::Symbol& s) { return Shift(0x01, d, s); }
  bool Encoder::RCL(const ir::Symbol& d, const ir::Symbol& s) { return Shift(0x02, d, s); }
  bool Encoder::RCR(const ir::Symbol& d, const ir::Symbol& s) { return Shift(0x03, d, s); }
  bool Encoder::SHL(const ir::Symbol& d, const ir::Symbol& s) { return Shift(0x04, d, s); }
  bool Encoder::SAL(const ir::Symbol& d, const ir::Symbol& s) { return SHL(d, s); }
  bool Encoder::SHR(const ir::Symbol& d, const ir::Symbol& s) { return Shift(0x05, d, s); }
  bool Encoder::SAR(const ir::Symbol& d, const ir::Symbol& s) { return Shift(0x07, d, s); }

  bool Encoder::CALL(const ir::Symbol& d) {
    return REL32(0xE8, d)
        || RM32(0xFF, 0x02, d)
        || RM64(0xFF, 0x02, d);
  }

  bool Encoder::RET() {
    OP(0xC3); // Emit the opcode directly
    return true;
  }

  bool Encoder::RET(const ir::Symbol& bytes) {
    // accept only small constexpr/imm16 symbols
    return IMM16(0xC2, bytes); // your helper: emit opcode + imm16
  }

  bool Encoder::NOP() {
    OP(0x90); // Emit the opcode directly
    return true;
  }

  bool Encoder::JMP(const ir::Symbol& d) {
    return REL8(0xEB, d)
        || REL32(0xE9, d)
        || RM32(0xFF, 0x04, d)
        || RM64(0xFF, 0x04, d);
  }

  bool Encoder::Jcc(uint8_t op8, uint8_t op, const ir::Symbol& d) {
    return REL8(op8, d)
        || REL32(op, d, true);
  }

  bool Encoder::JO(const ir::Symbol& d) { return Jcc(0x70, 0x80, d); }
  bool Encoder::JNO(const ir::Symbol& d) { return Jcc(0x71, 0x81, d); }
  bool Encoder::JB(const ir::Symbol& d) { return Jcc(0x72, 0x82, d); }
  bool Encoder::JNAE(const ir::Symbol& d) { return JB(d); }
  bool Encoder::JC(const ir::Symbol& d) { return JB(d); }
  bool Encoder::JNB(const ir::Symbol& d) { return Jcc(0x73, 0x83, d); }
  bool Encoder::JAE(const ir::Symbol& d) { return JNB(d); }
  bool Encoder::JNC(const ir::Symbol& d) { return JNB(d); }
  bool Encoder::JZ(const ir::Symbol& d) { return Jcc(0x74, 0x84, d); }
  bool Encoder::JE(const ir::Symbol& d) { return JZ(d); }
  bool Encoder::JNZ(const ir::Symbol& d) { return Jcc(0x75, 0x85, d); }
  bool Encoder::JNE(const ir::Symbol& d) { return JNZ(d); }
  bool Encoder::JBE(const ir::Symbol& d) { return Jcc(0x76, 0x86, d); }
  bool Encoder::JNA(const ir::Symbol& d) { return JBE(d); }
  bool Encoder::JNBE(const ir::Symbol& d) { return Jcc(0x77, 0x87, d); }
  bool Encoder::JA(const ir::Symbol& d) { return JNBE(d); }
  bool Encoder::JS(const ir::Symbol& d) { return Jcc(0x78, 0x88, d); }
  bool Encoder::JNS(const ir::Symbol& d) { return Jcc(0x79, 0x89, d); }
  bool Encoder::JP(const ir::Symbol& d) { return Jcc(0x7A, 0x8A, d); }
  bool Encoder::JPE(const ir::Symbol& d) { return JP(d); }
  bool Encoder::JNP(const ir::Symbol& d) { return Jcc(0x7B, 0x8B, d); }
  bool Encoder::JPO(const ir::Symbol& d) { return JNP(d); }
  bool Encoder::JL(const ir::Symbol& d) { return Jcc(0x7C, 0x8C, d); }
  bool Encoder::JNGE(const ir::Symbol& d) { return JL(d); }
  bool Encoder::JNL(const ir::Symbol& d) { return Jcc(0x7D, 0x8D, d); }
  bool Encoder::JGE(const ir::Symbol& d) { return JNL(d); }
  bool Encoder::JLE(const ir::Symbol& d) { return Jcc(0x7E, 0x8E, d); }
  bool Encoder::JNG(const ir::Symbol& d) { return JLE(d); }
  bool Encoder::JNLE(const ir::Symbol& d) { return Jcc(0x7F, 0x8F, d); }
  bool Encoder::JG(const ir::Symbol& d) { return JNLE(d); }

  bool Encoder::SETcc(uint8_t op, const ir::Symbol& d) {
    return RM8(op, 0x00, d, true);
  }

  bool Encoder::SETO(const ir::Symbol& d) { return SETcc(0x90, d); }
  bool Encoder::SETNO(const ir::Symbol& d) { return SETcc(0x91, d); }
  bool Encoder::SETB(const ir::Symbol& d) { return SETcc(0x92, d); }
  bool Encoder::SETNAE(const ir::Symbol& d) { return SETB(d); }
  bool Encoder::SETC(const ir::Symbol& d) { return SETB(d); }
  bool Encoder::SETNB(const ir::Symbol& d) { return SETcc(0x93, d); }
  bool Encoder::SETAE(const ir::Symbol& d) { return SETNB(d); }
  bool Encoder::SETNC(const ir::Symbol& d) { return SETNB(d); }
  bool Encoder::SETZ(const ir::Symbol& d) { return SETcc(0x94, d); }
  bool Encoder::SETE(const ir::Symbol& d) { return SETZ(d); }
  bool Encoder::SETNZ(const ir::Symbol& d) { return SETcc(0x95, d); }
  bool Encoder::SETNE(const ir::Symbol& d) { return SETNZ(d); }
  bool Encoder::SETBE(const ir::Symbol& d) { return SETcc(0x96, d); }
  bool Encoder::SETNA(const ir::Symbol& d) { return SETBE(d); }
  bool Encoder::SETNBE(const ir::Symbol& d) { return SETcc(0x97, d); }
  bool Encoder::SETA(const ir::Symbol& d) { return SETNBE(d); }
  bool Encoder::SETS(const ir::Symbol& d) { return SETcc(0x98, d); }
  bool Encoder::SETNS(const ir::Symbol& d) { return SETcc(0x99, d); }
  bool Encoder::SETP(const ir::Symbol& d) { return SETcc(0x9A, d); }
  bool Encoder::SETPE(const ir::Symbol& d) { return SETP(d); }
  bool Encoder::SETNP(const ir::Symbol& d) { return SETcc(0x9B, d); }
  bool Encoder::SETPO(const ir::Symbol& d) { return SETNP(d); }
  bool Encoder::SETL(const ir::Symbol& d) { return SETcc(0x9C, d); }
  bool Encoder::SETNGE(const ir::Symbol& d) { return SETL(d); }
  bool Encoder::SETNL(const ir::Symbol& d) { return SETcc(0x9D, d); }
  bool Encoder::SETGE(const ir::Symbol& d) { return SETNL(d); }
  bool Encoder::SETLE(const ir::Symbol& d) { return SETcc(0x9E, d); }
  bool Encoder::SETNG(const ir::Symbol& d) { return SETLE(d); }
  bool Encoder::SETNLE(const ir::Symbol& d) { return SETcc(0x9F, d); }
  bool Encoder::SETG(const ir::Symbol& d) { return SETNLE(d); }

  bool Encoder::CMOVcc(uint8_t op, const ir::Symbol& d, const ir::Symbol& s) {
    return R32_RM32(op, d, s, true)
        || R64_RM64(op, d, s, true)
        || R16_RM16(op, d, s, true);
  }

  bool Encoder::CMOVO(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x40, d, s); }
  bool Encoder::CMOVNO(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x41, d, s); }
  bool Encoder::CMOVB(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x42, d, s); }
  bool Encoder::CMOVNAE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVB(d, s); }
  bool Encoder::CMOVC(const ir::Symbol& d, const ir::Symbol& s) { return CMOVB(d, s); }
  bool Encoder::CMOVNB(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x43, d, s); }
  bool Encoder::CMOVAE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVNB(d, s); }
  bool Encoder::CMOVNC(const ir::Symbol& d, const ir::Symbol& s) { return CMOVNB(d, s); }
  bool Encoder::CMOVZ(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x44, d, s); }
  bool Encoder::CMOVE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVZ(d, s); }
  bool Encoder::CMOVNZ(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x45, d, s); }
  bool Encoder::CMOVNE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVNZ(d, s); }
  bool Encoder::CMOVBE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x46, d, s); }
  bool Encoder::CMOVNA(const ir::Symbol& d, const ir::Symbol& s) { return CMOVBE(d, s); }
  bool Encoder::CMOVNBE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x47, d, s); }
  bool Encoder::CMOVA(const ir::Symbol& d, const ir::Symbol& s) { return CMOVNBE(d, s); }
  bool Encoder::CMOVS(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x48, d, s); }
  bool Encoder::CMOVNS(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x49, d, s); }
  bool Encoder::CMOVP(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x4A, d, s); }
  bool Encoder::CMOVPE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVP(d, s); }
  bool Encoder::CMOVNP(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x4B, d, s); }
  bool Encoder::CMOVPO(const ir::Symbol& d, const ir::Symbol& s) { return CMOVNP(d, s); }
  bool Encoder::CMOVL(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x4C, d, s); }
  bool Encoder::CMOVNGE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVL(d, s); }
  bool Encoder::CMOVNL(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x4D, d, s); }
  bool Encoder::CMOVGE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVNL(d, s); }
  bool Encoder::CMOVLE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x4E, d, s); }
  bool Encoder::CMOVNG(const ir::Symbol& d, const ir::Symbol& s) { return CMOVLE(d, s); }
  bool Encoder::CMOVNLE(const ir::Symbol& d, const ir::Symbol& s) { return CMOVcc(0x4F, d, s); }
  bool Encoder::CMOVG(const ir::Symbol& d, const ir::Symbol& s) { return CMOVNLE(d, s); }

  bool Encoder::X32_RM32(uint8_t p, uint8_t op, const ir::Symbol& x, const ir::Symbol& rm, bool esc = false) {
    if (!x.IsAllocated() || !x.Is32Bit() || !x.IsSIMD() || !rm.Is32Bit()) return false;

    if (p) Prefix(p);
    REX(x, rm);
    if (esc) Escape();
    OP(op);
    ModRM(x, rm);

    return true;
  }

  bool Encoder::X32_RM64(uint8_t p, uint8_t op, const ir::Symbol& x, const ir::Symbol& rm, bool esc = false) {
    if (!x.IsAllocated() || !x.Is32Bit() || !x.IsSIMD() || !rm.Is64Bit()) return false;

    if (p) Prefix(p);
    REXW(x, rm);
    if (esc) Escape();
    OP(op);
    ModRM(x, rm);

    return true;
  }

  bool Encoder::X64_RM32(uint8_t p, uint8_t op, const ir::Symbol& x, const ir::Symbol& rm, bool esc = false) {
    if (!x.IsAllocated() || !x.Is32Bit() || !x.IsSIMD() || !rm.Is32Bit()) return false;

    if (p) Prefix(p);
    REX(x, rm);
    if (esc) Escape();
    OP(op);
    ModRM(x, rm);

    return true;
  }

  bool Encoder::X64_RM64(uint8_t p, uint8_t op, const ir::Symbol& x, const ir::Symbol& rm, bool esc = false) {
    if (!x.IsAllocated() || !x.Is64Bit() || !x.IsSIMD() || !rm.Is64Bit()) return false;

    if (p) Prefix(p);
    REXW(x, rm);
    if (esc) Escape();
    OP(op);
    ModRM(x, rm);

    return true;
  }

  bool Encoder::R32_XM32(uint8_t p, uint8_t op, const ir::Symbol& r, const ir::Symbol& xm, bool esc = false) {
    if (!r.IsAllocated() || !r.Is32Bit() || !xm.IsSIMD() || !xm.Is32Bit()) return false;

    if (p) Prefix(p);
    REX(r, xm);
    if (esc) Escape();
    OP(op);
    ModRM(r, xm);

    return true;
  }

  bool Encoder::R64_XM32(uint8_t p, uint8_t op, const ir::Symbol& r, const ir::Symbol& xm, bool esc = false) {
    if (!r.IsAllocated() || !r.Is64Bit() || !xm.IsSIMD() || !xm.Is32Bit()) return false;

    if (p) Prefix(p);
    REXW(r, xm);
    if (esc) Escape();
    OP(op);
    ModRM(r, xm);

    return true;
  }

  bool Encoder::R32_XM64(uint8_t p, uint8_t op, const ir::Symbol& r, const ir::Symbol& xm, bool esc = false) {
    if (!r.IsAllocated() || !r.Is32Bit() || !xm.IsSIMD() || !xm.Is64Bit()) return false;

    if (p) Prefix(p);
    REX(r, xm);
    if (esc) Escape();
    OP(op);
    ModRM(r, xm);

    return true;
  }

  bool Encoder::R64_XM64(uint8_t p, uint8_t op, const ir::Symbol& r, const ir::Symbol& xm, bool esc = false) {
    if (!r.IsAllocated() || !r.Is64Bit() || !xm.IsSIMD() || !xm.Is64Bit()) return false;

    if (p) Prefix(p);
    REXW(r, xm);
    if (esc) Escape();
    OP(op);
    ModRM(r, xm);

    return true;
  }

  bool Encoder::X32_XM64(uint8_t p, uint8_t op, const ir::Symbol& x, const ir::Symbol& xm, bool esc = false) {
    if (!x.IsAllocated() || !x.Is32Bit() || !x.IsSIMD() || !xm.Is64Bit() || !xm.IsSIMD()) return false;

    if (p) Prefix(p);
    REX(x, xm);
    if (esc) Escape();
    OP(op);
    ModRM(x, xm);

    return true;
  }

  bool Encoder::X64_XM32(uint8_t p, uint8_t op, const ir::Symbol& x, const ir::Symbol& xm, bool esc = false) {
    if (!x.IsAllocated() || !x.Is64Bit() || !x.IsSIMD() || !xm.Is32Bit() || !xm.IsSIMD()) return false;

    if (p) Prefix(p);
    REX(x, xm);
    if (esc) Escape();
    OP(op);
    ModRM(x, xm);

    return true;
  }

  bool Encoder::X64_XM64(uint8_t p, uint8_t op, const ir::Symbol& x, const ir::Symbol& xm, bool esc = false) {
    if (!x.IsAllocated() || !x.Is64Bit() || !x.IsSIMD() || !xm.Is64Bit() || !xm.IsSIMD()) return false;

    if (p) Prefix(p);
    REX(x, xm);
    if (esc) Escape();
    OP(op);
    ModRM(x, xm);

    return true;
  }

  bool Encoder::XM64_X64(uint8_t p, uint8_t op, const ir::Symbol& xm, const ir::Symbol& x, bool esc = false) {
    return X64_XM64(p, op, x, xm, esc);
  }

  bool Encoder::X128_XM128(uint8_t p, uint8_t op, const ir::Symbol& x, const ir::Symbol& xm, bool esc = false) {
    if (!x.IsAllocated() || !x.Is128Bit() || !x.IsSIMD() || !xm.Is128Bit() || !xm.IsSIMD()) return false;

    if (p) Prefix(p);
    REX(x, xm);
    if (esc) Escape();
    OP(op);
    ModRM(x, xm);

    return true;
  }

  bool Encoder::XM128_X128(uint8_t p, uint8_t op, const ir::Symbol& xm, const ir::Symbol& x, bool esc = false) {
    return X128_XM128(p, op, x, xm, esc);
  }

  // Convert Scalar Doubleword Integer to Scalar Single-Precision FP
  bool Encoder::CVTSI2SS(const ir::Symbol& d, const ir::Symbol& s) {
    return X32_RM32(0x2A, d, s, true)
        || X32_RM64(0x2A, d, s, true);
  }

  // Convert Scalar Doubleword Integer to Scalar Double-Precision FP
  bool Encoder::CVTSI2SD(const ir::Symbol& d, const ir::Symbol& s) {
    return X64_RM32(0x2A, d, s, true)
        || X64_RM64(0x2A, d, s, true);
  }

  // Convert Scalar Single-Precision FP to Scalar Doubleword Integer
  bool Encoder::CVTSS2SI(const ir::Symbol& d, const ir::Symbol& s) {
    return R32_XM32(0x2D, d, s, true)
        || R64_XM32(0x2D, d, s, true);
  }

  // Convert Scalar Double-Precision FP to Scalar Doubleword Integer
  bool Encoder::CVTSD2SI(const ir::Symbol& d, const ir::Symbol& s) {
    return R32_XM64(0x2D, d, s, true)
        || R64_XM64(0x2D, d, s, true);
  }

  // Convert with Trunc. Scalar Single-FP Value to Doubleword Integer
  bool Encoder::CVTTSS2SI(const ir::Symbol& d, const ir::Symbol& s) {
    return R32_XM32(0x2C, d, s, true)
        || R64_XM32(0x2C, d, s, true);
  }

  // Convert with Trunc. Scalar Double-FP Value to Signed Doubleword Integer
  bool Encoder::CVTTSD2SI(const ir::Symbol& d, const ir::Symbol& s) {
    return R32_XM64(0x2C, d, s, true)
        || R64_XM64(0x2C, d, s, true);
  }

  // Convert Scalar Single-Precision Floating-Point Value to Scalar Double-Precision Floating-Point Value
  bool Encoder::CVTSS2SD(const ir::Symbol& d, const ir::Symbol& s) {
    return X64_XM32(0x5A, d, s, true);
  }

  // Convert Scalar Double-Precision Floating-Point Value to Scalar Single-Precision Floating-Point Value
  bool Encoder::CVTSD2SS(const ir::Symbol& d, const ir::Symbol& s) {
    return X32_XM64(0x5A, d, s, true);
  }
};