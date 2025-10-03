export module compiler.ir.Instruction;

import compiler.ir.Code;
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
    ir::Code code;
    std::array<ir::Register, 3> registers;
    std::array<ir::Index, 3> operands;
  public:
    constexpr bool IsJump() const noexcept {
      return code >= ir::Code::Jump
          && code <= ir::Code::JumpIfLessOrEqual;
    }

    constexpr bool IsBranch() const noexcept {
      return IsJump()
          || code == ir::Code::Call
          || code == ir::Code::Return;
    }

    constexpr bool IsTerminator() const noexcept {
      return code == ir::Code::Return
          || code == ir::Code::Jump
          || code == ir::Code::Break
          || code == ir::Code::Continue;
    }

    constexpr bool IsMove() const noexcept {
      return code == ir::Code::Move
          || (code >= ir::Code::MoveIfTrue && code <= ir::Code::MoveIfLessOrEqual);
    }

    constexpr bool IsMath() const noexcept {
      return code >= ir::Code::Increment
          && code <= ir::Code::Negate;
    }

    constexpr ir::Index Dest() const noexcept { return operands[0]; }
    constexpr ir::Index LHS() const noexcept { return operands[1]; }
    constexpr ir::Index RHS() const noexcept { return operands[2]; }
  };
};

static_assert(sizeof(compiler::ir::Instruction) == 16, "ir::Instruction must remain 16 bytes");