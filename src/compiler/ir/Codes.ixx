export module ir.codes;

namespace ir {
  export enum class Codes : uint8_t {
    Extension, // A pseudo-instruction, whose operands apply to the **previous** instruction, as a way to have more than 3 operands
    Phi, // The representation of a phi node, can be extended to represent any number of operands
    NoOperation,

    // Control flow
    Call, // op1 = op2 -> op3;
    Return, // goto op1;
    Break, // goto op1; // Technically just a Jump, but useful to distinguish
    Continue, // goto op1; // Technically just a Jump, but useful to distinguish
    Jump, // goto op1;
    JumpIfTrue, // if (op1 == true) goto op2;
    JumpIfFalse, // if (op1 == false) goto op2;
    JumpIfTruthy, // if (op1) goto op2;
    JumpIfFalsy, // if (!op1) goto op2;
    JumpIfZero, // if (op1 != 0) goto op2;
    JumpIfNotZero, // if (op1 != 0) goto op2;
    JumpIfPositive, // if (op1 >= 0) goto op2;
    JumpIfNegative, // if (op1 < 0) goto op2;
    JumpIfEqual, // if (op1 == op2) goto op3;
    JumpIfNotEqual, // if (op1 != op2) goto op3;
    JumpIfGreater, // if (op1 > op2) goto op3;
    JumpIfGreaterOrEqual, // if (op1 >= op2) goto op3;
    JumpIfLess, // if (op1 < op2) goto op3;
    JumpIfLessOrEqual, // if (op1 <= op2) goto op3;

    // Memory
    Move, // op1 = op2;
    MoveIfTrue, // if (op1 == true) op2 = op3;
    MoveIfFalse, // if (op1 == false) op2 = op3;
    MoveIfTruthy, // if (op1) op2 = op3;
    MoveIfFalsy, // if (!op1) op2 = op3;
    MoveIfZero, // if (op1 == 0) op2 = op3;
    MoveIfNotZero, // if (op1 != 0) op2 = op3;
    MoveIfPositive, // if (op1 >= 0) op2 = op3;
    MoveIfNegative, // if (op1 < 0) op2 = op3;
    MoveIfEqual, // if (op1 == op2) op3 = op1;
    MoveIfNotEqual, // if (op1 != op2) op3 = op1;
    MoveIfGreater, // if (op1 > op2) op3 = op1;
    MoveIfGreaterOrEqual, // if (op1 >= op2) op3 = op1;
    MoveIfLess, // if (op1 < op2) op3 = op1;
    MoveIfLessOrEqual, // if (op1 <= op2) op3 = op1;
    
    Spill, // Specialized Move to mark the specific action of spilling a register
    Restore, // The opposite of Spill
    Construct, // Marks the creation of a new value and potentially the claiming of a register to hold that value
    Destruct, // Marks the end of a value and the release of its register, if any
    Push,
    Pop,
    Zero, // Set the operand to zero
    AddressOf, // op1 = &op2; (LEA)
    SymbolOf, // op1 = $op2; // Computes the AddressOf to the symbol
    CopyOf, // op1 = @op2; // Technically this is just a Move, but it is represented differently to preserve more information
    Exchange,
    CompareAndExchange,
    Spread, // Clones `op2`'s children into `op1`'s children

    // Math
    Increment, // op1 = op2 + 1;
    Decrement, // op1 = op2 - 1;
    Add, // op1 = op2 + op3;
    AddWithCarry,
    AddExchange, // temp = op1; op1 = op2 + op3; op2 = temp;
    Subtract, // op1 = op2 - op3;
    SubtractWithBorrow,
    Multiply, // op1 = op2 * op3;
    Divide, // op1 = op2 / op3;
    Modulo, // op1 = op2 % op3;
    Exponent, // op1 = op2 ** op3;
    Absolute, // op1 = abs(op2);
    Negate, // op1 = -op2;

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
    CheckBitAndToggle, // Checks if the bit at an index is 1, and toggles it
    CheckBitAndReset, // Checks if the bit at an index is 1, and sets it to 0
    CheckBitAndSet, // Checks if the bit at an index is 1, and sets it to 1

    CountLeadingZeros, // Counts the number of zero bits from high to low (LZCNT)
    CountTrailingZeros, // Counts the number of zero bits from low to high (TZCNT)
    CountPopulation, // Counts the number of bits set to 1 (POPCNT)

    SwapEndian, // Reverses endianness
    SwapEndianToBig, // Swaps from little to big endian, ignored if already big
    SwapEndianToLittle, // Swaps from big to little endian, ignored if already little

    // Conversions
    Cast, // Reinterprets it into a different type without changing the bits
    Convert, // Performs a type conversion, potentially changing the bits to maintain correctness

    Prefetch, // Very niche, but sometimes useful
  };
};