export module ir.codes;

namespace ir {
  export enum class Codes : uint8_t {
    Extension, // A pseudo-instruction, whose operands apply to the **previous** instruction as a way to have more than 3 operands
    Anchor, // A CFG block's anchor point, marking where the block begins
    NoOperation,
    Unreachable,
    Abort, // Abort();
    Panic, // Panic(function);
    Assume, // Assume(condition); 

    // Control flow
    Call, // result = Call(function, parameter);
    CallWithArray, // result = Call(function, array);
    CallWithTuple, // result = Call(function, tuple);
    CallWithObject, // result = Call(function, object);
    CallWithEnum, // result = Call(function, enum);
    CallWithString, // result = Call(function, string);
    CallWithTemplateString, // result = Call(function, template_string);
    Return, // Return();
    Jump, // Jump(block);
    Branch, // Branch(condition, block);
    Fork, // Fork(condition, block1, block2);

    // Memory
    Allocate, // Allocate(bytes); // Reserve stack space
    Deallocate, // Deallocate(bytes); // Free stack space
    Move, // result = Move(value);
    Load, // result = Load(value); // Read memory to register
    Store, // result = Store(value); // Write register to memory
    Index, // result = Index(base, index);
    Select, // result = Select(condition, value1, value2);
    Spill, // Special version of Store
    Restore, // Special version of Load
    Construct, // Marks the creation of a new value and potentially the claiming of a register to hold that value
    Destruct, // Marks the end of a value and the release of its register - if any
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
};