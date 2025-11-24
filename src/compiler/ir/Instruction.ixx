export module compiler.ir.Instruction;

import compiler.ir.Opcode;
import compiler.ir.Index;
import compiler.ir.Register;

import <array>;
import <cstdint>;
import <cstddef>;

// ──────────────────────────────────────────────────────────────
//  Warble Compiler – TAC Instruction
//  -------------------------------------------------------------
//  Compact 16‑byte POD holding one three‑address‑code operation.
//
//  The design keeps every instruction cache‑dense and trivially
//  serializable while still being expressive enough for the
//  encoder and optimizer.
// ──────────────────────────────────────────────────────────────

namespace compiler::ir {
  export class Instruction {
  private:
    Opcode opcode;
    std::array<Register, 3> registers{};
    std::array<Index, 3> operands{};
  public:
    // Constructors/assignment – trivially copyable/movable POD-like behavior
    constexpr Instruction() noexcept = default;
    constexpr Instruction(Opcode c, Index d = {}, Index l = {}, Index r = {}) noexcept
      : opcode{c}, registers{}, operands{d, l, r} {}
    constexpr Instruction(const Instruction&) noexcept = default;
    constexpr Instruction(Instruction&&) noexcept = default;
    constexpr Instruction& operator=(const Instruction&) noexcept = default;
    constexpr Instruction& operator=(Instruction&&) noexcept = default;
    ~Instruction() = default;

    constexpr void SetOpcode(Opcode value) noexcept { opcode = value; }
    constexpr void SetRegister(std::size_t index, Register value) noexcept { registers[index] = value; }
    constexpr void SetRegisters(Register r0, Register r1 = Register{}, Register r2 = Register{}) noexcept {
      registers[0] = r0;
      registers[1] = r1;
      registers[2] = r2;
    }
    constexpr void SetResult(Index value) noexcept { operands[0] = value; }
    constexpr void SetPrimary(Index value) noexcept { operands[1] = value; }
    constexpr void SetSecondary(Index value) noexcept { operands[2] = value; }
    constexpr void SetOperand(std::size_t index, Index value) noexcept { operands[index] = value; }

    // --- Accessors (existing + added) ---
    constexpr bool IsJump() const noexcept { return opcode == Opcode::Jump; }

    constexpr bool IsBranch() const noexcept {
      return IsJump() || opcode == Opcode::Call || opcode == Opcode::Return;
    }

    constexpr bool IsTerminator() const noexcept {
      return opcode == Opcode::Return || opcode == Opcode::Jump;
    }

    constexpr bool IsMove() const noexcept { return opcode == Opcode::Move; }

    constexpr bool IsMath() const noexcept {
      return opcode >= Opcode::Add && opcode <= Opcode::FusedMultiplyAdd;
    }

    constexpr compiler::ir::Opcode Opcode() const noexcept { return opcode; }
    constexpr Register Reg(std::size_t index) const noexcept { return registers[index]; }
    constexpr Index Operand(std::size_t index) const noexcept { return operands[index]; }
    constexpr Index Result() const noexcept { return operands[0]; }
    constexpr Index Primary() const noexcept { return operands[1]; }
    constexpr Index Secondary() const noexcept { return operands[2]; }

    constexpr bool Is(compiler::ir::Opcode value) const noexcept { return opcode == value; }
  };
};

static_assert(sizeof(compiler::ir::Instruction) == 16, "ir::Instruction must remain 16 bytes");