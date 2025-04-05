export module code.bytecode.instructions;

import code.bytecode;
import code.bytecode.virtual_machine;
import code.bytecodes;
import code.registers;
import utility.print;

import <cstdint>;
import <functional>;
import <string_view>;
import <typeinfo>;
import <bit>;
import <cmath>;
import <limits>;
import <string>;
import <stdexcept>;
import <iostream>;

// using Bytecodes = code::Bytecodes;
using VM = code::bytecode::VirtualMachine;

namespace code::bytecode {
  export template<typename Derived, code::Bytecodes TYPE>
  class Template : public Bytecode {
  private:
  public:
    using Super = Template<Derived, TYPE>;

    Template() : Bytecode{TYPE} {}
    Template(code::Bytecodes type) : Bytecode{type} {}

    static consteval code::Bytecodes Type() { return TYPE; }
  };

  export struct Extended : public Template<Extended, code::Bytecodes::Extended> {
    void Run(VM& vm) const {} // Intentionally left blank
  };

  export struct NoOperation : public Template<NoOperation, code::Bytecodes::NoOperation> {
    void Run(VM& vm) const {} // Intentionally left blank
  };

  export struct DirectJump : public Template<DirectJump, code::Bytecodes::DirectJump> {
    uint32_t address;

    void Run(VM& vm) const { vm.SetIP(address); }
  };

  export struct ShortJump : public Template<ShortJump, code::Bytecodes::ShortJump> {
    int8_t offset;

    void Run(VM& vm) const { vm.OffsetIP(offset); }
  };

  export struct NearJump : public Template<NearJump, code::Bytecodes::NearJump> {
    int32_t offset;

    void Run(VM& vm) const { vm.OffsetIP(offset); }
  };

  export template<typename Derived, code::Bytecodes type, typename Operation, typename T = uint64_t>
  struct UnaryJump : public Template<Derived, type> {
    constexpr static auto Op = Operation{};
    using Super = UnaryJump<Derived, type, Operation, T>;

    code::Registers reg;
    uint32_t address;

    UnaryJump(code::Registers reg, uint32_t address)
      : Template<Derived>{}, reg{reg}, address{address} {}

    void Run(VM& vm) const {
      auto val = vm.GetReg<T>(reg);

      if (Op(val)) {
        vm.SetIP(address);
      }
    }
  };

  export template<typename Derived, code::Bytecodes type, typename Operation, typename T = uint64_t>
  struct BinaryJump : public Template<Derived, type> {
    constexpr static auto Op = Operation{};
    using Super = BinaryJump<Derived, type, Operation, T>;

    code::Registers reg1;
    code::Registers reg2;
    uint32_t address;

    BinaryJump(code::Registers reg1, code::Registers reg2, uint32_t address)
      : Template<Derived>{}, reg1{reg1}, reg2{reg2}, address{address} {}

    void Run(VM& vm) const {
      auto val1 = vm.GetReg<T>(reg1);
      auto val2 = vm.GetReg<T>(reg2);

      if (Op(val1, val2)) {
        vm.SetIP(address);
      }
    }
  };

  constexpr auto JE = [](auto a, auto b){ return a == b; }; // Jump if Equal
  export struct JumpIfEqual : public BinaryJump<JumpIfEqual, code::Bytecodes::JumpIfEqual, decltype(JE), uint64_t> { using Super::Super; };

  constexpr auto JZ = [](auto a){ return a == 0; }; // Jump if Zero
  export struct JumpIfZero : public UnaryJump<JumpIfZero, code::Bytecodes::JumpIfZero, decltype(JZ), uint64_t> { using Super::Super; };

  constexpr auto JNE = [](auto a, auto b){ return a != b; }; // Jump if Not Equal
  export struct JumpIfNotEqual : public BinaryJump<JumpIfNotEqual, code::Bytecodes::JumpIfNotEqual, decltype(JNE), uint64_t> { using Super::Super; };

  constexpr auto JNZ = [](auto a){ return a != 0; }; // Jump if Not Zero
  export struct JumpIfNotZero : public UnaryJump<JumpIfNotZero, code::Bytecodes::JumpIfNotZero, decltype(JNZ), uint64_t> { using Super::Super; };

  constexpr auto JG = [](auto a, auto b){ return a > b; }; // Jump if Greater/Jump if Not Less or Equal
  export struct JumpIfGreater : public BinaryJump<JumpIfGreater, code::Bytecodes::JumpIfGreater, decltype(JG), int64_t> { using Super::Super; };

  constexpr auto JGE = [](auto a, auto b){ return a >= b; }; // Jump if Greater or Equal/Jump if Not Less
  export struct JumpIfGreaterOrEqual : public BinaryJump<JumpIfGreaterOrEqual, code::Bytecodes::JumpIfGreaterOrEqual, decltype(JGE), int64_t> { using Super::Super; };

  constexpr auto JL = [](auto a, auto b){ return a < b; }; // Jump if Less/Jump if Not Greater or Equal
  export struct JumpIfLess : public BinaryJump<JumpIfLess, code::Bytecodes::JumpIfLess, decltype(JL), int64_t> { using Super::Super; };

  constexpr auto JLE = [](auto a, auto b){ return a <= b; }; // Jump if Less or Equal/Jump if Not Greater
  export struct JumpIfLessOrEqual : public BinaryJump<JumpIfLessOrEqual, code::Bytecodes::JumpIfLessOrEqual, decltype(JLE), int64_t> { using Super::Super; };

  constexpr auto JS = [](auto a){ return a < 0; }; // Jump if Sign
  export struct JumpIfSign : public UnaryJump<JumpIfSign, code::Bytecodes::JumpIfSign, decltype(JS), int64_t> { using Super::Super; };

  constexpr auto JNS = [](auto a){ return a >= 0; }; // Jump if Not Sign
  export struct JumpIfNotSign : public UnaryJump<JumpIfNotSign, code::Bytecodes::JumpIfNotSign, decltype(JNS), int64_t> { using Super::Super; };

  constexpr auto JA = [](auto a, auto b){ return a > b; }; // Jump if Above/Jump if Not Below or Equal
  export struct JumpIfAbove : public BinaryJump<JumpIfAbove, code::Bytecodes::JumpIfAbove, decltype(JA), uint64_t> { using Super::Super; };

  constexpr auto JAE = [](auto a, auto b){ return a >= b; }; // Jump if Above or Equal/Jump if Not Below
  export struct JumpIfAboveOrEqual : public BinaryJump<JumpIfAboveOrEqual, code::Bytecodes::JumpIfAboveOrEqual, decltype(JAE), uint64_t> { using Super::Super; };

  constexpr auto JB = [](auto a, auto b){ return a < b; }; // Jump if Below/Jump if Not Above or Equal
  export struct JumpIfBelow : public BinaryJump<JumpIfBelow, code::Bytecodes::JumpIfBelow, decltype(JB), uint64_t> { using Super::Super; };

  constexpr auto JBE = [](auto a, auto b){ return a <= b; }; // Jump if Below or Equal/Jump if Not Above
  export struct JumpIfBelowOrEqual : public BinaryJump<JumpIfBelowOrEqual, code::Bytecodes::JumpIfBelowOrEqual, decltype(JBE), uint64_t> { using Super::Super; };

  export struct Push : public Template<Push, code::Bytecodes::Push> {
    code::Registers reg;

    Push(code::Registers reg)
      : Super{}, reg{reg} {}
    
    void Run(VM& vm) const { vm.Push(vm.GetReg(reg)); }
  };

  export struct Pop : public Template<Pop, code::Bytecodes::Pop> {
    code::Registers reg;

    Pop(code::Registers reg)
      : Super{}, reg{reg} {}

    void Run(VM& vm) const { vm.SetReg(reg, vm.Pop()); }
  };

  export struct Allocate : public Template<Allocate, code::Bytecodes::Allocate> {
    uint8_t bytes;

    Allocate(uint8_t bytes)
      : Super{}, bytes{bytes} {}

    void Run(VM& vm) const { vm.Reserve(bytes); }
  };

  export struct AllocateLarge : public Template<AllocateLarge, code::Bytecodes::AllocateLarge> {
    uint16_t bytes;

    AllocateLarge(uint16_t bytes)
      : Super{}, bytes{bytes} {}

    void Run(VM& vm) const { vm.Reserve(bytes); }
  };

  export struct Deallocate : public Template<Deallocate, code::Bytecodes::Deallocate> {
    uint8_t bytes;

    Deallocate(uint8_t bytes)
      : Super{}, bytes{bytes} {}

    void Run(VM& vm) const { vm.Release(bytes); }
  };

  export struct DeallocateLarge : public Template<DeallocateLarge, code::Bytecodes::DeallocateLarge> {
    uint16_t bytes;

    DeallocateLarge(uint16_t bytes)
      : Super{}, bytes{bytes} {}

    void Run(VM& vm) const { vm.Release(bytes); }
  };

  export struct Move : public Template<Move, code::Bytecodes::Move> {
    code::Registers dest;
    code::Registers source;

    Move(code::Registers dest, code::Registers source)
      : Super{}, dest{dest}, source{source} {}

    void Run(VM& vm) const { vm.SetReg(dest, vm.GetReg(source)); }
  };

  export struct Call : public Template<Call, code::Bytecodes::Call> {
    uint64_t address; // The address of the function to call

    void Run(VM& vm) const {
      // Push the address of the next instruction onto the stack
      vm.Push(vm.ReadIP());
      
      // Set the IP to the function's starting address
      vm.SetIP(address);
      
      // The function prologue is handled at the start of the function itself
    }
  };

  export struct Return : public Template<Return, code::Bytecodes::Return> {
    void Run(VM& vm) const {
      // Pop the return address off the stack
      uint64_t address = vm.Pop();
      
      // Set the IP to the return address
      vm.SetIP(address);
      
      // The function epilogue can be assumed to have been handled
      // just before this instruction, restoring any saved registers or local state as needed
    }
  };

  export template<typename Derived, code::Bytecodes TYPE, typename T = uint64_t>
  struct Load : public Template<Derived, TYPE> {
  private:
    code::Registers base;
    code::Registers destination;
    int32_t offset;
  protected:
    using Super = Load<Derived, TYPE, T>;
  public:
    Load(code::Registers base, code::Registers destination, int32_t offset)
      : Template<Derived, TYPE>{}, base{base}, destination{destination}, offset{offset} {}

    void Run(VM& vm) const {
      vm.SetReg(destination, vm.Read<T>(offset, base));
    }
  };
  
  export struct Load8  : public Load<Load8 , code::Bytecodes::Load8 , uint8_t > { using Super::Super; };
  export struct Load16 : public Load<Load16, code::Bytecodes::Load16, uint16_t> { using Super::Super; };
  export struct Load32 : public Load<Load32, code::Bytecodes::Load32, uint32_t> { using Super::Super; };
  export struct Load64 : public Load<Load64, code::Bytecodes::Load64, uint64_t> { using Super::Super; };

  export template<typename Derived, code::Bytecodes type, typename T = uint64_t>
  struct Store : public Template<Derived, type> {
    using Super = Store<Derived, type, T>;

    code::Registers base;
    code::Registers source;
    int32_t offset;

    Store(code::Registers base, code::Registers source, int32_t offset)
      : Template<Derived, type>{}, base{base}, source{source}, offset{offset} {}

    void Run(VM& vm) const {
      vm.Write(offset, vm.GetReg<T>(source), base);
    }
  };

  export struct Store8  : public Store<Store8 , code::Bytecodes::Store8 , uint8_t > { using Super::Super; };
  export struct Store16 : public Store<Store16, code::Bytecodes::Store16, uint16_t> { using Super::Super; };
  export struct Store32 : public Store<Store32, code::Bytecodes::Store32, uint32_t> { using Super::Super; };
  export struct Store64 : public Store<Store64, code::Bytecodes::Store64, uint64_t> { using Super::Super; };

  export template<typename Derived, code::Bytecodes type, typename T = uint64_t>
  struct StoreImmediate : public Template<Derived, type> {
    using Super = StoreImmediate<Derived, type, T>;

    code::Registers base;
    T value;
    int32_t offset;

    StoreImmediate(code::Registers base, T value, int32_t offset)
      : Template<Derived, type>{}, base{base}, value{value}, offset{offset} {}

    void Run(VM& vm) const {
      vm.Write(offset, value, base);
    }
  };

  export struct StoreImmediate8  : public StoreImmediate<StoreImmediate8 , code::Bytecodes::StoreImmediate8 , uint8_t > { using Super::Super; };
  export struct StoreImmediate16 : public StoreImmediate<StoreImmediate16, code::Bytecodes::StoreImmediate16, uint16_t> { using Super::Super; };
  export struct StoreImmediate32 : public StoreImmediate<StoreImmediate32, code::Bytecodes::StoreImmediate32, uint32_t> { using Super::Super; };
  export struct StoreImmediate64 : public StoreImmediate<StoreImmediate64, code::Bytecodes::StoreImmediate64, uint64_t> { using Super::Super; };

  export template<typename Derived, code::Bytecodes type, typename Operation, typename T = uint64_t>
  struct Unary : public Template<Derived, type> {
    constexpr static auto Op = Operation{};
    using Super = Unary<Derived, type, Operation, T>;

    code::Registers dest;
    code::Registers source;

    Unary(code::Registers dest, code::Registers source)
      : Template<Derived, type>{}, dest{dest}, source{source} {}

    void Run(VM& vm) const {
      auto value = vm.GetReg<T>(source);
      vm.SetReg(dest, Op(value));
    }
  };

  export template<typename Derived, code::Bytecodes type, typename Operation, typename T = uint64_t>
  struct Binary : public Template<Derived, type> {
  private:
    constexpr static auto Op = Operation{};

    code::Registers lhs;
    code::Registers rhs;
    code::Registers result;
  public:
    using Super = Binary<Derived, type, Operation, T>;

    Binary(code::Registers lhs, code::Registers rhs, code::Registers result)
      : Template<Derived, type>{}, lhs{lhs}, rhs{rhs}, result{result} {}

    void Run(VM& vm) const {
      auto val1 = vm.GetReg<T>(lhs);
      auto val2 = vm.GetReg<T>(rhs);
      auto output = Op(val1, val2);
      utility::Print("Binary operation produced", output, "from", val1, val2);
      vm.SetReg(result, output);
    }
  };

  export struct AddU8 : public Binary<AddU8, code::Bytecodes::AddU8, std::plus<>, uint8_t> { using Super::Super; };
  export struct AddU16 : public Binary<AddU16, code::Bytecodes::AddU16, std::plus<>, uint16_t> { using Super::Super; };
  export struct AddU32 : public Binary<AddU32, code::Bytecodes::AddU32, std::plus<>, uint32_t> { using Super::Super; };
  export struct AddU64 : public Binary<AddU64, code::Bytecodes::AddU64, std::plus<>, uint64_t> { using Super::Super; };
  export struct AddI8 : public Binary<AddI8, code::Bytecodes::AddI8, std::plus<>, int8_t> { using Super::Super; };
  export struct AddI16 : public Binary<AddI16, code::Bytecodes::AddI16, std::plus<>, int16_t> { using Super::Super; };
  export struct AddI32 : public Binary<AddI32, code::Bytecodes::AddI32, std::plus<>, int32_t> { using Super::Super; };
  export struct AddI64 : public Binary<AddI64, code::Bytecodes::AddI64, std::plus<>, int64_t> { using Super::Super; };
  export struct AddF32 : public Binary<AddF32, code::Bytecodes::AddF32, std::plus<>, float> { using Super::Super; };
  export struct AddF64 : public Binary<AddF64, code::Bytecodes::AddF64, std::plus<>, double> { using Super::Super; };

  export struct SubtractU8 : public Binary<SubtractU8, code::Bytecodes::SubtractU8, std::minus<>, uint8_t> { using Super::Super; };
  export struct SubtractU16 : public Binary<SubtractU16, code::Bytecodes::SubtractU16, std::minus<>, uint16_t> { using Super::Super; };
  export struct SubtractU32 : public Binary<SubtractU32, code::Bytecodes::SubtractU32, std::minus<>, uint32_t> { using Super::Super; };
  export struct SubtractU64 : public Binary<SubtractU64, code::Bytecodes::SubtractU64, std::minus<>, uint64_t> { using Super::Super; };
  export struct SubtractI8 : public Binary<SubtractI8, code::Bytecodes::SubtractI8, std::minus<>, int8_t> { using Super::Super; };
  export struct SubtractI16 : public Binary<SubtractI16, code::Bytecodes::SubtractI16, std::minus<>, int16_t> { using Super::Super; };
  export struct SubtractI32 : public Binary<SubtractI32, code::Bytecodes::SubtractI32, std::minus<>, int32_t> { using Super::Super; };
  export struct SubtractI64 : public Binary<SubtractI64, code::Bytecodes::SubtractI64, std::minus<>, int64_t> { using Super::Super; };
  export struct SubtractF32 : public Binary<SubtractF32, code::Bytecodes::SubtractF32, std::minus<>, float> { using Super::Super; };
  export struct SubtractF64 : public Binary<SubtractF64, code::Bytecodes::SubtractF64, std::minus<>, double> { using Super::Super; };

  export struct MultiplyU8 : public Binary<MultiplyU8, code::Bytecodes::MultiplyU8, std::multiplies<>, uint8_t> { using Super::Super; };
  export struct MultiplyU16 : public Binary<MultiplyU16, code::Bytecodes::MultiplyU16, std::multiplies<>, uint16_t> { using Super::Super; };
  export struct MultiplyU32 : public Binary<MultiplyU32, code::Bytecodes::MultiplyU32, std::multiplies<>, uint32_t> { using Super::Super; };
  export struct MultiplyU64 : public Binary<MultiplyU64, code::Bytecodes::MultiplyU64, std::multiplies<>, uint64_t> { using Super::Super; };
  export struct MultiplyI8 : public Binary<MultiplyI8, code::Bytecodes::MultiplyI8, std::multiplies<>, int8_t> { using Super::Super; };
  export struct MultiplyI16 : public Binary<MultiplyI16, code::Bytecodes::MultiplyI16, std::multiplies<>, int16_t> { using Super::Super; };
  export struct MultiplyI32 : public Binary<MultiplyI32, code::Bytecodes::MultiplyI32, std::multiplies<>, int32_t> { using Super::Super; };
  export struct MultiplyI64 : public Binary<MultiplyI64, code::Bytecodes::MultiplyI64, std::multiplies<>, int64_t> { using Super::Super; };
  export struct MultiplyF32 : public Binary<MultiplyF32, code::Bytecodes::MultiplyF32, std::multiplies<>, float> { using Super::Super; };
  export struct MultiplyF64 : public Binary<MultiplyF64, code::Bytecodes::MultiplyF64, std::multiplies<>, double> { using Super::Super; };

  export struct DivideU8 : public Binary<DivideU8, code::Bytecodes::DivideU8, std::divides<>, uint8_t> { using Super::Super; };
  export struct DivideU16 : public Binary<DivideU16, code::Bytecodes::DivideU16, std::divides<>, uint16_t> { using Super::Super; };
  export struct DivideU32 : public Binary<DivideU32, code::Bytecodes::DivideU32, std::divides<>, uint32_t> { using Super::Super; };
  export struct DivideU64 : public Binary<DivideU64, code::Bytecodes::DivideU64, std::divides<>, uint64_t> { using Super::Super; };
  export struct DivideI8 : public Binary<DivideI8, code::Bytecodes::DivideI8, std::divides<>, int8_t> { using Super::Super; };
  export struct DivideI16 : public Binary<DivideI16, code::Bytecodes::DivideI16, std::divides<>, int16_t> { using Super::Super; };
  export struct DivideI32 : public Binary<DivideI32, code::Bytecodes::DivideI32, std::divides<>, int32_t> { using Super::Super; };
  export struct DivideI64 : public Binary<DivideI64, code::Bytecodes::DivideI64, std::divides<>, int64_t> { using Super::Super; };
  export struct DivideF32 : public Binary<DivideF32, code::Bytecodes::DivideF32, std::divides<>, float> { using Super::Super; };
  export struct DivideF64 : public Binary<DivideF64, code::Bytecodes::DivideF64, std::divides<>, double> { using Super::Super; };

  export struct ModuloU8 : public Binary<ModuloU8, code::Bytecodes::ModuloU8, std::modulus<>, uint8_t> { using Super::Super; };
  export struct ModuloU16 : public Binary<ModuloU16, code::Bytecodes::ModuloU16, std::modulus<>, uint16_t> { using Super::Super; };
  export struct ModuloU32 : public Binary<ModuloU32, code::Bytecodes::ModuloU32, std::modulus<>, uint32_t> { using Super::Super; };
  export struct ModuloU64 : public Binary<ModuloU64, code::Bytecodes::ModuloU64, std::modulus<>, uint64_t> { using Super::Super; };
  export struct ModuloI8 : public Binary<ModuloI8, code::Bytecodes::ModuloI8, std::modulus<>, int8_t> { using Super::Super; };
  export struct ModuloI16 : public Binary<ModuloI16, code::Bytecodes::ModuloI16, std::modulus<>, int16_t> { using Super::Super; };
  export struct ModuloI32 : public Binary<ModuloI32, code::Bytecodes::ModuloI32, std::modulus<>, int32_t> { using Super::Super; };
  export struct ModuloI64 : public Binary<ModuloI64, code::Bytecodes::ModuloI64, std::modulus<>, int64_t> { using Super::Super; };

  export struct FMod {
    template<typename T>
    T operator()(const T& a, const T& b) const {
      return std::fmod(a, b);
    }
  };

  export struct ModuloF32 : public Binary<ModuloF32, code::Bytecodes::ModuloF32, FMod, float> { using Super::Super; };
  export struct ModuloF64 : public Binary<ModuloF64, code::Bytecodes::ModuloF64, FMod, double> { using Super::Super; };

  export struct NegateI8 : public Unary<NegateI8, code::Bytecodes::NegateI8, std::negate<>, int8_t> { using Super::Super; };
  export struct NegateI32 : public Unary<NegateI32, code::Bytecodes::NegateI32, std::negate<>, int32_t> { using Super::Super; };
  export struct NegateI16 : public Unary<NegateI16, code::Bytecodes::NegateI16, std::negate<>, int16_t> { using Super::Super; };
  export struct NegateI64 : public Unary<NegateI64, code::Bytecodes::NegateI64, std::negate<>, int64_t> { using Super::Super; };
  export struct NegateF32 : public Unary<NegateF32, code::Bytecodes::NegateF32, std::negate<>, float> { using Super::Super; };
  export struct NegateF64 : public Unary<NegateF64, code::Bytecodes::NegateF64, std::negate<>, double> { using Super::Super; };

  export struct EqualU64 : public Binary<EqualU64, code::Bytecodes::EqualU64, std::equal_to<>, uint64_t> { using Super::Super; };
  export struct EqualF32 : public Binary<EqualF32, code::Bytecodes::EqualF32, std::equal_to<>, float> { using Super::Super; };
  export struct EqualF64 : public Binary<EqualF64, code::Bytecodes::EqualF64, std::equal_to<>, double> { using Super::Super; };

  export struct NotEqualU64 : public Binary<NotEqualU64, code::Bytecodes::NotEqualU64, std::not_equal_to<>, uint64_t> { using Super::Super; };
  export struct NotEqualF32 : public Binary<NotEqualF32, code::Bytecodes::NotEqualF32, std::not_equal_to<>, float> { using Super::Super; };
  export struct NotEqualF64 : public Binary<NotEqualF64, code::Bytecodes::NotEqualF64, std::not_equal_to<>, double> { using Super::Super; };

  export struct Greater : public Binary<Greater, code::Bytecodes::Greater, std::greater<>> { using Super::Super; };
  export struct Lesser : public Binary<Lesser, code::Bytecodes::Lesser, std::less<>> { using Super::Super; };
  export struct GreaterOrEqual : public Binary<GreaterOrEqual, code::Bytecodes::GreaterOrEqual, std::greater_equal<>> { using Super::Super; };
  export struct LesserOrEqual : public Binary<LesserOrEqual, code::Bytecodes::LesserOrEqual, std::less_equal<>> { using Super::Super; };

  export struct LogicalAnd : public Binary<LogicalAnd, code::Bytecodes::LogicalAnd, std::logical_and<>> { using Super::Super; };
  export struct LogicalOr : public Binary<LogicalOr, code::Bytecodes::LogicalOr, std::logical_or<>> { using Super::Super; };
  export struct LogicalNot : public Unary<LogicalNot, code::Bytecodes::LogicalNot, std::logical_not<>> { using Super::Super; };

  export struct BitwiseAnd : public Binary<BitwiseAnd, code::Bytecodes::BitwiseAnd, std::bit_and<>> { using Super::Super; };
  export struct BitwiseOr : public Binary<BitwiseOr, code::Bytecodes::BitwiseOr, std::bit_or<>> { using Super::Super; };
  export struct BitwiseExclusiveOr : public Binary<BitwiseExclusiveOr, code::Bytecodes::BitwiseExclusiveOr, std::bit_xor<>> { using Super::Super; };
  export struct BitwiseNot : public Unary<BitwiseNot, code::Bytecodes::BitwiseNot, std::bit_not<>> { using Super::Super; };
};