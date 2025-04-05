import code.x64.encoder;
import code.operand;
import code._register;
import code.registers;
import code.address;
import code.immediate;
import code.placeholder;
import node.block;
import node.declaration;
import utility.os;
import utility.print;
import utility.to_string;
import <iostream>;
import <vector>;
import <limits>;
import <concepts>;

template<typename T>
concept IsOp = std::derived_from<T, code::Operand>;
// concept IsOp = std::same_as<T, code::Register> || std::same_as<T, code::Address> || std::same_as<T, code::Immediate> || std::same_as<T, code::Relative>;

namespace code::x64 {
  Encoder::Encoder() : code::Encoder{std::endian::little} {}

  // float Encoder::ScoreRegister(const code::Register& reg) {
  //   switch (reg.Value()) {
  //     case rAX(): return (reg.IsArithmetic() || reg.IsLogical()) ? 1.1f : 0.9f;
  //     case rCX(): return (reg.IsPredicate() && reg.IsStep()) ? 1.1f : 0.9f; // LOOPNZ/LOOPZ/etc
  //     case rDX(): return (reg.IsMultiply() || reg.IsDivide()) ? 1.1f : 0.9f; // MUL/IMUL/DIV/IDIV
  //     case rBX(): return 1.0f;
  //     case rSP(): return 1.0f;
  //     case rBP(): return 1.0f;
  //     case rSI(): return 1.0f;
  //     case rDI(): return 1.0f;
  //     case r8(): return 1.0f;
  //     case r9(): return 1.0f;
  //     case r10(): return 1.0f;
  //     case r11(): return 1.0f;
  //     case r12(): return 1.0f;
  //     case r13(): return 1.0f;
  //     case r14(): return 1.0f;
  //     case r15(): return 1.0f;
  //     case xMM0(): return 1.0f;
  //     case xMM1(): return 1.0f;
  //     case xMM2(): return 1.0f;
  //     case xMM3(): return 1.0f;
  //     case xMM4(): return 1.0f;
  //     case xMM5(): return 1.0f;
  //     case xMM6(): return 1.0f;
  //     case xMM7(): return 1.0f;
  //     case xMM8(): return 1.0f;
  //     case xMM9(): return 1.0f;
  //     case xMM10(): return 1.0f;
  //     case xMM11(): return 1.0f;
  //     case xMM12(): return 1.0f;
  //     case xMM13(): return 1.0f;
  //     case xMM14(): return 1.0f;
  //     case xMM15(): return 1.0f;
  //     case std::numeric_limits<uint8_t>::max(): return 0.0f;
  //     default: throw utility::Error("The register", reg, "does not have a legal value");
  //   }
  // }

  void Encoder::SpillRegister(node::Declaration& declaration, code::Registers& failed) {
    // TODO
  }

  bool Encoder::SelectRegister(node::Declaration& declaration, code::Registers& failed) {
    auto& reg = declaration.Register();

    if (reg.IsSIMD()) {
      // TODO: I'm just focusing on GPRs for now
    } else {
      if (reg.IsArithmetic() && !failed.Test(rax.ID())) {
        reg.Value(rax.ID()); // Assign it to RAX
        // reg.heat += 0.1; // Increase the heat to indicate the desire to keep using RAX
        return true;
      } else {
        // Get the index of a random bit within the range that is false
        // This returns `std::numeric_limits<uint8_t>::max()` if there was no valid index
        uint8_t index = failed.Random<0, 15>();
        if (index != std::numeric_limits<uint8_t>::max()) {
          reg.Value(index);
          failed.Set(index); // Activate it so it doesn't get reused
          return true;
        }
      }
    }

    return false;
  }

  void Encoder::CollideRegister(node::Declaration& declaration, code::Registers& failed) {
    auto& reg = declaration.Register();

    // First find what declaration we are colliding with
    node::Declaration* collision{nullptr};
    for (node::Declaration* ptr : ActiveDeclarations()) {
      if (reg == ptr->Register()) {
        collision = ptr; // Found it
        break;
      }
    }

    if (!collision) {
      throw utility::Error("Failed to find the collision for a declaration, this should never happen");
    }

    // Pick which one is colder. Ties are settled in favor of the older declaration.
    if (!reg.IsHotterThan(collision->Register())) {
      collision = &declaration;
    }

    // Reset the colder of the two
    collision->Register().Reset();

    // Now allocate it again. It won't select the same register because it will be toggled in the failed set.
    AllocateRegister(*collision, failed);
  }

  void Encoder::AllocateRegister(node::Declaration& declaration, code::Registers& failed) {
    while (true) {
      if (!SelectRegister(declaration, failed)) {
        // We failed to select a register
        // This happens when every compatible register is already failed
        SpillRegister(declaration, failed);
        break;
      }

      auto& reg = declaration.Register();

      // If the chosen register is already marked as live, we have a collision that must be resolved
      if (IsRegisterActive(reg.Value())) {
        CollideRegister(declaration, failed);
      } else {
        SetRegisterActive(reg.Value());
        break; // We got a valid register, so we're done
      }
    }
  }

  void Encoder::Patch() {
    // on x86-64 we can ignore the shift entirely as address displacements are always byte-aligned

    // for (auto& address : Addresses()) {
    //   auto displacement = address.Displacement(); // Either stack relative or RIP relative

    //   if (address.IsRIP()) {

    //   } else {

    //   }
    // }
  }

  size_t Encoder::MaxSizeGPR() const { return 8; }
  size_t Encoder::MaxSizeSIMD() const { return 16; }
  size_t Encoder::TotalGPR() const { return 16; }
  size_t Encoder::TotalSIMD() const { return 16; }
  size_t Encoder::TotalRegisters() const { return TotalGPR() + TotalSIMD(); }

  // void Encoder::Declare(node::Declaration& declaration) {
  //   code::Registers failed{};
  //   failed.Set(rSP); // Always disqualify rSP as an option, it's reserved
  //   AllocateRegister(declaration, failed);
  // }

  // void Encoder::Collide(node::Block& block, node::Block& previous) {
  //   // auto& context = block.Context();
  //   // auto& registers = block.Registers();
  //   // auto& prev_registers = previous.Registers();

  //   // // Early exit if no collisions
  //   // if (!registers.Overlaps(prev_registers)) return;

  //   // // Examine each collision and resolve it if necessary
  //   // for (node::Declaration* declaration : context.LiveStack()) {
  //   //   auto& reg = declaration->Register();
  //   //   auto index = reg.Value();

  //   //   // // If there's a conflict with a register not in the previous block's declarations
  //   //   // if (prev_registers.Test(index) && !previous.Has(node)) {
  //   //   //   Assign(block, declaration);
  //   //   // }
  //   // }
  // }

  void Encoder::ConvertToF32(code::Address& dest, code::Address& src) {
    if (!dest.IsSIMD()) throw utility::Error("ConvertToF32 requires a destination register to be a SIMD register");

    if      (src.IsF64()) CVTSD2SS(dest, src); // Convert double to float
    else if (src.IsF32()) MOVAPS(dest, src);   // No conversion needed for float to float
    else if (src.IsSIMD()) throw utility::Error("ConvertToF32 cannot have a non-decimal source in a SIMD register");
    else if (src.IsI64()) CVTSI2SS(dest, src); // Convert int64 to float
    else if (src.IsI32()) CVTSI2SS(dest, src); // Convert int32 to float
    else if (src.IsI16()) CVTSI2SS(dest, src); // Convert int16 to float
    else if (src.IsI8 ()) CVTSI2SS(dest, src); // Convert int8 to float
    else if (src.IsU64()) CVTSI2SS(dest, src); // Convert uint64 to float
    else if (src.IsU32()) CVTSI2SS(dest, src); // Convert uint32 to float
    else if (src.IsU16()) CVTSI2SS(dest, src); // Convert uint16 to float
    else if (src.IsU8 ()) CVTSI2SS(dest, src); // Convert uint8 to float
  }

  void Encoder::ConvertToF64(code::Address& dest, code::Address& src) {
    if (!dest.IsSIMD()) throw utility::Error("ConvertToF64 requires a destination register to be a SIMD register");
    
    if      (src.IsF64()) MOVAPD(dest, src);   // No conversion needed for double to double
    else if (src.IsF32()) CVTSS2SD(dest, src); // Convert float to double
    else if (src.IsSIMD()) throw utility::Error("ConvertToF64 cannot have a non-decimal source in a SIMD register");
    else if (src.IsI64()) CVTSI2SD(dest, src); // Convert int64 to double
    else if (src.IsI32()) CVTSI2SD(dest, src); // Convert int32 to double
    else if (src.IsI16()) CVTSI2SD(dest, src.To32Bit()); // Convert int16 to double
    else if (src.IsI8 ()) CVTSI2SD(dest, src.To32Bit()); // Convert int8 to double
    else if (src.IsU64()) CVTSI2SD(dest, src); // Convert uint64 to double
    else if (src.IsU32()) CVTSI2SD(dest, src); // Convert uint32 to double
    else if (src.IsU16()) CVTSI2SD(dest, src.To32Bit()); // Convert uint16 to double
    else if (src.IsU8 ()) CVTSI2SD(dest, src.To32Bit()); // Convert uint8 to double
  }

  void Encoder::ConvertToI8(code::Address& dest, code::Address& src) {
    if      (src.IsF64()) CVTTSD2SI(dest, src); // Convert double to int, then truncate to int8
    else if (src.IsF32()) CVTTSS2SI(dest, src); // Convert float to int, then truncate to int8
    else if (src.IsI64()) MOV(dest, src.To8Bit()); // Truncate int64 to int8
    else if (src.IsI32()) MOV(dest, src.To8Bit()); // Truncate int32 to int8
    else if (src.IsI16()) MOV(dest, src.To8Bit()); // Truncate int16 to int8
    else if (src.IsI8 ()) MOV(dest, src);
    else if (src.IsU64()) MOV(dest, src.To8Bit()); // Truncate uint64 to int8
    else if (src.IsU32()) MOV(dest, src.To8Bit()); // Truncate uint32 to int8
    else if (src.IsU16()) MOV(dest, src.To8Bit()); // Truncate uint16 to int8
    else if (src.IsU8 ()) MOV(dest, src);
  }

  void Encoder::ConvertToI16(code::Address& dest, code::Address& src) {
    if      (src.IsF64()) CVTTSD2SI(dest, src); // Convert double to int, then truncate to int16
    else if (src.IsF32()) CVTTSS2SI(dest, src); // Convert float to int, then truncate to int16
    else if (src.IsI64()) MOV(dest, src);       // Truncate int64 to int16
    else if (src.IsI32()) MOV(dest, src);       // Truncate int32 to int16
    else if (src.IsI16()) MOV(dest, src);
    else if (src.IsI8 ()) MOVSX(dest, src);     // Sign-extend int8 to int16
    else if (src.IsU64()) MOV(dest, src);       // Truncate uint64 to int16
    else if (src.IsU32()) MOV(dest, src);       // Truncate uint32 to int16
    else if (src.IsU16()) MOV(dest, src);
    else if (src.IsU8 ()) MOVSX(dest, src);     // Sign-extend uint8 to int16
  }

  void Encoder::ConvertToI32(code::Address& dest, code::Address& src) {
    if      (src.IsF64()) CVTTSD2SI(dest, src); // Convert double to int32
    else if (src.IsF32()) CVTTSS2SI(dest, src); // Convert float to int32
    else if (src.IsI64()) MOV(dest, src);       // Truncate int64 to int32
    else if (src.IsI32()) MOV(dest, src);
    else if (src.IsI16()) MOVSX(dest, src); 
    else if (src.IsI8 ()) MOVSX(dest, src);
    else if (src.IsU64()) MOV(dest, src);       // Truncate uint64 to uint32
    else if (src.IsU32()) MOV(dest, src);
    else if (src.IsU16()) MOVZX(dest, src);
    else if (src.IsU8 ()) MOVZX(dest, src);
  }

  void Encoder::ConvertToI64(code::Address& dest, code::Address& src) {
    if      (src.IsF64()) CVTTSD2SI(dest, src); // Convert double to int64
    else if (src.IsF32()) CVTTSS2SI(dest, src); // Convert float to int64
    else if (src.IsI64()) MOV(dest, src);
    else if (src.IsI32()) MOVSXD(dest, src);
    else if (src.IsI16()) MOVSX(dest, src); 
    else if (src.IsI8 ()) MOVSX(dest, src);
    else if (src.IsU64()) MOV(dest, src);
    else if (src.IsU32()) MOVZX(dest, src);
    else if (src.IsU16()) MOVZX(dest, src);
    else if (src.IsU8 ()) MOVZX(dest, src);
  }

  void Encoder::ConvertToU8(code::Address& dest, code::Address& src) {
    if      (src.IsF64()) CVTTSD2SI(dest, src); // Convert double to int, then truncate to uint8
    else if (src.IsF32()) CVTTSS2SI(dest, src); // Convert float to int, then truncate to uint8
    else if (src.IsI64()) MOV(dest, src.To8Bit()); // Truncate int64 to uint8
    else if (src.IsI32()) MOV(dest, src.To8Bit()); // Truncate int32 to uint8
    else if (src.IsI16()) MOV(dest, src.To8Bit()); // Truncate int16 to uint8
    else if (src.IsI8 ()) MOV(dest, src);
    else if (src.IsU64()) MOV(dest, src.To8Bit()); // Truncate uint64 to uint8
    else if (src.IsU32()) MOV(dest, src.To8Bit()); // Truncate uint32 to uint8
    else if (src.IsU16()) MOV(dest, src.To8Bit()); // Truncate uint16 to uint8
    else if (src.IsU8 ()) MOV(dest, src);
  }

  void Encoder::ConvertToU16(code::Address& dest, code::Address& src) {
    if      (src.IsF64()) CVTTSD2SI(dest, src); // Convert double to int, then truncate to uint16
    else if (src.IsF32()) CVTTSS2SI(dest, src); // Convert float to int, then truncate to uint16
    else if (src.IsI64()) MOV(dest, src.To16Bit()); // Truncate int64 to uint16
    else if (src.IsI32()) MOV(dest, src.To16Bit()); // Truncate int32 to uint16
    else if (src.IsI16()) MOV(dest, src);       
    else if (src.IsI8 ()) MOVZX(dest, src); // Zero-extend int8 to uint16
    else if (src.IsU64()) MOV(dest, src.To16Bit()); // Truncate uint64 to uint16
    else if (src.IsU32()) MOV(dest, src.To16Bit()); // Truncate uint32 to uint16
    else if (src.IsU16()) MOV(dest, src);
    else if (src.IsU8 ()) MOVZX(dest, src); // Zero-extend uint8 to uint16
  }

  void Encoder::ConvertToU32(code::Address& dest, code::Address& src) {
    if      (src.IsF64()) CVTTSD2SI(dest, src); // Convert double to uint32
    else if (src.IsF32()) CVTTSS2SI(dest, src); // Convert float to uint32
    else if (src.IsI64()) MOV(dest, src.To32Bit()); // Truncate int64 to uint32
    else if (src.IsI32()) MOV(dest, src);
    else if (src.IsI16()) MOVSX(dest, src);
    else if (src.IsI8 ()) MOVSX(dest, src);
    else if (src.IsU64()) MOV(dest, src.To32Bit()); // Truncate uint64 to uint32
    else if (src.IsU32()) MOV(dest, src);
    else if (src.IsU16()) MOVZX(dest, src);
    else if (src.IsU8 ()) MOVZX(dest, src);
  }

  void Encoder::ConvertToU64(code::Address& dest, code::Address& src) {
    if      (src.IsF64()) CVTTSD2SI(dest, src); // Convert double to uint64
    else if (src.IsF32()) CVTTSS2SI(dest, src); // Convert float to uint64
    else if (src.IsI64()) MOV(dest, src);
    else if (src.IsI32()) MOV(dest, src);   // Zero-extend int32 to uint64
    else if (src.IsI16()) MOVZX(dest, src); // Zero-extend int16 to uint64
    else if (src.IsI8 ()) MOVZX(dest, src); // Zero-extend int8 to uint64
    else if (src.IsU64()) MOV(dest, src);
    else if (src.IsU32()) MOV(dest, src);   // Zero-extend uint32 to uint64
    else if (src.IsU16()) MOVZX(dest, src); // Zero-extend uint16 to uint64
    else if (src.IsU8 ()) MOVZX(dest, src); // Zero-extend uint8 to uint64
  }

  void Encoder::ByteSwap(code::Address& src) {

  }

  void Encoder::Allocate(code::Address& address) {

  }

  void Encoder::Deallocate(code::Address& address) {

  }

  // Arithmetic operations
  // res = lhs + rhs;
  void Encoder::AddIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) {
    if (res == lhs) {
      ADD(lhs, rhs);
    } else {
      MOV(res, lhs);
      ADD(res, rhs);
    }
  }

  void Encoder::AddDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs - rhs;
  void Encoder::SubtractIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) {
    if (res == lhs) {
      SUB(lhs, rhs);
    } else {
      MOV(res, lhs);
      SUB(res, rhs);
    }
  }

  void Encoder::SubtractDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs * rhs;
  void Encoder::MultiplyIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) {
    // if (res == lhs) {
    //   MUL(lhs, rhs);
    // } else {
    //   MOV(res, lhs);
    //   MUL(res, rhs);
    // }
  }

  void Encoder::MultiplySignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) {
    // if (res == lhs) {
    //   IMUL(lhs, rhs);
    // } else {
    //   MOV(res, lhs);
    //   IMUL(res, rhs);
    // }
  }

  void Encoder::MultiplyDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs / rhs;
  void Encoder::DivideIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  void Encoder::DivideSignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  void Encoder::DivideDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs % rhs;
  void Encoder::ModuloIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  void Encoder::ModuloSignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  void Encoder::ModuloDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // ++value
  void Encoder::IncrementInteger(code::Address& value) {
    INC(value);
  }

  void Encoder::IncrementDecimal(code::Address& value) {

  }

  // --value
  void Encoder::DecrementInteger(code::Address& value) {
    DEC(value);
  }

  void Encoder::DecrementDecimal(code::Address& value) {

  }

  // Bitwise operations
  // res = lhs & rhs
  void Encoder::BitwiseAnd(code::Address& res, code::Address& lhs, code::Address& rhs) {
    if (res == lhs) {
      AND(lhs, rhs);
    } else {
      MOV(res, lhs);
      AND(res, rhs);
    }
  }

  // res = lhs | rhs
  void Encoder::BitwiseOr(code::Address& res, code::Address& lhs, code::Address& rhs) {
    if (res == lhs) {
      OR(lhs, rhs);
    } else {
      MOV(res, lhs);
      OR(res, rhs);
    }
  }

  // res = lhs ^ rhs
  void Encoder::BitwiseXor(code::Address& res, code::Address& lhs, code::Address& rhs) {
    if (res == lhs) {
      XOR(lhs, rhs);
    } else {
      MOV(res, lhs);
      XOR(res, rhs);
    }
  }

  // res = lhs >> rhs;
  void Encoder::BitwiseShiftLeft(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs << rhs;
  void Encoder::BitwiseShiftRight(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = ~lhs;
  void Encoder::BitwiseNot(code::Address& res, code::Address& lhs) {

  }

  void Encoder::BitwiseTest(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // Logical operations
  // res = !lhs;
  void Encoder::LogicalNot(code::Address& res, code::Address& lhs) {

  }

  // res = lhs && rhs;
  void Encoder::LogicalAnd(code::Address& res, code::Address& lhs, code::Address& rhs) {
    if (res == lhs) {
      TEST0(lhs); // Test if lhs is 0
      code::Relative8 r1{0};
      JZ(r1); // If lhs is 0, jump to set res to 0

      TEST0(rhs); // Test if rhs is 0
      code::Relative8 r2{0};
      JZ(r2); // If rhs is 0, jump to set res to 0

      MOV1(res); // If both are true, set res to 1

      Place(r1);
      Place(r2);
    } else {
      TEST0(lhs); // Test if lhs is 0
      code::Relative8 r1{0};
      JZ(r1); // If lhs is 0, jump to set res to 0

      TEST0(rhs); // Test if rhs is 0
      code::Relative8 r2{0};
      JZ(r2); // If rhs is 0, jump to set res to 0

      MOV1(res); // If both are true, set res to 1

      code::Relative8 r3{0};
      JMP(r3); // Skip the next instruction
      Place(r1);
      Place(r2);
      MOV0(res); // Set res to 0
      Place(r3);
    }
  }

  // res = lhs || rhs;
  void Encoder::LogicalOr(code::Address& res, code::Address& lhs, code::Address& rhs) {
    if (res == lhs) {
      TEST0(lhs); // Test if lhs is 0
      code::Relative8 r1{0};
      JNZ(r1); // If lhs is non-zero, jump to set res to 1

      TEST0(rhs); // Test if rhs is 0
      SETNZ(res); // If rhs is non-zero, set res to 1
      Place(r1);
    } else {
      TEST0(lhs); // Test if lhs is 0
      code::Relative8 r1{0};
      JNZ(r1); // If lhs is non-zero, jump to set res to 1

      if (!res.Is8Bit()) {
        XOR0(res); // Set res to 0 using the XOR trick
      }

      TEST0(rhs); // Test if rhs is 0
      SETNZ(res); // If rhs is non-zero, set res to 1

      code::Relative8 r2{0};
      JMP(r2); // Skip the next instruction
      Place(r1);
      MOV1(res); // Set res to 1
      Place(r2);
    }
  }

  // Comparison operations
  // res = lhs == rhs;
  void Encoder::LogicalEqual(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs != rhs;
  void Encoder::LogicalNotEqual(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs > rhs;
  void Encoder::LogicalGreaterThan(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs < rhs;
  void Encoder::LogicalLessThan(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs >= rhs;
  void Encoder::LogicalGreaterThanOrEqual(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs <= rhs;
  void Encoder::LogicalLessThanOrEqual(code::Address& res, code::Address& lhs, code::Address& rhs) {
  }

  // Data movement
  void Encoder::Exchange(code::Address& lhs, code::Address& rhs) {
  }

  // res = lhs;
  void Encoder::Move(code::Address& res, code::Address& lhs) {
  }

  // if (cond) res = lhs;
  void Encoder::ConditionalMove(code::Address& cond, code::Address& res, code::Address& lhs) {
  }

  // Stack operations
  void Encoder::Push(code::Address& value) {
    PUSH(value);
  }

  void Encoder::Pop(code::Address& value) {
    POP(value);
  }

  // Control flow
  // goto target;
  void Encoder::Jump(code::Address& target) {
    JMP(target);
  }

  // if (cond) goto target;
  void Encoder::ConditionalJump(code::Address& cond, code::Address& target) {

  }

  // target();
  void Encoder::Call(code::Address& target) {
    CALL(target);
  }

  // if (cond) target();
  void Encoder::ConditionalCall(code::Address& cond, code::Address& target) {
  }

  // return
  void Encoder::Return() {}
  // void Return(code::Register& value) {}
  // void Return(code::Address& value) {}

  // if (cond) return;
  void Encoder::ConditionalReturn() {}
  // void ConditionalReturn(code::Register& value) {}
  // void ConditionalReturn(code::Address& value) {}

  void Encoder::Epilogue() {}
  void Encoder::Prologue() {}

  // // res = lhs | rhs
  // void Encoder::LogicalOr(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     OR(lhs, rhs);
  //   } else {
  //     OR(res, lhs);
  //     OR(res, rhs);
  //   }
  // }

  // // res = lhs ^ rhs
  // void Encoder::LogicalXor(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     XOR(lhs, rhs);
  //   } else {
  //     XOR(res, lhs);
  //     XOR(res, rhs);
  //   }
  // }

  // // res = ~src
  // void Encoder::LogicalNot(code::Register& res, code::Register& src) {
  //   if (res == src) {
  //     NOT(src);
  //   } else {
  //     MOV(res, src);
  //     NOT(res);
  //   }
  // }

  // // !!a implementation
  // void Encoder::True(code::Register& res, code::Register& src) {
  //   if (res == src) {
  //     // Do nothing
  //   } else {
  //     MOV(res, src); // just copy src to res
  //   }
  // }

  // // res = !src
  // void Encoder::False(code::Register& res, code::Register& src) {
  //   if (res == src) {
  //     CMP(src, code::Immediate8::ZERO); // Compare src with 0
  //     SETE(res); // Set res to 1 if src is 0, otherwise set to 0
  //   } else {
  //     CMP(src, code::Immediate8::ZERO); // Compare src with 0
  //     MOV(res, code::Immediate8::ZERO); // Set res to 0 initially
  //     SETE(res); // Set res to 1 if src is 0
  //   }
  // }

  // // res = lhs && rhs
  // void Encoder::TrueAndTrue(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     TEST(lhs, lhs); // Compare lhs with 0
  //     code::Relative8 r1{0};
  //     JZ(r1); // If lhs is 0, jump to set res to 0

  //     TEST(rhs, rhs); // Compare rhs with 0
  //     code::Relative8 r2{0};
  //     JZ(r2); // If rhs is 0, jump to set res to 0

  //     MOV(res, code::Immediate8::ONE); // If both are true, set res to 1

  //     Label(r1);
  //     Label(r2);
  //   } else {
  //     // General path when res != lhs
  //     TEST(lhs, lhs); // Compare lhs with 0
  //     code::Relative8 r1{0};
  //     JZ(r1); // If lhs is 0, jump to set res to 0

  //     TEST(rhs, rhs); // Compare rhs with 0
  //     code::Relative8 r2{0};
  //     JZ(r2); // If rhs is 0, jump to set res to 0

  //     MOV(res, code::Immediate8::ONE); // If both are true, set res to 1

  //     code::Relative8 r3{0};
  //     JMP(r3); // Skip the next instruction
  //     Label(r1);
  //     Label(r2);
  //     MOV(res, code::Immediate8::ZERO); // Set res to 0
  //     Label(r3);
  //   }
  // }

  // // a && !b implementation
  // void Encoder::TrueAndFalse(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     TEST(lhs, lhs); // Compare lhs with 0
  //     code::Relative8 r1{0};
  //     JZ(r1); // If lhs is 0, jump to set res to 0

  //     TEST(rhs, rhs); // Compare rhs with 0
  //     code::Relative8 r2{0};
  //     JNZ(r2); // If rhs is not 0, jump to set res to 0

  //     MOV(res, code::Immediate8::ONE); // If lhs is true and rhs is false, set res to 1

  //     Label(r1);
  //     Label(r2);
  //   } else {
  //     // General path when res != lhs
  //     TEST(lhs, lhs); // Compare lhs with 0
  //     code::Relative8 r1{0};
  //     JZ(r1); // If lhs is 0, jump to set res to 0

  //     TEST(rhs, rhs); // Compare rhs with 0
  //     code::Relative8 r2{0};
  //     JNZ(r2); // If rhs is not 0, jump to set res to 0

  //     MOV(res, code::Immediate8::ONE); // If lhs is true and rhs is false, set res to 1

  //     code::Relative8 r3{0};
  //     JMP(r3); // Skip the next instruction
  //     Label(r1);
  //     Label(r2);
  //     MOV(res, code::Immediate8::ZERO); // Set res to 0
  //     Label(r3);
  //   }
  // }

  // void Encoder::FalseAndTrue(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   // // !a && b implementation
  //   // CMP lhs, 0;
  //   // JNE false_label;
  //   // CMP rhs, 0;
  //   // JE false_label;
  //   // MOV res, 1;
  //   // JMP end_label;

  //   // false_label:
  //   // MOV res, 0;
  //   // end_label:
  // }

  // void Encoder::FalseAndFalse(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   // // !a && !b implementation
  //   // CMP lhs, 0;
  //   // JNE false_label;
  //   // CMP rhs, 0;
  //   // JNE false_label;
  //   // MOV res, 1;
  //   // JMP end_label;

  //   // false_label:
  //   // MOV res, 0;
  //   // end_label:
  // }

  // // a || b implementation
  // void Encoder::TrueOrTrue(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     TEST(lhs, lhs); // Compare lhs with 0
  //     code::Relative8 r1{0};
  //     JNZ(r1); // If lhs is non-zero, jump to set res to 1

  //     TEST(rhs, rhs); // Compare rhs with 0
  //     SETNZ(res); // If rhs is non-zero, set res to 1

  //     Label(r1);
  //   } else {
  //     TEST(lhs, lhs); // Compare lhs with 0
  //     code::Relative8 r1{0};
  //     JNZ(r1); // If lhs is non-zero, jump to set res to 1

  //     TEST(rhs, rhs); // Compare rhs with 0
  //     MOV(res, code::Immediate8::ZERO); // Set res to 0 initially
  //     SETNZ(res); // If rhs is non-zero, set res to 1

  //     code::Relative8 r2{0};
  //     JMP(r2); // Skip the next instruction
  //     Label(r1);
  //     MOV(res, code::Immediate8::ONE); // Set res to 1
  //     Label(r2);
  //   }
  // }

  // void Encoder::TrueOrFalse(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   // // a || !b implementation
  //   // CMP lhs, 0;
  //   // JNE true_label;
  //   // CMP rhs, 0;
  //   // JE true_label;
  //   // MOV res, 0;
  //   // JMP end_label;

  //   // true_label:
  //   // MOV res, 1;
  //   // end_label:
  // }

  // void Encoder::FalseOrTrue(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   // // !a || b implementation
  //   // CMP lhs, 0;
  //   // JE true_label;
  //   // CMP rhs, 0;
  //   // JNE true_label;
  //   // MOV res, 0;
  //   // JMP end_label;

  //   // true_label:
  //   // MOV res, 1;
  //   // end_label:
  // }

  // void Encoder::FalseOrFalse(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   // // !a || !b implementation
  //   // CMP lhs, 0;
  //   // JE true_label;
  //   // CMP rhs, 0;
  //   // JE true_label;
  //   // MOV res, 0;
  //   // JMP end_label;

  //   // true_label:
  //   // MOV res, 1;
  //   // end_label:
  // }

  // // a + b implementation
  // void Encoder::Add(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (lhs.IsFloating() || rhs.IsFloating()) {
  //     if (!lhs.IsFloating()) {
  //       CVT(temp_sse, lhs);
  //       return Add(res, temp_sse, rhs);
  //     } else if (!rhs.IsFloating()) {
  //       CVT(temp_sse, rhs);
  //       return Add(res, lhs, temp_sse);
  //     }
  //   }

  //   if (lhs.IsFloating() && rhs.IsFloating()) {
  //     if (encoder.Same(res, lhs)) {
  //       if (res.IsDouble()) {
  //         ADDPD(res, rhs);
  //       } else {
  //         ADDPS(res, rhs);
  //       }
  //     } else {
  //       if (res.IsDouble()) {
  //         MOVAPD(res, lhs); // Move lhs to res
  //         ADDPD(res, rhs); // Add the value of rhs to the temp (packed double-precision)
  //       } else {
  //         MOVAPS(res, lhs); // Move lhs to res
  //         ADDPS(res, rhs); // Add the value of rhs to the temp (packed single-precision)
  //       }
  //     }
  //   } else {
  //     if (encoder.Same(res, lhs)) {
  //       ADD(res, rhs); // Directly add rhs to res
  //     } else {
  //       MOV(res, lhs); // Move lhs to res
  //       ADD(res, rhs); // Add rhs to res
  //     }
  //   }
  // }

  // void Encoder::Add(code::Register& res, code::Register& lhs, code::Address& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     ADD(res, rhs); // Add rhs to res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     ADD(res, rhs); // Add rhs to res
  //   }
  // }

  // void Encoder::Add(code::Register& res, code::Address& lhs, code::Register& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // void Encoder::Add(code::Register& res, code::Address& lhs, code::Address& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // void Encoder::Add(code::Address& res, code::Register& lhs, code::Register& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // void Encoder::Add(code::Address& res, code::Register& lhs, code::Address& rhs) {
  //   if (lhs.IsFloating() && rhs.IsFloating()) {
  //     // Floating-point addition
  //     MOVAPS(temp_sse, rhs); // Move rhs to the temp SSE register
  //     ADDPS(temp_sse, lhs); // Add the value of lhs to the temp (packed single-precision)
  //     MOVAPS(res, temp_sse); // Move the temp to the result
  //   } else {
  //     // Integer addition (signed and unsigned)
  //     MOV(temp_gpr, rhs); // Move rhs to the temp GPR register
  //     ADD(temp_gpr, lhs); // Add the value of lhs to the temp
  //     MOV(res, temp_gpr); // Move the temp to the result
  //   }
  // }

  // void Encoder::Add(code::Address& res, code::Address& lhs, code::Register& rhs) {
  //   if (rhs.IsFloating() && lhs.IsFloating()) {
  //     if (encoder.Same(res, lhs)) {
  //       ADDPS(res, rhs); // Directly add rhs to res (packed single-precision)
  //     } else {
  //       MOVAPS(temp_sse, lhs); // Move lhs to the temp SSE register
  //       ADDPS(temp_sse, rhs); // Add the value of rhs to the temp (packed single-precision)
  //       MOVAPS(res, temp_sse); // Move the temp to the result
  //     }
  //   } else {
  //     // Integer addition (signed and unsigned)
  //     if (encoder.Same(res, lhs)) {
  //       ADD(res, rhs); // Directly add rhs to res
  //     } else {
  //       MOV(temp_gpr, lhs); // Move lhs to the temp GPR register
  //       ADD(temp_gpr, rhs); // Add the value of rhs to the temp
  //       MOV(res, temp_gpr); // Move the temp to the result
  //     }
  //   }
  // }

  // void Encoder::Add(code::Register& res, code::Register& lhs, code::Address& rhs) {
  //   if (lhs.IsFloating() && rhs.IsFloating()) {
  //     // Floating-point addition
  //     if (encoder.Same(res, lhs)) {
  //       ADDPS(res, rhs); // Directly add rhs to res (packed single-precision)
  //     } else {
  //       MOVAPS(temp_sse, lhs); // Move lhs to the temp SSE register
  //       ADDPS(temp_sse, rhs); // Add the value of rhs to the temp (packed single-precision)
  //       MOVAPS(res, temp_sse); // Move the temp to the result
  //     }
  //   } else {
  //     // Integer addition (signed and unsigned)
  //     if (encoder.Same(res, lhs)) {
  //       ADD(res, rhs); // Directly add rhs to res
  //     } else {
  //       MOV(temp_gpr, lhs); // Move lhs to the temp GPR register
  //       ADD(temp_gpr, rhs); // Add the value of rhs to the temp
  //       MOV(res, temp_gpr); // Move the temp to the result
  //     }
  //   }
  // }

  // void Encoder::Add(code::Register& res, code::Address& lhs, code::Register& rhs) {
  //   if (rhs.IsFloating() && lhs.IsFloating()) {
  //     // Floating-point addition
  //     MOVAPS(temp_sse, lhs); // Move lhs to the temp SSE register
  //     ADDPS(temp_sse, rhs); // Add the value of rhs to the temp (packed single-precision)
  //     MOVAPS(res, temp_sse); // Move the temp to the result
  //   } else {
  //     // Integer addition (signed and unsigned)
  //     MOV(temp_gpr, lhs); // Move lhs to the temp GPR register
  //     ADD(temp_gpr, rhs); // Add the value of rhs to the temp
  //     MOV(res, temp_gpr); // Move the temp to the result
  //   }
  // }

  // void Encoder::Add(code::Register& res, code::Address& lhs, code::Address& rhs) {
  //   if (lhs.IsFloating() && rhs.IsFloating()) {
  //     // Floating-point addition
  //     MOVAPS(temp_sse, lhs); // Move lhs to the temp SSE register
  //     ADDPS(temp_sse, rhs); // Add the value of rhs to the temp (packed single-precision)
  //     MOVAPS(res, temp_sse); // Move the temp to the result
  //   } else {
  //     // Integer addition (signed and unsigned)
  //     MOV(temp_gpr, lhs); // Move lhs to the temp GPR register
  //     ADD(temp_gpr, rhs); // Add the value of rhs to the temp
  //     MOV(res, temp_gpr); // Move the temp to the result
  //   }
  // }

  // // a - b implementation
  // void Encoder::Subtract(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     SUB(res, rhs); // Subtract rhs from res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     SUB(res, rhs); // Subtract rhs from res
  //   }
  // }
  
  // void Encoder::Subtract(code::Register& res, code::Register& lhs, code::Address& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     SUB(res, rhs); // Subtract rhs from res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     SUB(res, rhs); // Subtract rhs from res
  //   }
  // }

  // void Encoder::Subtract(code::Register& res, code::Address& lhs, code::Register& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   SUB(res, rhs); // Subtract rhs from res
  // }

  // void Encoder::Subtract(code::Register& res, code::Address& lhs, code::Address& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   SUB(res, rhs); // Subtract rhs from res
  // }

  // // a * b implementation
  // void Encoder::Multiply(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     ADD(res, rhs); // Add rhs to res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     ADD(res, rhs); // Add rhs to res
  //   }
  // }
  
  // void Encoder::Multiply(code::Register& res, code::Register& lhs, code::Address& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     ADD(res, rhs); // Add rhs to res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     ADD(res, rhs); // Add rhs to res
  //   }
  // }

  // void Encoder::Multiply(code::Register& res, code::Address& lhs, code::Register& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // void Encoder::Multiply(code::Register& res, code::Address& lhs, code::Address& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // // a / b implementation
  // void Encoder::Divide(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     ADD(res, rhs); // Add rhs to res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     ADD(res, rhs); // Add rhs to res
  //   }
  // }
  
  // void Encoder::Divide(code::Register& res, code::Register& lhs, code::Address& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     ADD(res, rhs); // Add rhs to res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     ADD(res, rhs); // Add rhs to res
  //   }
  // }

  // void Encoder::Divide(code::Register& res, code::Address& lhs, code::Register& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // void Encoder::Divide(code::Register& res, code::Address& lhs, code::Address& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // // a % b implementation
  // void Encoder::Modulo(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     ADD(res, rhs); // Add rhs to res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     ADD(res, rhs); // Add rhs to res
  //   }
  // }
  
  // void Encoder::Modulo(code::Register& res, code::Register& lhs, code::Address& rhs) {
  //   if (encoder.Same(res, lhs)) {
  //     ADD(res, rhs); // Add rhs to res directly
  //   } else {
  //     MOV(res, lhs); // Move lhs to res
  //     ADD(res, rhs); // Add rhs to res
  //   }
  // }

  // void Encoder::Modulo(code::Register& res, code::Address& lhs, code::Register& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // void Encoder::Modulo(code::Register& res, code::Address& lhs, code::Address& rhs) {
  //   MOV(res, lhs); // Move lhs to res
  //   ADD(res, rhs); // Add rhs to res
  // }

  // void Encoder::CompareHelper(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   SetCMP(res); // Setup the flags

  //   if (lhs.IsFloating()) {
  //     // TODO: SIMD versions?
  //   } else {
  //     if (lhs.Is64Bit()) CMP_R64_R64(res, lhs);
  //     else if (lhs.Is32Bit()) CMP_R32_R32(res, lhs);
  //     else if (lhs.Is16Bit()) CMP_R16_R16(res, lhs);
  //     else if (lhs.Is8Bit()) CMP_R8_R8(res, lhs);
  //   }
  // }
  
  // void Encoder::CompareEqual(code::Register& res, code::Register& lhs, code::Register& rhs) {
  //   // CompareHelper(res, lhs, rhs);
  // }
  
  // void Encoder::CompareNotEqual(code::Register& res, code::Register& lhs, code::Register& rhs) { CompareHelper(res, lhs, rhs); }
  
  // void Encoder::CompareGreaterThan(code::Register& res, code::Register& lhs, code::Register& rhs) { CompareHelper(res, lhs, rhs); }
  
  // void Encoder::CompareLessThan(code::Register& res, code::Register& lhs, code::Register& rhs) { CompareHelper(res, lhs, rhs); }
  
  // void Encoder::CompareGreaterThanOrEqual(code::Register& res, code::Register& lhs, code::Register& rhs) { CompareHelper(res, lhs, rhs); }
  
  // void Encoder::CompareLessThanOrEqual(code::Register& res, code::Register& lhs, code::Register& rhs) { CompareHelper(res, lhs, rhs); }

  // void Encoder::MoveIfEqual(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   TestZF(cond); // Ensure the condition was setup

  //   if (lhs.IsFloating()) {
  //     // TODO: SIMD versions?
  //   }

  //   if (lhs.Is64Bit()) CMOVZ_R64_R64(res, lhs);
  //   else if (lhs.Is32Bit()) CMOVZ_R32_R32(res, lhs);
  //   else if (lhs.Is16Bit()) CMOVZ_R16_R16(res, lhs);
  //   // else if (lhs.Is8Bit()) CMOVZ_R8_R8(res, lhs);
  // }

  // void Encoder::MoveIfNotEqual(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVNZ_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfCarry(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVB_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfNotCarry(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVNB_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfNegative(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVS_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfPositive(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVNS_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfOverflow(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVO_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfNotOverflow(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVNO_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfGreaterThanOrEqual(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVNL_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfGreaterThan(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVNLE_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfLessThanOrEqual(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVLE_R64_R64(res, lhs);
  // }

  // void Encoder::MoveIfLessThan(code::Register& cond, code::Register& res, code::Register& lhs) {
  //   CMOVL_R64_R64(res, lhs);
  // }

  // void Encoder::Add(code::Register& a, code::Register& b) {}

  // void Encoder::Add(code::Register& a, code::Address& b) {
  //   // if (b.Unresolved()) Placeholder([=](){ Add(a, b); });

  //   // code::Immediate i{42};
  //   // ADD_RAX_IMM32(i);

  //   // if (a.Is8Bit()) a.IsLocked() ? ADD_M8_R8_LOCK(b, a) : ADD_M8_R8(b, a);
  //   // else if (a.Is16Bit()) a.IsLocked() ? ADD_M16_R16_LOCK(b, a) : ADD_M16_R16(b, a);
  //   // else if (a.Is32Bit()) a.IsLocked() ? ADD_M32_R32_LOCK(b, a) : ADD_M32_R32(b, a);
  //   // else if (a.Is64Bit()) a.IsLocked() ? ADD_M64_R64_LOCK(b, a) : ADD_M64_R64(b, a);
  // }

  // void Encoder::Add(code::Address& a, code::Register& b) {}
  // void Encoder::Add(code::Address& a, code::Address& b) {}

  // void Encoder::Subtract(code::Register& a, code::Register& b) {}
  // void Encoder::Subtract(code::Register& a, code::Address& b) {}
  // void Encoder::Subtract(code::Address& a, code::Register& b) {}
  // void Encoder::Subtract(code::Address& a, code::Address& b) {}

  // void Encoder::Multiply(code::Register& a, code::Register& b) {}
  // void Encoder::Multiply(code::Register& a, code::Address& b) {}
  // void Encoder::Multiply(code::Address& a, code::Register& b) {}
  // void Encoder::Multiply(code::Address& a, code::Address& b) {}

  // void Encoder::Divide(code::Register& a, code::Register& b) {}
  // void Encoder::Divide(code::Register& a, code::Address& b) {}
  // void Encoder::Divide(code::Address& a, code::Register& b) {}
  // void Encoder::Divide(code::Address& a, code::Address& b) {}

  // void Encoder::And(code::Register& a, code::Register& b) {}
  // void Encoder::And(code::Register& a, code::Address& b) {}
  // void Encoder::And(code::Address& a, code::Register& b) {}
  // void Encoder::And(code::Address& a, code::Address& b) {}

  // void Encoder::Or(code::Register& a, code::Register& b) {}
  // void Encoder::Or(code::Register& a, code::Address& b) {}
  // void Encoder::Or(code::Address& a, code::Register& b) {}
  // void Encoder::Or(code::Address& a, code::Address& b) {}

  // void Encoder::Xor(code::Register& a, code::Register& b) {}
  // void Encoder::Xor(code::Register& a, code::Address& b) {}
  // void Encoder::Xor(code::Address& a, code::Register& b) {}
  // void Encoder::Xor(code::Address& a, code::Address& b) {}

  // void Encoder::Not(code::Register& a) {}
  // void Encoder::Not(code::Address& a) {}

  // void Encoder::Load(code::Register& a, code::Address& b) {}
  // void Encoder::Load(code::Address& a, code::Register& b) {}

  // void Encoder::Store(code::Address& a, code::Register& b) {}
  // void Encoder::Store(code::Register& a, code::Address& b) {}

  // void Encoder::Move(code::Register& res, code::Register& lhs) {
  //   if (lhs.Is64Bit()) MOV_R64_R64(res, lhs);
  //   else if (lhs.Is32Bit()) MOV_R32_R32(res, lhs);
  //   else if (lhs.Is16Bit()) MOV_R16_R16(res, lhs);
  //   else if (lhs.Is8Bit()) MOV_R8_R8(res, lhs);
  // }

  // void Encoder::Move(code::Register& a, code::Address& b) {}
  // void Encoder::Move(code::Address& a, code::Register& b) {}
  // void Encoder::Move(code::Address& a, code::Address& b) {}

  // void Encoder::Push(code::Register& a) {}
  // void Encoder::Push(code::Address& a) {}

  // void Encoder::Pop(code::Register& a) {}
  // void Encoder::Pop(code::Address& a) {}

  // void Encoder::Jump(code::Address& target) {}
  // void Encoder::Jump(code::Register& target) {}

  // void Encoder::Call(code::Address& target) {}
  // void Encoder::Call(code::Register& target) {}

  // void Encoder::Return() { RET(); }

  // void Encoder::BranchIfEqual(code::Register& a, code::Register& b, code::Address& target) {}
  // void Encoder::BranchIfNotEqual(code::Register& a, code::Register& b, code::Address& target) {}
  // void Encoder::BranchIfLessThan(code::Register& a, code::Register& b, code::Address& target) {}
  // void Encoder::BranchIfGreaterThan(code::Register& a, code::Register& b, code::Address& target) {}
  // void Encoder::BranchIfLessOrEqual(code::Register& a, code::Register& b, code::Address& target) {}
  // void Encoder::BranchIfGreaterOrEqual(code::Register& a, code::Register& b, code::Address& target) {}

  // void Encoder::ShiftLeft(code::Register& a, code::Register& b) {}
  // void Encoder::ShiftLeft(code::Register& a, code::Address& b) {}

  // void Encoder::ShiftRight(code::Register& a, code::Register& b) {}
  // void Encoder::ShiftRight(code::Register& a, code::Address& b) {}

  // void Encoder::Emit8(const Immediate8& i) { code::Encoder::Emit8(i.Value()); }
  // void Encoder::Emit8(const code::Immediate& i) { code::Encoder::Emit8(i.Value()); }
  // void Encoder::Emit16(const Immediate16& i) { code::Encoder::Emit16(i.Value()); }
  // void Encoder::Emit16(const code::Immediate& i) { code::Encoder::Emit16(i.Value()); }
  // void Encoder::Emit32(const Immediate32& i) { code::Encoder::Emit32(i.Value()); }
  // void Encoder::Emit32(const code::Immediate& i) { code::Encoder::Emit32(i.Value()); }
  // void Encoder::Emit64(const Immediate64& i) { code::Encoder::Emit64(i.Value()); }
  // void Encoder::Emit64(const code::Immediate& i) { code::Encoder::Emit64(i.Value()); }

  void Encoder::SIMD() { Instructions().Emit8(0x66); }
  void Encoder::OpSize() { Instructions().Emit8(0x66); }
  void Encoder::AdSize() { Instructions().Emit8(0x67); }
  void Encoder::Single() { Instructions().Emit8(0xF3); } // Single precision floating point prefix in SSE
  void Encoder::Double() { Instructions().Emit8(0xF2); } // Double precision floating point prefix in SSE
  void Encoder::CS() { Instructions().Emit8(0x2E); }
  void Encoder::SS() { Instructions().Emit8(0x36); }
  void Encoder::DS() { Instructions().Emit8(0x3E); }
  void Encoder::ES() { Instructions().Emit8(0x26); }
  void Encoder::FS() { Instructions().Emit8(0x64); }
  void Encoder::GS() { Instructions().Emit8(0x65); }
  void Encoder::BNT() { Instructions().Emit8(0x2E); } // Branch not taken, same code as CS
  void Encoder::BT() { Instructions().Emit8(0x3E); } // Branch taken, same code as DS
  void Encoder::Lock() { Instructions().Emit8(0xF0); }
  void Encoder::REP() { Instructions().Emit8(0xF3); } // Repeat while equal
  void Encoder::REPNE() { Instructions().Emit8(0xF2); } // Repeat while not equal
  void Encoder::Escape() { Instructions().Emit8(0x0F); }
  void Encoder::Escape(uint8_t opcode) { Instructions().Emit8(0x0F); Instructions().Emit8(opcode); }
  void Encoder::Esc() { Instructions().Emit8(0x0F); }
  void Encoder::Esc(uint8_t opcode) { Instructions().Emit8(0x0F); Instructions().Emit8(opcode); }

  void Encoder::REX() { Instructions().Emit8(0x40); }
  void Encoder::REXB() { Instructions().Emit8(0x41); }
  void Encoder::REXX() { Instructions().Emit8(0x42); }
  void Encoder::REXXB() { Instructions().Emit8(0x43); }
  void Encoder::REXR() { Instructions().Emit8(0x44); }
  void Encoder::REXRB() { Instructions().Emit8(0x45); }
  void Encoder::REXRX() { Instructions().Emit8(0x46); }
  void Encoder::REXRXB() { Instructions().Emit8(0x47); }
  void Encoder::REXW() { Instructions().Emit8(0x48); }
  void Encoder::REXWB() { Instructions().Emit8(0x49); }
  void Encoder::REXWX() { Instructions().Emit8(0x4A); }
  void Encoder::REXWXB() { Instructions().Emit8(0x4B); }
  void Encoder::REXWR() { Instructions().Emit8(0x4C); }
  void Encoder::REXWRB() { Instructions().Emit8(0x4D); }
  void Encoder::REXWRX() { Instructions().Emit8(0x4E); }
  void Encoder::REXWRXB() { Instructions().Emit8(0x4F); }

  void Encoder::OP(uint8_t primary_opcode) { Instructions().Emit8(primary_opcode); }
  void Encoder::SO(uint8_t secondary_opcode) { Instructions().Emit8(secondary_opcode); }

  void Encoder::Displacement(int32_t displacement, uint8_t mod, bool is_bp) {
    // Emit displacement based on mod bits and base register
    if (mod == 0x80) { // 32-bit displacement
      Instructions().Emit32(displacement);
    } else if (mod == 0x40) { // 8-bit displacement
      Instructions().Emit8(static_cast<int8_t>(displacement));
    } else if (is_bp) { // Special case for BP with no displacement
      Instructions().Emit32(displacement);
    }
  }

  // void Encoder::REX(SGPR8 reg, SGPR64 index, SGPR64 base) { REX(); }
  // void Encoder::REX(EGPR8 reg, SGPR64 index, SGPR64 base) { REXR(); }
  // void Encoder::REX(SGPR8 reg, SGPR64 index, EGPR64 base) { REXB(); }
  // void Encoder::REX(SGPR8 reg, EGPR64 index, SGPR64 base) { REXX(); }
  // void Encoder::REX(SGPR8 reg, EGPR64 index, EGPR64 base) { REXXB(); }
  // void Encoder::REX(EGPR8 reg, SGPR64 index, EGPR64 base) { REXRB(); }
  // void Encoder::REX(EGPR8 reg, EGPR64 index, SGPR64 base) { REXRX(); }
  // void Encoder::REX(EGPR8 reg, EGPR64 index, EGPR64 base) { REXRXB(); }

  // void Encoder::REX(SGPR16 reg, SGPR64 index, SGPR64 base) { REX(); }
  // void Encoder::REX(EGPR16 reg, SGPR64 index, SGPR64 base) { REXR(); }
  // void Encoder::REX(SGPR16 reg, SGPR64 index, EGPR64 base) { REXB(); }
  // void Encoder::REX(SGPR16 reg, EGPR64 index, SGPR64 base) { REXX(); }
  // void Encoder::REX(SGPR16 reg, EGPR64 index, EGPR64 base) { REXXB(); }
  // void Encoder::REX(EGPR16 reg, SGPR64 index, EGPR64 base) { REXRB(); }
  // void Encoder::REX(EGPR16 reg, EGPR64 index, SGPR64 base) { REXRX(); }
  // void Encoder::REX(EGPR16 reg, EGPR64 index, EGPR64 base) { REXRXB(); }

  // void Encoder::REX(SGPR32 reg, SGPR64 index, SGPR64 base) { REX(); }
  // void Encoder::REX(EGPR32 reg, SGPR64 index, SGPR64 base) { REXR(); }
  // void Encoder::REX(SGPR32 reg, SGPR64 index, EGPR64 base) { REXB(); }
  // void Encoder::REX(SGPR32 reg, EGPR64 index, SGPR64 base) { REXX(); }
  // void Encoder::REX(SGPR32 reg, EGPR64 index, EGPR64 base) { REXXB(); }
  // void Encoder::REX(EGPR32 reg, SGPR64 index, EGPR64 base) { REXRB(); }
  // void Encoder::REX(EGPR32 reg, EGPR64 index, SGPR64 base) { REXRX(); }
  // void Encoder::REX(EGPR32 reg, EGPR64 index, EGPR64 base) { REXRXB(); }

  // void Encoder::REX(SGPR64 reg, SGPR64 index, SGPR64 base) { REXW(); }
  // void Encoder::REX(EGPR64 reg, SGPR64 index, SGPR64 base) { REXWR(); }
  // void Encoder::REX(SGPR64 reg, SGPR64 index, EGPR64 base) { REXWB(); }
  // void Encoder::REX(SGPR64 reg, EGPR64 index, SGPR64 base) { REXWX(); }
  // void Encoder::REX(SGPR64 reg, EGPR64 index, EGPR64 base) { REXWXB(); }
  // void Encoder::REX(EGPR64 reg, SGPR64 index, EGPR64 base) { REXWRB(); }
  // void Encoder::REX(EGPR64 reg, EGPR64 index, SGPR64 base) { REXWRX(); }
  // void Encoder::REX(EGPR64 reg, EGPR64 index, EGPR64 base) { REXWRXB(); }

  // // REX Handlers for two registers
  // void Encoder::REX(SGPR64 reg, SGPR64 base) { REXW(); }
  // void Encoder::REX(SGPR64 reg, EGPR64 base) { REXWR(); }
  // void Encoder::REX(EGPR64 reg, SGPR64 base) { REXWB(); }
  // void Encoder::REX(EGPR64 reg, EGPR64 base) { REXWRB(); }

  // // REX Handlers for one register
  // void Encoder::REX(SGPR64 reg) { REXW(); }
  // void Encoder::REX(EGPR64 reg) { REXWB(); }

  // // REX Handlers for 32-bit registers
  // void Encoder::REX(SGPR32 reg, SGPR32 index, SGPR32 base) { /* No REX needed */ }
  // void Encoder::REX(EGPR32 reg, SGPR32 index, SGPR32 base) { REXR(); }
  // void Encoder::REX(SGPR32 reg, SGPR32 index, EGPR32 base) { REXB(); }
  // void Encoder::REX(SGPR32 reg, EGPR32 index, SGPR32 base) { REXX(); }
  // void Encoder::REX(SGPR32 reg, EGPR32 index, EGPR32 base) { REXXB(); }
  // void Encoder::REX(EGPR32 reg, SGPR32 index, EGPR32 base) { REXRB(); }
  // void Encoder::REX(EGPR32 reg, EGPR32 index, SGPR32 base) { REXRX(); }
  // void Encoder::REX(EGPR32 reg, EGPR32 index, EGPR32 base) { REXRXB(); }

  // // REX Handlers for two registers
  // void Encoder::REX(SGPR32 reg, SGPR32 base) { /* No REX needed */ }
  // void Encoder::REX(SGPR32 reg, EGPR32 base) { REXR(); }
  // void Encoder::REX(EGPR32 reg, SGPR32 base) { REXB(); }
  // void Encoder::REX(EGPR32 reg, EGPR32 base) { REXRB(); }

  // // REX Handlers for one register
  // void Encoder::REX(SGPR32 reg) { /* No REX needed */ }
  // void Encoder::REX(EGPR32 reg) { REXB(); }

  // // REX Handlers for 16-bit registers
  // void Encoder::REX(SGPR16 reg, SGPR16 index, SGPR16 base) { /* No REX needed */ }
  // void Encoder::REX(EGPR16 reg, SGPR16 index, SGPR16 base) { REXR(); }
  // void Encoder::REX(SGPR16 reg, SGPR16 index, EGPR16 base) { REXB(); }
  // void Encoder::REX(SGPR16 reg, EGPR16 index, SGPR16 base) { REXX(); }
  // void Encoder::REX(SGPR16 reg, EGPR16 index, EGPR16 base) { REXXB(); }
  // void Encoder::REX(EGPR16 reg, SGPR16 index, EGPR16 base) { REXRB(); }
  // void Encoder::REX(EGPR16 reg, EGPR16 index, SGPR16 base) { REXRX(); }
  // void Encoder::REX(EGPR16 reg, EGPR16 index, EGPR16 base) { REXRXB(); }

  // // REX Handlers for two registers
  // void Encoder::REX(SGPR16 reg, SGPR16 base) { /* No REX needed */ }
  // void Encoder::REX(SGPR16 reg, EGPR16 base) { REXR(); }
  // void Encoder::REX(EGPR16 reg, SGPR16 base) { REXB(); }
  // void Encoder::REX(EGPR16 reg, EGPR16 base) { REXRB(); }

  // // REX Handlers for one register
  // void Encoder::REX(SGPR16 reg) { /* No REX needed */ }
  // void Encoder::REX(EGPR16 reg) { REXB(); }

  // // REX Handlers for 8-bit registers
  // void Encoder::REX(SGPR8 reg, SGPR8 index, SGPR8 base) { /* No REX needed */ }
  // void Encoder::REX(EGPR8 reg, SGPR8 index, SGPR8 base) { REXR(); }
  // void Encoder::REX(SGPR8 reg, SGPR8 index, EGPR8 base) { REXB(); }
  // void Encoder::REX(SGPR8 reg, EGPR8 index, SGPR8 base) { REXX(); }
  // void Encoder::REX(SGPR8 reg, EGPR8 index, EGPR8 base) { REXXB(); }
  // void Encoder::REX(EGPR8 reg, SGPR8 index, EGPR8 base) { REXRB(); }
  // void Encoder::REX(EGPR8 reg, EGPR8 index, SGPR8 base) { REXRX(); }
  // void Encoder::REX(EGPR8 reg, EGPR8 index, EGPR8 base) { REXRXB(); }

  // // REX Handlers for two registers
  // void Encoder::REX(SGPR8 reg, SGPR8 base) { /* No REX needed */ }
  // void Encoder::REX(SGPR8 reg, EGPR8 base) { REXR(); }
  // void Encoder::REX(EGPR8 reg, SGPR8 base) { REXB(); }
  // void Encoder::REX(EGPR8 reg, EGPR8 base) { REXRB(); }

  // // REX Handlers for one register
  // void Encoder::REX(SGPR8 reg) { /* No REX needed */ }
  // void Encoder::REX(EGPR8 reg) { REXB(); }

  // // REX Handlers for three registers (XMM and GPR combinations)
  // void Encoder::REX(SSSE128 reg, SGPR64 index, SGPR64 base) { /* No REX needed */ }
  // void Encoder::REX(ESSE128 reg, SGPR64 index, SGPR64 base) { REXR(); }
  // void Encoder::REX(SSSE128 reg, SGPR64 index, EGPR64 base) { REXB(); }
  // void Encoder::REX(SSSE128 reg, EGPR64 index, SGPR64 base) { REXX(); }
  // void Encoder::REX(SSSE128 reg, EGPR64 index, EGPR64 base) { REXXB(); }
  // void Encoder::REX(ESSE128 reg, SGPR64 index, EGPR64 base) { REXRB(); }
  // void Encoder::REX(ESSE128 reg, EGPR64 index, SGPR64 base) { REXRX(); }
  // void Encoder::REX(ESSE128 reg, EGPR64 index, EGPR64 base) { REXRXB(); }

  // // REX Handlers for two registers (XMM and GPR combinations)
  // void Encoder::REX(SSSE128 reg, SGPR64 base) { /* No REX needed */ }
  // void Encoder::REX(SSSE128 reg, EGPR64 base) { REXB(); }
  // void Encoder::REX(ESSE128 reg, SGPR64 base) { REXR(); }
  // void Encoder::REX(ESSE128 reg, EGPR64 base) { REXRB(); }

  // void Encoder::REX(SSSE128 reg, SGPR32 base) { /* No REX needed */ }
  // void Encoder::REX(SSSE128 reg, EGPR32 base) { REXB(); }
  // void Encoder::REX(ESSE128 reg, SGPR32 base) { REXR(); }
  // void Encoder::REX(ESSE128 reg, EGPR32 base) { REXRB(); }

  // void Encoder::REX(SSSE128 reg, SGPR16 base) { /* No REX needed */ }
  // void Encoder::REX(SSSE128 reg, EGPR16 base) { REXB(); }
  // void Encoder::REX(ESSE128 reg, SGPR16 base) { REXR(); }
  // void Encoder::REX(ESSE128 reg, EGPR16 base) { REXRB(); }

  // void Encoder::REX(SSSE128 reg, SGPR8 base) { /* No REX needed */ }
  // void Encoder::REX(SSSE128 reg, EGPR8 base) { REXB(); }
  // void Encoder::REX(ESSE128 reg, SGPR8 base) { REXR(); }
  // void Encoder::REX(ESSE128 reg, EGPR8 base) { REXRB(); }

  // // REX Handlers for one register (XMM)
  // void Encoder::REX(SSSE128 reg) { /* No REX needed */ }
  // void Encoder::REX(ESSE128 reg) { REXB(); }

  // // Opcode 0x1C: SBB AL, imm8
  // void Encoder::SBB_AL_IMM8(Immediate8 i) { OP(0x1C); Instructions().Emit8(i); }

  // // Opcode 0x19: SBB r/m16/32/64, r16/32/64
  // void Encoder::SBB_R16_R16(SGPR16 r1, SGPR16 r2) { OpSize(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R32_R32(SGPR32 r1, SGPR32 r2) { REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R64_R64(SGPR64 r1, SGPR64 r2) { REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R16_R16(SGPR16 r1, EGPR16 r2) { OpSize(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R32_R32(SGPR32 r1, EGPR32 r2) { REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R64_R64(SGPR64 r1, EGPR64 r2) { REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R16_R16(EGPR16 r1, SGPR16 r2) { OpSize(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R32_R32(EGPR32 r1, SGPR32 r2) { REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R64_R64(EGPR64 r1, SGPR64 r2) { REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R16_R16(EGPR16 r1, EGPR16 r2) { OpSize(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R32_R32(EGPR32 r1, EGPR32 r2) { REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R64_R64(EGPR64 r1, EGPR64 r2) { REX(r1, r2); OP(0x19); ModRM(r1, r2); }

  // void Encoder::SBB_R16_R16_LOCK(SGPR16 r1, SGPR16 r2) { Lock(); OpSize(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R32_R32_LOCK(SGPR32 r1, SGPR32 r2) { Lock(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R64_R64_LOCK(SGPR64 r1, SGPR64 r2) { Lock(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R16_R16_LOCK(SGPR16 r1, EGPR16 r2) { Lock(); OpSize(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R32_R32_LOCK(SGPR32 r1, EGPR32 r2) { Lock(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R64_R64_LOCK(SGPR64 r1, EGPR64 r2) { Lock(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R16_R16_LOCK(EGPR16 r1, SGPR16 r2) { Lock(); OpSize(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R32_R32_LOCK(EGPR32 r1, SGPR32 r2) { Lock(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R64_R64_LOCK(EGPR64 r1, SGPR64 r2) { Lock(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R16_R16_LOCK(EGPR16 r1, EGPR16 r2) { Lock(); OpSize(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R32_R32_LOCK(EGPR32 r1, EGPR32 r2) { Lock(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }
  // void Encoder::SBB_R64_R64_LOCK(EGPR64 r1, EGPR64 r2) { Lock(); REX(r1, r2); OP(0x19); ModRM(r1, r2); }

  // // Opcode 0x1D: SBB rAX, imm16/32
  // void Encoder::SBB_AX_IMM16(Immediate16 i) { OpSize(); OP(0x1D); Instructions().Emit16(i); }
  // void Encoder::SBB_EAX_IMM32(Immediate32 i) { OP(0x1D); Instructions().Emit32(i); }
  // void Encoder::SBB_RAX_IMM32(Immediate32 i) { REXW(); OP(0x1D); Instructions().Emit32(i); }

  // void Encoder::SBB_R16_IMM8(SGPR16 r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }
  // void Encoder::SBB_R32_IMM8(SGPR32 r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }
  // void Encoder::SBB_R64_IMM8(SGPR64 r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }
  // void Encoder::SBB_R16_IMM8(EGPR16 r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }
  // void Encoder::SBB_R32_IMM8(EGPR32 r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }
  // void Encoder::SBB_R64_IMM8(EGPR64 r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }
  // void Encoder::SBB_R16_IMM8_LOCK(EGPR16 r, Immediate8 i) { Lock(); OpSize(); REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }
  // void Encoder::SBB_R32_IMM8_LOCK(EGPR32 r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }
  // void Encoder::SBB_R64_IMM8_LOCK(EGPR64 r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x03, r); Instructions().Emit8(i); }

  // void Encoder::SBB_R16_IMM16(SGPR16 r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit16(i); }
  // void Encoder::SBB_R32_IMM32(SGPR32 r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit32(i); }
  // void Encoder::SBB_R64_IMM32(SGPR64 r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit32(i); }
  // void Encoder::SBB_R16_IMM16(EGPR16 r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit16(i); }
  // void Encoder::SBB_R32_IMM32(EGPR32 r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit32(i); }
  // void Encoder::SBB_R64_IMM32(EGPR64 r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit32(i); }
  // void Encoder::SBB_R16_IMM16_LOCK(EGPR16 r, Immediate16 i) { Lock(); OpSize(); REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit16(i); }
  // void Encoder::SBB_R32_IMM32_LOCK(EGPR32 r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit32(i); }
  // void Encoder::SBB_R64_IMM32_LOCK(EGPR64 r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x03, r); Instructions().Emit32(i); }

  // // Opcode 0x21: AND r/m16/32/64, r16/32/64
  // void Encoder::AND_R16_R16(SGPR16 r1, SGPR16 r2) { OpSize(); REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R32_R32(SGPR32 r1, SGPR32 r2) { REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R64_R64(SGPR64 r1, SGPR64 r2) { REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R16_R16(SGPR16 r1, EGPR16 r2) { OpSize(); REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R32_R32(SGPR32 r1, EGPR32 r2) { REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R64_R64(SGPR64 r1, EGPR64 r2) { REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R16_R16(EGPR16 r1, SGPR16 r2) { OpSize(); REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R32_R32(EGPR32 r1, SGPR32 r2) { REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R64_R64(EGPR64 r1, SGPR64 r2) { REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R16_R16(EGPR16 r1, EGPR16 r2) { OpSize(); REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R32_R32(EGPR32 r1, EGPR32 r2) { REX(r1, r2); OP(0x21); ModRM(r1, r2); }
  // void Encoder::AND_R64_R64(EGPR64 r1, EGPR64 r2) { REX(r1, r2); OP(0x21); ModRM(r1, r2); }

  // // 16-bit Versions
  // void Encoder::XCHG_AX_AX() { OpSize(); OP(0x90); } // Alias of NOP
  // void Encoder::XCHG_CX_AX() { OpSize(); OP(0x91); }
  // void Encoder::XCHG_DX_AX() { OpSize(); OP(0x92); }
  // void Encoder::XCHG_BX_AX() { OpSize(); OP(0x93); }
  // void Encoder::XCHG_SP_AX() { OpSize(); OP(0x94); }
  // void Encoder::XCHG_BP_AX() { OpSize(); OP(0x95); }
  // void Encoder::XCHG_SI_AX() { OpSize(); OP(0x96); }
  // void Encoder::XCHG_DI_AX() { OpSize(); OP(0x97); }
  // void Encoder::XCHG_R8W_AX() { REXB(); OpSize(); OP(0x90); }
  // void Encoder::XCHG_R9W_AX() { REXB(); OpSize(); OP(0x91); }
  // void Encoder::XCHG_R10W_AX() { REXB(); OpSize(); OP(0x92); }
  // void Encoder::XCHG_R11W_AX() { REXB(); OpSize(); OP(0x93); }
  // void Encoder::XCHG_R12W_AX() { REXB(); OpSize(); OP(0x94); }
  // void Encoder::XCHG_R13W_AX() { REXB(); OpSize(); OP(0x95); }
  // void Encoder::XCHG_R14W_AX() { REXB(); OpSize(); OP(0x96); }
  // void Encoder::XCHG_R15W_AX() { REXB(); OpSize(); OP(0x97); }

  // // 32-bit Versions
  // void Encoder::XCHG_EAX_EAX() { OP(0x90); } // Alias of NOP
  // void Encoder::XCHG_ECX_EAX() { OP(0x91); }
  // void Encoder::XCHG_EDX_EAX() { OP(0x92); }
  // void Encoder::XCHG_EBX_EAX() { OP(0x93); }
  // void Encoder::XCHG_ESP_EAX() { OP(0x94); }
  // void Encoder::XCHG_EBP_EAX() { OP(0x95); }
  // void Encoder::XCHG_ESI_EAX() { OP(0x96); }
  // void Encoder::XCHG_EDI_EAX() { OP(0x97); }
  // void Encoder::XCHG_R8D_EAX() { REXB(); OP(0x90); }
  // void Encoder::XCHG_R9D_EAX() { REXB(); OP(0x91); }
  // void Encoder::XCHG_R10D_EAX() { REXB(); OP(0x92); }
  // void Encoder::XCHG_R11D_EAX() { REXB(); OP(0x93); }
  // void Encoder::XCHG_R12D_EAX() { REXB(); OP(0x94); }
  // void Encoder::XCHG_R13D_EAX() { REXB(); OP(0x95); }
  // void Encoder::XCHG_R14D_EAX() { REXB(); OP(0x96); }
  // void Encoder::XCHG_R15D_EAX() { REXB(); OP(0x97); }

  // // 64-bit Versions
  // void Encoder::XCHG_RAX_RAX() { REXW(); OP(0x90); } // Alias of NOP
  // void Encoder::XCHG_RCX_RAX() { REXW(); OP(0x91); }
  // void Encoder::XCHG_RDX_RAX() { REXW(); OP(0x92); }
  // void Encoder::XCHG_RBX_RAX() { REXW(); OP(0x93); }
  // void Encoder::XCHG_RSP_RAX() { REXW(); OP(0x94); }
  // void Encoder::XCHG_RBP_RAX() { REXW(); OP(0x95); }
  // void Encoder::XCHG_RSI_RAX() { REXW(); OP(0x96); }
  // void Encoder::XCHG_RDI_RAX() { REXW(); OP(0x97); }
  // void Encoder::XCHG_R8_RAX() { REXWB(); OP(0x90); }
  // void Encoder::XCHG_R9_RAX() { REXWB(); OP(0x91); }
  // void Encoder::XCHG_R10_RAX() { REXWB(); OP(0x92); }
  // void Encoder::XCHG_R11_RAX() { REXWB(); OP(0x93); }
  // void Encoder::XCHG_R12_RAX() { REXWB(); OP(0x94); }
  // void Encoder::XCHG_R13_RAX() { REXWB(); OP(0x95); }
  // void Encoder::XCHG_R14_RAX() { REXWB(); OP(0x96); }
  // void Encoder::XCHG_R15_RAX() { REXWB(); OP(0x97); }

  // void Encoder::PAUSE() { Emit8(0xF3); OP(0x90); }

  // void Encoder::CBW() { OpSize(); OP(0x98); }
  // void Encoder::CWDE() { OP(0x98); }
  // void Encoder::CDQE() { REXW(); OP(0x98); }

  // void Encoder::CWD() { OpSize(); OP(0x99); }
  // void Encoder::CDQ() { OP(0x99); }
  // void Encoder::CQO() { REXW(); OP(0x99); }

  // // PUSH rAX, AX, rCX, CX, rDX, DX, rBX, BX, rSP, SP, rBP, BP, rSI, SI, rDI, DI
  // void Encoder::PUSH_R16(SGPR16 r) { OpSize(); REX(r); OP(0x50 + r.Value()); }
  // void Encoder::PUSH_R16(EGPR16 r) { OpSize(); REX(r); OP(0x50 + r.Value()); }
  // void Encoder::PUSH_AX() { OpSize(); OP(0x50); }
  // void Encoder::PUSH_CX() { OpSize(); OP(0x51); }
  // void Encoder::PUSH_DX() { OpSize(); OP(0x52); }
  // void Encoder::PUSH_BX() { OpSize(); OP(0x53); }
  // void Encoder::PUSH_SP() { OpSize(); OP(0x54); }
  // void Encoder::PUSH_BP() { OpSize(); OP(0x55); }
  // void Encoder::PUSH_SI() { OpSize(); OP(0x56); }
  // void Encoder::PUSH_DI() { OpSize(); OP(0x57); }

  // void Encoder::PUSH_R8W() { OpSize(); REXB(); OP(0x50); }
  // void Encoder::PUSH_R9W() { OpSize(); REXB(); OP(0x51); }
  // void Encoder::PUSH_R10W() { OpSize(); REXB(); OP(0x52); }
  // void Encoder::PUSH_R11W() { OpSize(); REXB(); OP(0x53); }
  // void Encoder::PUSH_R12W() { OpSize(); REXB(); OP(0x54); }
  // void Encoder::PUSH_R13W() { OpSize(); REXB(); OP(0x55); }
  // void Encoder::PUSH_R14W() { OpSize(); REXB(); OP(0x56); }
  // void Encoder::PUSH_R15W() { OpSize(); REXB(); OP(0x57); }

  // void Encoder::PUSH_R64(SGPR64 r) { REX(r); OP(0x50 + r.Value()); }
  // void Encoder::PUSH_R64(EGPR64 r) { REX(r); OP(0x50 + r.Value()); }
  // void Encoder::PUSH_RAX() { OP(0x50); }
  // void Encoder::PUSH_RCX() { OP(0x51); }
  // void Encoder::PUSH_RDX() { OP(0x52); }
  // void Encoder::PUSH_RBX() { OP(0x53); }
  // void Encoder::PUSH_RSP() { OP(0x54); }
  // void Encoder::PUSH_RBP() { OP(0x55); }
  // void Encoder::PUSH_RSI() { OP(0x56); }
  // void Encoder::PUSH_RDI() { OP(0x57); }

  // void Encoder::PUSH_R8() { REXB(); OP(0x50); }
  // void Encoder::PUSH_R9() { REXB(); OP(0x51); }
  // void Encoder::PUSH_R10() { REXB(); OP(0x52); }
  // void Encoder::PUSH_R11() { REXB(); OP(0x53); }
  // void Encoder::PUSH_R12() { REXB(); OP(0x54); }
  // void Encoder::PUSH_R13() { REXB(); OP(0x55); }
  // void Encoder::PUSH_R14() { REXB(); OP(0x56); }
  // void Encoder::PUSH_R15() { REXB(); OP(0x57); }

  // // PUSH imm16, imm32, imm8
  // void Encoder::PUSH_IMM16(Immediate16 i) { OpSize(); OP(0x68); Instructions().Emit16(i); }
  // void Encoder::PUSH_IMM32(Immediate32 i) { OP(0x68); Instructions().Emit32(i); }
  // void Encoder::PUSH_IMM8(Immediate8 i) { OP(0x6A); Instructions().Emit8(i); }

  // // POP rAX, AX, rCX, CX, rDX, DX, rBX, BX, rSP, SP, rBP, BP, rSI, SI, rDI, DI
  // void Encoder::POP_AX() { OpSize(); OP(0x58); }
  // void Encoder::POP_CX() { OpSize(); OP(0x59); }
  // void Encoder::POP_DX() { OpSize(); OP(0x5A); }
  // void Encoder::POP_BX() { OpSize(); OP(0x5B); }
  // void Encoder::POP_SP() { OpSize(); OP(0x5C); }
  // void Encoder::POP_BP() { OpSize(); OP(0x5D); }
  // void Encoder::POP_SI() { OpSize(); OP(0x5E); }
  // void Encoder::POP_DI() { OpSize(); OP(0x5F); }

  // void Encoder::POP_R8W() { OpSize(); REXB(); OP(0x58); }
  // void Encoder::POP_R9W() { OpSize(); REXB(); OP(0x59); }
  // void Encoder::POP_R10W() { OpSize(); REXB(); OP(0x5A); }
  // void Encoder::POP_R11W() { OpSize(); REXB(); OP(0x5B); }
  // void Encoder::POP_R12W() { OpSize(); REXB(); OP(0x5C); }
  // void Encoder::POP_R13W() { OpSize(); REXB(); OP(0x5D); }
  // void Encoder::POP_R14W() { OpSize(); REXB(); OP(0x5E); }
  // void Encoder::POP_R15W() { OpSize(); REXB(); OP(0x5F); }

  // void Encoder::POP_RAX() { OP(0x58); }
  // void Encoder::POP_RCX() { OP(0x59); }
  // void Encoder::POP_RDX() { OP(0x5A); }
  // void Encoder::POP_RBX() { OP(0x5B); }
  // void Encoder::POP_RSP() { OP(0x5C); }
  // void Encoder::POP_RBP() { OP(0x5D); }
  // void Encoder::POP_RSI() { OP(0x5E); }
  // void Encoder::POP_RDI() { OP(0x5F); }

  // void Encoder::POP_R8() { REXB(); OP(0x58); }
  // void Encoder::POP_R9() { REXB(); OP(0x59); }
  // void Encoder::POP_R10() { REXB(); OP(0x5A); }
  // void Encoder::POP_R11() { REXB(); OP(0x5B); }
  // void Encoder::POP_R12() { REXB(); OP(0x5C); }
  // void Encoder::POP_R13() { REXB(); OP(0x5D); }
  // void Encoder::POP_R14() { REXB(); OP(0x5E); }
  // void Encoder::POP_R15() { REXB(); OP(0x5F); }

  // // code::Placeholder8 Encoder::JO_REL8() { JO_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JO_REL16() { JO_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JO_REL32() { JO_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JNO_REL8() { JNO_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JNO_REL16() { JNO_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JNO_REL32() { JNO_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JB_REL8() { JB_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JB_REL16() { JB_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JB_REL32() { JB_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JNB_REL8() { JNB_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JNB_REL16() { JNB_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JNB_REL32() { JNB_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JZ_REL8() { JZ_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JZ_REL16() { JZ_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JZ_REL32() { JZ_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JNZ_REL8() { JNZ_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JNZ_REL16() { JNZ_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JNZ_REL32() { JNZ_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JBE_REL8() { JBE_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JBE_REL16() { JBE_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JBE_REL32() { JBE_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JNBE_REL8() { JNBE_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JNBE_REL16() { JNBE_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JNBE_REL32() { JNBE_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JS_REL8() { JS_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JS_REL16() { JS_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JS_REL32() { JS_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JNS_REL8() { JNS_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JNS_REL16() { JNS_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JNS_REL32() { JNS_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JP_REL8() { JP_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JP_REL16() { JP_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JP_REL32() { JP_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JNP_REL8() { JNP_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JNP_REL16() { JNP_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JNP_REL32() { JNP_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JL_REL8() { JL_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JL_REL16() { JL_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JL_REL32() { JL_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JNL_REL8() { JNL_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JNL_REL16() { JNL_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JNL_REL32() { JNL_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JLE_REL8() { JLE_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JLE_REL16() { JLE_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JLE_REL32() { JLE_REL32(0); return PH32(6, 0); }
  // // code::Placeholder8 Encoder::JNLE_REL8() { JNLE_REL8(0); return PH8(2, 0); }
  // // code::Placeholder16 Encoder::JNLE_REL16() { JNLE_REL16(0); return PH16(5, 0); }
  // // code::Placeholder32 Encoder::JNLE_REL32() { JNLE_REL32(0); return PH32(6, 0); }

  // // Opcode 0xB0 to 0xB7: MOV r8, imm8
  // void Encoder::MOV_AL_IMM8(Immediate8 i) { OP(0xB0); Instructions().Emit8(i); }
  // void Encoder::MOV_CL_IMM8(Immediate8 i) { OP(0xB1); Instructions().Emit8(i); }
  // void Encoder::MOV_DL_IMM8(Immediate8 i) { OP(0xB2); Instructions().Emit8(i); }
  // void Encoder::MOV_BL_IMM8(Immediate8 i) { OP(0xB3); Instructions().Emit8(i); }
  // void Encoder::MOV_SPL_IMM8(Immediate8 i) { OP(0xB4); Instructions().Emit8(i); }
  // void Encoder::MOV_BPL_IMM8(Immediate8 i) { OP(0xB5); Instructions().Emit8(i); }
  // void Encoder::MOV_SIL_IMM8(Immediate8 i) { OP(0xB6); Instructions().Emit8(i); }
  // void Encoder::MOV_DIL_IMM8(Immediate8 i) { OP(0xB7); Instructions().Emit8(i); }

  // void Encoder::MOV_R8B_IMM8(Immediate8 i) { REXB(); OP(0xB0); Instructions().Emit8(i); }
  // void Encoder::MOV_R9B_IMM8(Immediate8 i) { REXB(); OP(0xB1); Instructions().Emit8(i); }
  // void Encoder::MOV_R10B_IMM8(Immediate8 i) { REXB(); OP(0xB2); Instructions().Emit8(i); }
  // void Encoder::MOV_R11B_IMM8(Immediate8 i) { REXB(); OP(0xB3); Instructions().Emit8(i); }
  // void Encoder::MOV_R12B_IMM8(Immediate8 i) { REXB(); OP(0xB4); Instructions().Emit8(i); }
  // void Encoder::MOV_R13B_IMM8(Immediate8 i) { REXB(); OP(0xB5); Instructions().Emit8(i); }
  // void Encoder::MOV_R14B_IMM8(Immediate8 i) { REXB(); OP(0xB6); Instructions().Emit8(i); }
  // void Encoder::MOV_R15B_IMM8(Immediate8 i) { REXB(); OP(0xB7); Instructions().Emit8(i); }

  // void Encoder::MOV_AX_IMM16(Immediate16 i) { OpSize(); OP(0xB8); Instructions().Emit16(i); }
  // void Encoder::MOV_EAX_IMM32(Immediate32 i) { OP(0xB8); Instructions().Emit32(i); }
  // void Encoder::MOV_RAX_IMM64(Immediate64 i) { REXW(); OP(0xB8); Instructions().Emit64(i); }
  // void Encoder::MOV_CX_IMM16(Immediate16 i) { OpSize(); OP(0xB9); Instructions().Emit16(i); }
  // void Encoder::MOV_ECX_IMM32(Immediate32 i) { OP(0xB9); Instructions().Emit32(i); }
  // void Encoder::MOV_RCX_IMM64(Immediate64 i) { REXW(); OP(0xB9); Instructions().Emit64(i); }
  // void Encoder::MOV_DX_IMM16(Immediate16 i) { OpSize(); OP(0xBA); Instructions().Emit16(i); }
  // void Encoder::MOV_EDX_IMM32(Immediate32 i) { OP(0xBA); Instructions().Emit32(i); }
  // void Encoder::MOV_RDX_IMM64(Immediate64 i) { REXW(); OP(0xBA); Instructions().Emit64(i); }
  // void Encoder::MOV_BX_IMM16(Immediate16 i) { OpSize(); OP(0xBB); Instructions().Emit16(i); }
  // void Encoder::MOV_EBX_IMM32(Immediate32 i) { OP(0xBB); Instructions().Emit32(i); }
  // void Encoder::MOV_RBX_IMM64(Immediate64 i) { REXW(); OP(0xBB); Instructions().Emit64(i); }
  // void Encoder::MOV_SP_IMM16(Immediate16 i) { OpSize(); OP(0xBC); Instructions().Emit16(i); }
  // void Encoder::MOV_ESP_IMM32(Immediate32 i) { OP(0xBC); Instructions().Emit32(i); }
  // void Encoder::MOV_RSP_IMM64(Immediate64 i) { REXW(); OP(0xBC); Instructions().Emit64(i); }
  // void Encoder::MOV_BP_IMM16(Immediate16 i) { OpSize(); OP(0xBD); Instructions().Emit16(i); }
  // void Encoder::MOV_EBP_IMM32(Immediate32 i) { OP(0xBD); Instructions().Emit32(i); }
  // void Encoder::MOV_RBP_IMM64(Immediate64 i) { REXW(); OP(0xBD); Instructions().Emit64(i); }
  // void Encoder::MOV_SI_IMM16(Immediate16 i) { OpSize(); OP(0xBE); Instructions().Emit16(i); }
  // void Encoder::MOV_ESI_IMM32(Immediate32 i) { OP(0xBE); Instructions().Emit32(i); }
  // void Encoder::MOV_RSI_IMM64(Immediate64 i) { REXW(); OP(0xBE); Instructions().Emit64(i); }
  // void Encoder::MOV_DI_IMM16(Immediate16 i) { OpSize(); OP(0xBF); Instructions().Emit16(i); }
  // void Encoder::MOV_EDI_IMM32(Immediate32 i) { OP(0xBF); Instructions().Emit32(i); }
  // void Encoder::MOV_RDI_IMM64(Immediate64 i) { REXW(); OP(0xBF); Instructions().Emit64(i); }

  // // EGPR versions
  // void Encoder::MOV_R8W_IMM16(Immediate16 i) { REXB(); OpSize(); OP(0xB8); Instructions().Emit16(i); }
  // void Encoder::MOV_R8D_IMM32(Immediate32 i) { REXB(); OP(0xB8); Instructions().Emit32(i); }
  // void Encoder::MOV_R8_IMM64(Immediate64 i) { REXWB(); OP(0xB8); Instructions().Emit64(i); }
  // void Encoder::MOV_R9W_IMM16(Immediate16 i) { REXB(); OpSize(); OP(0xB9); Instructions().Emit16(i); }
  // void Encoder::MOV_R9D_IMM32(Immediate32 i) { REXB(); OP(0xB9); Instructions().Emit32(i); }
  // void Encoder::MOV_R9_IMM64(Immediate64 i) { REXWB(); OP(0xB9); Instructions().Emit64(i); }
  // void Encoder::MOV_R10W_IMM16(Immediate16 i) { REXB(); OpSize(); OP(0xBA); Instructions().Emit16(i); }
  // void Encoder::MOV_R10D_IMM32(Immediate32 i) { REXB(); OP(0xBA); Instructions().Emit32(i); }
  // void Encoder::MOV_R10_IMM64(Immediate64 i) { REXWB(); OP(0xBA); Instructions().Emit64(i); }
  // void Encoder::MOV_R11W_IMM16(Immediate16 i) { REXB(); OpSize(); OP(0xBB); Instructions().Emit16(i); }
  // void Encoder::MOV_R11D_IMM32(Immediate32 i) { REXB(); OP(0xBB); Instructions().Emit32(i); }
  // void Encoder::MOV_R11_IMM64(Immediate64 i) { REXWB(); OP(0xBB); Instructions().Emit64(i); }
  // void Encoder::MOV_R12W_IMM16(Immediate16 i) { REXB(); OpSize(); OP(0xBC); Instructions().Emit16(i); }
  // void Encoder::MOV_R12D_IMM32(Immediate32 i) { REXB(); OP(0xBC); Instructions().Emit32(i); }
  // void Encoder::MOV_R12_IMM64(Immediate64 i) { REXWB(); OP(0xBC); Instructions().Emit64(i); }
  // void Encoder::MOV_R13W_IMM16(Immediate16 i) { REXB(); OpSize(); OP(0xBD); Instructions().Emit16(i); }
  // void Encoder::MOV_R13D_IMM32(Immediate32 i) { REXB(); OP(0xBD); Instructions().Emit32(i); }
  // void Encoder::MOV_R13_IMM64(Immediate64 i) { REXWB(); OP(0xBD); Instructions().Emit64(i); }
  // void Encoder::MOV_R14W_IMM16(Immediate16 i) { REXB(); OpSize(); OP(0xBE); Instructions().Emit16(i); }
  // void Encoder::MOV_R14D_IMM32(Immediate32 i) { REXB(); OP(0xBE); Instructions().Emit32(i); }
  // void Encoder::MOV_R14_IMM64(Immediate64 i) { REXWB(); OP(0xBE); Instructions().Emit64(i); }
  // void Encoder::MOV_R15W_IMM16(Immediate16 i) { REXB(); OpSize(); OP(0xBF); Instructions().Emit16(i); }
  // void Encoder::MOV_R15D_IMM32(Immediate32 i) { REXB(); OP(0xBF); Instructions().Emit32(i); }
  // void Encoder::MOV_R15_IMM64(Immediate64 i) { REXWB(); OP(0xBF); Instructions().Emit64(i); }

  // void Encoder::NOP() { OP(0x90); }
  // void Encoder::PUSHFQ() { OP(0x9C); }
  // void Encoder::POPFQ() { OP(0x9D); }

  // void Encoder::STC() { OP(0xF9); } // Set the carry flag
  // void Encoder::CLC() { OP(0xF8); } // Clear the carry flag
  // void Encoder::CMC() { OP(0xF5); } // Complement the carry flag

  // // void MOVSXHelper(Encoder& encoder, auto& d, auto& s) {
  // //   if (d.Is16Bit()) encoder.OpSize();

  // //   encoder.REX(d, s);

  // //   if (s.Is8Bit()) encoder.Escape(0xBE);
  // //   else encoder.Escape(0xBF);

  // //   encoder.ModRM(d, s);
  // // }
  
  // // void Encoder::MOVSX(code::Register& d, code::Register& s) { MOVSXHelper(*this, d, s); }
  // // void Encoder::MOVSX(code::Register& d, code::Address& s) { MOVSXHelper(*this, d, s); }

  // // void MOVSXDHelper(Encoder& encoder, IsOp auto& d, IsOp auto& s) {
  // //   if (!d.Is64Bit()) throw utility::Error("MOVSXD expected its destination to be 64 bits");
  // //   if (!s.Is32Bit()) throw utility::Error("MOVSXD expected its source to be 32 bits");

  // //   encoder.REX(d, s);
  // //   encoder.OP(0x63);
  // //   encoder.ModRM(d, s);
  // // }

  // // void Encoder::MOVSXD(code::Register& d, code::Register& s) { REX(d, s); OP(0x63); ModRM(d, s); }
  // // void Encoder::MOVSXD(code::Register& d, code::Address& s) { REX(d, s); OP(0x63); ModRM(d, s); }

  // // void CVTSD2SSHelper(Encoder& encoder, IsOp auto& d, IsOp auto& s) {
  // //   // if (!d.Is64Bit()) throw utility::Error("CVTSD2SS expected its destination to be 64 bits");
  // //   // if (!s.Is32Bit()) throw utility::Error("CVTSD2SS expected its source to be 32 bits");

  // //   encoder.Double();
  // //   encoder.REX(d, s);
  // //   encoder.Escape(0x5A);
  // //   encoder.ModRM(d, s);
  // // }

  // // void Encoder::CVTSD2SS(code::Register& d, code::Register& s) { CVTSD2SSHelper(*this, d, s); }
  // // void Encoder::CVTSD2SS(code::Register& d, code::Address& s) { CVTSD2SSHelper(*this, d, s); }
};