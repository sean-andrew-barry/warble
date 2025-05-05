export module ir.instruction;

import ir.codes;
import ir.index;
import ir._register;
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

namespace ir {
  export class Instruction {
  private:
    ir::Codes code;
    std::array<ir::Register, 3> registers;
    std::array<ir::Index, 3> operands;
  public:
    constexpr bool IsJump() const noexcept {
      return code >= Codes::Jump
          && code <= Codes::JumpIfLessOrEqual;
    }

    constexpr bool IsBranch() const noexcept {
      return IsJump()
          || code == Codes::Call
          || code == Codes::Return;
    }

    constexpr bool IsTerminator() const noexcept {
      return code == Codes::Return
          || code == Codes::Jump
          || code == Codes::Break
          || code == Codes::Continue;
    }

    constexpr bool IsMove() const noexcept {
      return code == Codes::Move
          || (code >= Codes::MoveIfTrue && code <= Codes::MoveIfLessOrEqual);
    }

    constexpr bool IsMath() const noexcept {
      return code >= Codes::Increment
          && code <= Codes::Negate;
    }

    constexpr ir::Index Dest() const noexcept { return operands[0]; }
    constexpr ir::Index LHS() const noexcept { return operands[1]; }
    constexpr ir::Index RHS() const noexcept { return operands[2]; }
  };
};

static_assert(sizeof(ir::Instruction) == 16, "ir::Instruction must remain 16 bytes");