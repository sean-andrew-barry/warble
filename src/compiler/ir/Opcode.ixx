export module compiler.ir.Opcode;

import <cstdint>;
import <string_view>;

namespace compiler::ir {
  export enum class Opcode : uint8_t {
    Extension, // A pseudo-instruction, whose operands apply to the **previous** instruction as a way to have more than 3 operands. Currently unused, but reserved just in case.
    Label, // A connection to a `Label` symbol, marking its position
    NoOperation,
    Unreachable,
    Error,
    Assume, // Assume(expression); 
    Await, // Await(expression);
    Expect, // Expect(expression);
    
    // Control flow
    Call, // result = Call(function, parameter);
    Return, // Return(expression);
    ReturnAsync, // ReturnAsync(expression);
    ReturnPass, // ReturnPass(expression);
    ReturnFail, // ReturnFail(expression);
    ReturnAsyncPass, // ReturnAsyncPass(expression);
    ReturnAsyncFail, // ReturnAsyncFail(expression);
    Yield, // Yield(expression);
    YieldAsync, // YieldAsync(expression);
    YieldPass, // YieldPass(expression);
    YieldFail, // YieldFail(expression);
    YieldAsyncPass, // YieldAsyncPass(expression);
    YieldAsyncFail, // YieldAsyncFail(expression);
    Panic, // Panic(expression);
    Jump, // Jump(label);
    Branch, // Branch(condition, label);
    Fork, // Fork(condition, label1, label2);

    // Pattern matching branches
    Is, // Is(condition, value, label);
    Has, // Has(condition, value, label);
    From, // From(condition, value, label);

    // Memory
    Copy, // result = Copy(value);
    Move, // result = Move(value);
    Load, // result = Load(value); // Read memory to register
    Store, // result = Store(value); // Write register to memory
    Index, // result = Index(base, index);
    Select, // result = Select(condition, (value1, value2));
    Spill, // Special register version of Store
    Restore, // Special register version of Load
    Drop, // Marks the end of a lifetime
    AddressOf, // reference = AddressOf(value);
    SymbolOf, // reference = SymbolOf(value);
    CopyOf, // result = CopyOf(value);
    Exchange,
    CompareAndExchange,
    ReadBarrier, // ReadBarrier(borrow); // Inserted before a module reads an imported borrow
    WriteBarrier, // WriteBarrier(borrow); // Inserted before a module writes an exported mutable value
    Suspend, // Marks when a module hit a suspend point, meaning it completed and didn't get blocked by a barrier
    Spread, // Clones `op2`'s children into `op1`'s children

    // Math
    Add, // op1 = op2 + op3;
    AddWithCarry,
    AddExchange, // temp = op1; op1 = op2 + op3; op2 = temp;
    Subtract, // op1 = op2 - op3;
    SubtractWithBorrow,
    Multiply, // op1 = op2 * op3;
    Divide, // op1 = op2 / op3;
    DivideSigned,
    Modulo, // op1 = op2 % op3;
    ModuloSigned,
    Exponent, // op1 = exp(op2, op3);
    Logarithm, // op1 = log(op2, op3);
    Logarithm2, // op1 = log2(op2);
    Logarithm10, // op1 = log10(op2);
    SquareRoot, // op1 = sqrt(op2);
    CubeRoot, // op1 = cbrt(op2);
    Hypotenuse, // op1 = hypot(op2, op3);
    Sine, // op1 = sin(op2);
    Cosine, // op1 = cos(op2);
    Tangent, // op1 = tan(op2);
    Arctangent, // op1 = atan2(op2, op3);
    Arcsine, // op1 = asin(op2);
    Arccosine, // op1 = acos(op2);
    Absolute, // op1 = abs(op2);
    Negate, // op1 = -op2;
    FusedMultiplyAdd, // 
    Floor, // op1 = floor(op2);
    Ceiling, // op1 = ceiling(op2);
    Truncate, // op1 = truncate(op2);
    Round, // op1 = round(op2);
    Minimum, // op1 = min(op2, op3);
    Maximum, // op1 = max(op2, op3);

    // Logic
    CompareTrue, // op1 = op2 == true;
    CompareFalse, // op1 = op2 == false;
    ComparePass, // op1 = op2;
    CompareFail, // op1 = !op2;
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

    // Union, // op1 = op2 | op3; (union of pass/fail or bitwise OR)
    // Intersection, // op1 = op2 & op3; (intersection of pass/fail or bitwise AND)
    // Complement, // op1 = ~op2;

    // Bit manipulation
    BitwiseAnd, // op1 = op2 & op3; (AND)
    BitwiseOr, // op1 = op2 | op3; (OR)
    BitwiseExclusiveOr, // op1 = op2 ^ op3; (XOR)
    BitwiseNot, // op1 = ~op2; (NOT)
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

    BitwiseHighestSetIsolate,
    BitwiseHighestSetReset,
    BitwiseHighestSetMask,

    BitTest, // result = BitTest(value, index); // Test a bit at an index
    BitSet, // BitSet(value, index); // Set a bit at an index
    BitClear, // BitClear(value, index); // Clear a bit at an index
    BitToggle, // BitToggle(value, index); // Toggle a bit at an index
    BitExtract, // BitExtract(value, start, length); // Extract a bit range
    BitInsert, // BitInsert(value, index, bits); // Insert a bit range

    CountLeadingZeros, // Counts the number of zero bits from high to low (LZCNT)
    CountTrailingZeros, // Counts the number of zero bits from low to high (TZCNT)
    CountPopulation, // Counts the number of bits set to 1 (POPCNT)

    SwapEndian, // Reverses endianness
    SwapEndianToBig, // Swaps from little to big endian - ignored if already big
    SwapEndianToLittle, // Swaps from big to little endian - ignored if already little

    // Conversions
    Cast, // Reinterprets it into a different type without changing the bits
    Convert, // Performs a type conversion - potentially changing the bits to maintain correctness
  };

  export constexpr std::string_view ToString(Opcode opcode) {
    switch (opcode) {
      case Opcode::Extension: return "Extension";
      case Opcode::Label: return "Label";
      case Opcode::NoOperation: return "NoOperation";
      case Opcode::Unreachable: return "Unreachable";
      case Opcode::Error: return "Error";
      case Opcode::Panic: return "Panic";
      case Opcode::Assume: return "Assume";
      case Opcode::Await: return "Await";
      case Opcode::Expect: return "Expect";
      case Opcode::Call: return "Call";
      case Opcode::Return: return "Return";
      case Opcode::ReturnAsync: return "ReturnAsync";
      case Opcode::ReturnPass: return "ReturnPass";
      case Opcode::ReturnFail: return "ReturnFail";
      case Opcode::ReturnAsyncPass: return "ReturnAsyncPass";
      case Opcode::ReturnAsyncFail: return "ReturnAsyncFail";
      case Opcode::Yield: return "Yield";
      case Opcode::YieldAsync: return "YieldAsync";
      case Opcode::YieldPass: return "YieldPass";
      case Opcode::YieldFail: return "YieldFail";
      case Opcode::YieldAsyncPass: return "YieldAsyncPass";
      case Opcode::YieldAsyncFail: return "YieldAsyncFail";
      case Opcode::Jump: return "Jump";
      case Opcode::Branch: return "Branch";
      case Opcode::Fork: return "Fork";
      case Opcode::Is: return "Is";
      case Opcode::Has: return "Has";
      case Opcode::From: return "From";
      case Opcode::Copy: return "Copy";
      case Opcode::Move: return "Move";
      case Opcode::Load: return "Load";
      case Opcode::Store: return "Store";
      case Opcode::Index: return "Index";
      case Opcode::Select: return "Select";
      case Opcode::Spill: return "Spill";
      case Opcode::Restore: return "Restore";
      case Opcode::Drop: return "Drop";
      case Opcode::AddressOf: return "AddressOf";
      case Opcode::SymbolOf: return "SymbolOf";
      case Opcode::CopyOf: return "CopyOf";
      case Opcode::Exchange: return "Exchange";
      case Opcode::CompareAndExchange: return "CompareAndExchange";
      case Opcode::ReadBarrier: return "ReadBarrier";
      case Opcode::WriteBarrier: return "WriteBarrier";
      case Opcode::Suspend: return "Suspend";
      case Opcode::Spread: return "Spread";
      case Opcode::Add: return "Add";
      case Opcode::AddWithCarry: return "AddWithCarry";
      case Opcode::AddExchange: return "AddExchange";
      case Opcode::Subtract: return "Subtract";
      case Opcode::SubtractWithBorrow: return "SubtractWithBorrow";
      case Opcode::Multiply: return "Multiply";
      case Opcode::Divide: return "Divide";
      case Opcode::DivideSigned: return "DivideSigned";
      case Opcode::Modulo: return "Modulo";
      case Opcode::ModuloSigned: return "ModuloSigned";
      case Opcode::Exponent: return "Exponent";
      case Opcode::Logarithm: return "Logarithm";
      case Opcode::Logarithm2: return "Logarithm2";
      case Opcode::Logarithm10: return "Logarithm10";
      case Opcode::SquareRoot: return "SquareRoot";
      case Opcode::CubeRoot: return "CubeRoot";
      case Opcode::Hypotenuse: return "Hypotenuse";
      case Opcode::Sine: return "Sine";
      case Opcode::Cosine: return "Cosine";
      case Opcode::Tangent: return "Tangent";
      case Opcode::Arctangent: return "Arctangent";
      case Opcode::Arcsine: return "Arcsine";
      case Opcode::Arccosine: return "Arccosine";
      case Opcode::Absolute: return "Absolute";
      case Opcode::Negate: return "Negate";
      case Opcode::FusedMultiplyAdd: return "FusedMultiplyAdd";
      case Opcode::Floor: return "Floor";
      case Opcode::Ceiling: return "Ceiling";
      case Opcode::Truncate: return "Truncate";
      case Opcode::Round: return "Round";
      case Opcode::Minimum: return "Minimum";
      case Opcode::Maximum: return "Maximum";
      case Opcode::CompareTrue: return "CompareTrue";
      case Opcode::CompareFalse: return "CompareFalse";
      case Opcode::ComparePass: return "ComparePass";
      case Opcode::CompareFail: return "CompareFail";
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
      case Opcode::BitwiseHighestSetIsolate: return "BitwiseHighestSetIsolate";
      case Opcode::BitwiseHighestSetReset: return "BitwiseHighestSetReset";
      case Opcode::BitwiseHighestSetMask: return "BitwiseHighestSetMask";
      case Opcode::BitTest: return "BitTest";
      case Opcode::BitSet: return "BitSet";
      case Opcode::BitClear: return "BitClear";
      case Opcode::BitToggle: return "BitToggle";
      case Opcode::BitExtract: return "BitExtract";
      case Opcode::BitInsert: return "BitInsert";
      case Opcode::CountLeadingZeros: return "CountLeadingZeros";
      case Opcode::CountTrailingZeros: return "CountTrailingZeros";
      case Opcode::CountPopulation: return "CountPopulation";
      case Opcode::SwapEndian: return "SwapEndian";
      case Opcode::SwapEndianToBig: return "SwapEndianToBig";
      case Opcode::SwapEndianToLittle: return "SwapEndianToLittle";
      case Opcode::Cast: return "Cast";
      case Opcode::Convert: return "Convert";
    }
  
    return "Unknown";
  }
}