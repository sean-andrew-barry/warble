export module compiler.ir.Opcode;

import <cstdint>;
import <string_view>;

namespace compiler::ir {
  export enum class Opcode : uint8_t {
    Extension, // A pseudo-instruction, whose operands apply to the **previous** instruction as a way to have more than 3 operands. Currently unused, but reserved just in case.
    Anchor, // A CFG block's anchor point, marking where the block begins
    Label, // A connection to a `Label` symbol, marking its position
    NoOperation,
    Unreachable,
    Error,
    Abort, // Abort();
    Panic, // Panic(expression);
    Assume, // Assume(expression); 
    Await, // Await(expression);
    Expect, // Expect(expression);
    Assert, // Assert(expression);
    Yield, // Yield(expression);

    // Control flow
    Call, // result = Call(function, parameter);
    CallWithArray, // result = Call(function, array);
    CallWithTuple, // result = Call(function, tuple);
    CallWithObject, // result = Call(function, object);
    CallWithEnum, // result = Call(function, enum);
    CallWithString, // result = Call(function, string);
    CallWithTemplateString, // result = Call(function, template_string);
    Return, // Return();
    Jump, // Jump(label);
    Branch, // Branch(condition, label);
    Fork, // Fork(condition, label1, label2);
    BranchIs, // BranchIs(condition, value, label);
    BranchHas, // BranchHas(condition, value, label);
    BranchFrom, // BranchFrom(condition, value, label);
    BranchNotIs, // BranchNotIs(condition, value, label);
    BranchNotHas, // BranchNotHas(condition, value, label);
    BranchNotFrom, // BranchNotFrom(condition, value, label);

    // Memory
    Allocate, // Allocate(bytes); // Reserve stack space
    Deallocate, // Deallocate(bytes); // Free stack space
    Move, // result = Move(value);
    Load, // result = Load(value); // Read memory to register
    Store, // result = Store(value); // Write register to memory
    Index, // result = Index(base, index);
    Select, // result = Select(condition, (value1, value2));
    Spill, // Special version of Store
    Restore, // Special version of Load
    Construct, // Marks the creation of a new value and potentially the claiming of a register to hold that value
    Destruct, // Marks the end of a value and the release of its register - if any
    Drop, // Marks the end of a lifetime
    AddressOf, // reference = AddressOf(value);
    SymbolOf, // reference = SymbolOf(value);
    CopyOf, // result = CopyOf(value);
    Exchange,
    CompareAndExchange,
    Fence,
    Barrier,
    Spread, // Clones `op2`'s children into `op1`'s children

    // Math
    Add, // op1 = op2 + op3;
    AddFloat,
    AddWithCarry,
    AddExchange, // temp = op1; op1 = op2 + op3; op2 = temp;
    Subtract, // op1 = op2 - op3;
    SubtractFloat,
    SubtractWithBorrow,
    Multiply, // op1 = op2 * op3;
    MultiplyFloat,
    Divide, // op1 = op2 / op3;
    DivideSigned,
    DivideFloat,
    Modulo, // op1 = op2 % op3;
    ModuloSigned,
    ModuloFloat,
    Exponent, // op1 = op2 ** op3;
    Absolute, // op1 = abs(op2);
    Negate, // op1 = -op2;
    FusedMultiplyAdd, // 

    // Logic
    CompareTrue, // op1 = op2 == true;
    CompareFalse, // op1 = op2 == false;
    CompareTruthy, // op1 = op2;
    CompareFalsy, // op1 = !op2;
    CompareZero, // op1 = op2 == 0
    CompareNotZero, // op1 = op2 != 0;
    ComparePositive, // op1 = op2 >= 0;
    CompareNegative, // op1 = op2 < 0;
    CompareEqual, // op1 = op2 == op3;
    CompareNotEqual, // op1 = op2 != op3;
    CompareGreater, // op1 = op2 > op3;
    CompareGreaterOrEqual, // op1 = op2 >= op3;
    CompareLess, // op1 = op2 < op3;
    CompareLessOrEqual, // op1 = op2 <= op3;
    Test, // The bitwise AND based comparison
    And, // op1 = op2 && op3;
    Or, // op1 = op2 || op3;
    Not, // op1 = !op2;
    NotAnd, // op1 = (!op2) && op3;
    NotOr, // op1 = (!op2) || op3;

    // Bit manipulation
    BitwiseAnd, // op1 = op2 & op3;
    BitwiseOr, // op1 = op2 | op3;
    BitwiseExclusiveOr, // op1 = op2 ^ op3;
    BitwiseNot, // op1 = ~op2;
    BitwiseShiftLeft, // op1 = op2 << op3; (SOL)
    BitwiseShiftRight, // op1 = op2 >> op3; (SOR)
    BitwiseShiftRightSigned,
    BitwiseRotateLeft, // ROL
    BitwiseRotateRight, // ROR
    BitwiseShiftLeftWithCarry, // SCL
    BitwiseShiftRightWithCarry, // SCR
    BitwiseRotateLeftWithCarry, // RCL
    BitwiseRotateRightWithCarry, // RCR

    BitwiseLowestSetIsolate, // op1 = op2 & (-op2); (BLSI)
    BitwiseLowestSetReset, // op1 = op2 & (op2 - 1); (BLSR)
    BitwiseLowestSetMask, // op1 = op2 ^ (op2 - 1); (BLSMSK)

    CheckBit, // Check a bit at an index
    CheckBitAndToggle, // Checks if the bit at an index is 1 and toggles it
    CheckBitAndReset, // Checks if the bit at an index is 1 and sets it to 0
    CheckBitAndSet, // Checks if the bit at an index is 1 and sets it to 1

    CountLeadingZeros, // Counts the number of zero bits from high to low (LZCNT)
    CountTrailingZeros, // Counts the number of zero bits from low to high (TZCNT)
    CountPopulation, // Counts the number of bits set to 1 (POPCNT)

    SwapEndian, // Reverses endianness
    SwapEndianToBig, // Swaps from little to big endian - ignored if already big
    SwapEndianToLittle, // Swaps from big to little endian - ignored if already little

    // Conversions
    Cast, // Reinterprets it into a different type without changing the bits
    Convert, // Performs a type conversion - potentially changing the bits to maintain correctness

    Prefetch, // Very niche but sometimes useful
  };

  export constexpr std::string_view ToString(Opcode opcode) {
    switch (opcode) {
      case Opcode::Extension: return "Extension";
      case Opcode::Anchor: return "Anchor";
      case Opcode::Label: return "Label";
      case Opcode::NoOperation: return "NoOperation";
      case Opcode::Unreachable: return "Unreachable";
      case Opcode::Error: return "Error";
      case Opcode::Abort: return "Abort";
      case Opcode::Panic: return "Panic";
      case Opcode::Assume: return "Assume";
      case Opcode::Await: return "Await";
      case Opcode::Expect: return "Expect";
      case Opcode::Assert: return "Assert";
      case Opcode::Yield: return "Yield";
      case Opcode::Call: return "Call";
      case Opcode::CallWithArray: return "CallWithArray";
      case Opcode::CallWithTuple: return "CallWithTuple";
      case Opcode::CallWithObject: return "CallWithObject";
      case Opcode::CallWithEnum: return "CallWithEnum";
      case Opcode::CallWithString: return "CallWithString";
      case Opcode::CallWithTemplateString: return "CallWithTemplateString";
      case Opcode::Return: return "Return";
      case Opcode::Jump: return "Jump";
      case Opcode::Branch: return "Branch";
      case Opcode::Fork: return "Fork";
      case Opcode::BranchIs: return "BranchIs";
      case Opcode::BranchHas: return "BranchHas";
      case Opcode::BranchFrom: return "BranchFrom";
      case Opcode::BranchNotIs: return "BranchNotIs";
      case Opcode::BranchNotHas: return "BranchNotHas";
      case Opcode::BranchNotFrom: return "BranchNotFrom";
      case Opcode::Allocate: return "Allocate";
      case Opcode::Deallocate: return "Deallocate";
      case Opcode::Move: return "Move";
      case Opcode::Load: return "Load";
      case Opcode::Store: return "Store";
      case Opcode::Index: return "Index";
      case Opcode::Select: return "Select";
      case Opcode::Spill: return "Spill";
      case Opcode::Restore: return "Restore";
      case Opcode::Construct: return "Construct";
      case Opcode::Destruct: return "Destruct";
      case Opcode::Drop: return "Drop";
      case Opcode::AddressOf: return "AddressOf";
      case Opcode::SymbolOf: return "SymbolOf";
      case Opcode::CopyOf: return "CopyOf";
      case Opcode::Exchange: return "Exchange";
      case Opcode::CompareAndExchange: return "CompareAndExchange";
      case Opcode::Fence: return "Fence";
      case Opcode::Barrier: return "Barrier";
      case Opcode::Spread: return "Spread";
      case Opcode::Add: return "Add";
      case Opcode::AddFloat: return "AddFloat";
      case Opcode::AddWithCarry: return "AddWithCarry";
      case Opcode::AddExchange: return "AddExchange";
      case Opcode::Subtract: return "Subtract";
      case Opcode::SubtractFloat: return "SubtractFloat";
      case Opcode::SubtractWithBorrow: return "SubtractWithBorrow";
      case Opcode::Multiply: return "Multiply";
      case Opcode::MultiplyFloat: return "MultiplyFloat";
      case Opcode::Divide: return "Divide";
      case Opcode::DivideSigned: return "DivideSigned";
      case Opcode::DivideFloat: return "DivideFloat";
      case Opcode::Modulo: return "Modulo";
      case Opcode::ModuloSigned: return "ModuloSigned";
      case Opcode::ModuloFloat: return "ModuloFloat";
      case Opcode::Exponent: return "Exponent";
      case Opcode::Absolute: return "Absolute";
      case Opcode::Negate: return "Negate";
      case Opcode::FusedMultiplyAdd: return "FusedMultiplyAdd";
      case Opcode::CompareTrue: return "CompareTrue";
      case Opcode::CompareFalse: return "CompareFalse";
      case Opcode::CompareTruthy: return "CompareTruthy";
      case Opcode::CompareFalsy: return "CompareFalsy";
      case Opcode::CompareZero: return "CompareZero";
      case Opcode::CompareNotZero: return "CompareNotZero";
      case Opcode::ComparePositive: return "ComparePositive";
      case Opcode::CompareNegative: return "CompareNegative";
      case Opcode::CompareEqual: return "CompareEqual";
      case Opcode::CompareNotEqual: return "CompareNotEqual";
      case Opcode::CompareGreater: return "CompareGreater";
      case Opcode::CompareGreaterOrEqual: return "CompareGreaterOrEqual";
      case Opcode::CompareLess: return "CompareLess";
      case Opcode::CompareLessOrEqual: return "CompareLessOrEqual";
      case Opcode::Test: return "Test";
      case Opcode::And: return "And";
      case Opcode::Or: return "Or";
      case Opcode::Not: return "Not";
      case Opcode::NotAnd: return "NotAnd";
      case Opcode::NotOr: return "NotOr";
      case Opcode::BitwiseAnd: return "BitwiseAnd";
      case Opcode::BitwiseOr: return "BitwiseOr";
      case Opcode::BitwiseExclusiveOr: return "BitwiseExclusiveOr";
      case Opcode::BitwiseNot: return "BitwiseNot";
      case Opcode::BitwiseShiftLeft: return "BitwiseShiftLeft";
      case Opcode::BitwiseShiftRight: return "BitwiseShiftRight";
      case Opcode::BitwiseShiftRightSigned: return "BitwiseShiftRightSigned";
      case Opcode::BitwiseRotateLeft: return "BitwiseRotateLeft";
      case Opcode::BitwiseRotateRight: return "BitwiseRotateRight";
      case Opcode::BitwiseShiftLeftWithCarry: return "BitwiseShiftLeftWithCarry";
      case Opcode::BitwiseShiftRightWithCarry: return "BitwiseShiftRightWithCarry";
      case Opcode::BitwiseRotateLeftWithCarry: return "BitwiseRotateLeftWithCarry";
      case Opcode::BitwiseRotateRightWithCarry: return "BitwiseRotateRightWithCarry";
      case Opcode::BitwiseLowestSetIsolate: return "BitwiseLowestSetIsolate";
      case Opcode::BitwiseLowestSetReset: return "BitwiseLowestSetReset";
      case Opcode::BitwiseLowestSetMask: return "BitwiseLowestSetMask";
      case Opcode::CheckBit: return "CheckBit";
      case Opcode::CheckBitAndToggle: return "CheckBitAndToggle";
      case Opcode::CheckBitAndReset: return "CheckBitAndReset";
      case Opcode::CheckBitAndSet: return "CheckBitAndSet";
      case Opcode::CountLeadingZeros: return "CountLeadingZeros";
      case Opcode::CountTrailingZeros: return "CountTrailingZeros";
      case Opcode::CountPopulation: return "CountPopulation";
      case Opcode::SwapEndian: return "SwapEndian";
      case Opcode::SwapEndianToBig: return "SwapEndianToBig";
      case Opcode::SwapEndianToLittle: return "SwapEndianToLittle";
      case Opcode::Cast: return "Cast";
      case Opcode::Convert: return "Convert";
      case Opcode::Prefetch: return "Prefetch";
    }
  
    return "Unknown";
  }
}