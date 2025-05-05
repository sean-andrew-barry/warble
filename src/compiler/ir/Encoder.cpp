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
};