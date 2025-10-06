export module compiler.ir.Opcode;

// import compiler.utility.Flag;
import <cstdint>;
// import <string_view>;

namespace compiler::ir {
  enum class Opcode : uint8_t {
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
    Select, // result = Select(condition, (value1, value2));
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

  // export class Opcode : Flag<uint8_t, Opcodes> {
  // public:
  //   static consteval Opcode Extension() { return Cast(Opcodes::Extension); }
  //   static consteval Opcode operands() { return Cast(Opcodes::operands); }
  //   static consteval Opcode Anchor() { return Cast(Opcodes::Anchor); }
  //   static consteval Opcode NoOperation() { return Cast(Opcodes::NoOperation); }
  //   static consteval Opcode Unreachable() { return Cast(Opcodes::Unreachable); }
  //   static consteval Opcode Abort() { return Cast(Opcodes::Abort); }
  //   static consteval Opcode Panic() { return Cast(Opcodes::Panic); }
  //   static consteval Opcode Assume() { return Cast(Opcodes::Assume); }
  //   static consteval Opcode Call() { return Cast(Opcodes::Call); }
  //   static consteval Opcode CallWithArray() { return Cast(Opcodes::CallWithArray); }
  //   static consteval Opcode CallWithTuple() { return Cast(Opcodes::CallWithTuple); }
  //   static consteval Opcode CallWithObject() { return Cast(Opcodes::CallWithObject); }
  //   static consteval Opcode CallWithEnum() { return Cast(Opcodes::CallWithEnum); }
  //   static consteval Opcode CallWithString() { return Cast(Opcodes::CallWithString); }
  //   static consteval Opcode CallWithTemplateString() { return Cast(Opcodes::CallWithTemplateString); }
  //   static consteval Opcode Return() { return Cast(Opcodes::Return); }
  //   static consteval Opcode Jump() { return Cast(Opcodes::Jump); }
  //   static consteval Opcode Branch() { return Cast(Opcodes::Branch); }
  //   static consteval Opcode Fork() { return Cast(Opcodes::Fork); }
  //   static consteval Opcode Allocate() { return Cast(Opcodes::Allocate); }
  //   static consteval Opcode Deallocate() { return Cast(Opcodes::Deallocate); }
  //   static consteval Opcode Move() { return Cast(Opcodes::Move); }
  //   static consteval Opcode Load() { return Cast(Opcodes::Load); }
  //   static consteval Opcode Store() { return Cast(Opcodes::Store); }
  //   static consteval Opcode Index() { return Cast(Opcodes::Index); }
  //   static consteval Opcode Select() { return Cast(Opcodes::Select); }
  //   static consteval Opcode Spill() { return Cast(Opcodes::Spill); }
  //   static consteval Opcode Restore() { return Cast(Opcodes::Restore); }
  //   static consteval Opcode Construct() { return Cast(Opcodes::Construct); }
  //   static consteval Opcode Destruct() { return Cast(Opcodes::Destruct); }
  //   static consteval Opcode AddressOf() { return Cast(Opcodes::AddressOf); }
  //   static consteval Opcode SymbolOf() { return Cast(Opcodes::SymbolOf); }
  //   static consteval Opcode CopyOf() { return Cast(Opcodes::CopyOf); }
  //   static consteval Opcode Exchange() { return Cast(Opcodes::Exchange); }
  //   static consteval Opcode CompareAndExchange() { return Cast(Opcodes::CompareAndExchange); }
  //   static consteval Opcode Fence() { return Cast(Opcodes::Fence); }
  //   static consteval Opcode Barrier() { return Cast(Opcodes::Barrier); }
  //   static consteval Opcode Spread() { return Cast(Opcodes::Spread); }
  //   static consteval Opcode Add() { return Cast(Opcodes::Add); }
  //   static consteval Opcode AddFloat() { return Cast(Opcodes::AddFloat); }
  //   static consteval Opcode AddWithCarry() { return Cast(Opcodes::AddWithCarry); }
  //   static consteval Opcode AddExchange() { return Cast(Opcodes::AddExchange); }
  //   static consteval Opcode Subtract() { return Cast(Opcodes::Subtract); }
  //   static consteval Opcode SubtractFloat() { return Cast(Opcodes::SubtractFloat); }
  //   static consteval Opcode SubtractWithBorrow() { return Cast(Opcodes::SubtractWithBorrow); }
  //   static consteval Opcode Multiply() { return Cast(Opcodes::Multiply); }
  //   static consteval Opcode MultiplyFloat() { return Cast(Opcodes::MultiplyFloat); }
  //   static consteval Opcode Divide() { return Cast(Opcodes::Divide); }
  //   static consteval Opcode DivideSigned() { return Cast(Opcodes::DivideSigned); }
  //   static consteval Opcode DivideFloat() { return Cast(Opcodes::DivideFloat); }
  //   static consteval Opcode Modulo() { return Cast(Opcodes::Modulo); }
  //   static consteval Opcode ModuloSigned() { return Cast(Opcodes::ModuloSigned); }
  //   static consteval Opcode ModuloFloat() { return Cast(Opcodes::ModuloFloat); }
  //   static consteval Opcode Exponent() { return Cast(Opcodes::Exponent); }
  //   static consteval Opcode Absolute() { return Cast(Opcodes::Absolute); }
  //   static consteval Opcode Negate() { return Cast(Opcodes::Negate); }
  //   static consteval Opcode FusedMultiplyAdd() { return Cast(Opcodes::FusedMultiplyAdd); }
  //   static consteval Opcode CompareTrue() { return Cast(Opcodes::CompareTrue); }
  //   static consteval Opcode CompareFalse() { return Cast(Opcodes::CompareFalse); }
  //   static consteval Opcode CompareTruthy() { return Cast(Opcodes::CompareTruthy); }
  //   static consteval Opcode CompareFalsy() { return Cast(Opcodes::CompareFalsy); }
  //   static consteval Opcode CompareZero() { return Cast(Opcodes::CompareZero); }
  //   static consteval Opcode CompareNotZero() { return Cast(Opcodes::CompareNotZero); }
  //   static consteval Opcode ComparePositive() { return Cast(Opcodes::ComparePositive); }
  //   static consteval Opcode CompareNegative() { return Cast(Opcodes::CompareNegative); }
  //   static consteval Opcode CompareEqual() { return Cast(Opcodes::CompareEqual); }
  //   static consteval Opcode CompareNotEqual() { return Cast(Opcodes::CompareNotEqual); }
  //   static consteval Opcode CompareGreater() { return Cast(Opcodes::CompareGreater); }
  //   static consteval Opcode CompareGreaterOrEqual() { return Cast(Opcodes::CompareGreaterOrEqual); }
  //   static consteval Opcode CompareLess() { return Cast(Opcodes::CompareLess); }
  //   static consteval Opcode CompareLessOrEqual() { return Cast(Opcodes::CompareLessOrEqual); }
  //   static consteval Opcode Test() { return Cast(Opcodes::Test); }
  //   static consteval Opcode And() { return Cast(Opcodes::And); }
  //   static consteval Opcode Or() { return Cast(Opcodes::Or); }
  //   static consteval Opcode Not() { return Cast(Opcodes::Not); }
  //   static consteval Opcode NotAnd() { return Cast(Opcodes::NotAnd); }
  //   static consteval Opcode NotOr() { return Cast(Opcodes::NotOr); }
  //   static consteval Opcode BitwiseAnd() { return Cast(Opcodes::BitwiseAnd); }
  //   static consteval Opcode BitwiseOr() { return Cast(Opcodes::BitwiseOr); }
  //   static consteval Opcode BitwiseExclusiveOr() { return Cast(Opcodes::BitwiseExclusiveOr); }
  //   static consteval Opcode BitwiseNot() { return Cast(Opcodes::BitwiseNot); }
  //   static consteval Opcode BitwiseShiftLeft() { return Cast(Opcodes::BitwiseShiftLeft); }
  //   static consteval Opcode BitwiseShiftRight() { return Cast(Opcodes::BitwiseShiftRight); }
  //   static consteval Opcode BitwiseShiftRightSigned() { return Cast(Opcodes::BitwiseShiftRightSigned); }
  //   static consteval Opcode BitwiseRotateLeft() { return Cast(Opcodes::BitwiseRotateLeft); }
  //   static consteval Opcode BitwiseRotateRight() { return Cast(Opcodes::BitwiseRotateRight); }
  //   static consteval Opcode BitwiseShiftLeftWithCarry() { return Cast(Opcodes::BitwiseShiftLeftWithCarry); }
  //   static consteval Opcode BitwiseShiftRightWithCarry() { return Cast(Opcodes::BitwiseShiftRightWithCarry); }
  //   static consteval Opcode BitwiseRotateLeftWithCarry() { return Cast(Opcodes::BitwiseRotateLeftWithCarry); }
  //   static consteval Opcode BitwiseRotateRightWithCarry() { return Cast(Opcodes::BitwiseRotateRightWithCarry); }
  //   static consteval Opcode BitwiseLowestSetIsolate() { return Cast(Opcodes::BitwiseLowestSetIsolate); }
  //   static consteval Opcode BitwiseLowestSetReset() { return Cast(Opcodes::BitwiseLowestSetReset); }
  //   static consteval Opcode BitwiseLowestSetMask() { return Cast(Opcodes::BitwiseLowestSetMask); }
  //   static consteval Opcode CheckBit() { return Cast(Opcodes::CheckBit); }
  //   static consteval Opcode CheckBitAndToggle() { return Cast(Opcodes::CheckBitAndToggle); }
  //   static consteval Opcode CheckBitAndReset() { return Cast(Opcodes::CheckBitAndReset); }
  //   static consteval Opcode CheckBitAndSet() { return Cast(Opcodes::CheckBitAndSet); }
  //   static consteval Opcode CountLeadingZeros() { return Cast(Opcodes::CountLeadingZeros); }
  //   static consteval Opcode CountTrailingZeros() { return Cast(Opcodes::CountTrailingZeros); }
  //   static consteval Opcode CountPopulation() { return Cast(Opcodes::CountPopulation); }
  //   static consteval Opcode SwapEndian() { return Cast(Opcodes::SwapEndian); }
  //   static consteval Opcode SwapEndianToBig() { return Cast(Opcodes::SwapEndianToBig); }
  //   static consteval Opcode SwapEndianToLittle() { return Cast(Opcodes::SwapEndianToLittle); }
  //   static consteval Opcode Cast() { return Cast(Opcodes::Cast); }
  //   static consteval Opcode Convert() { return Cast(Opcodes::Convert); }
  //   static consteval Opcode Prefetch() { return Cast(Opcodes::Prefetch); }

  //   [[nodiscard]] constexpr std::string_view ToString() const {
  //     switch (bits) {
  //       case static_cast<uint8_t>(Opcodes::Extension): return "Extension";
  //       case static_cast<uint8_t>(Opcodes::operands): return "operands";
  //       case static_cast<uint8_t>(Opcodes::Anchor): return "Anchor";
  //       case static_cast<uint8_t>(Opcodes::NoOperation): return "NoOperation";
  //       case static_cast<uint8_t>(Opcodes::Unreachable): return "Unreachable";
  //       case static_cast<uint8_t>(Opcodes::Abort): return "Abort";
  //       case static_cast<uint8_t>(Opcodes::Panic): return "Panic";
  //       case static_cast<uint8_t>(Opcodes::Assume): return "Assume";
  //       case static_cast<uint8_t>(Opcodes::Call): return "Call";
  //       case static_cast<uint8_t>(Opcodes::CallWithArray): return "CallWithArray";
  //       case static_cast<uint8_t>(Opcodes::CallWithTuple): return "CallWithTuple";
  //       case static_cast<uint8_t>(Opcodes::CallWithObject): return "CallWithObject";
  //       case static_cast<uint8_t>(Opcodes::CallWithEnum): return "CallWithEnum";
  //       case static_cast<uint8_t>(Opcodes::CallWithString): return "CallWithString";
  //       case static_cast<uint8_t>(Opcodes::CallWithTemplateString): return "CallWithTemplateString";
  //       case static_cast<uint8_t>(Opcodes::Return): return "Return";
  //       case static_cast<uint8_t>(Opcodes::Jump): return "Jump";
  //       case static_cast<uint8_t>(Opcodes::Branch): return "Branch";
  //       case static_cast<uint8_t>(Opcodes::Fork): return "Fork";
  //       case static_cast<uint8_t>(Opcodes::Allocate): return "Allocate";
  //       case static_cast<uint8_t>(Opcodes::Deallocate): return "Deallocate";
  //       case static_cast<uint8_t>(Opcodes::Move): return "Move";
  //       case static_cast<uint8_t>(Opcodes::Load): return "Load";
  //       case static_cast<uint8_t>(Opcodes::Store): return "Store";
  //       case static_cast<uint8_t>(Opcodes::Index): return "Index";
  //       case static_cast<uint8_t>(Opcodes::Select): return "Select";
  //       case static_cast<uint8_t>(Opcodes::Spill): return "Spill";
  //       case static_cast<uint8_t>(Opcodes::Restore): return "Restore";
  //       case static_cast<uint8_t>(Opcodes::Construct): return "Construct";
  //       case static_cast<uint8_t>(Opcodes::Destruct): return "Destruct";
  //       case static_cast<uint8_t>(Opcodes::AddressOf): return "AddressOf";
  //       case static_cast<uint8_t>(Opcodes::SymbolOf): return "SymbolOf";
  //       case static_cast<uint8_t>(Opcodes::CopyOf): return "CopyOf";
  //       case static_cast<uint8_t>(Opcodes::Exchange): return "Exchange";
  //       case static_cast<uint8_t>(Opcodes::CompareAndExchange): return "CompareAndExchange";
  //       case static_cast<uint8_t>(Opcodes::Fence): return "Fence";
  //       case static_cast<uint8_t>(Opcodes::Barrier): return "Barrier";
  //       case static_cast<uint8_t>(Opcodes::Spread): return "Spread";
  //       case static_cast<uint8_t>(Opcodes::Add): return "Add";
  //       case static_cast<uint8_t>(Opcodes::AddFloat): return "AddFloat";
  //       case static_cast<uint8_t>(Opcodes::AddWithCarry): return "AddWithCarry";
  //       case static_cast<uint8_t>(Opcodes::AddExchange): return "AddExchange";
  //       case static_cast<uint8_t>(Opcodes::Subtract): return "Subtract";
  //       case static_cast<uint8_t>(Opcodes::SubtractFloat): return "SubtractFloat";
  //       case static_cast<uint8_t>(Opcodes::SubtractWithBorrow): return "SubtractWithBorrow";
  //       case static_cast<uint8_t>(Opcodes::Multiply): return "Multiply";
  //       case static_cast<uint8_t>(Opcodes::MultiplyFloat): return "MultiplyFloat";
  //       case static_cast<uint8_t>(Opcodes::Divide): return "Divide";
  //       case static_cast<uint8_t>(Opcodes::DivideSigned): return "DivideSigned";
  //       case static_cast<uint8_t>(Opcodes::DivideFloat): return "DivideFloat";
  //       case static_cast<uint8_t>(Opcodes::Modulo): return "Modulo";
  //       case static_cast<uint8_t>(Opcodes::ModuloSigned): return "ModuloSigned";
  //       case static_cast<uint8_t>(Opcodes::ModuloFloat): return "ModuloFloat";
  //       case static_cast<uint8_t>(Opcodes::Exponent): return "Exponent";
  //       case static_cast<uint8_t>(Opcodes::Absolute): return "Absolute";
  //       case static_cast<uint8_t>(Opcodes::Negate): return "Negate";
  //       case static_cast<uint8_t>(Opcodes::FusedMultiplyAdd): return "FusedMultiplyAdd";
  //       case static_cast<uint8_t>(Opcodes::CompareTrue): return "CompareTrue";
  //       case static_cast<uint8_t>(Opcodes::CompareFalse): return "CompareFalse";
  //       case static_cast<uint8_t>(Opcodes::CompareTruthy): return "CompareTruthy";
  //       case static_cast<uint8_t>(Opcodes::CompareFalsy): return "CompareFalsy";
  //       case static_cast<uint8_t>(Opcodes::CompareZero): return "CompareZero";
  //       case static_cast<uint8_t>(Opcodes::CompareNotZero): return "CompareNotZero";
  //       case static_cast<uint8_t>(Opcodes::ComparePositive): return "ComparePositive";
  //       case static_cast<uint8_t>(Opcodes::CompareNegative): return "CompareNegative";
  //       case static_cast<uint8_t>(Opcodes::CompareEqual): return "CompareEqual";
  //       case static_cast<uint8_t>(Opcodes::CompareNotEqual): return "CompareNotEqual";
  //       case static_cast<uint8_t>(Opcodes::CompareGreater): return "CompareGreater";
  //       case static_cast<uint8_t>(Opcodes::CompareGreaterOrEqual): return "CompareGreaterOrEqual";
  //       case static_cast<uint8_t>(Opcodes::CompareLess): return "CompareLess";
  //       case static_cast<uint8_t>(Opcodes::CompareLessOrEqual): return "CompareLessOrEqual";
  //       case static_cast<uint8_t>(Opcodes::Test): return "Test";
  //       case static_cast<uint8_t>(Opcodes::And): return "And";
  //       case static_cast<uint8_t>(Opcodes::Or): return "Or";
  //       case static_cast<uint8_t>(Opcodes::Not): return "Not";
  //       case static_cast<uint8_t>(Opcodes::NotAnd): return "NotAnd";
  //       case static_cast<uint8_t>(Opcodes::NotOr): return "NotOr";
  //       case static_cast<uint8_t>(Opcodes::BitwiseAnd): return "BitwiseAnd";
  //       case static_cast<uint8_t>(Opcodes::BitwiseOr): return "BitwiseOr";
  //       case static_cast<uint8_t>(Opcodes::BitwiseExclusiveOr): return "BitwiseExclusiveOr";
  //       case static_cast<uint8_t>(Opcodes::BitwiseNot): return "BitwiseNot";
  //       case static_cast<uint8_t>(Opcodes::BitwiseShiftLeft): return "BitwiseShiftLeft";
  //       case static_cast<uint8_t>(Opcodes::BitwiseShiftRight): return "BitwiseShiftRight";
  //       case static_cast<uint8_t>(Opcodes::BitwiseShiftRightSigned): return "BitwiseShiftRightSigned";
  //       case static_cast<uint8_t>(Opcodes::BitwiseRotateLeft): return "BitwiseRotateLeft";
  //       case static_cast<uint8_t>(Opcodes::BitwiseRotateRight): return "BitwiseRotateRight";
  //       case static_cast<uint8_t>(Opcodes::BitwiseShiftLeftWithCarry): return "BitwiseShiftLeftWithCarry";
  //       case static_cast<uint8_t>(Opcodes::BitwiseShiftRightWithCarry): return "BitwiseShiftRightWithCarry";
  //       case static_cast<uint8_t>(Opcodes::BitwiseRotateLeftWithCarry): return "BitwiseRotateLeftWithCarry";
  //       case static_cast<uint8_t>(Opcodes::BitwiseRotateRightWithCarry): return "BitwiseRotateRightWithCarry";
  //       case static_cast<uint8_t>(Opcodes::BitwiseLowestSetIsolate): return "BitwiseLowestSetIsolate";
  //       case static_cast<uint8_t>(Opcodes::BitwiseLowestSetReset): return "BitwiseLowestSetReset";
  //       case static_cast<uint8_t>(Opcodes::BitwiseLowestSetMask): return "BitwiseLowestSetMask";
  //       case static_cast<uint8_t>(Opcodes::CheckBit): return "CheckBit";
  //       case static_cast<uint8_t>(Opcodes::CheckBitAndToggle): return "CheckBitAndToggle";
  //       case static_cast<uint8_t>(Opcodes::CheckBitAndReset): return "CheckBitAndReset";
  //       case static_cast<uint8_t>(Opcodes::CheckBitAndSet): return "CheckBitAndSet";
  //       case static_cast<uint8_t>(Opcodes::CountLeadingZeros): return "CountLeadingZeros";
  //       case static_cast<uint8_t>(Opcodes::CountTrailingZeros): return "CountTrailingZeros";
  //       case static_cast<uint8_t>(Opcodes::CountPopulation): return "CountPopulation";
  //       case static_cast<uint8_t>(Opcodes::SwapEndian): return "SwapEndian";
  //       case static_cast<uint8_t>(Opcodes::SwapEndianToBig): return "SwapEndianToBig";
  //       case static_cast<uint8_t>(Opcodes::SwapEndianToLittle): return "SwapEndianToLittle";
  //       case static_cast<uint8_t>(Opcodes::Cast): return "Cast";
  //       case static_cast<uint8_t>(Opcodes::Convert): return "Convert";
  //       case static_cast<uint8_t>(Opcodes::Prefetch): return "Prefetch";
  //       default: return "Unknown Opcode";
  //     }
  //   }
  };
};