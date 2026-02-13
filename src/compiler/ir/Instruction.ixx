export module compiler.ir.Instruction;

import compiler.ir.Opcode;
import compiler.ir.Symbol;
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
    std::array<ir::Symbol, 3> operands{};
    std::array<ir::Register, 3> registers{};
    Opcode opcode;
  public:
    // Constructors/assignment – trivially copyable/movable POD-like behavior
    constexpr Instruction() noexcept = default;
    constexpr Instruction(Opcode c, ir::Symbol d = {}, ir::Symbol l = {}, ir::Symbol r = {}) noexcept
      : opcode{c}, registers{}, operands{d, l, r} {}
    constexpr Instruction(const Instruction&) noexcept = default;
    constexpr Instruction(Instruction&&) noexcept = default;
    constexpr Instruction& operator=(const Instruction&) noexcept = default;
    constexpr Instruction& operator=(Instruction&&) noexcept = default;
    ~Instruction() = default;

    constexpr void SetOpcode(Opcode value) noexcept { opcode = value; }
    constexpr void SetRegister(std::size_t index, ir::Register value) noexcept { registers[index] = value; }
    constexpr void SetRegisters(ir::Register r0, ir::Register r1 = ir::Register{}, ir::Register r2 = ir::Register{}) noexcept {
      registers[0] = r0;
      registers[1] = r1;
      registers[2] = r2;
    }
    constexpr void SetResult(ir::Symbol value) noexcept { operands[0] = value; }
    constexpr void SetPrimary(ir::Symbol value) noexcept { operands[1] = value; }
    constexpr void SetSecondary(ir::Symbol value) noexcept { operands[2] = value; }
    constexpr void SetOperand(std::size_t index, ir::Symbol value) noexcept { operands[index] = value; }

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
    constexpr ir::Register Reg(std::size_t index) const noexcept { return registers[index]; }
    constexpr ir::Symbol Operand(std::size_t index) const noexcept { return operands[index]; }
    constexpr ir::Symbol Result() const noexcept { return operands[0]; }
    constexpr ir::Symbol Primary() const noexcept { return operands[1]; }
    constexpr ir::Symbol Secondary() const noexcept { return operands[2]; }

    constexpr bool Is(compiler::ir::Opcode value) const noexcept { return opcode == value; }
  };
}

static_assert(sizeof(compiler::ir::Instruction) == 16, "ir::Instruction must remain 16 bytes");