export module code.encoder;

import code.frame;
import code._register;
import code.registers;
import code.address;
import code.immediate;
import code.flag;
import code.cost;
import code.placeholder;
import code.relative;
import code.patch;
import utility.to_bytes;
import utility.buffer;

import <unordered_set>;
import <functional>;
import <vector>;
import <bitset>;
import <cstdint>;
import <cstddef>;
import <bit>;
import <ranges>;
import <span>;
import <limits>;
import <concepts>;

export namespace node {
  class Scope;
  class Block;
  class Declaration;
};

namespace node::scope {
  export class Context;
};

namespace code {
  export class Encoder {
  private:
    // std::unordered_map<node::Scope*, Frame> frames;
    // std::vector<std::byte> bytes;
    utility::Buffer instructions;
    utility::Buffer data;
    // code::Address instructions_address;
    // code::Address data_address;

    // std::vector<node::scope::Context*> contexts;

    // code::Frame global;
    std::vector<code::Frame*> stack;
    std::vector<code::Patch> patches;
    std::array<code::Address*, 64> registers;
    // std::vector<node::Declaration*> active_declarations;
    // std::unordered_set<code::Frame*> frames;
    // std::vector<code::Address> addresses;
    code::Registers active_registers;
    // code::Register null_register;
    // code::Address null_address;
    std::endian endian;
  public:
    Encoder();
    Encoder(std::endian endian);
    virtual ~Encoder() = default;

    utility::Buffer& Instructions() { return instructions; }
    utility::Buffer& Data() { return data; }
    std::vector<code::Patch>& Patches() { return patches; }
    std::array<code::Address*, 64>& Registers() { return registers; }
    code::Address* Register(size_t index) { return registers[index]; }
    code::Address* Register(uint8_t index) { return registers[index]; }

    size_t RIP() const;
    size_t RSP() const;

    bool IsRegisterActive(uint8_t index) const { return active_registers.Test(index); }
    void SetRegisterActive(uint8_t index) { active_registers.Set(index); }
    const std::vector<node::Declaration*>& ActiveDeclarations() const;
    const code::Registers& ActiveRegisters() const { return active_registers; }

    // size_t Size() const;
    std::endian Endian() const { return endian; }
    // const std::vector<std::byte>& Bytes() const;
    // const std::vector<code::Address>& Addresses() const;
    // const std::vector<node::Declaration*>& LiveDeclarations() const;
    // void PushLiveDeclaration(node::Declaration& declaration);
    // void PopLiveDeclaration(node::Declaration& declaration);

    // code::Placeholder8 PH8() { return {Top(), endian}; }
    // code::Placeholder16 PH16() { return {Top(), endian}; }
    // code::Placeholder32 PH32() { return {Top(), endian}; }
    // code::Placeholder64 PH64() { return {Top(), endian}; }

    // code::Placeholder8 PH8(int8_t offset, uint8_t shift) { return {Top(), endian, offset, shift}; }
    // code::Placeholder16 PH16(int8_t offset, uint8_t shift) { return {Top(), endian, offset, shift}; }
    // code::Placeholder32 PH32(int8_t offset, uint8_t shift) { return {Top(), endian, offset, shift}; }
    // code::Placeholder64 PH64(int8_t offset, uint8_t shift) { return {Top(), endian, offset, shift}; }

    void Label(code::Placeholder8 ph) {
      // code::Frame& top = Top();
      // top.Emit8(top.Size() - ph.Index(), endian, ph.Index(), ph.Shift());
    }

    void Label(code::Placeholder16 ph) {
      // code::Frame& top = Top();
      // top.Emit16(top.Size() - ph.Index(), endian, ph.Index(), ph.Shift());
    }

    void Label(code::Placeholder32 ph) {
      // code::Frame& top = Top();
      // top.Emit32(top.Size() - ph.Index(), endian, ph.Index(), ph.Shift());
    }

    void Label(code::Placeholder64 ph) {
      // code::Frame& top = Top();
      // top.Emit64(top.Size() - ph.Index(), endian, ph.Index(), ph.Shift());
    }

    void Label(code::Immediate& i) {
      // code::Frame& top = Top();
      // top.Emit8(i.Value(), endian, i.Index(), 0);
    }

    // void Label(code::Immediate8& i) {
    //   code::Frame& top = Top();
    //   top.Emit8(i.Value(), endian, i.Index(), 0);
    // }

    // void Label(code::Immediate16& i) {
    //   code::Frame& top = Top();
    //   top.Emit16(i.Value(), endian, i.Index(), 0);
    // }

    // void Label(code::Immediate32& i) {
    //   code::Frame& top = Top();
    //   top.Emit32(i.Value(), endian, i.Index(), 0);
    // }

    // void Label(code::Immediate64& i) {
    //   code::Frame& top = Top();
    //   top.Emit64(i.Value(), endian, i.Index(), 0);
    // }

    void Place(code::Relative8& r) {
      // code::Frame& top = Top();
      // top.Emit8(top.Size() - r.Index(), endian, r.Index(), 0);
    }

    void Place(code::Relative16& r) {
      // code::Frame& top = Top();
      // top.Emit16(top.Size() - r.Index(), endian, r.Index(), 0);
    }

    void Place(code::Relative32& r) {
      // code::Frame& top = Top();
      // top.Emit32(top.Size() - r.Index(), endian, r.Index(), 0);
    }

    void Place(code::Relative64& r) {
      // code::Frame& top = Top();
      // top.Emit64(top.Size() - r.Index(), endian, r.Index(), 0);
    }

    // std::vector<std::byte> Compile();
    // void Save(const code::Address& address);
    virtual void Patch() = 0;
    virtual void Print();
    virtual void Execute();

    virtual size_t MaxSizeGPR() const = 0;
    virtual size_t MaxSizeSIMD() const = 0;
    virtual size_t TotalGPR() const = 0;
    virtual size_t TotalSIMD() const = 0;
    virtual size_t TotalRegisters() const = 0;

    // code::Frame& Frame(node::Scope& scope);
    code::Frame& Top();
    void Open(code::Frame& frame);
    void Close(code::Frame& frame);

    virtual void Declare(node::Declaration& declaration) = 0;
    // virtual void Collide(node::Block& block, node::Block& previous) = 0;

    virtual void Add(code::Cost& cost) = 0;
    virtual void Subtract(code::Cost& cost) = 0;
    virtual void Multiply(code::Cost& cost) = 0;
    virtual void Divide(code::Cost& cost) = 0;

    virtual void Allocate(code::Register& res, code::Register& lhs) = 0;
    virtual void Allocate(code::Register& res, code::Address& lhs) = 0;
    virtual void Allocate(code::Address& res, code::Register& lhs) = 0;
    virtual void Allocate(code::Address& res, code::Address& lhs) = 0;
    virtual void Allocate(code::Register& res, code::Register& lhs, code::Register& rhs) = 0;
    virtual void Allocate(code::Register& res, code::Register& lhs, code::Address& rhs) = 0;
    virtual void Allocate(code::Register& res, code::Address& lhs, code::Register& rhs) = 0;
    virtual void Allocate(code::Register& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void Allocate(code::Address& res, code::Register& lhs, code::Register& rhs) = 0;
    virtual void Allocate(code::Address& res, code::Register& lhs, code::Address& rhs) = 0;
    virtual void Allocate(code::Address& res, code::Address& lhs, code::Register& rhs) = 0;
    virtual void Allocate(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // Convert to float
    virtual void ConvertToF32(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToF32(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToF32(code::Register& dest, code::Address& src) = 0;

    // Convert to double
    virtual void ConvertToF64(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToF64(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToF64(code::Register& dest, code::Address& src) = 0;

    // Convert to signed integer
    virtual void ConvertToI8(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToI8(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToI8(code::Register& dest, code::Address& src) = 0;

    virtual void ConvertToI16(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToI16(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToI16(code::Register& dest, code::Address& src) = 0;

    virtual void ConvertToI32(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToI32(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToI32(code::Register& dest, code::Address& src) = 0;

    virtual void ConvertToI64(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToI64(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToI64(code::Register& dest, code::Address& src) = 0;

    // Convert to unsigned integer
    virtual void ConvertToU8(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToU8(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToU8(code::Register& dest, code::Address& src) = 0;

    virtual void ConvertToU16(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToU16(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToU16(code::Register& dest, code::Address& src) = 0;

    virtual void ConvertToU32(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToU32(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToU32(code::Register& dest, code::Address& src) = 0;

    virtual void ConvertToU64(code::Address& dest, code::Address& src) = 0;
    virtual void ConvertToU64(code::Register& dest, code::Register& src) = 0;
    virtual void ConvertToU64(code::Register& dest, code::Address& src) = 0;

    virtual void ByteSwap(code::Register& src) = 0;
    virtual void ByteSwap(code::Address& src) = 0;

    virtual void Allocate(code::Address& address) = 0;
    virtual void Deallocate(code::Address& address) = 0;

    // Arithmetic operations
    // res = lhs + rhs;
    virtual void AddIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void AddDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs - rhs;
    virtual void SubtractIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void SubtractDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs * rhs;
    virtual void MultiplyIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void MultiplySignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void MultiplyDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs / rhs;
    virtual void DivideIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void DivideSignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void DivideDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs % rhs;
    virtual void ModuloIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void ModuloSignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;
    virtual void ModuloDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // ++value
    virtual void IncrementInteger(code::Address& value) = 0;
    virtual void IncrementDecimal(code::Address& value) = 0;

    // --value
    virtual void DecrementInteger(code::Address& value) = 0;
    virtual void DecrementDecimal(code::Address& value) = 0;

    // Bitwise operations
    // res = lhs & rhs
    virtual void BitwiseAnd(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs | rhs
    virtual void BitwiseOr(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs ^ rhs
    virtual void BitwiseXor(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs >> rhs;
    virtual void BitwiseShiftLeft(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs << rhs;
    virtual void BitwiseShiftRight(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = ~lhs;
    virtual void BitwiseNot(code::Address& res, code::Address& lhs) = 0;

    virtual void BitwiseTest(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // Logical operations
    // res = !lhs;
    virtual void LogicalNot(code::Address& res, code::Address& lhs) = 0;

    // res = lhs && rhs;
    virtual void LogicalAnd(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs || rhs;
    virtual void LogicalOr(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // Comparison operations
    // res = lhs == rhs;
    virtual void LogicalEqual(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs != rhs;
    virtual void LogicalNotEqual(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs > rhs;
    virtual void LogicalGreaterThan(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs < rhs;
    virtual void LogicalLessThan(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs >= rhs;
    virtual void LogicalGreaterThanOrEqual(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs <= rhs;
    virtual void LogicalLessThanOrEqual(code::Address& res, code::Address& lhs, code::Address& rhs) = 0;

    // Data movement
    virtual void Exchange(code::Address& lhs, code::Address& rhs) = 0;

    // res = lhs;
    virtual void Move(code::Address& res, code::Address& lhs) = 0;

    // if (cond) res = lhs;
    virtual void ConditionalMove(code::Address& cond, code::Address& res, code::Address& lhs) = 0;

    // Stack operations
    virtual void Push(code::Address& a) = 0;
    virtual void Pop(code::Address& a) = 0;

    // Control flow
    // goto target;
    virtual void Jump(code::Address& target) = 0;

    // if (cond) goto target;
    virtual void ConditionalJump(code::Address& cond, code::Address& target) = 0;

    // target();
    virtual void Call(code::Address& target) = 0;

    // if (cond) target();
    virtual void ConditionalCall(code::Address& cond, code::Address& target) = 0;

    // return
    virtual void Return() = 0;
    // virtual void Return(code::Register& value) = 0;
    // virtual void Return(code::Address& value) = 0;

    // if (cond) return;
    virtual void ConditionalReturn() = 0;
    // virtual void ConditionalReturn(code::Register& value) = 0;
    // virtual void ConditionalReturn(code::Address& value) = 0;

    virtual void Prologue() = 0;
    virtual void Epilogue() = 0;

    // void Emit(std::byte byte) { stack.back().Emit(byte); }

    // template<size_t S>
    // void Emit(std::span<std::byte, S> data) { stack.back().Emit(data, endian); }

    // void Emit(std::ranges::contiguous_range auto& value) { Emit(utility::ToBytes(value)); }
    // void Emit(int8_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit(uint8_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit(int16_t value) { Emit(utility::ToBytes(value)); }
    // void Emit(uint16_t value) { Emit(utility::ToBytes(value)); }
    // void Emit(int32_t value) { Emit(utility::ToBytes(value)); }
    // void Emit(uint32_t value) { Emit(utility::ToBytes(value)); }
    // void Emit(int64_t value) { Emit(utility::ToBytes(value)); }
    // void Emit(uint64_t value) { Emit(utility::ToBytes(value)); }

    // // void Emit32(int32_t value) { Emit(utility::ToBytes(static_cast<uint32_t>(value))); }
    // // void Emit32(uint32_t value) { Emit(utility::ToBytes(static_cast<uint32_t>(value))); }

    // void Emit8(bool value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(int8_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(uint8_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(int16_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(uint16_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(int32_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(uint32_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(int64_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(uint64_t value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(float value) { Emit(static_cast<std::byte>(value)); }
    // void Emit8(double value) { Emit(static_cast<std::byte>(value)); }

    // void Emit16(bool value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(int8_t value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(uint8_t value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(int16_t value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(uint16_t value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(int32_t value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(uint32_t value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(int64_t value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(uint64_t value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(float value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit16(double value) { Emit(static_cast<uint16_t>(value)); }

    // void Emit32(bool value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(int8_t value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(uint8_t value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(int16_t value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(uint16_t value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(int32_t value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(uint32_t value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(int64_t value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(uint64_t value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(float value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit32(double value) { Emit(static_cast<uint32_t>(value)); }

    // void Emit64(bool value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(int8_t value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(uint8_t value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(int16_t value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(uint16_t value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(int32_t value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(uint32_t value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(int64_t value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(uint64_t value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(float value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit64(double value) { Emit(static_cast<uint64_t>(value)); }

    // // These are useful for enforcing a specific size
    // // void Emit8(std::integral auto value) { Emit(static_cast<std::byte>(value)); }
    // void Emit16(std::integral auto value) { Emit(static_cast<uint16_t>(value)); }
    // void Emit32(std::integral auto value) { Emit(static_cast<uint32_t>(value)); }
    // void Emit64(std::integral auto value) { Emit(static_cast<uint64_t>(value)); }
    // void Emit32(std::floating_point auto value) { Emit(static_cast<float>(value)); }
    // void Emit64(std::floating_point auto value) { Emit(static_cast<double>(value)); }

    template <size_t Alignment>
    constexpr size_t Align(size_t size) {
      static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be a power of 2");
      return (size + (Alignment - 1)) & ~(Alignment - 1);
    }

    constexpr size_t Align2(size_t size) { return Align<2>(size); }
    constexpr size_t Align4(size_t size) { return Align<4>(size); }
    constexpr size_t Align8(size_t size) { return Align<8>(size); }
    constexpr size_t Align16(size_t size) { return Align<16>(size); }
    constexpr size_t Align32(size_t size) { return Align<32>(size); }
    constexpr size_t Align64(size_t size) { return Align<64>(size); }
  };
};