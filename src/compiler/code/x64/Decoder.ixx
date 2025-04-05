export module code.x64.decoder;

import utility.print;
import utility.to_string;
import code.x64._register;
import code.x64.memory;
import code.x64.memory_offset;
import code.x64.immediate;
import lexical.cursor;
import <cstdint>;
import <vector>;
import <type_traits>;
import <concepts>;
import <bitset>;
import <iostream>;
import <iomanip>;
import <stdexcept>;
import <memory>;
import <span>;
import <ranges>;
import <array>;
import <string_view>;
import <variant>;
import <sstream>;

// using Operand = std::variant<
//   std::monostate,
//   GPR8,
//   GPR16,
//   GPR32,
//   GPR64,
//   EGPR8,
//   EGPR16,
//   EGPR32,
//   EGPR64,
//   XMM,
//   EXMM,
//   Memory8<GPR64, GPR64>,
//   Memory16<GPR64, GPR64>,
//   Memory32<GPR64, GPR64>,
//   Memory64<GPR64, GPR64>,
//   Memory128<GPR64, GPR64>,
//   Memory8<GPR64, EGPR64>,
//   Memory16<GPR64, EGPR64>,
//   Memory32<GPR64, EGPR64>,
//   Memory64<GPR64, EGPR64>,
//   Memory128<GPR64, EGPR64>,
//   Memory8<EGPR64, GPR64>,
//   Memory16<EGPR64, GPR64>,
//   Memory32<EGPR64, GPR64>,
//   Memory64<EGPR64, GPR64>,
//   Memory128<EGPR64, GPR64>,
//   Memory8<EGPR64, EGPR64>,
//   Memory16<EGPR64, EGPR64>,
//   Memory32<EGPR64, EGPR64>,
//   Memory64<EGPR64, EGPR64>,
//   Memory128<EGPR64, EGPR64>,
//   MemoryOffset8,
//   MemoryOffset16,
//   MemoryOffset32,
//   MemoryOffset64,
//   Imm8,
//   Imm16,
//   Imm32,
//   Imm64,
//   SignedImmediate8,
//   SignedImmediate16,
//   SignedImmediate32,
//   SignedImmediate64,
//   Rel8,
//   Relative16,
//   Relative32
// >;

namespace code::x64 {
  export class Decoder : public lexical::Cursor<std::vector<std::byte>> {
  public:
  private:
    class Instruction {
    private:
      bool locked{false};
      uint8_t segment_override{0};
      bool operand_size{false};
      bool address_size{false};
      bool narrow{false};
      bool unary{false};
      uint8_t rex{0};
      uint8_t primary_opcode{0};
      uint8_t secondary_opcode{0};
      uint8_t mod_rm{0};
      uint8_t sib{0};
      uint8_t op_count{0};
      int8_t implicit1{-1};
      int8_t implicit2{-1};
      int32_t displacement{0};
      std::span<const std::byte> operand;
      std::string_view name;
    public:
      bool Locked() const { return locked; }
      uint8_t SegmentOverride() const { return segment_override; }
      bool OperandSize() const { return operand_size; }
      bool AddressSize() const { return address_size; }
      bool Narrow() const { return narrow; }
      bool Unary() const { return unary; }
      uint8_t REX() const { return rex; }
      uint8_t PrimaryOpcode() const { return primary_opcode; }
      uint8_t SecondaryOpcode() const { return secondary_opcode; }
      uint8_t ModRM() const { return mod_rm; }
      uint8_t SIB() const { return sib; }
      int8_t Implicit1() const { return implicit1; }
      int8_t Implicit2() const { return implicit2; }
      int32_t Displacement() const { return displacement; }
      std::span<const std::byte> Operand() const { return operand; }
      std::string_view Name() const { return name; }

      void Locked(bool v) { locked = v; }
      void SegmentOverride(std::byte v) { segment_override = static_cast<uint8_t>(v); }
      void OperandSize(bool v) { operand_size = v; }
      void AddressSize(bool v) { address_size = v; }
      void Narrow(bool v) { narrow = v; }
      void Unary(bool v) { unary = v; }
      void REX(std::byte v) { rex = static_cast<uint8_t>(v); }
      void PrimaryOpcode(std::byte v) { primary_opcode = static_cast<uint8_t>(v); }
      void SecondaryOpcode(std::byte v) { secondary_opcode = static_cast<uint8_t>(v); }
      void ModRM(std::byte v) { mod_rm = static_cast<uint8_t>(v); }
      void SIB(std::byte v) { sib = static_cast<uint8_t>(v); }
      void AddOperand() { op_count += 1; }
      // void Implicit(std::byte v) { implicit = static_cast<int8_t>(v); }
      void Implicit1(IsGPR auto v) { implicit1 = static_cast<int8_t>(v); }
      void Implicit2(IsGPR auto v) { implicit2 = static_cast<int8_t>(v); }
      void Displacement(int32_t v) { displacement = v; }
      void Operand(std::span<const std::byte> v) { operand = v; }
      void Name(std::string_view v) { name = v; }

      uint8_t Mod() const { return (mod_rm >> 6) & 0x03; }
      uint8_t Reg() const { return (mod_rm >> 3) & 0x07; }
      uint8_t RM() const { return mod_rm & 0x07; }

      uint8_t Scale() const { return (sib >> 6) & 0x03; }
      uint8_t Index() const { return (sib >> 3) & 0x07; }
      uint8_t Base() const { return sib & 0x07; }

      bool UsesNoDisplacementOrSIB() const { return Mod() == 0x00; }
      bool UsesDisplacement8() const { return Mod() == 0x01; }
      bool UsesDisplacement32() const { return Mod() == 0x02; }
      bool UsesRegisterAddressing() const { return Mod() == 0x03; }
      bool IsREXB() const { return (rex & 0x01) != 0; }
      bool IsREXX() const { return (rex & 0x02) != 0; }
      bool IsREXR() const { return (rex & 0x04) != 0; }
      bool IsREXW() const { return (rex & 0x08) != 0; }

      // Check if the register is within the valid range for 8-bit registers (0-7)
      bool IsValidR8() const { return Reg() < 8; }

      // Check if the rm field is a valid 8-bit register or memory operand
      bool IsValidRM8() const { return RM() < 8; }

      bool NeedsSIB() const { return Mod() != 0b11 && RM() == 0b100; }

      std::string_view GPR8(uint8_t reg) const {
        switch (reg) {
          case 0: return "AL";
          case 1: return "CL";
          case 2: return "DL";
          case 3: return "BL";
          case 4: return "SPL";
          case 5: return "BPL";
          case 6: return "SIL";
          case 7: return "DIL";
          default: throw utility::Error("Unknown GPR8 register", std::byte{reg});
        }
      }

      std::string_view GPR16(uint8_t reg) const {
        switch (reg) {
          case 0: return "AX";
          case 1: return "CX";
          case 2: return "DX";
          case 3: return "BX";
          case 4: return "SP";
          case 5: return "BP";
          case 6: return "SI";
          case 7: return "DI";
          default: throw utility::Error("Unknown GPR16 register", std::byte{reg});
        }
      }

      std::string_view GPR32(uint8_t reg) const {
        switch (reg) {
          case 0: return "EAX";
          case 1: return "ECX";
          case 2: return "EDX";
          case 3: return "EBX";
          case 4: return "ESP";
          case 5: return "EBP";
          case 6: return "ESI";
          case 7: return "EDI";
          default: throw utility::Error("Unknown GPR32 register", std::byte{reg});
        }
      }

      std::string_view GPR64(uint8_t reg) const {
        switch (reg) {
          case 0: return "RAX";
          case 1: return "RCX";
          case 2: return "RDX";
          case 3: return "RBX";
          case 4: return "RSP";
          case 5: return "RBP";
          case 6: return "RSI";
          case 7: return "RDI";
          default: throw utility::Error("Unknown GPR64 register", std::byte{reg});
        }
      }

      std::string_view EGPR8(uint8_t reg) const {
        switch (reg) {
          case 0: return "R8B";
          case 1: return "R9B";
          case 2: return "R10B";
          case 3: return "R11B";
          case 4: return "R12B";
          case 5: return "R13B";
          case 6: return "R14B";
          case 7: return "R15B";
          default: throw utility::Error("Unknown EGPR8 register", std::byte{reg});
        }
      }

      std::string_view EGPR16(uint8_t reg) const {
        switch (reg) {
          case 0: return "R8W";
          case 1: return "R9W";
          case 2: return "R10W";
          case 3: return "R11W";
          case 4: return "R12W";
          case 5: return "R13W";
          case 6: return "R14W";
          case 7: return "R15W";
          default: throw utility::Error("Unknown EGPR16 register", std::byte{reg});
        }
      }

      std::string_view EGPR32(uint8_t reg) const {
        switch (reg) {
          case 0: return "R8D";
          case 1: return "R9D";
          case 2: return "R10D";
          case 3: return "R11D";
          case 4: return "R12D";
          case 5: return "R13D";
          case 6: return "R14D";
          case 7: return "R15D";
          default: throw utility::Error("Unknown EGPR32 register", std::byte{reg});
        }
      }

      std::string_view EGPR64(uint8_t reg) const {
        switch (reg) {
          case 0: return "R8";
          case 1: return "R9";
          case 2: return "R10";
          case 3: return "R11";
          case 4: return "R12";
          case 5: return "R13";
          case 6: return "R14";
          case 7: return "R15";
          default: throw utility::Error("Unknown EGPR64 register", std::byte{reg});
        }
      }

      std::string_view Register(uint8_t reg, bool extended) const {
        if (OperandSize()) return extended ? EGPR16(reg) : GPR16(reg);
        else if (IsREXW()) return extended ? EGPR64(reg) : GPR64(reg);
        else if (Narrow()) return extended ? EGPR8(reg) : GPR8(reg);
        else return extended ? EGPR32(reg) : GPR32(reg);
      }

      std::string ToString() const {
        std::ostringstream oss;

        // utility::Print("Instruction ToString", Name(), REXW());

        if (locked) {
          oss << "lock ";
        }

        // Append the instruction name
        oss << Name();

        // Append operands based on the ModR/M byte
        if (ModRM() != 0) {
          if (UsesRegisterAddressing()) {
            if (Unary()) {
              oss << " " << Register(RM(), IsREXB());
            }
            else {
              oss << " " << Register(RM(), IsREXB()) << ", " << Register(Reg(), IsREXR());
            }
          } else {
            oss << " " << Register(Reg(), IsREXR()) << ", [";

            if (NeedsSIB()) {
              oss << Register(Reg(), IsREXB()) << " + " << Register(Index(), IsREXX()) << " + " << Scale();
            } else {
              oss << Register(RM(), IsREXB());
            }

            if (UsesDisplacement8()) {
              oss << " + " << displacement;
            } else if (UsesDisplacement32()) {
              oss << " + " << displacement;
            }

            oss << "]";
          }
        } else if (implicit1 != -1) {
          oss << " " << Register(implicit1, IsREXB());

          if (implicit2 != -1) {
            oss << ", " << Register(implicit2, IsREXR());
          }
        }

        // Append immediate operand if present
        if (!Operand().empty()) {
          // oss << ", " + utility::ToString(Operand());
          oss << ", 0x";
          // if (operand.size() == 8) {
          //   std::array<std::byte, 8> array;
          //   std::copy(operand.begin(), operand.end(), array.begin());
          //   uint64_t value;
          //   std::memcpy(&value, array.data(), sizeof(value));
          //   oss << value;
          // }
          // else if (operand.size() == 4) {
          //   std::array<std::byte, 4> array;
          //   std::copy(operand.begin(), operand.end(), array.begin());
          //   uint32_t value;
          //   std::memcpy(&value, array.data(), sizeof(value));
          //   oss << value;
          // }
          // else if (operand.size() == 2) {
          //   std::array<std::byte, 2> array;
          //   std::copy(operand.begin(), operand.end(), array.begin());
          //   uint16_t value;
          //   std::memcpy(&value, array.data(), sizeof(value));
          //   oss << value;
          // }
          // else if (operand.size() == 1) {
          //   oss << utility::ToString(operand[0]);
          // }

          for (auto byte : Operand() | std::views::reverse) {
            // oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            oss << utility::ToString(byte);
          }
        }

        return oss.str();
      }
    };
  private:
    std::vector<Instruction> instructions;
  public:
    using Super::Super;

    const std::vector<Instruction>& Instructions() const { return instructions; }

    bool OperandSize() { return Peek() == std::byte{0x66}; }
    bool AddressSize() { return Peek() == std::byte{0x67}; }
    bool BNT() { return Peek() == std::byte{0x2E}; }
    bool BT() { return Peek() == std::byte{0x3E}; }
    bool Locked() { return Peek() == std::byte{0xF0}; }
    bool REP() { return Peek() == std::byte{0xF3}; }
    bool REPNE() { return Peek() == std::byte{0xF2}; }
    bool Escape() { return Peek() == std::byte{0x0F}; }
    
    bool Name(std::string_view name, bool lockable = false) {
      instructions.back().Name(name);

      if (!lockable && instructions.back().Locked()) {
        throw utility::Error("Instruction", instructions.back().Name(), "is flagged as locked when it isn't allowed to be");
      }

      return true;
    }

    bool Lockable() {
      if (instructions.back().Locked()) {
        throw utility::Error("Instruction", instructions.back().Name(), "is flagged as locked when it isn't allowed to be");
      }

      return true;
    }

    bool Implicit(IsGPR auto reg) {
      instructions.back().Implicit1(reg);
      return true;
    }

    bool Implicit(IsGPR auto r1, IsGPR auto r2) {
      instructions.back().Implicit1(r1);
      instructions.back().Implicit2(r2);
      return true;
    }

    bool SegmentOverride() {
      switch (static_cast<uint8_t>(Peek())) {
        case 0x2E:
        case 0x36:
        case 0x3E:
        case 0x26:
        case 0x64:
        case 0x65: return true;
        default: return false;
      }
    }

    bool REX() {
      switch (static_cast<uint8_t>(Peek())) {
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F: return true;
        default: return false;
      }
    }

    bool ModRM(const std::string_view name, bool lockable = false) {
      auto& instruction = instructions.back();
      instruction.Name(name);

      if (!lockable && instruction.Locked()) {
        throw utility::Error("Instruction", name, "is flagged as locked when it isn't allowed to be");
      }

      instruction.ModRM(Take());

      if (instruction.NeedsSIB()) {
        instruction.SIB(Take());
      }

      return true;
    }

    bool Reg(uint8_t comparison) { return instructions.back().Reg() == comparison; }

    bool Rel8() {
      instructions.back().Operand(Span(1)); // Consume a std::span<const std::byte, 1>
      return true;
    }

    bool Rel16() {
      instructions.back().Operand(Span(2)); // Consume a std::span<const std::byte, 2>
      return true;
    }

    bool Rel32() {
      instructions.back().Operand(Span(4)); // Consume a std::span<const std::byte, 4>
      return true;
    }

    bool Imm8() {
      instructions.back().Operand(Span(1)); // Consume a std::span<const std::byte, 1>
      return true;
    }

    bool Imm16() {
      auto& instruction = instructions.back();

      if (instruction.OperandSize()) {
        instruction.Operand(Span(2)); // Consume a std::span<const std::byte, 2>
        return true;
      }

      return false;
    }

    bool Imm32() {
      instructions.back().Operand(Span(4)); // Consume a std::span<const std::byte, 4>
      return true;
    }

    bool Imm64() {
      auto& instruction = instructions.back();

      if (instruction.IsREXW()) {
        instruction.Operand(Span(8)); // Consume a std::span<const std::byte, 8>
        return true;
      }

      return false;
    }

    bool Imm16_32() { return Imm16() || Imm32(); }
    bool Imm16_32_64() { return Imm16() || Imm64() || Imm32(); }

    bool RM8() {
      instructions.back().AddOperand();
      instructions.back().Narrow(true);
      return true;
    }

    bool R8() {
      instructions.back().AddOperand();
      instructions.back().Narrow(true);
      return true;
    }

    bool RM16_32_64() {
      instructions.back().AddOperand();
      return true;
    }

    bool R16_32_64() {
      instructions.back().AddOperand();
      return true;
    }

    bool AL() {
      instructions.back().AddOperand();
      return true;
    }

    bool RAX() {
      instructions.back().AddOperand();
      return true;
    }

    bool R64_16() {
      instructions.back().AddOperand();
      return true;
    }

    bool RM32() {
      instructions.back().AddOperand();
      return true;
    }

    bool R32_64() {
      instructions.back().AddOperand();
      return true;
    }

    bool M16_32_64() {
      instructions.back().AddOperand();
      return true;
    }

    bool Unary() {
      instructions.back().Unary(true);
      return true;
    }

    bool PrimaryOpcode() {
      instructions.back().PrimaryOpcode(Peek());

      // utility::Print("Decoding", Peek());

      switch (static_cast<uint8_t>(Take())) {
        case 0x0F: return SecondaryOpcode();

        case 0x00: return ModRM("ADD", true) && RM8() && R8();
        case 0x01: return ModRM("ADD", true) && RM16_32_64() && R16_32_64();
        case 0x02: return ModRM("ADD") && R8() && RM8();
        case 0x03: return ModRM("ADD") && R16_32_64() && RM16_32_64();
        case 0x04: return Name("ADD") && Implicit(code::x64::AL) && Imm8();
        case 0x05: return Name("ADD") && Implicit(code::x64::RAX) && Imm16_32();

        case 0x08: return ModRM("OR", true) && RM8() && R8();
        case 0x09: return ModRM("OR", true) && RM16_32_64() && R16_32_64();
        case 0x0A: return ModRM("OR") && R8() && RM8();
        case 0x0B: return ModRM("OR") && R16_32_64() && RM16_32_64();
        case 0x0C: return Name("OR") && Implicit(code::x64::AL) && Imm8();
        case 0x0D: return Name("OR") && Implicit(code::x64::RAX) && Imm16_32();

        case 0x10: return ModRM("ADC", true) && RM8() && R8();
        case 0x11: return ModRM("ADC", true) && RM16_32_64() && R16_32_64();
        case 0x12: return ModRM("ADC") && R8() && RM8();
        case 0x13: return ModRM("ADC") && R16_32_64() && RM16_32_64();
        case 0x14: return Name("ADC") && Implicit(code::x64::AL) && Imm8();
        case 0x15: return Name("ADC") && Implicit(code::x64::RAX) && Imm16_32();

        case 0x18: return ModRM("SBB", true) && RM8() && R8();
        case 0x19: return ModRM("SBB", true) && RM16_32_64() && R16_32_64();
        case 0x1A: return ModRM("SBB") && R8() && RM8();
        case 0x1B: return ModRM("SBB") && R16_32_64() && RM16_32_64();
        case 0x1C: return Name("SBB") && Implicit(code::x64::AL) && Imm8();
        case 0x1D: return Name("SBB") && Implicit(code::x64::RAX) && Imm16_32();

        case 0x20: return ModRM("AND", true) && RM8() && R8();
        case 0x21: return ModRM("AND", true) && RM16_32_64() && R16_32_64();
        case 0x22: return ModRM("AND") && R8() && RM8();
        case 0x23: return ModRM("AND") && R16_32_64() && RM16_32_64();
        case 0x24: return Name("AND") && Implicit(code::x64::AL) && Imm8();
        case 0x25: return Name("AND") && Implicit(code::x64::RAX) && Imm16_32();

        case 0x28: return ModRM("SUB", true) && RM8() && R8();
        case 0x29: return ModRM("SUB", true) && RM16_32_64() && R16_32_64();
        case 0x2A: return ModRM("SUB") && R8() && RM8();
        case 0x2B: return ModRM("SUB") && R16_32_64() && RM16_32_64();
        case 0x2C: return Name("SUB") && Implicit(code::x64::AL) && Imm8();
        case 0x2D: return Name("SUB") && Implicit(code::x64::RAX) && Imm16_32();

        case 0x30: return ModRM("XOR", true) && RM8() && R8();
        case 0x31: return ModRM("XOR", true) && RM16_32_64() && R16_32_64();
        case 0x32: return ModRM("XOR") && R8() && RM8();
        case 0x33: return ModRM("XOR") && R16_32_64() && RM16_32_64();
        case 0x34: return Name("XOR") && Implicit(code::x64::AL) && Imm8();
        case 0x35: return Name("XOR") && Implicit(code::x64::RAX) && Imm16_32();

        case 0x38: return ModRM("CMP", true) && RM8() && R8();
        case 0x39: return ModRM("CMP", true) && RM16_32_64() && R16_32_64();
        case 0x3A: return ModRM("CMP") && R8() && RM8();
        case 0x3B: return ModRM("CMP") && R16_32_64() && RM16_32_64();
        case 0x3C: return Name("CMP") && Implicit(code::x64::AL) && Imm8();
        case 0x3D: return Name("CMP") && Implicit(code::x64::RAX) && Imm16_32();

        case 0x50: return Name("PUSH") && Implicit(code::x64::RAX) && R64_16();
        case 0x51: return Name("PUSH") && Implicit(code::x64::RCX) && R64_16();
        case 0x52: return Name("PUSH") && Implicit(code::x64::RDX) && R64_16();
        case 0x53: return Name("PUSH") && Implicit(code::x64::RBX) && R64_16();
        case 0x54: return Name("PUSH") && Implicit(code::x64::RSP) && R64_16();
        case 0x55: return Name("PUSH") && Implicit(code::x64::RBP) && R64_16();
        case 0x56: return Name("PUSH") && Implicit(code::x64::RSI) && R64_16();
        case 0x57: return Name("PUSH") && Implicit(code::x64::RDI) && R64_16();

        case 0x58: return Name("POP") && Implicit(code::x64::RAX) && R64_16();
        case 0x59: return Name("POP") && Implicit(code::x64::RCX) && R64_16();
        case 0x5A: return Name("POP") && Implicit(code::x64::RDX) && R64_16();
        case 0x5B: return Name("POP") && Implicit(code::x64::RBX) && R64_16();
        case 0x5C: return Name("POP") && Implicit(code::x64::RSP) && R64_16();
        case 0x5D: return Name("POP") && Implicit(code::x64::RBP) && R64_16();
        case 0x5E: return Name("POP") && Implicit(code::x64::RSI) && R64_16();
        case 0x5F: return Name("POP") && Implicit(code::x64::RDI) && R64_16();

        case 0x63: return ModRM("MOVSXD") && R32_64() && RM32();
        case 0x68: return ModRM("PUSH") && Imm16_32();
        case 0x69: return ModRM("IMUL") && R16_32_64() && RM16_32_64() && Imm16_32();
        case 0x6A: return ModRM("PUSH") && Imm8();
        case 0x6B: return ModRM("IMUL") && R16_32_64() && RM16_32_64() && Imm8();

        case 0x70: return ModRM("JO") && Rel8();
        case 0x71: return ModRM("JNO") && Rel8();
        case 0x72: return ModRM("JB") && Rel8();
        case 0x73: return ModRM("JNB") && Rel8();
        case 0x74: return ModRM("JZ") && Rel8();
        case 0x75: return ModRM("JNZ") && Rel8();
        case 0x76: return ModRM("JBE") && Rel8();
        case 0x77: return ModRM("JNBE") && Rel8();
        case 0x78: return ModRM("JS") && Rel8();
        case 0x79: return ModRM("JNS") && Rel8();
        case 0x7A: return ModRM("JP") && Rel8();
        case 0x7B: return ModRM("JNP") && Rel8();
        case 0x7C: return ModRM("JL") && Rel8();
        case 0x7D: return ModRM("JNL") && Rel8();
        case 0x7E: return ModRM("JNE") && Rel8();
        case 0x7F: return ModRM("JNLE") && Rel8();

        case 0x80: return X80() && RM8() && Imm8();
        case 0x81: return X81() && RM16_32_64() && Imm16_32();
        case 0x83: return X81() && RM16_32_64() && Imm8();

        case 0x84: return ModRM("TEST") && RM8() && R8();
        case 0x85: return ModRM("TEST") && RM16_32_64() && R16_32_64();
        case 0x86: return ModRM("XCHG", true) && R8() && RM8();
        case 0x87: return ModRM("XCHG", true) && R16_32_64() && RM16_32_64();
        case 0x88: return ModRM("MOV") && RM8() && R8();
        case 0x89: return ModRM("MOV") && RM16_32_64() && R16_32_64();
        case 0x8A: return ModRM("MOV") && R8() && RM8();
        case 0x8B: return ModRM("MOV") && R16_32_64() && RM16_32_64();
        case 0x8D: return ModRM("LEA") && R16_32_64() && M16_32_64();

        case 0x8F: return ModRM("POP") && Unary() && RM16_32_64();

        case 0x90: return Name("NOP");
        // case 0x90: return Name("XCHG") && Implicit(GPR64::RAX, GPR64::RAX) && R16_32_64() && RAX();
        case 0x91: return Name("XCHG") && Implicit(code::x64::RCX, code::x64::RAX) && R16_32_64() && RAX();
        case 0x92: return Name("XCHG") && Implicit(code::x64::RDX, code::x64::RAX) && R16_32_64() && RAX();
        case 0x93: return Name("XCHG") && Implicit(code::x64::RBX, code::x64::RAX) && R16_32_64() && RAX();
        case 0x94: return Name("XCHG") && Implicit(code::x64::RSP, code::x64::RAX) && R16_32_64() && RAX();
        case 0x95: return Name("XCHG") && Implicit(code::x64::RBP, code::x64::RAX) && R16_32_64() && RAX();
        case 0x96: return Name("XCHG") && Implicit(code::x64::RSI, code::x64::RAX) && R16_32_64() && RAX();
        case 0x97: return Name("XCHG") && Implicit(code::x64::RDI, code::x64::RAX) && R16_32_64() && RAX();

        // case 0x98: return Name("CBW") && AX() && AL();

        // Opcode 0xB8 to 0xBF: MOV r16/32/64, imm16/32/64
        case 0xB8: return Name("MOV") && Implicit(code::x64::RAX) && R16_32_64() && Imm16_32_64();
        case 0xB9: return Name("MOV") && Implicit(code::x64::RCX) && R16_32_64() && Imm16_32_64();
        case 0xBA: return Name("MOV") && Implicit(code::x64::RDX) && R16_32_64() && Imm16_32_64();
        case 0xBB: return Name("MOV") && Implicit(code::x64::RBX) && R16_32_64() && Imm16_32_64();
        case 0xBC: return Name("MOV") && Implicit(code::x64::RSP) && R16_32_64() && Imm16_32_64();
        case 0xBD: return Name("MOV") && Implicit(code::x64::RBP) && R16_32_64() && Imm16_32_64();
        case 0xBE: return Name("MOV") && Implicit(code::x64::RSI) && R16_32_64() && Imm16_32_64();
        case 0xBF: return Name("MOV") && Implicit(code::x64::RDI) && R16_32_64() && Imm16_32_64();

        case 0xC2: return Name("RET") && Imm16();
        case 0xC3: return Name("RET");
        case 0xC6: return XC6();
        case 0xC7: return XC7();
        case 0xFF: return XFF();

        default: throw utility::Error("Unknown opcode", Peek(-1));
      }
    }

    bool X80() {
      instructions.back().ModRM(Take());

      switch (instructions.back().Reg()) {
        case 0x00: return Name("ADD", true);
        case 0x01: return Name("OR", true);
        case 0x02: return Name("ADC", true);
        case 0x03: return Name("SBB", true);
        case 0x04: return Name("AND", true);
        case 0x05: return Name("SUB", true);
        case 0x06: return Name("XOR", true);
        case 0x07: return Name("CMP");
        default: throw utility::Error("Invalid reg field in ModR/M byte for 0x80 family");
      }
    }

    bool X81() {
      instructions.back().ModRM(Take());

      switch (instructions.back().Reg()) {
        case 0x00: return Name("ADD", true);
        case 0x01: return Name("OR", true);
        case 0x02: return Name("ADC", true);
        case 0x03: return Name("SBB", true);
        case 0x04: return Name("AND", true);
        case 0x05: return Name("SUB", true);
        case 0x06: return Name("XOR", true);
        case 0x07: return Name("CMP");
        default: throw utility::Error("Invalid reg field in ModR/M byte for 0x81 family");
      }
    }

    bool X83() {
      instructions.back().ModRM(Take());

      switch (instructions.back().Reg()) {
        case 0x00: return Name("ADD", true);
        case 0x01: return Name("OR", true);
        case 0x02: return Name("ADC", true);
        case 0x03: return Name("SBB", true);
        case 0x04: return Name("AND", true);
        case 0x05: return Name("SUB", true);
        case 0x06: return Name("XOR", true);
        case 0x07: return Name("CMP");
        default: throw utility::Error("Invalid reg field in ModR/M byte for 0x83 family");
      }
    }

    bool XC6() {
      instructions.back().ModRM(Take());

      switch (instructions.back().Reg()) {
        case 0x00: return Name("MOV") && Unary() && RM8() && Imm8();
        default: throw utility::Error("Invalid reg field in ModR/M byte for 0xC6 family");
      }
    }

    bool XC7() {
      instructions.back().ModRM(Take());

      switch (instructions.back().Reg()) {
        case 0x00: return Name("MOV") && Unary() && RM16_32_64() && Imm16_32();
        default: throw utility::Error("Invalid reg field in ModR/M byte for 0xC7 family");
      }
    }

    bool XFF() {
      instructions.back().ModRM(Take());

      switch (instructions.back().Reg()) {
        case 0x00: return Name("INC", true) && Unary() && RM16_32_64();
        case 0x01: return Name("DEC", true) && Unary() && RM16_32_64();
        case 0x02: return Name("CALL") && Unary() && RM16_32_64();
        case 0x03: return Name("CALLF") && Unary() && M16_32_64(); // A weird one, not sure what it means
        case 0x04: return Name("JMP") && Unary() && RM16_32_64();
        case 0x05: return Name("JMPF") && Unary() && M16_32_64(); // A weird one, not sure what it means
        case 0x06: return Name("PUSH") && Unary() && RM16_32_64();
        default: throw utility::Error("Invalid reg field in ModR/M byte for 0xFF family");
      }
    }

    bool SecondaryOpcode() {
      auto& instruction = instructions.back();
      instruction.SecondaryOpcode(Peek());

      switch (static_cast<uint8_t>(Take())) {
        case 0x01: return true;
        default: throw utility::Error("Unknown secondary opcode", Peek(-1));
      }
    }

    void Decode() {
      while (!Done()) {
        // Create a new instruction
        auto& instruction = instructions.emplace_back();

        if (Locked()) {
          instruction.Locked(true);
          Advance();
        }

        while (SegmentOverride()) {
          // Currently the instruction only supports one segment override prefix
          instruction.SegmentOverride(Take());
        }

        if (OperandSize()) {
          instruction.OperandSize(true);
          Advance();
        }
        
        if (AddressSize()) {
          instruction.AddressSize(true);
          Advance();
        }

        // Test for any of the REX states
        if (REX()) {
          instruction.REX(Take());
        }

        if (!PrimaryOpcode()) {
          throw utility::Error("Failed to decode instruction");
        }
      }
    }
  };
};