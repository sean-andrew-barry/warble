export module compiler.ir.Instruction;

import compiler.ir.Opcode;
import compiler.ir.Index;
import compiler.ir.Register;

import <array>;
import <cstdint>;

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
    ir::Opcode opcode;
    std::array<ir::Register, 3> registers;
    std::array<ir::Index, 3> operands;
  public:
    // Constructors/assignment – trivially copyable/movable POD-like behavior
    constexpr Instruction() noexcept = default;
    constexpr Instruction(ir::Opcode c, ir::Index d = {}, ir::Index l = {}, ir::Index r = {}) noexcept
      : opcode{c}, registers{}, operands{d, l, r} {}
    constexpr Instruction(const Instruction&) noexcept = default;
    constexpr Instruction(Instruction&&) noexcept = default;
    constexpr Instruction& operator=(const Instruction&) noexcept = default;
    constexpr Instruction& operator=(Instruction&&) noexcept = default;
    ~Instruction() = default;

    constexpr void Opcode(ir::Opcode c) noexcept { opcode = c; }
    constexpr void Register(size_t i, ir::Register r) noexcept { registers[i] = r; }
    constexpr void Registers(ir::Register r0, ir::Register r1 = {}, ir::Register r2 = {}) noexcept {
      registers[0] = r0; registers[1] = r1; registers[2] = r2;
    }
    constexpr void Dest(ir::Index v) noexcept { operands[0] = v; }
    constexpr void LHS(ir::Index v) noexcept { operands[1] = v; }
    constexpr void RHS(ir::Index v) noexcept { operands[2] = v; }
    constexpr void Operand(size_t i, ir::Index v) noexcept { operands[i] = v; }

    // --- Accessors (existing + added) ---
    constexpr bool IsJump() const noexcept { return opcode == ir::Opcode::Jump; }

    constexpr bool IsBranch() const noexcept {
      return IsJump() || opcode == ir::Opcode::Call || opcode == ir::Opcode::Return;
    }

    constexpr bool IsTerminator() const noexcept {
      return opcode == ir::Opcode::Return || opcode == ir::Opcode::Jump;
    }

    constexpr bool IsMove() const noexcept { return opcode == ir::Opcode::Move; }

    constexpr bool IsMath() const noexcept {
      return opcode >= ir::Opcode::Increment && opcode <= ir::Opcode::Negate;
    }

    constexpr ir::Opcode Opcode() const noexcept { return opcode; }
    constexpr ir::Register Reg(size_t i) const noexcept { return registers[i]; }
    constexpr ir::Index Operand(size_t i) const noexcept { return operands[i]; }
    constexpr ir::Index Dest() const noexcept { return operands[0]; }
    constexpr ir::Index LHS() const noexcept { return operands[1]; }
    constexpr ir::Index RHS() const noexcept { return operands[2]; }

    constexpr bool Is(ir::Opcode c) const noexcept { return Opcode() == c; }
  };
};

static_assert(sizeof(compiler::ir::Instruction) == 16, "ir::Instruction must remain 16 bytes");