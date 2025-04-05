import code.riscv.encoder;
import code._register;
import code.address;

namespace code::riscv {
  void Encoder::CompareEqual(code::Register& res, code::Register& lhs, code::Register& rhs) {
    Allocate(lhs, rhs, res); // Allocate lhs, rhs, and res registers
    SLT(res, lhs, rhs); // Set res to 1 if lhs < rhs, otherwise set res to 0
  }

  void Encoder::CompareEqual(code::Register& res, code::Register& lhs, code::Address& rhs) {
    Allocate(lhs, res);
    LW(tempRegister, rhs); // Load the address into a temporary register
    SLT(res, lhs, tempRegister); // Set res to 1 if lhs < tempRegister, otherwise set res to 0
  }

  void Encoder::Move(code::Register& res, code::Register& lhs) {
    Allocate(res, lhs);
    MV(res, lhs); // Emit the move instruction
  }

  void Encoder::MoveIfEqual(code::Register& cond, code::Register& res, code::Register& lhs) {
    Allocate(res, lhs, cond);
    BEQ(cond, zeroRegister, 1); // Skip if cond is not zero
    MV(res, lhs);
  }
};