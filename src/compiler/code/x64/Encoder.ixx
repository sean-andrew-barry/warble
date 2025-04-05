export module code.x64.encoder;

import code.encoder;
import code.operand;
import code.placeholder;
import code.flag;
import code.x64._register;
import code.x64.memory;
import code.x64.memory_offset;
import code.x64.immediate;
import code.x64.relative;
import code.address;
import code._register;
import code.immediate;
import code.relative;
import utility.print;
// import utility.os;
import utility.macros;
import utility.to_bytes;
import <cstdint>;
import <vector>;
import <type_traits>;
import <concepts>;
import <bitset>;
import <iostream>;
import <iomanip>;
import <stdexcept>;
import <memory>;
import <span>;
import <ranges>;
import <array>;
import <typeinfo>;
import <bit>;
import <variant>;
import <unordered_map>;
import <initializer_list>;
import <utility>;

template<typename T>
concept IsPrimitive = std::integral<T> || std::floating_point<T> || std::same_as<T, char8_t> || std::same_as<T, char16_t> || std::same_as<T, char32_t> || std::same_as<T, wchar_t>;

template<typename T>
concept IsPrimitiveByte = IsPrimitive<T> && sizeof(T) == 1;

template<typename T>
concept IsPrimitiveMultiByte = IsPrimitive<T> && sizeof(T) > 1;

namespace node {
  export class Declaration;
};

template<typename T>
concept IsR8 = std::same_as<T, code::x64::SGPR8> || std::same_as<T, code::x64::EGPR8>;

template<typename T>
concept IsR16 = std::same_as<T, code::x64::SGPR16> || std::same_as<T, code::x64::EGPR16>;

template<typename T>
concept IsR32 = std::same_as<T, code::x64::SGPR32> || std::same_as<T, code::x64::EGPR32>;

template<typename T>
concept IsR64 = std::same_as<T, code::x64::SGPR64> || std::same_as<T, code::x64::EGPR64>;

template<typename T>
concept IsRM = std::same_as<T, code::Register> || std::same_as<T, code::Address>;

template<typename T>
concept IsRMI = std::same_as<T, code::Register> || std::same_as<T, code::Address> || std::same_as<T, code::Immediate>;

template<typename T>
concept IsAddressOrUint8 = std::same_as<T, code::Address&> || std::same_as<T, uint8_t>;

namespace code::x64 {
  constexpr auto TARGET = std::endian::little;

  export class Encoder : public code::Encoder {
  private:
    std::bitset<16> gpr = 0b0001100000000000;
    std::bitset<8> sgpr = 0b00011000;
    std::bitset<8> egpr;
    uint8_t rflags{255};
    // code::Register temp_gpr{r15()}; // A reserved register
    // code::Register temp_sse{xMM15()}; // A reserved register
    // code::Register rsp{rSP()}; // A reserved register
    // code::Register rbp{rBP()}; // A reserved register

    const code::Address zero{code::Flag::Sizes() | code::Flag::IMMEDIATE(), 0};
    const code::Address one{code::Flag::Sizes() | code::Flag::IMMEDIATE(), 1};

    std::array<std::optional<uint64_t>, 32> register_values;

    code::Address rax{code::Flag::Sizes(), 0, uint8_t{0}};
    code::Address rcx{code::Flag::Sizes(), 0, uint8_t{1}};
    code::Address rdx{code::Flag::Sizes(), 0, uint8_t{2}};
    code::Address rbx{code::Flag::Sizes(), 0, uint8_t{3}};
    code::Address rsp{code::Flag::Sizes(), 0, uint8_t{4}};
    code::Address rbp{code::Flag::Sizes(), 0, uint8_t{5}};
    code::Address rsi{code::Flag::Sizes(), 0, uint8_t{6}};
    code::Address rdi{code::Flag::Sizes(), 0, uint8_t{7}};
    code::Address r8{code::Flag::Sizes(), 0, uint8_t{8}};
    code::Address r9{code::Flag::Sizes(), 0, uint8_t{9}};
    code::Address r10{code::Flag::Sizes(), 0, uint8_t{10}};
    code::Address r11{code::Flag::Sizes(), 0, uint8_t{11}};
    code::Address r12{code::Flag::Sizes(), 0, uint8_t{12}};
    code::Address r13{code::Flag::Sizes(), 0, uint8_t{13}};
    code::Address r14{code::Flag::Sizes(), 0, uint8_t{14}};
    code::Address r15{code::Flag::Sizes(), 0, uint8_t{15}};
    code::Address xmm0{code::Flag::Sizes(), 0, uint8_t{16}};
    code::Address xmm1{code::Flag::Sizes(), 0, uint8_t{17}};
    code::Address xmm2{code::Flag::Sizes(), 0, uint8_t{18}};
    code::Address xmm3{code::Flag::Sizes(), 0, uint8_t{19}};
    code::Address xmm4{code::Flag::Sizes(), 0, uint8_t{20}};
    code::Address xmm5{code::Flag::Sizes(), 0, uint8_t{21}};
    code::Address xmm6{code::Flag::Sizes(), 0, uint8_t{22}};
    code::Address xmm7{code::Flag::Sizes(), 0, uint8_t{23}};
    code::Address xmm8{code::Flag::Sizes(), 0, uint8_t{24}};
    code::Address xmm9{code::Flag::Sizes(), 0, uint8_t{25}};
    code::Address xmm10{code::Flag::Sizes(), 0, uint8_t{26}};
    code::Address xmm11{code::Flag::Sizes(), 0, uint8_t{27}};
    code::Address xmm12{code::Flag::Sizes(), 0, uint8_t{28}};
    code::Address xmm13{code::Flag::Sizes(), 0, uint8_t{29}};
    code::Address xmm14{code::Flag::Sizes(), 0, uint8_t{30}};
    code::Address xmm15{code::Flag::Sizes(), 0, uint8_t{31}};

    const code::Register* carry_flag{nullptr};
    const code::Register* parity_flag{nullptr};
    const code::Register* auxiliary_carry_flag{nullptr};
    const code::Register* zero_flag{nullptr};
    const code::Register* sign_flag{nullptr};
    const code::Register* trap_flag{nullptr};
    const code::Register* interrupt_enable_flag{nullptr};
    const code::Register* direction_flag{nullptr};
    const code::Register* overflow_flag{nullptr};

    static constexpr uint8_t REX_W = 0x08;
    static constexpr uint8_t REX_R = 0x04;
    static constexpr uint8_t REX_X = 0x02;
    static constexpr uint8_t REX_B = 0x01;
    static constexpr std::bitset<16> CF{0x0001}; // Carry flag
    static constexpr std::bitset<16> PF{0x0004}; // Parity flag
    static constexpr std::bitset<16> AF{0x0010}; // Auxiliary carry flag
    static constexpr std::bitset<16> ZF{0x0040}; // Zero flag
    static constexpr std::bitset<16> SF{0x0080}; // Sign flag
    static constexpr std::bitset<16> TF{0x0100}; // Trap flag (IDK what this is used for)
    static constexpr std::bitset<16> IF{0x0200}; // Interrupt enable flag
    static constexpr std::bitset<16> DF{0x0400}; // Direction flag
    static constexpr std::bitset<16> OF{0x0800}; // Overflow flag

    // void Assign(node::Block& block, node::Declaration& declaration);

    void SpillRegister(node::Declaration& declaration, code::Registers& failed);
    bool SelectRegister(node::Declaration& declaration, code::Registers& failed);
    void CollideRegister(node::Declaration& declaration, code::Registers& failed);
    void AllocateRegister(node::Declaration& declaration, code::Registers& failed);

    void ClearCF() { carry_flag = nullptr; }
    void ClearPF() { parity_flag = nullptr; }
    void ClearAF() { auxiliary_carry_flag = nullptr; }
    void ClearZF() { zero_flag = nullptr; }
    void ClearSF() { sign_flag = nullptr; }
    void ClearDF() { direction_flag = nullptr; }
    void ClearOF() { overflow_flag = nullptr; }

    void SetCF(const code::Register& r) { carry_flag = &r; }
    void SetPF(const code::Register& r) { parity_flag = &r; }
    void SetAF(const code::Register& r) { auxiliary_carry_flag = &r; }
    void SetZF(const code::Register& r) { zero_flag = &r; }
    void SetSF(const code::Register& r) { sign_flag = &r; }
    void SetDF(const code::Register& r) { direction_flag = &r; }
    void SetOF(const code::Register& r) { overflow_flag = &r; }

    void TestCF(const code::Register& r) {
      if (carry_flag != &r) {
        throw utility::Error("Carry flag (CF) not set by the expected register");
      }
    }

    void TestPF(const code::Register& r) {
      if (parity_flag != &r) {
        throw utility::Error("Parity flag (PF) not set by the expected register");
      }
    }

    void TestAF(const code::Register& r) {
      if (auxiliary_carry_flag != &r) {
        throw utility::Error("Auxiliary carry flag (AF) not set by the expected register");
      }
    }

    void TestZF(const code::Register& r) {
      if (zero_flag != &r) {
        throw utility::Error("Zero flag (ZF) not set by the expected register");
      }
    }

    void TestSF(const code::Register& r) {
      if (sign_flag != &r) {
        throw utility::Error("Sign flag (SF) not set by the expected register");
      }
    }

    void TestDF(const code::Register& r) {
      if (direction_flag != &r) {
        throw utility::Error("Direction flag (DF) not set by the expected register");
      }
    }

    void TestOF(const code::Register& r) {
      if (overflow_flag != &r) {
        throw utility::Error("Overflow flag (OF) not set by the expected register");
      }
    }

    void SetCMP(const code::Register& r) {
      SetAF(r);
      SetCF(r);
      SetOF(r);
      SetPF(r);
      SetSF(r);
      SetZF(r);
    }

    void SetMOV(const code::Register& r) {}
    
    void SetCMOVZ(const code::Register& r) { TestZF(r); }

    void SetOR(const code::Register& r) {
      ClearOF();
      ClearCF();
      ClearAF();
      SetSF(r);
      SetZF(r);
      SetPF(r);
    }

    code::Register FindRegisterFor(const code::Register& r) {
      return code::Register{0}; // RAX
    }

    code::Register FindRegisterFor(const code::Address& r) {
      return code::Register{0}; // RAX
    }

    void FreeRegister(const code::Register& r) {

    }

    bool Same(const code::Register& a, const code::Register& b) { return &a == &b; }
    bool Same(const code::Address& a, const code::Address& b) { return &a == &b; }
    bool Same(const code::Register& a, const code::Address& b) { return false; }
    bool Same(const code::Address& a, const code::Register& b) { return false; }

    // static constexpr uint8_t rAX{0};
    // static constexpr uint8_t rCX{1};
    // static constexpr uint8_t rDX{2};
    // static constexpr uint8_t rBX{3};
    // static constexpr uint8_t rSP{4};
    // static constexpr uint8_t rBP{5};
    // static constexpr uint8_t rSI{6};
    // static constexpr uint8_t rDI{7};
    // static constexpr uint8_t r8{8};
    // static constexpr uint8_t r9{9};
    // static constexpr uint8_t r10{10};
    // static constexpr uint8_t r11{11};
    // static constexpr uint8_t r12{12};
    // static constexpr uint8_t r13{13};
    // static constexpr uint8_t r14{14};
    // static constexpr uint8_t r15{15};
    // static constexpr uint8_t xMM0{16};
    // static constexpr uint8_t xMM1{17};
    // static constexpr uint8_t xMM2{18};
    // static constexpr uint8_t xMM3{19};
    // static constexpr uint8_t xMM4{20};
    // static constexpr uint8_t xMM5{21};
    // static constexpr uint8_t xMM6{22};
    // static constexpr uint8_t xMM7{23};
    // static constexpr uint8_t xMM8{24};
    // static constexpr uint8_t xMM9{25};
    // static constexpr uint8_t xMM10{26};
    // static constexpr uint8_t xMM11{27};
    // static constexpr uint8_t xMM12{28};
    // static constexpr uint8_t xMM13{29};
    // static constexpr uint8_t xMM14{30};
    // static constexpr uint8_t xMM15{31};
    // static constexpr uint8_t rIP{std::numeric_limits<uint8_t>::max() - 1};
    // static constexpr uint8_t NONE{std::numeric_limits<uint8_t>::max()};

    static constexpr auto S8 = Flag::SIZE8();
    static constexpr auto S16 = Flag::SIZE16();
    static constexpr auto S16_8 = Flag::SIZE16() | Flag::SIZE8();
    static constexpr auto S32 = Flag::SIZE32();
    static constexpr auto S64 = Flag::SIZE64();
    static constexpr auto S64_8 = Flag::SIZE64() | Flag::SIZE8();
    static constexpr auto S64_16 = Flag::SIZE64() | Flag::SIZE16();
    static constexpr auto S64_32 = Flag::SIZE64() | Flag::SIZE32();
    using List = std::initializer_list<std::pair<Flag, Flag>>;

    static constexpr List SAME{{S64, S64}, {S32, S32}, {S16, S16}, {S8, S8}};
    static constexpr List SAME_FROM_8{{S64, S64}, {S32, S32}, {S16, S16}, {S8, S8}};
    static constexpr List SAME_FROM_16{{S64, S64}, {S32, S32}, {S16, S16}};
    static constexpr List IMM{{S64, S32}, {S32, S32}, {S16, S16}, {S8, S8}};

    template<bool lockable, bool escape, bool use_id>
    void EncodeHelper(std::string_view name, uint8_t opcode, List flags, auto& dest, auto& src) {
      if (flags.size() % 2 != 0) {
        throw utility::Error("Instruction", name, "got an odd number of pairs in its flags list");
      }

      code::Flag dest_size;
      code::Flag src_size;

      for (auto [first, second] : flags) {
        if (dest.Any(first) && src.Any(second)) {
          dest_size = first;
          src_size = second;
        }
      }

      if (!dest_size || !src_size) {
        throw utility::Error("Instruction", name, "failed to match a size for the destination operand", dest);
      }

      if constexpr (lockable) {
        if (dest.IsAtomic()) Lock();
      }

      if constexpr (use_id) {
        opcode = static_cast<uint8_t>(opcode + (dest.ID() & 7));
      }

      OpSize(dest_size, src_size); // Emit 0x66 if 16-bit
      REX(dest, src, dest_size, src_size);

      if constexpr (escape) Esc(opcode);
      else OP(opcode);

      ModRM(dest, src, dest_size, src_size);

      // if constexpr (std::same_as<decltype(src), code::Immediate>) Emit(src);
    }

    void EncodeDefault(auto&&... args) {
      return EncodeHelper<false, false, false>(std::forward<decltype(args)>(args)...);
    }

    void EncodeWithID(auto&&... args) {
      return EncodeHelper<false, false, true>(std::forward<decltype(args)>(args)...);
    }

    void EncodeEsc(auto&&... args) {
      return EncodeHelper<false, true, false>(std::forward<decltype(args)>(args)...);
    }

    void EncodeLock(auto&&... args) {
      return EncodeHelper<true, false, false>(std::forward<decltype(args)>(args)...);
    }

    void EncodeLockEsc(auto&&... args) {
      return EncodeHelper<true, true, false>(std::forward<decltype(args)>(args)...);
    }
  public:
    Encoder();

    virtual void Patch() final;

    virtual size_t MaxSizeGPR() const final;
    virtual size_t MaxSizeSIMD() const final;
    virtual size_t TotalGPR() const final;
    virtual size_t TotalSIMD() const final;
    virtual size_t TotalRegisters() const final;

    void Allocate(code::Address& address) final;
    void Deallocate(code::Address& address) final;

    // Convert to float
    void ConvertToF32(code::Address& dest, code::Address& src) final;

    // Convert to double
    void ConvertToF64(code::Address& dest, code::Address& src) final;

    // Convert to signed integer
    void ConvertToI8(code::Address& dest, code::Address& src) final;
    void ConvertToI16(code::Address& dest, code::Address& src) final;
    void ConvertToI32(code::Address& dest, code::Address& src) final;
    void ConvertToI64(code::Address& dest, code::Address& src) final;

    // Convert to unsigned integer
    void ConvertToU8(code::Address& dest, code::Address& src) final;
    void ConvertToU16(code::Address& dest, code::Address& src) final;
    void ConvertToU32(code::Address& dest, code::Address& src) final;
    void ConvertToU64(code::Address& dest, code::Address& src) final;

    void ByteSwap(code::Address& src) final;

    // Arithmetic operations
    // res = lhs + rhs;
    void AddIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) final;
    void AddDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs - rhs;
    void SubtractIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) final;
    void SubtractDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs * rhs;
    void MultiplyIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) final;
    void MultiplySignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) final;
    void MultiplyDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs / rhs;
    void DivideIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) final;
    void DivideSignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) final;
    void DivideDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs % rhs;
    void ModuloIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) final;
    void ModuloSignedIntegers(code::Address& res, code::Address& lhs, code::Address& rhs) final;
    void ModuloDecimals(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // ++value
    void IncrementInteger(code::Address& value) final;
    void IncrementDecimal(code::Address& value) final;

    // --value
    void DecrementInteger(code::Address& value) final;
    void DecrementDecimal(code::Address& value) final;

    // Bitwise operations
    // res = lhs & rhs
    void BitwiseAnd(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs | rhs
    void BitwiseOr(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs ^ rhs
    void BitwiseXor(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs >> rhs;
    void BitwiseShiftLeft(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs << rhs;
    void BitwiseShiftRight(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = ~lhs;
    void BitwiseNot(code::Address& res, code::Address& lhs) final;

    void BitwiseTest(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // Logical operations
    // res = !lhs;
    void LogicalNot(code::Address& res, code::Address& lhs) final;

    // res = lhs && rhs;
    void LogicalAnd(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs || rhs;
    void LogicalOr(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // Comparison operations
    // res = lhs == rhs;
    void LogicalEqual(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs != rhs;
    void LogicalNotEqual(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs > rhs;
    void LogicalGreaterThan(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs < rhs;
    void LogicalLessThan(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs >= rhs;
    void LogicalGreaterThanOrEqual(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // res = lhs <= rhs;
    void LogicalLessThanOrEqual(code::Address& res, code::Address& lhs, code::Address& rhs) final;

    // Data movement
    void Exchange(code::Address& lhs, code::Address& rhs) final;

    // res = lhs;
    void Move(code::Address& res, code::Address& lhs) final;

    // if (cond) res = lhs;
    void ConditionalMove(code::Address& cond, code::Address& res, code::Address& lhs) final;

    // Stack operations
    void Push(code::Address& a) final;
    void Pop(code::Address& a) final;

    // Control flow
    // goto target;
    void Jump(code::Address& target) final;

    // if (cond) goto target;
    void ConditionalJump(code::Address& cond, code::Address& target) final;

    // target();
    void Call(code::Address& target) final;

    // if (cond) target();
    void ConditionalCall(code::Address& cond, code::Address& target) final;

    // return
    void Return() final;

    // if (cond) return;
    void ConditionalReturn() final;

    void Prologue() final;
    void Epilogue() final;

    void Displacement(int32_t displacement, uint8_t mod, bool is_bp);

    void Emit(code::Relative8& r) { r.Index(RIP()); Instructions().Emit8(r.Value()); }
    void Emit(code::Relative16& r) { r.Index(RIP()); Instructions().Emit16(r.Value()); }
    void Emit(code::Relative32& r) { r.Index(RIP()); Instructions().Emit32(r.Value()); }
    // void Emit(const code::Relative8& r) { Emit8(r.Value()); }
    // void Emit(const code::Relative16& r) { Emit16(r.Value()); }
    // void Emit(const code::Relative32& r) { Emit32(r.Value()); }
    // void Emit(Relative8 r) { Emit8(r.Value()); }
    // void Emit(Relative16 r) { Emit16(r.Value()); }
    // void Emit(Relative32 r) { Emit32(r.Value()); }
    // void Emit(MemoryOffset8 r) { Emit8(r.Value()); }
    // void Emit(MemoryOffset16 r) { Emit16(r.Value()); }
    // void Emit(MemoryOffset32 r) { Emit32(r.Value()); }
    // void Emit(MemoryOffset64 r) { Emit64(r.Value()); } // TODO check that this is right
    // void Emit(Immediate8 r) { Emit8(r.Value()); }
    // void Emit(Immediate16 r) { Emit16(r.Value()); }
    // void Emit(Immediate32 r) { Emit32(r.Value()); }
    // void Emit(Immediate64 r) { Emit64(r.Value()); }

    // void Emit(code::Immediate8& r) { r.Index(Top().Size()); Emit8(r.Value()); }
    // void Emit(code::Immediate16& r) { r.Index(Top().Size()); Emit16(r.Value()); }
    // void Emit(code::Immediate32& r) { r.Index(Top().Size()); Emit32(r.Value()); }
    // void Emit(code::Immediate64& r) { r.Index(Top().Size()); Emit64(r.Value()); }
    // void Emit(const code::Immediate8& r) { Emit8(r.Value()); }
    // void Emit(const code::Immediate16& r) { Emit16(r.Value()); }
    // void Emit(const code::Immediate32& r) { Emit32(r.Value()); }
    // void Emit(const code::Immediate64& r) { Emit64(r.Value()); }

    // void Emit(const code::Immediate& i, Flag dest_size, Flag src_size) {
    //   if (src_size.Has(Flag::SIZE8())) code::Encoder::Emit8(i.Value());
    //   else if (src_size.Has(Flag::SIZE16())) code::Encoder::Emit16(i.Value());
    //   else if (src_size.Has(Flag::SIZE32())) code::Encoder::Emit32(i.Value());
    //   else if (src_size.Has(Flag::SIZE64())) code::Encoder::Emit64(i.Value());
    // }

    // void Emit(SignedImmediate8 r) { Emit8(r.Value()); }
    // void Emit(SignedImmediate16 r) { Emit16(r.Value()); }
    // void Emit(SignedImmediate32 r) { Emit32(r.Value()); }
    // void Emit(SignedImmediate64 r) { Emit64(r.Value()); }

    // void Emit8(Immediate8 i) { code::Encoder::Emit8(i.Value()); }
    // void Emit16(Immediate16 i) { code::Encoder::Emit16(i.Value()); }
    // void Emit32(Immediate32 i) { code::Encoder::Emit32(i.Value()); }
    // void Emit64(Immediate64 i) { code::Encoder::Emit64(i.Value()); }
    // void Emit8(const Immediate8& i);
    // void Emit8(const code::Immediate& i);
    // void Emit16(const Immediate16& i);
    // void Emit16(const code::Immediate& i);
    // void Emit32(const Immediate32& i);
    // void Emit32(const code::Immediate& i);
    // void Emit64(const Immediate64& i);
    // void Emit64(const code::Immediate& i);

    void SIMD();
    void OpSize();
    void AdSize();
    void Single();
    void Double();
    void CS();
    void SS();
    void DS();
    void ES();
    void FS();
    void GS();
    void BNT();
    void BT();
    void Lock();
    void REP();
    void REPNE();
    void Escape();
    void Escape(uint8_t opcode);
    void Esc();
    void Esc(uint8_t opcode);

    void REX();
    void REXB();
    void REXX();
    void REXXB();
    void REXR();
    void REXRB();
    void REXRX();
    void REXRXB();
    void REXW();
    void REXWB();
    void REXWX();
    void REXWXB();
    void REXWR();
    void REXWRB();
    void REXWRX();
    void REXWRXB();

    void OP(uint8_t primary_opcode);
    void SO(uint8_t secondary_opcode);

    void Lock(const code::Register& dest) { if (dest.IsAtomic()) Lock(); }
    void Lock(const code::Address& dest) { if (dest.IsAtomic()) Lock(); }
    void Lock(const code::Register& dest, const code::Register& src) { if (dest.IsAtomic() || src.IsAtomic()) Lock(); }
    void Lock(const code::Address& dest, const code::Register& src) { if (dest.IsAtomic() || src.IsAtomic()) Lock(); }
    void Lock(const code::Register& dest, const code::Address& src) { if (dest.IsAtomic() || src.IsAtomic()) Lock(); }

    void OpSize(const code::Register& dest) { if (dest.Is16Bit()) OpSize(); }
    void OpSize(const code::Address& dest) { if (dest.Is16Bit()) OpSize(); }
    void OpSize(const code::Register& dest, const code::Register& src) { if (dest.Is16Bit() || src.Is16Bit()) OpSize(); }
    void OpSize(const code::Address& dest, const code::Register& src) { if (dest.Is16Bit() || src.Is16Bit()) OpSize(); }
    void OpSize(const code::Register& dest, const code::Address& src) { if (dest.Is16Bit() || src.Is16Bit()) OpSize(); }
    void OpSize(const code::Flag dest, const code::Flag src) { if (dest.Has(code::Flag::SIZE16()) || src.Has(code::Flag::SIZE16())) OpSize(); }

    bool IsStandard(const code::Register& reg) { return (reg.ID() & 0b11101000) == 0b00000000; }
    bool IsExtended(const code::Register& reg) { return (reg.ID() & 0b11101000) != 0b00001000; }

    bool IsStandard(const code::Address& address) { return (address.ID() & 0b11101000) == 0b00000000; }
    bool IsExtended(const code::Address& address) { return (address.ID() & 0b11101000) != 0b00001000; }

    bool IsStandard(uint8_t reg) { return (reg & 0b11101000) == 0b00000000; }
    bool IsExtended(uint8_t reg) { return (reg & 0b11101000) != 0b00001000; }

    uint8_t ToValue(const code::Register& reg) {
      uint8_t id = reg.ID();
      if (id >= 24) return id - 24;
      else if (id >= 16) return id - 16;
      else if (id >= 8) return id - 8;
      else return id;
    }

    // Function to return the log base 2 of the scale factor for SIB encoding
    uint8_t ScaleLog2(const code::Address& address) const {
      return 1;
      // switch (address.Scale()) {
      //   case 1: return 0; // 2^0 = 1
      //   case 2: return 1; // 2^1 = 2
      //   case 4: return 2; // 2^2 = 4
      //   case 8: return 3; // 2^3 = 8
      //   default: throw std::runtime_error("Invalid scale factor");
      // }
    }

    uint8_t CreateREX(const code::Address& rm) {
      uint8_t rex = 0x40; // Start with the fixed prefix (0100)

      // Ignore the R bit

      // Set the B bit if rm is allocated and is R8-R15 or if the base is in R8-R15
      if (rm.IsAllocated() && rm.ID() >= 8) {
        rex |= 0x01;
      } else if (rm.Base() && rm.Base()->IsAllocated() && rm.Base()->ID() >= 8) {
        rex |= 0x01;
      }

      // Set the X bit if the index is allocated and is R8-R15
      if (rm.Index() && rm.Index()->IsAllocated() && rm.Index()->ID() >= 8) {
        rex |= 0x02;
      }

      return rex;
    }

    void REX(const code::Address& rm) {
      uint8_t rex = CreateREX(rm);

      // Emit the REX prefix if any of the bits are set beyond the fixed 0100 prefix
      if (rex != 0x40) Instructions().Emit8(rex);
    }

    void REX64(const code::Address& rm) {
      Instructions().Emit8(CreateREX(rm) | 0x08); // Add the W bit
    }

    uint8_t CreateREX(const code::Address& reg, const code::Address& rm) {
      uint8_t rex = 0x40; // Start with the fixed prefix (0100)

      // Set the R bit if reg is allocated and is R8-R15
      if (reg.IsAllocated() && reg.ID() >= 8) rex |= 0x04;

      // Set the B bit if rm is allocated and is R8-R15 or if the base is in R8-R15
      if (rm.IsAllocated() && rm.ID() >= 8) {
        rex |= 0x01;
      } else if (rm.Base() && rm.Base()->IsAllocated() && rm.Base()->ID() >= 8) {
        rex |= 0x01;
      }

      // Set the X bit if the index is allocated and is R8-R15
      if (rm.Index() && rm.Index()->IsAllocated() && rm.Index()->ID() >= 8) {
        rex |= 0x02;
      }

      return rex;
    }

    void REX(const code::Address& reg, const code::Address& rm) {
      uint8_t rex = CreateREX(reg, rm);

      // Emit the REX prefix if any of the bits are set beyond the fixed 0100 prefix
      if (rex != 0x40) Instructions().Emit8(rex);
    }

    void REX64(const code::Address& reg, const code::Address& rm) {
      Instructions().Emit8(CreateREX(reg, rm) | 0x08); // Add the W bit
    }

    template<typename T>
    requires std::same_as<T, uint8_t> || std::same_as<T, code::Address>
    void ModRM(const T& reg, const code::Address& rm) {
      auto start = RIP(); // Capture the instruction buffer size before encoding
      uint8_t modrm = 0;
      uint8_t reg_id = 0;

      if constexpr (std::same_as<T, uint8_t>) {
        // Use the extended opcode directly in the `reg` field of ModR/M (bits 3-5)
        reg_id = reg & 0x07;
      } else {
        // For `code::Address&` in reg
        reg_id = reg.ID() & 0x07;
      }
      
      if (rm.IsAllocated()) {
        // Register to register mode
        modrm = 0xC0 | modrm | (rm.ID() & 0x07);
        Instructions().Emit8(modrm);

        return; // No need to handle memory addressing
      } else if (!rm.IsResolved()) {
        Instructions().Emit8(0b11000100 | (reg_id << 3)); // ModRM placeholder with the reg and a 32 bit displacement
        Instructions().Emit8(0b00100000); // No-op SIB as placeholder
        Instructions().Emit32(0); // Empty displacement as placeholder

        Patches().emplace_back(rm, start, 6, 0); // Create a patch that is 6 bytes long with a shift of 0

        return; // Done for now
      }

      auto displacement = rm.Displacement();

      uint8_t mod = 0x00; // Default Mod field
      if (displacement > 127 || displacement < -128) {
        mod = 0x80; // 32-bit displacement
      } else {
        mod = 0x40; // 8-bit displacement
      }

      uint8_t base_id = 0;
      if (rm.IsInstructionRelative()) {
        base_id = 0x05; // RIP-relative mode
      } else if (rm.IsStackRelative()) {
        base_id = 0x04; // RSP-relative mode

        if (displacement == 0) {
          mod = 0x00; // No displacement
        }
      } else {
        if (rm.Base() == nullptr || !rm.Base()->IsAllocated()) {
          throw std::runtime_error("Invalid memory address: Base register is required and must be allocated unless RIP or RSP-relative addressing is used.");
        }

        base_id = rm.Base()->ID() & 0x07;

        if (base_id != 0x05 && displacement == 0) {
          mod = 0x00; // No displacement
        }
      }

      modrm |= (reg_id & 0x07) << 3;
      modrm |= base_id; // Direct addressing with base register
      modrm |= mod;
      Instructions().Emit8(modrm); // Emit ModR/M byte

      // Emit the SIB if necessary
      if (base_id == 0x04) {
        // TODO: I think this makes the SIB mandatory?
      } else if (rm.Base() && rm.Index()) {
        if (!rm.Base()->IsAllocated()) {
          throw std::runtime_error("Invalid address: The base register is not allocated. A valid base register is required when using an index with SIB addressing.");
        }

        if (!rm.Index()->IsAllocated()) {
          throw std::runtime_error("Invalid address: The index register is not allocated. A valid index register is required when using an index with SIB addressing.");
        }

        uint8_t sib = 0;
        uint8_t scale = 0;

        if (rm.IsScale2()) scale = 0x40;
        if (rm.IsScale4()) scale = 0x80;
        if (rm.IsScale8()) scale = 0xC0;

        uint8_t index_id = rm.Index()->ID();
        uint8_t base_id = rm.Base()->ID();

        sib = scale | ((index_id & 0x07) << 3) | (base_id & 0x07);
        Instructions().Emit8(sib); // Emit SIB byte
      }

      // Emit the displacement if necessary
      if (mod == 0x40) { // 8-bit displacement
        if (rm.IsRAP()) {
          displacement = displacement - (static_cast<int32_t>(RIP()) + 1);
        } else if (rm.IsRBP()) {
          displacement = displacement - (static_cast<int32_t>(RSP()) + 1);
        }

        Instructions().Emit8(static_cast<int8_t>(displacement));
      } else if (mod == 0x80) { // 32-bit displacement
        if (rm.IsRAP()) {
          displacement = displacement - (static_cast<int32_t>(RIP()) + 4);
        } else if (rm.IsRBP()) {
          displacement = displacement - (static_cast<int32_t>(RSP()) + 4);
        }

        Instructions().Emit32(displacement);
      }
    }

    // template<typename T>
    // requires std::same_as<T, uint8_t> || std::same_as<T, code::Address&>
    // void ModRM(const T reg, const code::Address& rm) {
    //   auto start = RIP();
    //   uint8_t modrm = 0;

    //   if constexpr (std::same_as<T, uint8_t>) {
    //     // Use the extended opcode directly in the `reg` field of ModR/M (bits 3-5)
    //     modrm |= (reg & 0x07) << 3;
    //   } else {
    //     // For `code::Address&` in reg
    //     modrm |= (reg.ID() & 0x07) << 3;
    //   }
      
    //   if (rm.IsAllocated()) {
    //     // Register to register mode
    //     modrm = 0xC0 | modrm | (rm.ID() & 0x07);
    //     Instructions().Emit8(modrm);
    //     return; // No need to handle memory addressing
    //   }

    //   bool resolved = rm.IsResolved();
    //   auto displacement = rm.Displacement();

    //   uint8_t mod = 0x00; // Default Mod field
    //   if (!resolved || displacement > 127 || displacement < -128) {
    //     mod = 0x80; // 32-bit displacement
    //   } else {
    //     mod = 0x40; // 8-bit displacement
    //   }

    //   uint8_t base_id = 0;
    //   if (rm.IsInstructionRelative()) {
    //     base_id = 0x05; // RIP-relative mode
    //   } else if (rm.IsStackRelative()) {
    //     base_id = 0x04; // RSP-relative mode

    //     if (resolved && displacement == 0) {
    //       mod = 0x00; // No displacement
    //     }
    //   } else {
    //     if (rm.Base() == nullptr || !rm.Base()->IsAllocated()) {
    //       throw std::runtime_error("Invalid memory address: Base register is required and must be allocated unless RIP or RSP-relative addressing is used.");
    //     }

    //     base_id = rm.Base()->ID() & 0x07;

    //     if (resolved && base_id != 0x05 && displacement == 0) {
    //       mod = 0x00; // No displacement
    //     }
    //   }

    //   modrm |= base_id; // Direct addressing with base register
    //   modrm |= mod;
    //   Instructions().Emit8(modrm); // Emit ModR/M byte

    //   // Emit the SIB if necessary
    //   if (base_id == 0x04) {
    //     // TODO: I think this makes the SIB mandatory?
    //   } else if (rm.Base() && rm.Index()) {
    //     if (!rm.Base()->IsAllocated()) {
    //       throw std::runtime_error("Invalid address: The base register is not allocated. A valid base register is required when using an index with SIB addressing.");
    //     }

    //     if (!rm.Index()->IsAllocated()) {
    //       throw std::runtime_error("Invalid address: The index register is not allocated. A valid index register is required when using an index with SIB addressing.");
    //     }

    //     uint8_t sib = 0;
    //     uint8_t scale = 0;

    //     if (rm.IsScale2()) scale = 0x40;
    //     if (rm.IsScale4()) scale = 0x80;
    //     if (rm.IsScale8()) scale = 0xC0;

    //     uint8_t index_id = rm.Index()->ID();
    //     uint8_t base_id = rm.Base()->ID();

    //     sib = scale | ((index_id & 0x07) << 3) | (base_id & 0x07);
    //     Instructions().Emit8(sib); // Emit SIB byte
    //   }

    //   // Emit the displacement if necessary
    //   if (mod == 0x40) { // 8-bit displacement
    //     if (resolved && rm.IsRAP()) {
    //       displacement = displacement - (static_cast<int32_t>(RIP()) + 1);
    //     } else if (resolved && rm.IsRBP()) {
    //       displacement = displacement - (static_cast<int32_t>(RSP()) + 1);
    //     }

    //     Instructions().Emit8(static_cast<int8_t>(displacement));
    //   } else if (mod == 0x80) { // 32-bit displacement
    //     if (!resolved) {
    //       Patches().emplace_back(rm, RIP(), 4); // Create a patch with 32-bit displacement
    //     }

    //     if (resolved && rm.IsRAP()) {
    //       displacement = displacement - (static_cast<int32_t>(RIP()) + 4);
    //     } else if (resolved && rm.IsRBP()) {
    //       displacement = displacement - (static_cast<int32_t>(RSP()) + 4);
    //     }

    //     Instructions().Emit32(displacement);
    //   }
    // }

    void REL8(const code::Address& imm) {
      Instructions().Emit8(imm.Displacement());
    }

    void REL16(const code::Address& imm) {
      Instructions().Emit16(imm.Displacement());
    }

    void REL32(const code::Address& imm) {
      Instructions().Emit32(imm.Displacement());
    }

    void IMM8(const code::Address& imm) {
      Instructions().Emit8(imm.Displacement());
    }

    void IMM16(const code::Address& imm) {
      Instructions().Emit16(imm.Displacement());
    }

    void IMM32(const code::Address& imm) {
      Instructions().Emit32(imm.Displacement());
    }

    void IMM64(const code::Address& imm) {
      Instructions().Emit64(imm.Displacement());
    }

    void OP(uint8_t op, const code::Address& reg) {
      OP(op + (reg.ID() & 0x07));
    }

    bool R16(uint8_t op, const code::Address& reg) {
      if (!reg.IsAllocated() || !reg.Is16Bit()) return false;

      OpSize();
      REX(reg);
      OP(op, reg);

      return true;
    }

    bool R32(uint8_t op, const code::Address& reg) {
      if (!reg.IsAllocated() || !reg.Is32Bit()) return false;

      REX(reg);
      OP(op, reg);

      return true;
    }

    bool R64(uint8_t op, const code::Address& reg) {
      if (!reg.IsAllocated() || !reg.Is64Bit()) return false;

      REX64(reg);
      OP(op, reg);

      return true;
    }

    bool REL8(uint8_t op, const code::Address& mem) {
      if (!mem.Is8Bit() || !(mem.IsRIP() || mem.IsRAP())) return false;

      OP(op);
      REL8(mem);

      return true;
    }

    bool REL16(uint8_t op, const code::Address& mem) {
      if (!mem.Is16Bit() || !(mem.IsRIP() || mem.IsRAP())) return false;

      OP(op);
      REL16(mem);

      return true;
    }

    bool REL32(uint8_t op, const code::Address& mem) {
      if (!mem.Is32Bit() || !(mem.IsRIP() || mem.IsRAP())) return false;

      OP(op);
      REL32(mem);

      return true;
    }

    bool ESC_REL16(uint8_t op, const code::Address& mem) {
      if (!mem.Is16Bit() || !(mem.IsRIP() || mem.IsRAP())) return false;

      Escape(op);
      REL16(mem);

      return true;
    }

    bool ESC_REL32(uint8_t op, const code::Address& mem) {
      if (!mem.Is32Bit() || !(mem.IsRIP() || mem.IsRAP())) return false;

      Escape(op);
      REL32(mem);

      return true;
    }

    bool ESC_RM8(uint8_t op, uint8_t ext, const code::Address& rm) {
      if (!rm.Is8Bit()) return false;

      REX(rm);
      Escape(op);
      ModRM(ext, rm);

      return true;
    }

    bool RM8(uint8_t op, uint8_t ext, const code::Address& rm) {
      if (!rm.Is8Bit()) return false;

      REX(rm);
      OP(op);
      ModRM(ext, rm);

      return true;
    }

    bool RM16(uint8_t op, uint8_t ext, const code::Address& rm) {
      if (!rm.Is16Bit()) return false;

      OpSize();
      REX(rm);
      OP(op);
      ModRM(ext, rm);

      return true;
    }

    bool RM32(uint8_t op, uint8_t ext, const code::Address& rm) {
      if (!rm.Is32Bit()) return false;

      REX(rm);
      OP(op);
      ModRM(ext, rm);

      return true;
    }

    bool RM64(uint8_t op, uint8_t ext, const code::Address& rm) {
      if (!rm.Is64Bit()) return false;

      REX64(rm);
      OP(op);
      ModRM(ext, rm);

      return true;
    }

    bool IMM8(uint8_t op, const code::Address& imm) {
      if (!imm.Is8Bit() || !imm.IsImmediate()) return false;

      OP(op);
      IMM8(imm);
      return true;
    }

    bool IMM16(uint8_t op, const code::Address& imm) {
      if (!imm.Is16Bit() || !imm.IsImmediate()) return false;

      OpSize();
      OP(op);
      IMM16(imm);
      return true;
    }

    bool IMM32(uint8_t op, const code::Address& imm) {
      if (!imm.Is32Bit() || !imm.IsImmediate()) return false;

      OP(op);
      IMM32(imm);
      return true;
    }

    bool AL_IMM8(uint8_t op, const code::Address& reg, const code::Address& imm) {
      if (!reg.IsAllocated() || !reg.Is8Bit() || reg.ID() != 0 || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      OP(op);
      IMM8(imm);

      return true;
    }

    bool AX_IMM16(uint8_t op, const code::Address& reg, const code::Address& imm) {
      if (!reg.IsAllocated() || !reg.Is16Bit() || reg.ID() != 0 || !imm.Is16Bit() || !imm.IsImmediate()) return false;

      OpSize();
      OP(op);
      IMM16(imm);

      return true;
    }

    bool EAX_IMM32(uint8_t op, const code::Address& reg, const code::Address& imm) {
      if (!reg.IsAllocated() || !reg.Is32Bit() || reg.ID() != 0 || !imm.Is32Bit() || !imm.IsImmediate()) return false;

      OP(op);
      IMM32(imm);

      return true;
    }

    bool RAX_IMM32(uint8_t op, const code::Address& reg, const code::Address& imm) {
      if (!reg.IsAllocated() || !reg.Is64Bit() || reg.ID() != 0 || !imm.Is32Bit() || !imm.IsImmediate()) return false;

      REXW();
      OP(op);
      IMM32(imm);

      return true;
    }

    bool R16_AX(uint8_t op, const code::Address& reg, const code::Address& ax) {
      if (!reg.IsAllocated() || !reg.Is16Bit() || !ax.IsAllocated() || !ax.Is16Bit() || ax.ID() != 0) return false;

      OpSize();
      OP(op, reg);

      return true;
    }

    bool R32_EAX(uint8_t op, const code::Address& reg, const code::Address& eax) {
      if (!reg.IsAllocated() || !reg.Is32Bit() || !eax.IsAllocated() || !eax.Is32Bit() || eax.ID() != 0) return false;

      OP(op, reg);

      return true;
    }

    bool R64_RAX(uint8_t op, const code::Address& reg, const code::Address& rax) {
      if (!reg.IsAllocated() || !reg.Is64Bit() || !rax.IsAllocated() || !rax.Is64Bit() || rax.ID() != 0) return false;

      REX(reg);
      OP(op, reg);

      return true;
    }

    bool RM8_IMM8(uint8_t op, uint8_t ext, const code::Address& rm, const code::Address& imm) {
      if (!rm.Is8Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      REX(rm);
      OP(op);
      ModRM(ext, rm);
      IMM8(imm);

      return true;
    }

    bool RM16_IMM8(uint8_t op, uint8_t ext, const code::Address& rm, const code::Address& imm) {
      if (!rm.Is16Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      OpSize();
      REX(rm);
      OP(op);
      ModRM(ext, rm);
      IMM8(imm);

      return true;
    }

    bool RM32_IMM8(uint8_t op, uint8_t ext, const code::Address& rm, const code::Address& imm) {
      if (!rm.Is32Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      REX(rm);
      OP(op);
      ModRM(ext, rm);
      IMM8(imm);

      return true;
    }

    bool RM64_IMM8(uint8_t op, uint8_t ext, const code::Address& rm, const code::Address& imm) {
      if (!rm.Is64Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      REX64(rm);
      OP(op);
      ModRM(ext, rm);
      IMM8(imm);

      return true;
    }

    bool R8_IMM8(uint8_t op, const code::Address& reg, const code::Address& imm) {
      if (!reg.IsAllocated() || !reg.Is8Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      REX(reg);

      if (8 > reg.ID()) OP(op + reg.ID());
      else OP(op + (reg.ID() - 7));

      IMM8(imm);

      return true;
    }

    bool R16_IMM16(uint8_t op, const code::Address& reg, const code::Address& imm) {
      if (!reg.IsAllocated() || !reg.Is16Bit() || !imm.Is16Bit() || !imm.IsImmediate()) return false;

      OpSize();
      REX(reg);

      if (8 > reg.ID()) OP(op + reg.ID());
      else OP(op + (reg.ID() - 7));

      IMM16(imm);

      return true;
    }

    bool R32_IMM32(uint8_t op, const code::Address& reg, const code::Address& imm) {
      if (!reg.IsAllocated() || !reg.Is32Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

      REX(reg);

      if (8 > reg.ID()) OP(op + reg.ID());
      else OP(op + (reg.ID() - 7));

      IMM32(imm);

      return true;
    }

    bool R64_IMM64(uint8_t op, const code::Address& reg, const code::Address& imm) {
      if (!reg.IsAllocated() || !reg.Is64Bit() || !imm.Is64Bit() || !imm.IsImmediate()) return false;

      REX64(reg);

      if (8 > reg.ID()) OP(op + reg.ID());
      else OP(op + (reg.ID() - 7));

      IMM64(imm);

      return true;
    }

    bool RM_IMM8(uint8_t op8, uint8_t op, uint8_t ext, const code::Address& rm, const code::Address& imm) {
      if (!imm.Is8Bit() || !imm.IsImmediate()) return false;

      return RM8_IMM8(op8, ext, rm, imm)
          || RM32_IMM8(op, ext, rm, imm)
          || RM64_IMM8(op, ext, rm, imm)
          || RM16_IMM8(op, ext, rm, imm);
    }

    bool RM16_IMM16(uint8_t op, uint8_t ext, const code::Address& rm, const code::Address& imm) {
      if (!rm.Is16Bit() || !imm.Is16Bit() || !imm.IsImmediate()) return false;

      OpSize();
      REX(rm);
      OP(op);
      ModRM(ext, rm);
      IMM16(imm);

      return true;
    }

    bool RM32_IMM32(uint8_t op, uint8_t ext, const code::Address& rm, const code::Address& imm) {
      if (!rm.Is32Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

      REX(rm);
      OP(op);
      ModRM(ext, rm);
      IMM32(imm);

      return true;
    }

    bool RM64_IMM32(uint8_t op, uint8_t ext, const code::Address& rm, const code::Address& imm) {
      if (!rm.Is64Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

      REX64(rm);
      OP(op);
      ModRM(ext, rm);
      IMM32(imm);

      return true;
    }

    bool R8_RM8(uint8_t op, const code::Address& reg, const code::Address& rm) {
      if (!reg.Is8Bit() || !rm.Is8Bit() || !reg.IsAllocated()) return false;

      REX(reg, rm);
      OP(op);
      ModRM(reg, rm);

      return true;
    }

    bool RM8_R8(uint8_t op, const code::Address& rm, const code::Address& reg) { return R8_RM8(op, reg, rm); }

    bool R16_RM16(uint8_t op, const code::Address& reg, const code::Address& rm) {
      if (!reg.Is16Bit() || !rm.Is16Bit() || !reg.IsAllocated()) return false;

      OpSize();
      REX(reg, rm);
      OP(op);
      ModRM(reg, rm);

      return true;
    }

    bool RM16_R16(uint8_t op, const code::Address& rm, const code::Address& reg) { return R16_RM16(op, reg, rm); }

    bool R32_RM32(uint8_t op, const code::Address& reg, const code::Address& rm) {
      if (!reg.Is32Bit() || !rm.Is32Bit() || !reg.IsAllocated()) return false;

      REX(reg, rm);
      OP(op);
      ModRM(reg, rm);

      return true;
    }

    bool RM32_R32(uint8_t op, const code::Address& rm, const code::Address& reg) { return R32_RM32(op, reg, rm); }

    bool R64_RM64(uint8_t op, const code::Address& reg, const code::Address& rm) {
      if (!reg.Is64Bit() || !rm.Is64Bit() || !reg.IsAllocated()) return false;

      REX64(reg, rm); // Use the 64 bit version of REX
      OP(op);
      ModRM(reg, rm);

      return true;
    }

    bool R64_RM32(uint8_t op, const code::Address& r, const code::Address& rm) {
      if (!r.Is64Bit() || !rm.Is32Bit() || !r.IsAllocated()) return false;

      REX64(r, rm); // Use the 64 bit version of REX
      OP(op);
      ModRM(r, rm);

      return true;
    }

    bool RM64_R64(uint8_t op, const code::Address& rm, const code::Address& reg) { return R64_RM64(op, reg, rm); }

    bool ESC_R16_RM16(uint8_t op, const code::Address& reg, const code::Address& rm) {
      if (!reg.Is16Bit() || !rm.Is16Bit() || !reg.IsAllocated()) return false;

      OpSize();
      REX(reg, rm);
      Escape(op);
      ModRM(reg, rm);

      return true;
    }

    bool ESC_R32_RM32(uint8_t op, const code::Address& reg, const code::Address& rm) {
      if (!reg.Is32Bit() || !rm.Is32Bit() || !reg.IsAllocated()) return false;

      REX(reg, rm);
      Escape(op);
      ModRM(reg, rm);

      return true;
    }

    bool ESC_R64_RM64(uint8_t op, const code::Address& reg, const code::Address& rm) {
      if (!reg.Is64Bit() || !rm.Is64Bit() || !reg.IsAllocated()) return false;

      REX64(reg, rm);
      Escape(op);
      ModRM(reg, rm);

      return true;
    }

    void PreserveRDXAndRAX(const code::Address& r1, const code::Address& r2) {
      // Handle r1 (should be in RDX)
      if (r1.ID() != rdx.ID()) { // Check if r1 is not RDX
        if (r1.ID() == rax.ID()) {
          // r1 is RAX, swap r1 with another available register instead of RDX
          // Find an available register to temporarily hold RAX, e.g., RCX
          XCHG(r1, rcx);
        } else {
          // r1 is not RAX, swap it directly with RDX
          XCHG(r1, rdx); // Swap r1 with RDX
        }
      }

      // Handle r2 (should be in RAX)
      if (r2.ID() != rax.ID()) { // Check if r2 is not RAX
        if (r2.ID() == rdx.ID()) {
          // r2 is RDX, swap r2 with another available register instead of RAX
          // Find an available register to temporarily hold RDX, e.g., RSI
          XCHG(r2, rsi);
        } else {
          // r2 is not RDX, swap it directly with RAX
          XCHG(r2, rax); // Swap r2 with RAX
        }
      }
    }

    bool DX_AX_RM16(uint8_t op, uint8_t ext, const code::Address& r1, const code::Address& r2, const code::Address& rm) {
      if (!r1.Is16Bit() || !r2.Is16Bit() || !rm.Is16Bit() || !r1.IsAllocated() || !r2.IsAllocated()) return false;

      // Swap r1 and r2 with RDX and RAX if necessary to save them
      PreserveRDXAndRAX(r1, r2);

      // Emit the instruction with appropriate prefixes
      OpSize(); // Emit 0x66 prefix for 16-bit operation
      REX(rm); // Emit REX prefix if necessary
      OP(op); // Emit the opcode
      ModRM(ext, rm); // Emit ModR/M byte with the extension

      // Undo the swaps, if necessary
      PreserveRDXAndRAX(r1, r2);

      return true;
    }

    bool EDX_EAX_RM32(uint8_t op, uint8_t ext, const code::Address& r1, const code::Address& r2, const code::Address& rm) {
      if (!r1.Is32Bit() || !r2.Is32Bit() || !rm.Is32Bit() || !r1.IsAllocated() || !r2.IsAllocated()) return false;

      // Swap r1 and r2 with RDX and RAX if necessary to save them
      PreserveRDXAndRAX(r1, r2);

      REX(rm); // Emit REX prefix if necessary
      OP(op);
      ModRM(ext, rm); // Emit ModR/M byte with the extension

      // Undo the swaps, if necessary
      PreserveRDXAndRAX(r1, r2);

      return true;
    }

    bool RDX_RAX_RM64(uint8_t op, uint8_t ext, const code::Address& r1, const code::Address& r2, const code::Address& rm) {
      if (!r1.Is64Bit() || !r2.Is64Bit() || !rm.Is64Bit() || !r1.IsAllocated() || !r2.IsAllocated()) return false;

      // Swap r1 and r2 with RDX and RAX if necessary to save them
      PreserveRDXAndRAX(r1, r2);

      REX64(rm);
      OP(op);
      ModRM(ext, rm);

      // Undo the swaps, if necessary
      PreserveRDXAndRAX(r1, r2);

      return true;
    }

    // TODO: Implement
    bool AL_AH_AX_RM8(uint8_t op, uint8_t ext, const code::Address& r1, const code::Address& r2, const code::Address& rm) {
      return false;
    }

    bool R16_RM16_IMM8(uint8_t op, const code::Address& reg, const code::Address& rm, const code::Address& imm) {
      if (!reg.Is16Bit() || !reg.IsAllocated() || !rm.Is16Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      OpSize();
      REX(reg, rm);
      OP(op);
      ModRM(reg, rm);
      IMM8(imm);

      return true;
    }

    bool R32_RM32_IMM8(uint8_t op, const code::Address& reg, const code::Address& rm, const code::Address& imm) {
      if (!reg.Is32Bit() || !reg.IsAllocated() || !rm.Is32Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      REX(reg, rm);
      OP(op);
      ModRM(reg, rm);
      IMM8(imm);

      return true;
    }

    bool R64_RM64_IMM8(uint8_t op, const code::Address& reg, const code::Address& rm, const code::Address& imm) {
      if (!reg.Is64Bit() || !reg.IsAllocated() || !rm.Is64Bit() || !imm.Is8Bit() || !imm.IsImmediate()) return false;

      REX64(reg, rm);
      OP(op);
      ModRM(reg, rm);
      IMM8(imm);

      return true;
    }

    bool R16_RM16_IMM16(uint8_t op, const code::Address& reg, const code::Address& rm, const code::Address& imm) {
      if (!reg.Is16Bit() || !reg.IsAllocated() || !rm.Is16Bit() || !imm.Is16Bit() || !imm.IsImmediate()) return false;

      OpSize();
      REX(reg, rm);
      OP(op);
      ModRM(reg, rm);
      IMM16(imm);

      return true;
    }

    bool R32_RM32_IMM32(uint8_t op, const code::Address& reg, const code::Address& rm, const code::Address& imm) {
      if (!reg.Is32Bit() || !reg.IsAllocated() || !rm.Is32Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

      REX(reg, rm);
      OP(op);
      ModRM(reg, rm);
      IMM32(imm);

      return true;
    }

    bool R64_RM64_IMM32(uint8_t op, const code::Address& reg, const code::Address& rm, const code::Address& imm) {
      if (!reg.Is64Bit() || !reg.IsAllocated() || !rm.Is64Bit() || !imm.Is32Bit() || !imm.IsImmediate()) return false;

      REX64(reg, rm);
      OP(op);
      ModRM(reg, rm);
      IMM32(imm);

      return true;
    }

    bool ESC_R16_RM8(uint8_t op, const code::Address& r, const code::Address& rm) {
      if (!r.Is16Bit() || !r.IsAllocated() || !rm.Is8Bit()) return false;

      OpSize();
      REX(r, rm);
      Escape(op);
      ModRM(r, rm);

      return true;
    }

    bool ESC_R32_RM8(uint8_t op, const code::Address& r, const code::Address& rm) {
      if (!r.Is32Bit() || !r.IsAllocated() || !rm.Is8Bit()) return false;

      REX(r, rm);
      Escape(op);
      ModRM(r, rm);

      return true;
    }

    bool ESC_R64_RM8(uint8_t op, const code::Address& r, const code::Address& rm) {
      if (!r.Is64Bit() || !r.IsAllocated() || !rm.Is8Bit()) return false;

      REX64(r, rm);
      Escape(op);
      ModRM(r, rm);

      return true;
    }

    bool ESC_R32_RM16(uint8_t op, const code::Address& r, const code::Address& rm) {
      if (!r.Is32Bit() || !r.IsAllocated() || !rm.Is16Bit()) return false;

      REX(r, rm);
      Escape(op);
      ModRM(r, rm);

      return true;
    }

    bool ESC_R64_RM16(uint8_t op, const code::Address& r, const code::Address& rm) {
      if (!r.Is64Bit() || !r.IsAllocated() || !rm.Is16Bit()) return false;

      REX64(r, rm);
      Escape(op);
      ModRM(r, rm);

      return true;
    }

    bool Failed(std::string_view name, const code::Address& s) {
      throw utility::Error("Failed to encode instruction", name, "with operand", s);
    }

    bool Failed(std::string_view name, const code::Address& d, const code::Address& s) {
      throw utility::Error("Failed to encode instruction", name, "with operands", d, "and", s);
    }

    bool Failed(std::string_view name, const code::Address& d, const code::Address& s, const code::Address& i) {
      throw utility::Error("Failed to encode instruction", name, "with operands", d, "and", s, "and", i);
    }

    bool ADC(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0x14, d, s)
          || AX_IMM16(0x15, d, s)
          || EAX_IMM32(0x15, d, s)
          || RAX_IMM32(0x15, d, s)
          || RM8_IMM8(0x82, 0x02, d, s)
          || RM32_IMM8(0x83, 0x02, d, s)
          || RM64_IMM8(0x83, 0x02, d, s)
          || RM16_IMM8(0x83, 0x02, d, s)
          || RM32_IMM32(0x81, 0x02, d, s)
          || RM64_IMM32(0x81, 0x02, d, s)
          || RM16_IMM16(0x81, 0x02, d, s)
          || R8_RM8(0x12, d, s)
          || R32_RM32(0x13, d, s)
          || R64_RM64(0x13, d, s)
          || R16_RM16(0x13, d, s)
          || RM8_R8(0x10, d, s)
          || RM32_R32(0x11, d, s)
          || RM64_R64(0x11, d, s)
          || RM16_R16(0x11, d, s)
          || Failed("ADC", d, s); // Throw an error
    }

    bool ADD(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0x04, d, s)
          || AX_IMM16(0x05, d, s)
          || EAX_IMM32(0x05, d, s)
          || RAX_IMM32(0x05, d, s)
          || RM8_IMM8(0x82, 0x00, d, s)
          || RM32_IMM8(0x83, 0x00, d, s)
          || RM64_IMM8(0x83, 0x00, d, s)
          || RM16_IMM8(0x83, 0x00, d, s)
          || RM32_IMM32(0x81, 0x00, d, s)
          || RM64_IMM32(0x81, 0x00, d, s)
          || RM16_IMM16(0x81, 0x00, d, s)
          || R8_RM8(0x02, d, s)
          || R32_RM32(0x03, d, s)
          || R64_RM64(0x03, d, s)
          || R16_RM16(0x03, d, s)
          || RM8_R8(0x00, d, s)
          || RM32_R32(0x01, d, s)
          || RM64_R64(0x01, d, s)
          || RM16_R16(0x01, d, s)
          || Failed("ADD", d, s); // Throw an error
    }

    bool SUB(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0x2C, d, s)
          || AX_IMM16(0x2D, d, s)
          || EAX_IMM32(0x2D, d, s)
          || RAX_IMM32(0x2D, d, s)
          || RM8_IMM8(0x82, 0x05, d, s)
          || RM32_IMM8(0x83, 0x05, d, s)
          || RM64_IMM8(0x83, 0x05, d, s)
          || RM16_IMM8(0x83, 0x05, d, s)
          || RM32_IMM32(0x81, 0x05, d, s)
          || RM64_IMM32(0x81, 0x05, d, s)
          || RM16_IMM16(0x81, 0x05, d, s)
          || R8_RM8(0x2A, d, s)
          || R32_RM32(0x2B, d, s)
          || R64_RM64(0x2B, d, s)
          || R16_RM16(0x2B, d, s)
          || RM8_R8(0x28, d, s)
          || RM32_R32(0x29, d, s)
          || RM64_R64(0x29, d, s)
          || RM16_R16(0x29, d, s)
          || Failed("SUB", d, s); // Throw an error
    }

    bool SBB(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0x1C, d, s)
          || AX_IMM16(0x1D, d, s)
          || EAX_IMM32(0x1D, d, s)
          || RAX_IMM32(0x1D, d, s)
          || RM8_IMM8(0x82, 0x03, d, s)
          || RM32_IMM8(0x83, 0x03, d, s)
          || RM64_IMM8(0x83, 0x03, d, s)
          || RM16_IMM8(0x83, 0x03, d, s)
          || RM32_IMM32(0x81, 0x03, d, s)
          || RM64_IMM32(0x81, 0x03, d, s)
          || RM16_IMM16(0x81, 0x03, d, s)
          || R8_RM8(0x1A, d, s)
          || R32_RM32(0x1B, d, s)
          || R64_RM64(0x1B, d, s)
          || R16_RM16(0x1B, d, s)
          || RM8_R8(0x18, d, s)
          || RM32_R32(0x19, d, s)
          || RM64_R64(0x19, d, s)
          || RM16_R16(0x19, d, s)
          || Failed("SBB", d, s); // Throw an error
    }

    bool XCHG(const code::Address& d, const code::Address& s) {
      return R64_RAX(0x90, d, s)
          || R32_EAX(0x90, d, s)
          || R16_AX(0x90, d, s)
          || R64_RM64(0x87, d, s)
          || R32_RM32(0x87, d, s)
          || R16_RM16(0x87, d, s)
          || R8_RM8(0x86, d, s)
          || Failed("XCHG", d, s);
    }

    bool XCHG_RAX(const code::Address& s) {
      return R64(0x90, s)
          || R32(0x90, s)
          || R16(0x90, s)
          || Failed("XCHG", s);
    }

    // TODO: MUL and DIV are definitely not right
    bool MUL(const code::Address& d, const code::Address& s, const code::Address& i) {
      return AL_AH_AX_RM8(0xF6, 0x06, d, s, i)
          || DX_AX_RM16(0xF7, 0x06, d, s, i)
          || EDX_EAX_RM32(0xF7, 0x06, d, s, i)
          || RDX_RAX_RM64(0xF7, 0x06, d, s, i)
          || Failed("MUL", d, s, i);
    }

    bool IMUL(const code::Address& d, const code::Address& s) {
      return ESC_R16_RM16(0xAF, d, s)
          || ESC_R32_RM32(0xAF, d, s)
          || ESC_R64_RM64(0xAF, d, s)
          || Failed("IMUL", d, s);
    }

    bool IMUL(const code::Address& d, const code::Address& s, const code::Address& i) {
      return R16_RM16_IMM8(0x6B, d, s, i)
          || R32_RM32_IMM8(0x6B, d, s, i)
          || R64_RM64_IMM8(0x6B, d, s, i)
          || R16_RM16_IMM16(0x69, d, s, i)
          || R32_RM32_IMM32(0x69, d, s, i)
          || R64_RM64_IMM32(0x69, d, s, i)
          // || AX_AL_RM8(0xF6, 0x05, d, s, i)
          || DX_AX_RM16(0xF7, 0x05, d, s, i)
          || EDX_EAX_RM32(0xF7, 0x05, d, s, i)
          || RDX_RAX_RM64(0xF7, 0x05, d, s, i)
          || Failed("IMUL", d, s, i);
    }

    bool DIV(const code::Address& d, const code::Address& s, const code::Address& i) {
      return AL_AH_AX_RM8(0xF6, 0x06, d, s, i)
          || DX_AX_RM16(0xF7, 0x06, d, s, i)
          || EDX_EAX_RM32(0xF7, 0x06, d, s, i)
          || RDX_RAX_RM64(0xF7, 0x06, d, s, i)
          || Failed("DIV", d, s, i);
    }

    bool IDIV(const code::Address& d, const code::Address& s, const code::Address& i) {
      return AL_AH_AX_RM8(0xF6, 0x07, d, s, i)
          || DX_AX_RM16(0xF7, 0x07, d, s, i)
          || EDX_EAX_RM32(0xF7, 0x07, d, s, i)
          || RDX_RAX_RM64(0xF7, 0x07, d, s, i)
          || Failed("IDIV", d, s, i);
    }

    bool AND(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0x24, d, s)
          || AX_IMM16(0x25, d, s)
          || EAX_IMM32(0x25, d, s)
          || RAX_IMM32(0x25, d, s)
          || RM8_IMM8(0x82, 0x04, d, s)
          || RM32_IMM8(0x83, 0x04, d, s)
          || RM64_IMM8(0x83, 0x04, d, s)
          || RM16_IMM8(0x83, 0x04, d, s)
          || RM32_IMM32(0x81, 0x04, d, s)
          || RM64_IMM32(0x81, 0x04, d, s)
          || RM16_IMM16(0x81, 0x04, d, s)
          || R8_RM8(0x22, d, s)
          || R32_RM32(0x23, d, s)
          || R64_RM64(0x23, d, s)
          || R16_RM16(0x23, d, s)
          || RM8_R8(0x20, d, s)
          || RM32_R32(0x21, d, s)
          || RM64_R64(0x21, d, s)
          || RM16_R16(0x21, d, s)
          || Failed("AND", d, s); // Throw an error
    }

    bool OR(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0x0C, d, s)
          || AX_IMM16(0x0D, d, s)
          || EAX_IMM32(0x0D, d, s)
          || RAX_IMM32(0x0D, d, s)
          || RM8_IMM8(0x82, 0x01, d, s)
          || RM32_IMM8(0x83, 0x01, d, s)
          || RM64_IMM8(0x83, 0x01, d, s)
          || RM16_IMM8(0x83, 0x01, d, s)
          || RM32_IMM32(0x81, 0x01, d, s)
          || RM64_IMM32(0x81, 0x01, d, s)
          || RM16_IMM16(0x81, 0x01, d, s)
          || R8_RM8(0x0A, d, s)
          || R32_RM32(0x0B, d, s)
          || R64_RM64(0x0B, d, s)
          || R16_RM16(0x0B, d, s)
          || RM8_R8(0x08, d, s)
          || RM32_R32(0x09, d, s)
          || RM64_R64(0x09, d, s)
          || RM16_R16(0x09, d, s)
          || Failed("OR", d, s); // Throw an error
    }

    bool XOR(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0x34, d, s)
          || AX_IMM16(0x35, d, s)
          || EAX_IMM32(0x35, d, s)
          || RAX_IMM32(0x35, d, s)
          || RM8_IMM8(0x82, 0x06, d, s)
          || RM32_IMM8(0x83, 0x06, d, s)
          || RM64_IMM8(0x83, 0x06, d, s)
          || RM16_IMM8(0x83, 0x06, d, s)
          || RM32_IMM32(0x81, 0x06, d, s)
          || RM64_IMM32(0x81, 0x06, d, s)
          || RM16_IMM16(0x81, 0x06, d, s)
          || R8_RM8(0x32, d, s)
          || R32_RM32(0x33, d, s)
          || R64_RM64(0x33, d, s)
          || R16_RM16(0x33, d, s)
          || RM8_R8(0x30, d, s)
          || RM32_R32(0x31, d, s)
          || RM64_R64(0x31, d, s)
          || RM16_R16(0x31, d, s)
          || Failed("XOR", d, s); // Throw an error
    }

    bool XOR0(const code::Address& d) { return XOR(d, d); }

    bool CMP(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0x3C, d, s)
          || AX_IMM16(0x3D, d, s)
          || EAX_IMM32(0x3D, d, s)
          || RAX_IMM32(0x3D, d, s)
          || RM8_IMM8(0x82, 0x07, d, s)
          || RM16_IMM8(0x83, 0x07, d, s)
          || RM32_IMM8(0x83, 0x07, d, s)
          || RM64_IMM8(0x83, 0x07, d, s)
          || RM16_IMM16(0x81, 0x07, d, s)
          || RM32_IMM32(0x81, 0x07, d, s)
          || RM64_IMM32(0x81, 0x07, d, s)
          || R8_RM8(0x3A, d, s)
          || R16_RM16(0x3B, d, s)
          || R32_RM32(0x3B, d, s)
          || R64_RM64(0x3B, d, s)
          || RM8_R8(0x38, d, s)
          || RM16_R16(0x39, d, s)
          || RM32_R32(0x39, d, s)
          || RM64_R64(0x39, d, s)
          || Failed("CMP", d, s); // Throw an error
    }

    bool CMP0(const code::Address& d) { return CMP(d, zero); }
    bool CMP1(const code::Address& d) { return CMP(d, one); }

    bool TEST(const code::Address& d, const code::Address& s) {
      return AL_IMM8(0xA8, d, s)
          || AX_IMM16(0xA9, d, s)
          || EAX_IMM32(0xA9, d, s)
          || RAX_IMM32(0xA9, d, s)
          || RM8_IMM8(0xF6, 0x00, d, s)
          || RM16_IMM16(0xF7, 0x00, d, s)
          || RM32_IMM32(0xF7, 0x00, d, s)
          || RM64_IMM32(0xF7, 0x00, d, s)
          || RM8_R8(0x84, d, s)
          || RM16_R16(0x85, d, s)
          || RM32_R32(0x85, d, s)
          || RM64_R64(0x85, d, s)
          || Failed("TEST", d, s);
    }

    bool TEST0(const code::Address& d) { return TEST(d, d); }

    bool ShiftHelper(uint8_t ext, const code::Address& d, const code::Address& s, std::string_view name) {
      return RM8_IMM8(0xC0, ext, d, s)
          || RM16_IMM16(0xC1, ext, d, s)
          || RM32_IMM32(0xC1, ext, d, s)
          || RM64_IMM32(0xC1, ext, d, s)
          || Failed(name, d, s);
    }

    bool ShiftHelper1(uint8_t ext, const code::Address& d, std::string_view name) {
      return RM8(0xD0, ext, d)
          || RM16(0xD1, ext, d)
          || RM32(0xD1, ext, d)
          || RM64(0xD1, ext, d)
          || Failed(name, d);
    }

    bool ROL(const code::Address& d, const code::Address& s) { return ShiftHelper(0x00, d, s, "ROL"); }
    bool ROL1(const code::Address& d) { return ShiftHelper1(0x00, d, "ROL1"); }
    bool ROR(const code::Address& d, const code::Address& s) { return ShiftHelper(0x01, d, s, "ROR"); }
    bool ROR1(const code::Address& d) { return ShiftHelper1(0x01, d, "ROR1"); }
    bool RCL(const code::Address& d, const code::Address& s) { return ShiftHelper(0x02, d, s, "RCL"); }
    bool RCL1(const code::Address& d) { return ShiftHelper1(0x02, d, "RCL1"); }
    bool RCR(const code::Address& d, const code::Address& s) { return ShiftHelper(0x03, d, s, "RCR"); }
    bool RCR1(const code::Address& d) { return ShiftHelper1(0x03, d, "RCR1"); }
    bool SHL(const code::Address& d, const code::Address& s) { return ShiftHelper(0x04, d, s, "SHL"); }
    bool SHL1(const code::Address& d) { return ShiftHelper1(0x04, d, "SHL1"); }
    bool SHR(const code::Address& d, const code::Address& s) { return ShiftHelper(0x05, d, s, "SHR"); }
    bool SHR1(const code::Address& d) { return ShiftHelper1(0x05, d, "SHR1"); }
    bool SAL(const code::Address& d, const code::Address& s) { return ShiftHelper(0x06, d, s, "SAL"); }
    bool SAL1(const code::Address& d) { return ShiftHelper1(0x06, d, "SAL1"); }
    bool SAR(const code::Address& d, const code::Address& s) { return ShiftHelper(0x07, d, s, "SAR"); }
    bool SAR1(const code::Address& d) { return ShiftHelper1(0x07, d, "SAR1"); }

    bool MOV(const code::Address& d, const code::Address& s) {
      return R8_IMM8(0xB0, d, s)
          || R16_IMM16(0xB8, d, s)
          || R32_IMM32(0xB8, d, s)
          || R64_IMM64(0xB8, d, s)
          || RM8_IMM8(0xC6, 0x00, d, s)
          || RM16_IMM16(0xC7, 0x00, d, s)
          || RM32_IMM32(0xC7, 0x00, d, s)
          || RM64_IMM32(0xC7, 0x00, d, s)
          || R8_RM8(0x8A, d, s)
          || R16_RM16(0x8B, d, s)
          || R32_RM32(0x8B, d, s)
          || R64_RM64(0x8B, d, s)
          || RM8_R8(0x88, d, s)
          || RM16_R16(0x89, d, s)
          || RM32_R32(0x89, d, s)
          || RM64_R64(0x89, d, s)
          || Failed("MOV", d, s); // Throw an error
    }

    bool MOV0(const code::Address& d) { return MOV(d, zero); }
    bool MOV1(const code::Address& d) { return MOV(d, one); }

    bool MOVSX(const code::Address& d, const code::Address& s) {
      return ESC_R16_RM8(0xBE, d, s)
          || ESC_R32_RM8(0xBE, d, s)
          || ESC_R64_RM8(0xBE, d, s)
          || ESC_R32_RM16(0xBF, d, s)
          || ESC_R64_RM16(0xBF, d, s)
          || Failed("MOVSX", d, s);
    }

    bool MOVSXD(const code::Address& d, const code::Address& s) {
      return R64_RM32(0x63, d, s)
          || Failed("MOVSXD", d, s);
    }

    bool MOVZX(const code::Address& d, const code::Address& s) {
      return ESC_R16_RM8(0xB6, d, s)
          || ESC_R32_RM8(0xB6, d, s)
          || ESC_R64_RM8(0xB6, d, s)
          || ESC_R32_RM16(0xB7, d, s)
          || ESC_R64_RM16(0xB7, d, s)
          || Failed("MOVZX", d, s);
    }

    bool PUSH(const code::Address& s) {
      return R16(0x50, s)
          || R32(0x50, s)
          || R64(0x50, s)
          || IMM8(0x6A, s)
          || IMM16(0x68, s)
          || IMM32(0x68, s)
          || RM16(0xFF, 0x06, s)
          || RM32(0xFF, 0x06, s)
          || RM64(0xFF, 0x06, s)
          || Failed("PUSH", s);
    }

    bool POP(const code::Address& s) {
      return R16(0x58, s)
          || R32(0x58, s)
          || R64(0x58, s)
          || RM16(0x8F, 0x00, s)
          || RM32(0x8F, 0x00, s)
          || RM64(0x8F, 0x00, s)
          || Failed("POP", s);
    }

    bool INC(const code::Address& s) {
      return R16(0x40, s)
          || R32(0x40, s)
          || RM8(0xFE, 0x00, s)
          || RM16(0xFF, 0x00, s)
          || RM32(0xFF, 0x00, s)
          || RM64(0xFF, 0x00, s)
          || Failed("INC", s);
    }

    bool DEC(const code::Address& s) {
      return R16(0x48, s)
          || R32(0x48, s)
          || RM8(0xFE, 0x01, s)
          || RM16(0xFF, 0x01, s)
          || RM32(0xFF, 0x01, s)
          || RM64(0xFF, 0x01, s)
          || Failed("DEC", s);
    }

    bool NEG(const code::Address& s) {
      return RM8(0xF6, 0x03, s)
          || RM16(0xF7, 0x03, s)
          || RM32(0xF7, 0x03, s)
          || RM64(0xF7, 0x03, s)
          || Failed("NEG", s);
    }

    bool NOT(const code::Address& s) {
      return RM8(0xF6, 0x02, s)
          || RM16(0xF7, 0x02, s)
          || RM32(0xF7, 0x02, s)
          || RM64(0xF7, 0x02, s)
          || Failed("NOT", s);
    }

    bool CALL(const code::Address& s) {
      return REL16(0xE8, s)
          || REL32(0xE8, s)
          || RM16(0xFF, 0x02, s)
          || RM32(0xFF, 0x02, s)
          || RM64(0xFF, 0x02, s)
          || Failed("CALL", s);
    }

    bool JMP(const code::Address& s) {
      return REL8(0xEB, s)
          || REL16(0xE9, s)
          || REL32(0xE9, s)
          || RM16(0xFF, 0x04, s)
          || RM32(0xFF, 0x04, s)
          || RM64(0xFF, 0x04, s)
          || Failed("JMP", s);
    }

    // JMP instructions
    void JMP(code::Relative8& rel) { OP(0xEB); Emit(rel); }
    void JMP(code::Relative16& rel) { OpSize(); OP(0xE9); Emit(rel); }
    void JMP(code::Relative32& rel) { OP(0xE9); Emit(rel); }
    // void JMP_REL8(Relative8 rel) { OP(0xEB); Emit(rel - 2); }
    // void JMP_REL16(Relative16 rel) { OpSize(); OP(0xE9); Emit(rel - 4); }
    // void JMP_REL32(Relative32 rel) { OP(0xE9); Emit(rel - 5); }

    bool Jcc(uint8_t op8, uint8_t op, const code::Address& a, std::string_view name) {
      return REL8(op8, a)
          || ESC_REL16(op, a)
          || ESC_REL32(op, a)
          || Failed(name, a);
    }

    bool JO(const code::Address& a) { return Jcc(0x70, 0x80, a, "JO"); }
    bool JNO(const code::Address& a) { return Jcc(0x71, 0x81, a, "JNO"); }
    bool JB(const code::Address& a) { return Jcc(0x72, 0x82, a, "JB"); }
    bool JNAE(const code::Address& a) { return JB(a); }
    bool JC(const code::Address& a) { return JB(a); }
    bool JNB(const code::Address& a) { return Jcc(0x73, 0x83, a, "JNB"); }
    bool JAE(const code::Address& a) { return JNB(a); }
    bool JNC(const code::Address& a) { return JNB(a); }
    bool JZ(const code::Address& a) { return Jcc(0x74, 0x84, a, "JZ"); }
    bool JE(const code::Address& a) { return JZ(a); }
    bool JNZ(const code::Address& a) { return Jcc(0x75, 0x85, a, "JNZ"); }
    bool JNE(const code::Address& a) { return JNZ(a); }
    bool JBE(const code::Address& a) { return Jcc(0x76, 0x86, a, "JBE"); }
    bool JNA(const code::Address& a) { return JBE(a); }
    bool JNBE(const code::Address& a) { return Jcc(0x77, 0x87, a, "JNBE"); }
    bool JA(const code::Address& a) { return JNBE(a); }
    bool JS(const code::Address& a) { return Jcc(0x78, 0x88, a, "JS"); }
    bool JNS(const code::Address& a) { return Jcc(0x79, 0x89, a, "JNS"); }
    bool JP(const code::Address& a) { return Jcc(0x7A, 0x8A, a, "JP"); }
    bool JPE(const code::Address& a) { return JP(a); }
    bool JNP(const code::Address& a) { return Jcc(0x7B, 0x8B, a, "JNP"); }
    bool JPO(const code::Address& a) { return JNP(a); }
    bool JL(const code::Address& a) { return Jcc(0x7C, 0x8C, a, "JL"); }
    bool JNGE(const code::Address& a) { return JL(a); }
    bool JNL(const code::Address& a) { return Jcc(0x7D, 0x8D, a, "JNL"); }
    bool JGE(const code::Address& a) { return JNL(a); }
    bool JLE(const code::Address& a) { return Jcc(0x7E, 0x8E, a, "JLE"); }
    bool JNG(const code::Address& a) { return JLE(a); }
    bool JNLE(const code::Address& a) { return Jcc(0x7F, 0x8F, a, "JNLE"); }
    bool JG(const code::Address& a) { return JNLE(a); }

    void JO(code::Relative8& rel) { OP(0x70); Emit(rel); }
    void JO(code::Relative16& rel) { OpSize(); Escape(0x80); Emit(rel); }
    void JO(code::Relative32& rel) { Escape(0x80); Emit(rel); }
    void JNO(code::Relative8& rel) { OP(0x71); Emit(rel); }
    void JNO(code::Relative16& rel) { OpSize(); Escape(0x81); Emit(rel); }
    void JNO(code::Relative32& rel) { Escape(0x81); Emit(rel); }
    void JB(code::Relative8& rel) { OP(0x72); Emit(rel); }
    void JB(code::Relative16& rel) { OpSize(); Escape(0x82); Emit(rel); }
    void JB(code::Relative32& rel) { Escape(0x82); Emit(rel); }
    void JNB(code::Relative8& rel) { OP(0x73); Emit(rel); }
    void JNB(code::Relative16& rel) { OpSize(); Escape(0x83); Emit(rel); }
    void JNB(code::Relative32& rel) { Escape(0x83); Emit(rel); }
    void JZ(code::Relative8& rel) { OP(0x74); Emit(rel); }
    void JZ(code::Relative16& rel) { OpSize(); Escape(0x84); Emit(rel); }
    void JZ(code::Relative32& rel) { Escape(0x84); Emit(rel); }
    void JNZ(code::Relative8& rel) { OP(0x75); Emit(rel); }
    void JNZ(code::Relative16& rel) { OpSize(); Escape(0x85); Emit(rel); }
    void JNZ(code::Relative32& rel) { Escape(0x85); Emit(rel); }
    void JBE(code::Relative8& rel) { OP(0x76); Emit(rel); }
    void JBE(code::Relative16& rel) { OpSize(); Escape(0x86); Emit(rel); }
    void JBE(code::Relative32& rel) { Escape(0x86); Emit(rel); }
    void JNBE(code::Relative8& rel) { OP(0x77); Emit(rel); }
    void JNBE(code::Relative16& rel) { OpSize(); Escape(0x87); Emit(rel); }
    void JNBE(code::Relative32& rel) { Escape(0x87); Emit(rel); }
    void JS(code::Relative8& rel) { OP(0x78); Emit(rel); }
    void JS(code::Relative16& rel) { OpSize(); Escape(0x88); Emit(rel); }
    void JS(code::Relative32& rel) { Escape(0x88); Emit(rel); }
    void JNS(code::Relative8& rel) { OP(0x79); Emit(rel); }
    void JNS(code::Relative16& rel) { OpSize(); Escape(0x89); Emit(rel); }
    void JNS(code::Relative32& rel) { Escape(0x89); Emit(rel); }
    void JP(code::Relative8& rel) { OP(0x7A); Emit(rel); }
    void JP(code::Relative16& rel) { OpSize(); Escape(0x8A); Emit(rel); }
    void JP(code::Relative32& rel) { Escape(0x8A); Emit(rel); }
    void JNP(code::Relative8& rel) { OP(0x7B); Emit(rel); }
    void JNP(code::Relative16& rel) { OpSize(); Escape(0x8B); Emit(rel); }
    void JNP(code::Relative32& rel) { Escape(0x8B); Emit(rel); }
    void JL(code::Relative8& rel) { OP(0x7C); Emit(rel); }
    void JL(code::Relative16& rel) { OpSize(); Escape(0x8C); Emit(rel); }
    void JL(code::Relative32& rel) { Escape(0x8C); Emit(rel); }
    void JNL(code::Relative8& rel) { OP(0x7D); Emit(rel); }
    void JNL(code::Relative16& rel) { OpSize(); Escape(0x8D); Emit(rel); }
    void JNL(code::Relative32& rel) { Escape(0x8D); Emit(rel); }
    void JLE(code::Relative8& rel) { OP(0x7E); Emit(rel); }
    void JLE(code::Relative16& rel) { OpSize(); Escape(0x8E); Emit(rel); }
    void JLE(code::Relative32& rel) { Escape(0x8E); Emit(rel); }
    void JNLE(code::Relative8& rel) { OP(0x7F); Emit(rel); }
    void JNLE(code::Relative16& rel) { OpSize(); Escape(0x8F); Emit(rel); }
    void JNLE(code::Relative32& rel) { Escape(0x8F); Emit(rel); }

    bool SETcc(uint8_t op, const code::Address& a, std::string_view name) {
      return ESC_RM8(op, 0x00, a)
          || Failed(name, a);
    }

    bool SETO(const code::Address& a) { return SETcc(0x90, a, "SETO"); }
    bool SETNO(const code::Address& a) { return SETcc(0x91, a, "SETNO"); }
    bool SETB(const code::Address& a) { return SETcc(0x92, a, "SETB"); }
    bool SETNAE(const code::Address& a) { return SETB(a); }
    bool SETC(const code::Address& a) { return SETB(a); }
    bool SETNB(const code::Address& a) { return SETcc(0x93, a, "SETNB"); }
    bool SETAE(const code::Address& a) { return SETNB(a); }
    bool SETNC(const code::Address& a) { return SETNB(a); }
    bool SETZ(const code::Address& a) { return SETcc(0x94, a, "SETZ"); }
    bool SETE(const code::Address& a) { return SETZ(a); }
    bool SETNZ(const code::Address& a) { return SETcc(0x95, a, "SETNZ"); }
    bool SETNE(const code::Address& a) { return SETNZ(a); }
    bool SETBE(const code::Address& a) { return SETcc(0x96, a, "SETBE"); }
    bool SETNA(const code::Address& a) { return SETBE(a); }
    bool SETNBE(const code::Address& a) { return SETcc(0x97, a, "SETNBE"); }
    bool SETA(const code::Address& a) { return SETNBE(a); }
    bool SETS(const code::Address& a) { return SETcc(0x98, a, "SETS"); }
    bool SETNS(const code::Address& a) { return SETcc(0x99, a, "SETNS"); }
    bool SETP(const code::Address& a) { return SETcc(0x9A, a, "SETP"); }
    bool SETPE(const code::Address& a) { return SETP(a); }
    bool SETNP(const code::Address& a) { return SETcc(0x9B, a, "SETNP"); }
    bool SETPO(const code::Address& a) { return SETNP(a); }
    bool SETL(const code::Address& a) { return SETcc(0x9C, a, "SETL"); }
    bool SETNGE(const code::Address& a) { return SETL(a); }
    bool SETNL(const code::Address& a) { return SETcc(0x9D, a, "SETNL"); }
    bool SETGE(const code::Address& a) { return SETNL(a); }
    bool SETLE(const code::Address& a) { return SETcc(0x9E, a, "SETLE"); }
    bool SETNG(const code::Address& a) { return SETLE(a); }
    bool SETNLE(const code::Address& a) { return SETcc(0x9F, a, "SETNLE"); }
    bool SETG(const code::Address& a) { return SETNLE(a); }

    bool CMOVcc(uint8_t op, const code::Address& d, const code::Address& s, std::string_view name) {
      return ESC_R16_RM16(op, d, s)
          || ESC_R32_RM32(op, d, s)
          || ESC_R64_RM64(op, d, s)
          || Failed(name, d, s);
    }

    bool CMOVO(const code::Address& d, const code::Address& s) { return CMOVcc(0x40, d, s, "CMOVO"); }
    bool CMOVNO(const code::Address& d, const code::Address& s) { return CMOVcc(0x41, d, s, "CMOVNO"); }
    bool CMOVB(const code::Address& d, const code::Address& s) { return CMOVcc(0x42, d, s, "CMOVB"); }
    bool CMOVNAE(const code::Address& d, const code::Address& s) { return CMOVB(d, s); }
    bool CMOVC(const code::Address& d, const code::Address& s) { return CMOVB(d, s); }
    bool CMOVNB(const code::Address& d, const code::Address& s) { return CMOVcc(0x43, d, s, "CMOVNB"); }
    bool CMOVAE(const code::Address& d, const code::Address& s) { return CMOVNB(d, s); }
    bool CMOVNC(const code::Address& d, const code::Address& s) { return CMOVNB(d, s); }
    bool CMOVZ(const code::Address& d, const code::Address& s) { return CMOVcc(0x44, d, s, "CMOVZ"); }
    bool CMOVE(const code::Address& d, const code::Address& s) { return CMOVZ(d, s); }
    bool CMOVNZ(const code::Address& d, const code::Address& s) { return CMOVcc(0x45, d, s, "CMOVNZ"); }
    bool CMOVNE(const code::Address& d, const code::Address& s) { return CMOVNZ(d, s); }
    bool CMOVBE(const code::Address& d, const code::Address& s) { return CMOVcc(0x46, d, s, "CMOVBE"); }
    bool CMOVNA(const code::Address& d, const code::Address& s) { return CMOVBE(d, s); }
    bool CMOVNBE(const code::Address& d, const code::Address& s) { return CMOVcc(0x47, d, s, "CMOVNBE"); }
    bool CMOVA(const code::Address& d, const code::Address& s) { return CMOVNBE(d, s); }
    bool CMOVS(const code::Address& d, const code::Address& s) { return CMOVcc(0x48, d, s, "CMOVS"); }
    bool CMOVNS(const code::Address& d, const code::Address& s) { return CMOVcc(0x49, d, s, "CMOVNS"); }
    bool CMOVP(const code::Address& d, const code::Address& s) { return CMOVcc(0x4A, d, s, "CMOVP"); }
    bool CMOVPE(const code::Address& d, const code::Address& s) { return CMOVP(d, s); }
    bool CMOVNP(const code::Address& d, const code::Address& s) { return CMOVcc(0x4B, d, s, "CMOVNP"); }
    bool CMOVPO(const code::Address& d, const code::Address& s) { return CMOVNP(d, s); }
    bool CMOVL(const code::Address& d, const code::Address& s) { return CMOVcc(0x4C, d, s, "CMOVL"); }
    bool CMOVNGE(const code::Address& d, const code::Address& s) { return CMOVL(d, s); }
    bool CMOVNL(const code::Address& d, const code::Address& s) { return CMOVcc(0x4D, d, s, "CMOVNL"); }
    bool CMOVGE(const code::Address& d, const code::Address& s) { return CMOVNL(d, s); }
    bool CMOVLE(const code::Address& d, const code::Address& s) { return CMOVcc(0x4E, d, s, "CMOVLE"); }
    bool CMOVNG(const code::Address& d, const code::Address& s) { return CMOVLE(d, s); }
    bool CMOVNLE(const code::Address& d, const code::Address& s) { return CMOVcc(0x4F, d, s, "CMOVNLE"); }
    bool CMOVG(const code::Address& d, const code::Address& s) { return CMOVNLE(d, s); }

    bool X32_RM32(uint8_t op, const code::Address& x, const code::Address& rm, bool esc = false) {
      if (!x.IsAllocated() || !x.Is32Bit() || !x.IsSIMD() || !rm.Is32Bit()) return false;

      Single();
      REX(x, rm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(x, rm);

      return true;
    }

    bool X32_RM64(uint8_t op, const code::Address& x, const code::Address& rm, bool esc = false) {
      if (!x.IsAllocated() || !x.Is32Bit() || !x.IsSIMD() || !rm.Is64Bit()) return false;

      Single();
      REX64(x, rm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(x, rm);

      return true;
    }

    bool X64_RM32(uint8_t op, const code::Address& x, const code::Address& rm, bool esc = false) {
      if (!x.IsAllocated() || !x.Is32Bit() || !x.IsSIMD() || !rm.Is32Bit()) return false;

      Double();
      REX(x, rm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(x, rm);

      return true;
    }

    bool X64_RM64(uint8_t op, const code::Address& x, const code::Address& rm, bool esc = false) {
      if (!x.IsAllocated() || !x.Is64Bit() || !x.IsSIMD() || !rm.Is64Bit()) return false;

      Double();
      REX64(x, rm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(x, rm);

      return true;
    }

    bool R32_XM32(uint8_t op, const code::Address& r, const code::Address& xm, bool esc = false) {
      if (!r.IsAllocated() || !r.Is32Bit() || !xm.IsSIMD() || !xm.Is32Bit()) return false;

      Single();
      REX(r, xm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(r, xm);

      return true;
    }

    bool R64_XM32(uint8_t op, const code::Address& r, const code::Address& xm, bool esc = false) {
      if (!r.IsAllocated() || !r.Is64Bit() || !xm.IsSIMD() || !xm.Is32Bit()) return false;

      Single();
      REX64(r, xm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(r, xm);

      return true;
    }

    bool R32_XM64(uint8_t op, const code::Address& r, const code::Address& xm, bool esc = false) {
      if (!r.IsAllocated() || !r.Is32Bit() || !xm.IsSIMD() || !xm.Is64Bit()) return false;

      Double();
      REX(r, xm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(r, xm);

      return true;
    }

    bool R64_XM64(uint8_t op, const code::Address& r, const code::Address& xm, bool esc = false) {
      if (!r.IsAllocated() || !r.Is64Bit() || !xm.IsSIMD() || !xm.Is64Bit()) return false;

      Double();
      REX64(r, xm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(r, xm);

      return true;
    }

    bool X32_XM64(uint8_t op, const code::Address& x, const code::Address& xm, bool esc = false) {
      if (!x.IsAllocated() || !x.Is32Bit() || !x.IsSIMD() || !xm.Is64Bit() || !xm.IsSIMD()) return false;

      Single();
      REX64(x, xm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(x, xm);

      return true;
    }

    bool X64_XM32(uint8_t op, const code::Address& x, const code::Address& xm, bool esc = false) {
      if (!x.IsAllocated() || !x.Is64Bit() || !x.IsSIMD() || !xm.Is32Bit() || !xm.IsSIMD()) return false;

      Double();
      REX(x, xm);
      if (esc) Escape(op);
      else OP(op);
      ModRM(x, xm);

      return true;
    }

    bool X64_XM64(uint8_t op, const code::Address& x, const code::Address& xm, bool esc = false, bool size = false) {
      if (!x.IsAllocated() || !x.Is64Bit() || !x.IsSIMD() || !xm.Is64Bit() || !xm.IsSIMD()) return false;

      if (size) OpSize();
      REX(x, xm);
      if (esc) Escape(op);
      else OP(op);

      ModRM(x, xm);

      return true;
    }

    bool XM64_X64(uint8_t op, const code::Address& xm, const code::Address& x, bool esc = false, bool size = false) {
      return X64_XM64(op, x, xm, esc, size);
    }

    bool X128_XM128(uint8_t op, const code::Address& x, const code::Address& xm, bool esc = false, bool size = false) {
      if (!x.IsAllocated() || !x.Is128Bit() || !x.IsSIMD() || !xm.Is128Bit() || !xm.IsSIMD()) return false;

      if (size) OpSize();
      REX(x, xm);
      if (esc) Escape(op);
      else OP(op);

      ModRM(x, xm);

      return true;
    }

    bool XM128_X128(uint8_t op, const code::Address& xm, const code::Address& x, bool esc = false, bool size = false) {
      return X128_XM128(op, x, xm, esc, size);
    }

    // Convert Scalar Doubleword Integer to Scalar Single-Precision FP
    bool CVTSI2SS(const code::Address& d, const code::Address& s) {
      return X32_RM32(0x2A, d, s, true)
          || X32_RM64(0x2A, d, s, true)
          || Failed("CVTSI2SS", d, s);
    }

    // Convert Scalar Doubleword Integer to Scalar Double-Precision FP
    bool CVTSI2SD(const code::Address& d, const code::Address& s) {
      return X64_RM32(0x2A, d, s, true)
          || X64_RM64(0x2A, d, s, true)
          || Failed("CVTSI2SD", d, s);
    }

    // Convert Scalar Single-Precision FP to Scalar Doubleword Integer
    bool CVTSS2SI(const code::Address& d, const code::Address& s) {
      return R32_XM32(0x2D, d, s, true)
          || R64_XM32(0x2D, d, s, true)
          || Failed("CVTSS2SI", d, s);
    }

    // Convert Scalar Double-Precision FP to Scalar Doubleword Integer
    bool CVTSD2SI(const code::Address& d, const code::Address& s) {
      return R32_XM64(0x2D, d, s, true)
          || R64_XM64(0x2D, d, s, true)
          || Failed("CVTSD2SI", d, s);
    }

    // Convert with Trunc. Scalar Single-FP Value to Doubleword Integer
    bool CVTTSS2SI(const code::Address& d, const code::Address& s) {
      return R32_XM32(0x2C, d, s, true)
          || R64_XM32(0x2C, d, s, true)
          || Failed("CVTTSS2SI", d, s);
    }

    // Convert with Trunc. Scalar Double-FP Value to Signed Doubleword Integer
    bool CVTTSD2SI(const code::Address& d, const code::Address& s) {
      return R32_XM32(0x2C, d, s, true)
          || R64_XM32(0x2C, d, s, true)
          || Failed("CVTTSD2SI", d, s);
    }

    // Convert Scalar Single-Precision Floating-Point Value to Scalar Double-Precision Floating-Point Value
    bool CVTSS2SD(const code::Address& d, const code::Address& s) {
      return X64_XM32(0x5A, d, s, true)
          || X64_XM32(0x5A, d, s, true)
          || Failed("CVTSS2SD", d, s);
    }

    // Convert Scalar Double-Precision Floating-Point Value to Scalar Single-Precision Floating-Point Value
    bool CVTSD2SS(const code::Address& d, const code::Address& s) {
      return X32_XM64(0x5A, d, s, true)
          || X32_XM64(0x5A, d, s, true)
          || Failed("CVTSD2SS", d, s);
    }

    // Move Aligned Packed Single-Precision Floating-Point Values
    bool MOVAPS(const code::Address& d, const code::Address& s) {
      return X128_XM128(0x28, d, s, true, false)
          || XM128_X128(0x29, d, s, true, false)
          || Failed("MOVAPS", d, s);
    }

    // Move Aligned Packed Double-Precision Floating-Point Values
    bool MOVAPD(const code::Address& d, const code::Address& s) {
      return X128_XM128(0x28, d, s, true, true)
          || XM128_X128(0x29, d, s, true, true)
          || Failed("MOVAPD", d, s);
    }

    // Move Low Packed Single-Precision Floating-Point Values
    bool MOVLPS(const code::Address& d, const code::Address& s) {
      return X64_XM64(0x12, d, s, true, false)
          || XM64_X64(0x13, d, s, true, false)
          || Failed("MOVLPS", d, s);
    }

    // Move Low Packed Double-Precision Floating-Point Value
    bool MOVLPD(const code::Address& d, const code::Address& s) {
      return X64_XM64(0x12, d, s, true, true)
          || XM64_X64(0x13, d, s, true, true)
          || Failed("MOVLPD", d, s);
    }

    bool INT3() { OP(0xCC); return true; }
    bool SYSCALL() { Escape(0x05); return true; }

    // // Opcode 0x00: ADD r/m8, r8
    // void ADD_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x00); ModRM(r1, r2); }
    // void ADD_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x00); ModRM(m, r); }
    // void ADD_R8_R8_LOCK(IsGPR8 auto r1, IsGPR8 auto r2) { Lock(); REX(r1, r2); OP(0x00); ModRM(r1, r2); }
    // void ADD_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x00); ModRM(m, r); }

    // // Opcode 0x01: ADD r/m16/32/64, r16/32/64
    // void ADD_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    // void ADD_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x01); ModRM(m, r); }
    // void ADD_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    // void ADD_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x01); ModRM(m, r); }
    // void ADD_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    // void ADD_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x01); ModRM(m, r); }

    // void ADD_R16_R16_LOCK(IsGPR16 auto r1, IsGPR16 auto r2) { Lock(); OpSize(); REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    // void ADD_M16_R16_LOCK(IsMemory16 auto m, IsGPR16 auto r) { Lock(); OpSize(); REX(m, r); OP(0x01); ModRM(m, r); }
    // void ADD_R32_R32_LOCK(IsGPR32 auto r1, IsGPR32 auto r2) { Lock(); REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    // void ADD_M32_R32_LOCK(IsMemory32 auto m, IsGPR32 auto r) { Lock(); REX(m, r); OP(0x01); ModRM(m, r); }
    // void ADD_R64_R64_LOCK(IsGPR64 auto r1, IsGPR64 auto r2) { Lock(); REX(r1, r2); OP(0x01); ModRM(r1, r2); }
    // void ADD_M64_R64_LOCK(IsMemory64 auto m, IsGPR64 auto r) { Lock(); REX(m, r); OP(0x01); ModRM(m, r); }

    // struct Op {
    //   uint8_t code;

    //   explicit Op(uint8_t code) : code{code} {}
    // };

    // struct Op8 {
    //   uint8_t code;

    //   explicit Op8(uint8_t code) : code{code} {}
    // };

    // struct OpEsc {
    //   uint8_t code;

    //   explicit OpEsc(uint8_t code) : code{code} {}
    // };

    // struct Mod {
    //   uint8_t value;

    //   explicit Mod(uint8_t value) : value{value} {}
    // };

    // void Encode(const code::Register& dest, Op op, Op8 alt, Mod mod, bool lock = false) {
    //   // if (lock) Lock();
    //   // if (dest.Is16Bit()) OpSize();

    //   // REX(dest);

    //   // if (dest.Is8Bit()) OP(alt.code);
    //   // else OP(op.code);

    //   // ModRM(mod.value, dest);
    // }

    // void Encode(const code::Address& dest, Op op, Op8 alt, Mod mod, bool lock = false) {
    //   // if (lock) Lock();
    //   // if (dest.Is16Bit()) OpSize();

    //   // REX(dest);

    //   // if (dest.Is8Bit()) OP(alt.code);
    //   // else OP(op.code);

    //   // ModRM(mod.value, dest);
    // }

    // void Encode(const code::Register& dest, const code::Register& src, Op op, Op8 alt, bool lock = false) {
    //   // if (lock) Lock();
    //   // if (dest.Is16Bit()) OpSize();
    //   // REX(dest, src);

    //   // if (dest.Is8Bit()) OP(alt.code);
    //   // else OP(op.code);

    //   // ModRM(dest, src);
    // }

    // void Encode(const code::Address& dest, const code::Register& src, Op op, Op8 alt, bool lock = false) {
    //   // if (lock) Lock();
    //   // if (dest.Is16Bit()) OpSize();
    //   // REX(dest, src);

    //   // if (dest.Is8Bit()) OP(alt.code);
    //   // else OP(op.code);

    //   // ModRM(dest, src);
    // }

    // void Encode(const code::Register& dest, const code::Address& src, Op op, Op8 alt, bool lock = false) {
    //   // if (lock) Lock();
    //   // if (dest.Is16Bit()) OpSize();
    //   // REX(dest, src);

    //   // if (dest.Is8Bit()) OP(alt.code);
    //   // else OP(op.code);

    //   // ModRM(dest, src);
    // }

    // // void Encode(const code::Register& dest, code::Immediate8& src, Op op, bool lock = false) {
    // //   if (lock) Lock();
    // //   if (dest.Is16Bit()) OpSize();

    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Register& dest, code::Immediate8& src, Op op, Op8 alt, Mod mod, bool lock = false) {
    // //   if (lock) Lock();
    // //   if (dest.Is16Bit()) OpSize();

    // //   REX(dest);

    // //   if (dest.Is8Bit()) OP(alt.code);
    // //   else OP(op.code);

    // //   ModRM(mod.value, dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Register& dest, code::Immediate16& src, Op op, bool lock = false) {
    // //   if (!dest.Is16Bit()) throw utility::Error("Cannot use a 16 bit immediate with a destination that isn't also 16 bits");

    // //   if (lock) Lock();
    // //   OpSize();
    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Register& dest, code::Immediate16& src, Op op, Mod mod, bool lock = false) {
    // //   if (!dest.Is16Bit()) throw utility::Error("Cannot use a 16 bit immediate with a destination that isn't also 16 bits");

    // //   if (lock) Lock();
    // //   OpSize();
    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(mod.value, dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Register& dest, code::Immediate32& src, Op op, bool lock = false) {
    // //   if (dest.Is8Bit() || dest.Is16Bit()) throw utility::Error("Cannot use a 32 bit immediate with an 8 or 16 bit destination");

    // //   if (lock) Lock();
    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Register& dest, code::Immediate32& src, Op op, Mod mod, bool lock = false) {
    // //   if (dest.Is8Bit() || dest.Is16Bit()) throw utility::Error("Cannot use a 32 bit immediate with an 8 or 16 bit destination");

    // //   if (lock) Lock();
    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(mod.value, dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Register& dest, code::Immediate64& src, Op op, bool lock = false) {
    // //   if (!dest.Is64Bit()) throw utility::Error("Cannot use a 64 bit immediate with a destination that isn't 64 bit");

    // //   if (lock) Lock();
    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Address& dest, code::Immediate8& src, Op op, Mod mod, bool lock = false) {
    // //   if (lock) Lock();
    // //   if (dest.Is16Bit()) OpSize();

    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(mod.value, dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Address& dest, code::Immediate8& src, Op op, Op8 alt, Mod mod, bool lock = false) {
    // //   if (lock) Lock();
    // //   if (dest.Is16Bit()) OpSize();

    // //   REX(dest);

    // //   if (dest.Is8Bit()) OP(alt.code);
    // //   else OP(op.code);

    // //   ModRM(mod.value, dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Address& dest, code::Immediate16& src, Op op, Mod mod, bool lock = false) {
    // //   if (!dest.Is16Bit()) throw utility::Error("Cannot use a 16 bit immediate with a destination that isn't also 16 bits");

    // //   if (lock) Lock();
    // //   OpSize();
    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(mod.value, dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Address& dest, code::Immediate32& src, Op op, Mod mod, bool lock = false) {
    // //   if (dest.Is8Bit() || dest.Is16Bit()) throw utility::Error("Cannot use a 32 bit immediate with an 8 or 16 bit destination");

    // //   if (lock) Lock();
    // //   REX(dest);
    // //   OP(op.code);
    // //   ModRM(mod.value, dest);
    // //   Emit(src);
    // // }

    // // void Encode(const code::Register& dest, const code::Register& src, code::Immediate8& imm, Op op, bool lock = false) {
    // //   if (lock) Lock();
    // //   REX(dest, src);
    // //   OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Register& src, code::Immediate16& imm, Op op, bool lock = false) {
    // //   if (lock) Lock();
    // //   OpSize();
    // //   REX(dest, src);
    // //   OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Register& src, code::Immediate32& imm, Op op, bool lock = false) {
    // //   if (dest.Is8Bit() || dest.Is16Bit()) throw utility::Error("Cannot use a 32 bit immediate with an 8 or 16 bit destination");

    // //   if (lock) Lock();
    // //   REX(dest, src);
    // //   OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Address& src, code::Immediate8& imm, Op op, bool lock = false) {
    // //   if (lock) Lock();
    // //   REX(dest, src);
    // //   OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Address& src, code::Immediate16& imm, Op op, bool lock = false) {
    // //   if (lock) Lock();
    // //   OpSize();
    // //   REX(dest, src);
    // //   OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Address& src, code::Immediate32& imm, Op op, bool lock = false) {
    // //   if (dest.Is8Bit() || dest.Is16Bit()) throw utility::Error("Cannot use a 32 bit immediate with an 8 or 16 bit destination");

    // //   if (lock) Lock();
    // //   REX(dest, src);
    // //   OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Register& src, code::Immediate16& imm, Op op, Op8 alt, bool lock = false) {
    // //   if (lock) Lock();
    // //   OpSize();
    // //   REX(dest, src);
    // //   if (dest.Is8Bit()) OP(alt.code);
    // //   else OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Register& src, code::Immediate32& imm, Op op, Op8 alt, bool lock = false) {
    // //   if (dest.Is8Bit() || dest.Is16Bit()) throw utility::Error("Cannot use a 32 bit immediate with an 8 or 16 bit destination");

    // //   if (lock) Lock();
    // //   REX(dest, src);
    // //   if (dest.Is8Bit()) OP(alt.code);
    // //   else OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Address& src, code::Immediate16& imm, Op op, Op8 alt, bool lock = false) {
    // //   if (lock) Lock();
    // //   OpSize();
    // //   REX(dest, src);
    // //   if (dest.Is8Bit()) OP(alt.code);
    // //   else OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // // void Encode(const code::Register& dest, const code::Address& src, code::Immediate32& imm, Op op, Op8 alt, bool lock = false) {
    // //   if (dest.Is8Bit() || dest.Is16Bit()) throw utility::Error("Cannot use a 32 bit immediate with an 8 or 16 bit destination");

    // //   if (lock) Lock();
    // //   REX(dest, src);
    // //   if (dest.Is8Bit()) OP(alt.code);
    // //   else OP(op.code);
    // //   ModRM(dest, src);
    // //   Emit(imm);
    // // }

    // void Encode(const code::Register& dest, OpEsc op, Mod mod) {
    //   // if (!dest.Is8Bit()) throw utility::Error("As far as I know this encode version is only used with SETcc so it should be 8 bit");

    //   // REX(dest);
    //   // Escape(op.code);
    //   // ModRM(mod.value, dest);
    // }

    // void Encode(const code::Address& dest, OpEsc op, Mod mod) {
    //   // if (!dest.Is8Bit()) throw utility::Error("As far as I know this encode version is only used with SETcc so it should be 8 bit");

    //   // REX(dest);
    //   // Escape(op.code);
    //   // ModRM(mod.value, dest);
    // }

    // void Encode(const code::Register& dest, const code::Register& src, OpEsc op, bool lock = false) {
    //   // if (lock) Lock();
    //   // if (dest.Is16Bit()) OpSize();

    //   // REX(dest, src);
    //   // Escape(op.code);
    //   // ModRM(dest, src);
    // }

    // void Encode(const code::Register& dest, const code::Address& src, OpEsc op, bool lock = false) {
    //   // if (lock) Lock();
    //   // if (dest.Is16Bit()) OpSize();

    //   // REX(dest, src);
    //   // Escape(op.code);
    //   // ModRM(dest, src);
    // }

    // void ADD(const code::Register& d, const code::Register& s) { Encode(d, s, Op{0x01}, Op8{0x00}); }
    // void ADD(const code::Address& d, const code::Register& s) { Encode(d, s, Op{0x01}, Op8{0x00}); }
    // void ADD(const code::Register& d, const code::Address& s) { Encode(d, s, Op{0x03}, Op8{0x02}); }
    // // void ADD(const code::Register& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x00}); }
    // // void ADD(const code::Register& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x00}); }
    // // void ADD(const code::Register& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x00}); }
    // // void ADD(const code::Address& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x00}); }
    // // void ADD(const code::Address& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x00}); }
    // // void ADD(const code::Address& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x00}); }

    // void ADD_AL(Immediate8 i) { OP(0x04); Emit8(i); }
    // void ADD_AX(Immediate16 i) { OpSize(); OP(0x05); Emit16(i); }
    // void ADD_EAX(Immediate32 i) { OP(0x05); Emit32(i); }
    // void ADD_RAX(Immediate32 i) { REXW(); OP(0x05); Emit32(i); }

    // // Opcode 0x02: ADD r8, r/m8
    // void ADD_R8_R8_ALT(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x02); ModRM(r1, r2); }
    // void ADD_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { REX(r, m); OP(0x02); ModRM(r, m); }

    // // Opcode 0x03: ADD r16/32/64, r/m16/32/64
    // void ADD_R16_R16_ALT(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x03); ModRM(r1, r2); }
    // void ADD_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x03); ModRM(r, m); }
    // void ADD_R32_R32_ALT(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x03); ModRM(r1, r2); }
    // void ADD_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(r, m); OP(0x03); ModRM(r, m); }
    // void ADD_R64_R64_ALT(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x03); ModRM(r1, r2); }
    // void ADD_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x03); ModRM(r, m);}

    // // Opcode 0x04: ADD AL, imm8
    // void ADD_AL_IMM8(Immediate8 i) { OP(0x04); Emit8(i); }

    // // Opcode 0x05: ADD rAX, imm16/32
    // void ADD_AX_IMM16(Immediate16 i) { OpSize(); OP(0x05); Emit16(i); }
    // void ADD_EAX_IMM32(Immediate32 i) { OP(0x05); Emit32(i); }
    // void ADD_RAX_IMM32(Immediate32 i) { REXW(); OP(0x05); Emit32(i); }

    // // Opcode 0x83: ADD r/m16/32/64, imm8
    // void ADD_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); REX(m); OP(0x83); ModRM(0x00, m); Emit8(i); }
    // void ADD_M32_IMM8(IsMemory32 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x00, m); Emit8(i); }
    // void ADD_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x00, m); Emit8(i); }
    // void ADD_M16_IMM8_LOCK(IsMemory16 auto m, Immediate8 i) { Lock(); OpSize(); REX(m); OP(0x83); ModRM(0x00, m); Emit8(i); }
    // void ADD_M32_IMM8_LOCK(IsMemory32 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x00, m); Emit8(i); }
    // void ADD_M64_IMM8_LOCK(IsMemory64 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x00, m); Emit8(i); }

    // void ADD_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x00, r); Emit8(i); }
    // void ADD_R32_IMM8(IsGPR32 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x00, r); Emit8(i); }
    // void ADD_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x00, r); Emit8(i); }
    // // Placeholder8 ADD_R16_IMM8(IsGPR16 auto r) { ADD_R16_IMM8(r, 0); return Placeholder8{code}; }
    // // Placeholder8 ADD_R32_IMM8(IsGPR32 auto r) { ADD_R32_IMM8(r, 0); return Placeholder8{code}; }
    // // Placeholder8 ADD_R64_IMM8(IsGPR64 auto r) { ADD_R64_IMM8(r, 0); return Placeholder8{code}; }
    // void ADD_R16_IMM8_LOCK(IsGPR16 auto r, Immediate8 i) { Lock(); OpSize(); REX(r); OP(0x83); ModRM(0x00, r); Emit8(i); }
    // void ADD_R32_IMM8_LOCK(IsGPR32 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x00, r); Emit8(i); }
    // void ADD_R64_IMM8_LOCK(IsGPR64 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x00, r); Emit8(i); }

    // // Opcode 0x81: ADD r/m16/32/64, imm16/32
    // void ADD_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); REX(m); OP(0x81); ModRM(0x00, m); Emit16(i); }
    // void ADD_M32_IMM32(IsMemory32 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x00, m); Emit32(i); }
    // void ADD_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x00, m); Emit32(i); }
    // // Placeholder16 ADD_M16_IMM16(IsMemory16 auto m) { ADD_M16_IMM16(m, 0); return Placeholder16{code}; }
    // // Placeholder32 ADD_M32_IMM32(IsMemory32 auto m) { ADD_M32_IMM32(m, 0); return Placeholder32{code}; }
    // // Placeholder32 ADD_M64_IMM32(IsMemory64 auto m) { ADD_M64_IMM32(m, 0); return Placeholder32{code}; }
    // void ADD_M16_IMM16_LOCK(IsMemory16 auto m, Immediate16 i) { Lock(); OpSize(); REX(m); OP(0x81); ModRM(0x00, m); Emit16(i); }
    // void ADD_M32_IMM32_LOCK(IsMemory32 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x00, m); Emit32(i); }
    // void ADD_M64_IMM32_LOCK(IsMemory64 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x00, m); Emit32(i); }

    // void ADD_R16_IMM16(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x00, r); Emit16(i); }
    // void ADD_R32_IMM32(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x00, r); Emit32(i); }
    // void ADD_R64_IMM32(IsGPR64 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x00, r); Emit32(i); }
    // // Placeholder16 ADD_R16_IMM16(IsGPR16 auto r) { ADD_R16_IMM16(r, 0); return Placeholder16{code}; }
    // // Placeholder32 ADD_R32_IMM32(IsGPR32 auto r) { ADD_R32_IMM32(r, 0); return Placeholder32{code}; }
    // // Placeholder32 ADD_R64_IMM32(IsGPR64 auto r) { ADD_R64_IMM32(r, 0); return Placeholder32{code}; }
    // void ADD_R16_IMM16_LOCK(IsGPR16 auto r, Immediate16 i) { Lock(); OpSize(); REX(r); OP(0x81); ModRM(0x00, r); Emit16(i); }
    // void ADD_R32_IMM32_LOCK(IsGPR32 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x00, r); Emit32(i); }
    // void ADD_R64_IMM32_LOCK(IsGPR64 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x00, r); Emit32(i); }

    // void OR(const code::Register& d, const code::Register& s) { Encode(d, s, Op{0x09}, Op8{0x08}); }
    // void OR(const code::Address& d, const code::Register& s) { Encode(d, s, Op{0x09}, Op8{0x08}); }
    // void OR(const code::Register& d, const code::Address& s) { Encode(d, s, Op{0x0B}, Op8{0x0A}); }
    // // void OR(const code::Register& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x01}); }
    // // void OR(const code::Register& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x01}); }
    // // void OR(const code::Register& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x01}); }
    // // void OR(const code::Address& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x01}); }
    // // void OR(const code::Address& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x01}); }
    // // void OR(const code::Address& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x01}); }

    // // Opcode 0x08: OR r/m8, r8
    // void OR_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x08); ModRM(r1, r2); }
    // void OR_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x08); ModRM(m, r); }
    // void OR_R8_R8_LOCK(IsGPR8 auto r1, IsGPR8 auto r2) { Lock(); REX(r1, r2); OP(0x08); ModRM(r1, r2); }
    // void OR_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x08); ModRM(m, r); }

    // // Opcode 0x09: OR r/m16/32/64, r16/32/64
    // void OR_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x09); ModRM(r1, r2); }
    // void OR_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x09); ModRM(m, r); }
    // void OR_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x09); ModRM(r1, r2); }
    // void OR_R16_R16_LOCK(IsGPR16 auto r1, IsGPR16 auto r2) { Lock(); OpSize(); REX(r1, r2); OP(0x09); ModRM(r1, r2); }
    // void OR_M16_R16_LOCK(IsMemory16 auto m, IsGPR16 auto r) { Lock(); OpSize(); REX(m, r); OP(0x09); ModRM(m, r); }
    // void OR_R32_R32_LOCK(IsGPR32 auto r1, IsGPR32 auto r2) { Lock(); REX(r1, r2); OP(0x09); ModRM(r1, r2); }

    // void OR_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x09); ModRM(m, r); }
    // void OR_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x09); ModRM(r1, r2); }
    // void OR_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x09); ModRM(m, r); }
    // void OR_M32_R32_LOCK(IsMemory32 auto m, IsGPR32 auto r) { Lock(); REX(m, r); OP(0x09); ModRM(m, r); }
    // void OR_R64_R64_LOCK(IsGPR64 auto r1, IsGPR64 auto r2) { Lock(); REX(r1, r2); OP(0x09); ModRM(r1, r2); }
    // void OR_M64_R64_LOCK(IsMemory64 auto m, IsGPR64 auto r) { Lock(); REX(m, r); OP(0x09); ModRM(m, r); }

    // // Opcode 0x0A: OR r8, r/m8
    // void OR_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { REX(m, r); OP(0x0A); ModRM(r, m); }
    // void OR_R8_R8_ALT(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x0A); ModRM(r1, r2); }

    // // Opcode 0x0B: OR r16/32/64, r/m16/32/64
    // void OR_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x0B); ModRM(r, m); }
    // void OR_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(r, m); OP(0x0B); ModRM(r, m); }
    // void OR_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x0B); ModRM(r, m); }
    // void OR_R16_R16_ALT(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x0B); ModRM(r1, r2); }
    // void OR_R32_R32_ALT(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x0B); ModRM(r1, r2); }
    // void OR_R64_R64_ALT(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x0B); ModRM(r1, r2); }

    // // Opcode 0x0C: OR AL, imm8
    // void OR_AL_IMM8(Immediate8 i) { OP(0x0C); Emit8(i); }

    // // Opcode 0x0D: OR rAX, imm16/32
    // void OR_AX_IMM16(Immediate16 i) { OpSize(); OP(0x0D); Emit16(i); }
    // void OR_EAX_IMM16(Immediate16 i) { OP(0x0D); Emit16(i); }
    // void OR_RAX_IMM32(Immediate32 i) { REXW(); OP(0x0D); Emit32(i); }

    // // Opcode 0x83: OR r/m16/32/64, imm8
    // void OR_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); REX(m); OP(0x83); ModRM(0x01, m); Emit8(i); }
    // void OR_M32_IMM8(IsMemory32 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x01, m); Emit8(i); }
    // void OR_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x01, m); Emit8(i); }
    // void OR_M16_IMM8_LOCK(IsMemory16 auto m, Immediate8 i) { Lock(); OpSize(); REX(m); OP(0x83); ModRM(0x01, m); Emit8(i); }
    // void OR_M32_IMM8_LOCK(IsMemory32 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x01, m); Emit8(i); }
    // void OR_M64_IMM8_LOCK(IsMemory64 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x01, m); Emit8(i); }

    // void OR_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x01, r); Emit8(i); }
    // void OR_R32_IMM8(IsGPR32 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x01, r); Emit8(i); }
    // void OR_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x01, r); Emit8(i); }
    // void OR_R16_IMM8_LOCK(IsGPR16 auto r, Immediate8 i) { Lock(); OpSize(); REX(r); OP(0x83); ModRM(0x01, r); Emit8(i); }
    // void OR_R32_IMM8_LOCK(IsGPR32 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x01, r); Emit8(i); }
    // void OR_R64_IMM8_LOCK(IsGPR64 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x01, r); Emit8(i); }

    // // Opcode 0x81: OR r/m16/32/64, imm16/32
    // void OR_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); REX(m); OP(0x81); ModRM(0x01, m); Emit16(i); }
    // void OR_M32_IMM32(IsMemory32 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x01, m); Emit32(i); }
    // void OR_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x01, m); Emit32(i); }
    // void OR_M16_IMM16_LOCK(IsMemory16 auto m, Immediate16 i) { Lock(); OpSize(); REX(m); OP(0x81); ModRM(0x01, m); Emit16(i); }
    // void OR_M32_IMM32_LOCK(IsMemory32 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x01, m); Emit32(i); }
    // void OR_M64_IMM32_LOCK(IsMemory64 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x01, m); Emit32(i); }

    // void OR_R16_IMM16(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x01, r); Emit16(i); }
    // void OR_R32_IMM32(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x01, r); Emit32(i); }
    // void OR_R64_IMM32(IsGPR64 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x01, r); Emit32(i); }
    // void OR_R16_IMM16_LOCK(IsGPR16 auto r, Immediate16 i) { Lock(); OpSize(); REX(r); OP(0x81); ModRM(0x01, r); Emit16(i); }
    // void OR_R32_IMM32_LOCK(IsGPR32 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x01, r); Emit32(i); }
    // void OR_R64_IMM32_LOCK(IsGPR64 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x01, r); Emit32(i); }

    // void ADC(code::Register& d, code::Register& s) { Encode(d, s, Op{0x11}, Op8{0x10}); }
    // void ADC(code::Address& d, code::Register& s) { Encode(d, s, Op{0x11}, Op8{0x10}); }
    // void ADC(code::Register& d, code::Address& s) { Encode(d, s, Op{0x13}, Op8{0x12}); }
    // // void ADC(code::Register& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x02}); }
    // // void ADC(code::Register& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x02}); }
    // // void ADC(code::Register& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x02}); }
    // // void ADC(code::Address& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x02}); }
    // // void ADC(code::Address& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x02}); }
    // // void ADC(code::Address& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x02}); }

    // // Opcode 0x10: ADC r/m8, r8
    // void ADC_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x10); ModRM(r1, r2); }
    // void ADC_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x10); ModRM(m, r); }
    // void ADC_R8_R8_LOCK(IsGPR8 auto r1, IsGPR8 auto r2) { Lock(); REX(r1, r2); OP(0x10); ModRM(r1, r2); }
    // void ADC_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x10); ModRM(m, r); }

    // // Opcode 0x11: ADC r/m16/32/64, r16/32/64
    // void ADC_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x11); ModRM(r1, r2); }
    // void ADC_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x11); ModRM(m, r); }
    // void ADC_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x11); ModRM(r1, r2); }
    // void ADC_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x11); ModRM(m, r); }
    // void ADC_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x11); ModRM(r1, r2); }
    // void ADC_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x11); ModRM(m, r); }

    // void ADC_R16_R16_LOCK(IsGPR16 auto r1, IsGPR16 auto r2) { Lock(); OpSize(); REX(r1, r2); OP(0x11); ModRM(r1, r2); }
    // void ADC_M16_R16_LOCK(IsMemory16 auto m, IsGPR16 auto r) { Lock(); OpSize(); REX(m, r); OP(0x11); ModRM(m, r); }
    // void ADC_R32_R32_LOCK(IsGPR32 auto r1, IsGPR32 auto r2) { Lock(); REX(r1, r2); OP(0x11); ModRM(r1, r2); }
    // void ADC_M32_R32_LOCK(IsMemory32 auto m, IsGPR32 auto r) { Lock(); REX(m, r); OP(0x11); ModRM(m, r); }
    // void ADC_R64_R64_LOCK(IsGPR64 auto r1, IsGPR64 auto r2) { Lock(); REX(r1, r2); OP(0x11); ModRM(r1, r2); }
    // void ADC_M64_R64_LOCK(IsMemory64 auto m, IsGPR64 auto r) { Lock(); REX(m, r); OP(0x11); ModRM(m, r); }

    // // Opcode 0x12: ADC r8, r/m8
    // void ADC_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { REX(r, m); OP(0x12); ModRM(r, m); }
    // void ADC_R8_R8_ALT(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x12); ModRM(r1, r2); }

    // // Opcode 0x13: ADC r16/32/64, r/m16/32/64
    // void ADC_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x13); ModRM(r, m); }
    // void ADC_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(r, m); OP(0x13); ModRM(r, m); }
    // void ADC_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x13); ModRM(r, m); }
    // void ADC_R16_R16_ALT(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x13); ModRM(r1, r2); }
    // void ADC_R32_R32_ALT(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x13); ModRM(r1, r2); }
    // void ADC_R64_R64_ALT(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x13); ModRM(r1, r2); }

    // // Opcode 0x14: ADC AL, imm8
    // void ADC_AL_IMM8(Immediate8 i) { OP(0x14); Emit(i); }

    // // Opcode 0x15: ADC rAX, imm16/32
    // void ADC_AX_IMM16(Immediate16 i) { OpSize(); OP(0x15); Emit(i); }
    // void ADC_EAX_IMM32(Immediate32 i) { OP(0x15); Emit(i); }
    // void ADC_RAX_IMM32(Immediate32 i) { REXW(); OP(0x15); Emit(i); }

    // // Opcode 0x83: ADD r/m16/32/64, imm8
    // void ADC_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); REX(m); OP(0x83); ModRM(0x02, m); Emit8(i); }
    // void ADC_M32_IMM8(IsMemory32 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x02, m); Emit8(i); }
    // void ADC_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x02, m); Emit8(i); }
    // void ADC_M16_IMM8_LOCK(IsMemory16 auto m, Immediate8 i) { Lock(); OpSize(); REX(m); OP(0x83); ModRM(0x02, m); Emit8(i); }
    // void ADC_M32_IMM8_LOCK(IsMemory32 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x02, m); Emit8(i); }
    // void ADC_M64_IMM8_LOCK(IsMemory64 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x02, m); Emit8(i); }

    // void ADC_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x02, r); Emit8(i); }
    // void ADC_R32_IMM8(IsGPR32 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x02, r); Emit8(i); }
    // void ADC_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x02, r); Emit8(i); }
    // void ADC_R16_IMM8_LOCK(IsGPR16 auto r, Immediate8 i) { Lock(); OpSize(); REX(r); OP(0x83); ModRM(0x02, r); Emit8(i); }
    // void ADC_R32_IMM8_LOCK(IsGPR32 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x02, r); Emit8(i); }
    // void ADC_R64_IMM8_LOCK(IsGPR64 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x02, r); Emit8(i); }

    // // Opcode 0x81: ADD r/m16/32/64, imm16/32
    // void ADC_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); REX(m); OP(0x81); ModRM(0x02, m); Emit16(i); }
    // void ADC_M32_IMM32(IsMemory32 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x02, m); Emit32(i); }
    // void ADC_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x02, m); Emit32(i); }
    // void ADC_M16_IMM16_LOCK(IsMemory16 auto m, Immediate16 i) { Lock(); OpSize(); REX(m); OP(0x81); ModRM(0x02, m); Emit16(i); }
    // void ADC_M32_IMM32_LOCK(IsMemory32 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x02, m); Emit32(i); }
    // void ADC_M64_IMM32_LOCK(IsMemory64 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x02, m); Emit32(i); }

    // void ADC_R16_IMM16(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x02, r); Emit16(i); }
    // void ADC_R32_IMM32(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x02, r); Emit32(i); }
    // void ADC_R64_IMM32(IsGPR64 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x02, r); Emit32(i); }
    // void ADC_R16_IMM16_LOCK(IsGPR16 auto r, Immediate16 i) { Lock(); OpSize(); REX(r); OP(0x81); ModRM(0x02, r); Emit16(i); }
    // void ADC_R32_IMM32_LOCK(IsGPR32 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x02, r); Emit32(i); }
    // void ADC_R64_IMM32_LOCK(IsGPR64 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x02, r); Emit32(i); }

    // void SBB(const code::Register& d, const code::Register& s) { Encode(d, s, Op{0x19}, Op8{0x18}); }
    // void SBB(const code::Address& d, const code::Register& s) { Encode(d, s, Op{0x19}, Op8{0x18}); }
    // void SBB(const code::Register& d, const code::Address& s) { Encode(d, s, Op{0x1B}, Op8{0x1A}); }
    // // void SBB(const code::Register& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x03}); }
    // // void SBB(const code::Register& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x03}); }
    // // void SBB(const code::Register& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x03}); }
    // // void SBB(const code::Address& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x03}); }
    // // void SBB(const code::Address& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x03}); }
    // // void SBB(const code::Address& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x03}); }

    // // Opcode 0x18: SBB r/m8, r8
    // void SBB_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x18); ModRM(r1, r2); }
    // void SBB_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x18); ModRM(m, r); }
    // void SBB_R8_R8_LOCK(IsGPR8 auto r1, IsGPR8 auto r2) { Lock(); REX(r1, r2); OP(0x18); ModRM(r1, r2); }
    // void SBB_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x18); ModRM(m, r); }

    // // Opcode 0x19: SBB r/m16/32/64, r16/32/64
    // void SBB_R16_R16(SGPR16 r1, SGPR16 r2);
    // void SBB_R32_R32(SGPR32 r1, SGPR32 r2);
    // void SBB_R64_R64(SGPR64 r1, SGPR64 r2);
    // void SBB_R16_R16(SGPR16 r1, EGPR16 r2);
    // void SBB_R32_R32(SGPR32 r1, EGPR32 r2);
    // void SBB_R64_R64(SGPR64 r1, EGPR64 r2);
    // void SBB_R16_R16(EGPR16 r1, SGPR16 r2);
    // void SBB_R32_R32(EGPR32 r1, SGPR32 r2);
    // void SBB_R64_R64(EGPR64 r1, SGPR64 r2);
    // void SBB_R16_R16(EGPR16 r1, EGPR16 r2);
    // void SBB_R32_R32(EGPR32 r1, EGPR32 r2);
    // void SBB_R64_R64(EGPR64 r1, EGPR64 r2);
    // void SBB_M16_R16(IsMemory16 auto m, SGPR16 r) { OpSize(); REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M32_R32(IsMemory32 auto m, SGPR32 r) { REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M64_R64(IsMemory64 auto m, SGPR64 r) { REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M16_R16(IsMemory16 auto m, EGPR16 r) { OpSize(); REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M32_R32(IsMemory32 auto m, EGPR32 r) { REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M64_R64(IsMemory64 auto m, EGPR64 r) { REX(m, r); OP(0x19); ModRM(m, r); }

    // void SBB_R16_R16_LOCK(SGPR16 r1, SGPR16 r2);
    // void SBB_R32_R32_LOCK(SGPR32 r1, SGPR32 r2);
    // void SBB_R64_R64_LOCK(SGPR64 r1, SGPR64 r2);
    // void SBB_R16_R16_LOCK(SGPR16 r1, EGPR16 r2);
    // void SBB_R32_R32_LOCK(SGPR32 r1, EGPR32 r2);
    // void SBB_R64_R64_LOCK(SGPR64 r1, EGPR64 r2);
    // void SBB_R16_R16_LOCK(EGPR16 r1, SGPR16 r2);
    // void SBB_R32_R32_LOCK(EGPR32 r1, SGPR32 r2);
    // void SBB_R64_R64_LOCK(EGPR64 r1, SGPR64 r2);
    // void SBB_R16_R16_LOCK(EGPR16 r1, EGPR16 r2);
    // void SBB_R32_R32_LOCK(EGPR32 r1, EGPR32 r2);
    // void SBB_R64_R64_LOCK(EGPR64 r1, EGPR64 r2);
    // void SBB_M16_R16_LOCK(IsMemory16 auto m, SGPR16 r) { Lock(); OpSize(); REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M32_R32_LOCK(IsMemory32 auto m, SGPR32 r) { Lock(); REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M64_R64_LOCK(IsMemory64 auto m, SGPR64 r) { Lock(); REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M16_R16_LOCK(IsMemory16 auto m, EGPR16 r) { Lock(); OpSize(); REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M32_R32_LOCK(IsMemory32 auto m, EGPR32 r) { Lock(); REX(m, r); OP(0x19); ModRM(m, r); }
    // void SBB_M64_R64_LOCK(IsMemory64 auto m, EGPR64 r) { Lock(); REX(m, r); OP(0x19); ModRM(m, r); }

    // // Opcode 0x1A: SBB r8, r/m8
    // void SBB_R8_M8(SGPR8 r, IsMemory8 auto m) { REX(r, m); OP(0x1A); ModRM(r, m); }
    // void SBB_R8_R8_ALT(SGPR8 r1, SGPR8 r2) { REX(r1, r2); OP(0x1A); ModRM(r1, r2); }
    // void SBB_R8_R8_ALT(SGPR8 r1, EGPR8 r2) { REX(r1, r2); OP(0x1A); ModRM(r1, r2); }
    // void SBB_R8_R8_ALT(EGPR8 r1, SGPR8 r2) { REX(r1, r2); OP(0x1A); ModRM(r1, r2); }
    // void SBB_R8_M8(EGPR8 r, IsMemory8 auto m) { REX(r, m); OP(0x1A); ModRM(r, m); }
    // void SBB_R8_R8_ALT(EGPR8 r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x1A); ModRM(r1, r2); }

    // // Opcode 0x1B: SBB r16/32/64, r/m16/32/64
    // void SBB_R16_M16(SGPR16 r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x1B); ModRM(r, m); }
    // void SBB_R32_M32(SGPR32 r, IsMemory32 auto m) { REX(r, m); OP(0x1B); ModRM(r, m); }
    // void SBB_R64_M64(SGPR64 r, IsMemory64 auto m) { REX(r, m); OP(0x1B); ModRM(r, m); }
    // void SBB_R16_M16(EGPR16 r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x1B); ModRM(r, m); }
    // void SBB_R32_M32(EGPR32 r, IsMemory32 auto m) { REX(r, m); OP(0x1B); ModRM(r, m); }
    // void SBB_R64_M64(EGPR64 r, IsMemory64 auto m) { REX(r, m); OP(0x1B); ModRM(r, m); }
    // void SBB_R16_R16_ALT(SGPR16 r1, SGPR16 r2) { OpSize(); REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R32_R32_ALT(SGPR32 r1, SGPR32 r2) { REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R64_R64_ALT(SGPR64 r1, SGPR64 r2) { REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R16_R16_ALT(EGPR16 r1, SGPR16 r2) { OpSize(); REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R32_R32_ALT(EGPR32 r1, SGPR32 r2) { REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R64_R64_ALT(EGPR64 r1, SGPR64 r2) { REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R16_R16_ALT(SGPR16 r1, EGPR16 r2) { OpSize(); REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R32_R32_ALT(SGPR32 r1, EGPR32 r2) { REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R64_R64_ALT(SGPR64 r1, EGPR64 r2) { REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R16_R16_ALT(EGPR16 r1, EGPR16 r2) { OpSize(); REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R32_R32_ALT(EGPR32 r1, EGPR32 r2) { REX(r1, r2); OP(0x1B); ModRM(r1, r2); }
    // void SBB_R64_R64_ALT(EGPR64 r1, EGPR64 r2) { REX(r1, r2); OP(0x1B); ModRM(r1, r2); }

    // // Opcode 0x1C: SBB AL, imm8
    // void SBB_AL_IMM8(Immediate8 i);

    // // Opcode 0x1D: SBB rAX, imm16/32
    // void SBB_AX_IMM16(Immediate16 i);
    // void SBB_EAX_IMM32(Immediate32 i);
    // void SBB_RAX_IMM32(Immediate32 i);

    // // Opcode 0x83: ADD r/m16/32/64, imm8
    // void SBB_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); REX(m); OP(0x83); ModRM(0x03, m); Emit8(i); }
    // void SBB_M32_IMM8(IsMemory32 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x03, m); Emit8(i); }
    // void SBB_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x03, m); Emit8(i); }
    // void SBB_M16_IMM8_LOCK(IsMemory16 auto m, Immediate8 i) { Lock(); OpSize(); REX(m); OP(0x83); ModRM(0x03, m); Emit8(i); }
    // void SBB_M32_IMM8_LOCK(IsMemory32 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x03, m); Emit8(i); }
    // void SBB_M64_IMM8_LOCK(IsMemory64 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x03, m); Emit8(i); }

    // void SBB_R16_IMM8(SGPR16 r, Immediate8 i);
    // void SBB_R32_IMM8(SGPR32 r, Immediate8 i);
    // void SBB_R64_IMM8(SGPR64 r, Immediate8 i);
    // void SBB_R16_IMM8(EGPR16 r, Immediate8 i);
    // void SBB_R32_IMM8(EGPR32 r, Immediate8 i);
    // void SBB_R64_IMM8(EGPR64 r, Immediate8 i);
    // void SBB_R16_IMM8_LOCK(EGPR16 r, Immediate8 i);
    // void SBB_R32_IMM8_LOCK(EGPR32 r, Immediate8 i);
    // void SBB_R64_IMM8_LOCK(EGPR64 r, Immediate8 i);

    // // Opcode 0x81: ADD r/m16/32/64, imm16/32
    // void SBB_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); REX(m); OP(0x81); ModRM(0x03, m); Emit16(i); }
    // void SBB_M32_IMM32(IsMemory32 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x03, m); Emit32(i); }
    // void SBB_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x03, m); Emit32(i); }
    // void SBB_M16_IMM16_LOCK(IsMemory16 auto m, Immediate16 i) { Lock(); OpSize(); REX(m); OP(0x81); ModRM(0x03, m); Emit16(i); }
    // void SBB_M32_IMM32_LOCK(IsMemory32 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x03, m); Emit32(i); }
    // void SBB_M64_IMM32_LOCK(IsMemory64 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x03, m); Emit32(i); }

    // void SBB_R16_IMM16(SGPR16 r, Immediate16 i);
    // void SBB_R32_IMM32(SGPR32 r, Immediate32 i);
    // void SBB_R64_IMM32(SGPR64 r, Immediate32 i);
    // void SBB_R16_IMM16(EGPR16 r, Immediate16 i);
    // void SBB_R32_IMM32(EGPR32 r, Immediate32 i);
    // void SBB_R64_IMM32(EGPR64 r, Immediate32 i);
    // void SBB_R16_IMM16_LOCK(EGPR16 r, Immediate16 i);
    // void SBB_R32_IMM32_LOCK(EGPR32 r, Immediate32 i);
    // void SBB_R64_IMM32_LOCK(EGPR64 r, Immediate32 i);

    // void AND(const code::Register& d, const code::Register& s) { Encode(d, s, Op{0x21}, Op8{0x20}); }
    // void AND(const code::Address& d, const code::Register& s) { Encode(d, s, Op{0x21}, Op8{0x20}); }
    // void AND(const code::Register& d, const code::Address& s) { Encode(d, s, Op{0x23}, Op8{0x22}); }
    // // void AND(const code::Register& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x04}); }
    // // void AND(const code::Register& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x04}); }
    // // void AND(const code::Register& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x04}); }
    // // void AND(const code::Address& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x04}); }
    // // void AND(const code::Address& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x04}); }
    // // void AND(const code::Address& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x04}); }

    // // Opcode 0x20: AND r/m8, r8
    // void AND_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x20); ModRM(r1, r2); }
    // void AND_R8_R8_LOCK(IsGPR8 auto r1, IsGPR8 auto r2) { Lock(); REX(r1, r2); OP(0x20); ModRM(r1, r2); }

    // void AND_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x20); ModRM(m, r); }
    // void AND_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x20); ModRM(m, r); }

    // // Opcode 0x21: AND r/m16/32/64, r16/32/64
    // void AND_R16_R16(SGPR16 r1, SGPR16 r2);
    // void AND_R32_R32(SGPR32 r1, SGPR32 r2);
    // void AND_R64_R64(SGPR64 r1, SGPR64 r2);
    // void AND_R16_R16(SGPR16 r1, EGPR16 r2);
    // void AND_R32_R32(SGPR32 r1, EGPR32 r2);
    // void AND_R64_R64(SGPR64 r1, EGPR64 r2);
    // void AND_R16_R16(EGPR16 r1, SGPR16 r2);
    // void AND_R32_R32(EGPR32 r1, SGPR32 r2);
    // void AND_R64_R64(EGPR64 r1, SGPR64 r2);
    // void AND_R16_R16(EGPR16 r1, EGPR16 r2);
    // void AND_R32_R32(EGPR32 r1, EGPR32 r2);
    // void AND_R64_R64(EGPR64 r1, EGPR64 r2);
    // void AND_R16_R16_LOCK(IsGPR16 auto r1, IsGPR16 auto r2) { Lock(); OpSize(); REX(r1, r2); OP(0x21); ModRM(r1, r2); }
    // void AND_R32_R32_LOCK(IsGPR32 auto r1, IsGPR32 auto r2) { Lock(); REX(r1, r2); OP(0x21); ModRM(r1, r2); }
    // void AND_R64_R64_LOCK(IsGPR64 auto r1, IsGPR64 auto r2) { Lock(); REX(r1, r2); OP(0x21); ModRM(r1, r2); }

    // void AND_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x21); ModRM(m, r); }
    // void AND_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x21); ModRM(m, r); }
    // void AND_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x21); ModRM(m, r); }
    // void AND_M16_R16_LOCK(IsMemory16 auto m, IsGPR16 auto r) { Lock(); OpSize(); REX(m, r); OP(0x21); ModRM(m, r); }
    // void AND_M32_R32_LOCK(IsMemory32 auto m, IsGPR32 auto r) { Lock(); REX(m, r); OP(0x21); ModRM(m, r); }
    // void AND_M64_R64_LOCK(IsMemory64 auto m, IsGPR64 auto r) { Lock(); REX(m, r); OP(0x21); ModRM(m, r); }

    // // Opcode 0x22: AND r8, r/m8
    // void AND_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { REX(r, m); OP(0x22); ModRM(r, m); }
    // void AND_R8_R8_ALT(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x22); ModRM(r1, r2); }

    // // Opcode 0x23: AND r16/32/64, r/m16/32/64
    // void AND_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x23); ModRM(r, m); }
    // void AND_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(r, m); OP(0x23); ModRM(r, m); }
    // void AND_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x23); ModRM(r, m); }
    // void AND_R16_R16_ALT(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x23); ModRM(r1, r2); }
    // void AND_R32_R32_ALT(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x23); ModRM(r1, r2); }
    // void AND_R64_R64_ALT(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x23); ModRM(r1, r2); }

    // // Opcode 0x24: AND AL, imm8
    // void AND_AL_IMM8(Immediate8 i) { OP(0x24); Emit8(i); }

    // // Opcode 0x25: AND rAX, imm16/32
    // void AND_AX_IMM16(Immediate16 i) { OpSize(); OP(0x25); Emit16(i); }
    // void AND_EAX_IMM32(Immediate32 i) { OP(0x25); Emit32(i); }
    // void AND_RAX_IMM32(Immediate32 i) { REXW(); OP(0x25); Emit32(i); }

    // // Opcode 0x83: ADD r/m16/32/64, imm8
    // void AND_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); REX(m); OP(0x83); ModRM(0x04, m); Emit8(i); }
    // void AND_M32_IMM8(IsMemory32 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x04, m); Emit8(i); }
    // void AND_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x04, m); Emit8(i); }
    // void AND_M16_IMM8_LOCK(IsMemory16 auto m, Immediate8 i) { Lock(); OpSize(); REX(m); OP(0x83); ModRM(0x04, m); Emit8(i); }
    // void AND_M32_IMM8_LOCK(IsMemory32 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x04, m); Emit8(i); }
    // void AND_M64_IMM8_LOCK(IsMemory64 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x04, m); Emit8(i); }

    // void AND_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x04, r); Emit8(i); }
    // void AND_R32_IMM8(IsGPR32 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x04, r); Emit8(i); }
    // void AND_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x04, r); Emit8(i); }
    // void AND_R16_IMM8_LOCK(IsGPR16 auto r, Immediate8 i) { Lock(); OpSize(); REX(r); OP(0x83); ModRM(0x04, r); Emit8(i); }
    // void AND_R32_IMM8_LOCK(IsGPR32 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x04, r); Emit8(i); }
    // void AND_R64_IMM8_LOCK(IsGPR64 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x04, r); Emit8(i); }

    // // Opcode 0x81: ADD r/m16/32/64, imm16/32
    // void AND_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); REX(m); OP(0x81); ModRM(0x04, m); Emit16(i); }
    // void AND_M32_IMM32(IsMemory32 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x04, m); Emit32(i); }
    // void AND_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x04, m); Emit32(i); }
    // void AND_M16_IMM16_LOCK(IsMemory16 auto m, Immediate16 i) { Lock(); OpSize(); REX(m); OP(0x81); ModRM(0x04, m); Emit16(i); }
    // void AND_M32_IMM32_LOCK(IsMemory32 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x04, m); Emit32(i); }
    // void AND_M64_IMM32_LOCK(IsMemory64 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x04, m); Emit32(i); }

    // void AND_R16_IMM16(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x04, r); Emit16(i); }
    // void AND_R32_IMM32(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x04, r); Emit32(i); }
    // void AND_R64_IMM32(IsGPR64 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x04, r); Emit32(i); }
    // void AND_R16_IMM16_LOCK(IsGPR16 auto r, Immediate16 i) { Lock(); OpSize(); REX(r); OP(0x81); ModRM(0x04, r); Emit16(i); }
    // void AND_R32_IMM32_LOCK(IsGPR32 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x04, r); Emit32(i); }
    // void AND_R64_IMM32_LOCK(IsGPR64 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x04, r); Emit32(i); }

    // void SUB(const code::Register& d, const code::Register& s) { Encode(d, s, Op{0x29}, Op8{0x28}); }
    // void SUB(const code::Address& d, const code::Register& s) { Encode(d, s, Op{0x29}, Op8{0x28}); }
    // void SUB(const code::Register& d, const code::Address& s) { Encode(d, s, Op{0x2B}, Op8{0x2A}); }
    // // void SUB(const code::Register& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x05}); }
    // // void SUB(const code::Register& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x05}); }
    // // void SUB(const code::Register& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x05}); }
    // // void SUB(const code::Address& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x05}); }
    // // void SUB(const code::Address& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x05}); }
    // // void SUB(const code::Address& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x05}); }

    // // Opcode 0x28: SUB r/m8, r8
    // void SUB_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x28); ModRM(r1, r2); }
    // void SUB_R8_R8_LOCK(IsGPR8 auto r1, IsGPR8 auto r2) { Lock(); REX(r1, r2); OP(0x28); ModRM(r1, r2); }
    
    // void SUB_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x28); ModRM(m, r); }
    // void SUB_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x28); ModRM(m, r); }

    // // Opcode 0x29: SUB r/m16/32/64, r16/32/64
    // void SUB_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x29); ModRM(r1, r2); }
    // void SUB_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x29); ModRM(r1, r2); }
    // void SUB_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x29); ModRM(r1, r2); }
    // void SUB_R16_R16_LOCK(IsGPR16 auto r1, IsGPR16 auto r2) { Lock(); OpSize(); REX(r1, r2); OP(0x29); ModRM(r1, r2); }
    // void SUB_R32_R32_LOCK(IsGPR32 auto r1, IsGPR32 auto r2) { Lock(); REX(r1, r2); OP(0x29); ModRM(r1, r2); }
    // void SUB_R64_R64_LOCK(IsGPR64 auto r1, IsGPR64 auto r2) { Lock(); REX(r1, r2); OP(0x29); ModRM(r1, r2); }

    // void SUB_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x29); ModRM(m, r); }
    // void SUB_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x29); ModRM(m, r); }
    // void SUB_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x29); ModRM(m, r); }
    // void SUB_M16_R16_LOCK(IsMemory16 auto m, IsGPR16 auto r) { Lock(); OpSize(); REX(m, r); OP(0x29); ModRM(m, r); }
    // void SUB_M32_R32_LOCK(IsMemory32 auto m, IsGPR32 auto r) { Lock(); REX(m, r); OP(0x29); ModRM(m, r); }
    // void SUB_M64_R64_LOCK(IsMemory64 auto m, IsGPR64 auto r) { Lock(); REX(m, r); OP(0x29); ModRM(m, r); }

    // // Opcode 0x2A: SUB r8, r/m8
    // void SUB_R8_R8_ALT(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x2A); ModRM(r1, r2); }
    // void SUB_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { REX(r, m); OP(0x2A); ModRM(r, m); }

    // // Opcode 0x2B: SUB r16/32/64, r/m16/32/64
    // void SUB_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x2B); ModRM(r, m); }
    // void SUB_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(r, m); OP(0x2B); ModRM(r, m); }
    // void SUB_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x2B); ModRM(r, m); }
    // void SUB_R16_R16_ALT(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x2B); ModRM(r1, r2); }
    // void SUB_R32_R32_ALT(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x2B); ModRM(r1, r2); }
    // void SUB_R64_R64_ALT(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x2B); ModRM(r1, r2); }

    // // Opcode 0x2C: SUB AL, imm8
    // void SUB_AL_IMM8(Immediate8 i) { OP(0x2C); Emit(i); }

    // // Opcode 0x2D: SUB rAX, imm16/32
    // void SUB_AX_IMM16(Immediate16 i) { OpSize(); OP(0x2D); Emit(i); }
    // void SUB_EAX_IMM32(Immediate32 i) { OP(0x2D); Emit(i); }
    // void SUB_RAX_IMM32(Immediate32 i) { REXW(); OP(0x2D); Emit(i); }

    // // Opcode 0x83: ADD r/m16/32/64, imm8
    // void SUB_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); REX(m); OP(0x83); ModRM(0x05, m); Emit8(i); }
    // void SUB_M32_IMM8(IsMemory32 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x05, m); Emit8(i); }
    // void SUB_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x05, m); Emit8(i); }
    // // Placeholder8 SUB_M16_IMM8(IsMemory16 auto m) { SUB_M16_IMM8(m, 0); return Placeholder8{code}; }
    // // Placeholder8 SUB_M32_IMM8(IsMemory32 auto m) { SUB_M32_IMM8(m, 0); return Placeholder8{code}; }
    // // Placeholder8 SUB_M64_IMM8(IsMemory64 auto m) { SUB_M64_IMM8(m, 0); return Placeholder8{code}; }
    // void SUB_M16_IMM8_LOCK(IsMemory16 auto m, Immediate8 i) { Lock(); OpSize(); REX(m); OP(0x83); ModRM(0x05, m); Emit8(i); }
    // void SUB_M32_IMM8_LOCK(IsMemory32 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x05, m); Emit8(i); }
    // void SUB_M64_IMM8_LOCK(IsMemory64 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x05, m); Emit8(i); }

    // void SUB_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x05, r); Emit8(i); }
    // void SUB_R32_IMM8(IsGPR32 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x05, r); Emit8(i); }
    // void SUB_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x05, r); Emit8(i); }
    // // Placeholder8 SUB_R16_IMM8(IsGPR16 auto r) { SUB_R16_IMM8(r, 0); return Placeholder8{code}; }
    // // Placeholder8 SUB_R32_IMM8(IsGPR32 auto r) { SUB_R32_IMM8(r, 0); return Placeholder8{code}; }
    // // Placeholder8 SUB_R64_IMM8(IsGPR64 auto r) { SUB_R64_IMM8(r, 0); return Placeholder8{code}; }
    // void SUB_R16_IMM8_LOCK(IsGPR16 auto r, Immediate8 i) { Lock(); OpSize(); REX(r); OP(0x83); ModRM(0x05, r); Emit8(i); }
    // void SUB_R32_IMM8_LOCK(IsGPR32 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x05, r); Emit8(i); }
    // void SUB_R64_IMM8_LOCK(IsGPR64 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x05, r); Emit8(i); }

    // // OpSUBe 0x81: ADD r/m16/32/64, imm16/32
    // void SUB_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); REX(m); OP(0x81); ModRM(0x05, m); Emit16(i); }
    // void SUB_M32_IMM32(IsMemory32 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x05, m); Emit32(i); }
    // void SUB_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x05, m); Emit32(i); }
    // // Placeholder16 SUB_M16_IMM16(IsMemory16 auto m) { SUB_M16_IMM16(m, 0); return Placeholder16{code}; }
    // // Placeholder32 SUB_M32_IMM32(IsMemory32 auto m) { SUB_M32_IMM32(m, 0); return Placeholder32{code}; }
    // // Placeholder32 SUB_M64_IMM32(IsMemory64 auto m) { SUB_M64_IMM32(m, 0); return Placeholder32{code}; }
    // void SUB_M16_IMM16_LOCK(IsMemory16 auto m, Immediate16 i) { Lock(); OpSize(); REX(m); OP(0x81); ModRM(0x05, m); Emit16(i); }
    // void SUB_M32_IMM32_LOCK(IsMemory32 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x05, m); Emit32(i); }
    // void SUB_M64_IMM32_LOCK(IsMemory64 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x05, m); Emit32(i); }

    // void SUB_R16_IMM16(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x05, r); Emit16(i); }
    // void SUB_R32_IMM32(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x05, r); Emit32(i); }
    // void SUB_R64_IMM32(IsGPR64 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x05, r); Emit32(i); }
    // // Placeholder16 SUB_R16_IMM16(IsGPR16 auto r) { SUB_R16_IMM16(r, 0); return Placeholder16{code}; }
    // // Placeholder32 SUB_R32_IMM32(IsGPR32 auto r) { SUB_R32_IMM32(r, 0); return Placeholder32{code}; }
    // // Placeholder32 SUB_R64_IMM32(IsGPR64 auto r) { SUB_R64_IMM32(r, 0); return Placeholder32{code}; }
    // void SUB_R16_IMM16_LOCK(IsGPR16 auto r, Immediate16 i) { Lock(); OpSize(); REX(r); OP(0x81); ModRM(0x05, r); Emit16(i); }
    // void SUB_R32_IMM32_LOCK(IsGPR32 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x05, r); Emit32(i); }
    // void SUB_R64_IMM32_LOCK(IsGPR64 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x05, r); Emit32(i); }

    // void XOR(const code::Register& d, const code::Register& s) { Encode(d, s, Op{0x31}, Op8{0x30}); }
    // void XOR(const code::Address& d, const code::Register& s) { Encode(d, s, Op{0x31}, Op8{0x30}); }
    // void XOR(const code::Register& d, const code::Address& s) { Encode(d, s, Op{0x33}, Op8{0x32}); }
    // // void XOR(const code::Register& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x06}); }
    // // void XOR(const code::Register& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x06}); }
    // // void XOR(const code::Register& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x06}); }
    // // void XOR(const code::Address& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x06}); }
    // // void XOR(const code::Address& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x06}); }
    // // void XOR(const code::Address& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x06}); }

    // // Opcode 0x30: XOR r/m8, r8
    // void XOR_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x30); ModRM(r1, r2); }
    // void XOR_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x30); ModRM(m, r); }

    // void XOR_R8_R8_LOCK(IsGPR8 auto r1, IsGPR8 auto r2) { Lock(); REX(r1, r2); OP(0x30); ModRM(r1, r2); }
    // void XOR_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x30); ModRM(m, r); }

    // // Opcode 0x31: XOR r/m16/32/64, r16/32/64
    // void XOR_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x31); ModRM(r1, r2); }
    // void XOR_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x31); ModRM(m, r); }
    // void XOR_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x31); ModRM(r1, r2); }
    // void XOR_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x31); ModRM(m, r); }
    // void XOR_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x31); ModRM(r1, r2); }
    // void XOR_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x31); ModRM(m, r); }

    // void XOR_R16_R16_LOCK(IsGPR16 auto r1, IsGPR16 auto r2) { Lock(); OpSize(); REX(r1, r2); OP(0x31); ModRM(r1, r2); }
    // void XOR_M16_R16_LOCK(IsMemory16 auto m, IsGPR16 auto r) { Lock(); OpSize(); REX(m, r); OP(0x31); ModRM(m, r); }
    // void XOR_R32_R32_LOCK(IsGPR32 auto r1, IsGPR32 auto r2) { Lock(); REX(r1, r2); OP(0x31); ModRM(r1, r2); }
    // void XOR_M32_R32_LOCK(IsMemory32 auto m, IsGPR32 auto r) { Lock(); REX(m, r); OP(0x31); ModRM(m, r); }
    // void XOR_R64_R64_LOCK(IsGPR64 auto r1, IsGPR64 auto r2) { Lock(); REX(r1, r2); OP(0x31); ModRM(r1, r2); }
    // void XOR_M64_R64_LOCK(IsMemory64 auto m, IsGPR64 auto r) { Lock(); REX(m, r); OP(0x31); ModRM(m, r); }

    // // Opcode 0x32: XOR r8, r/m8
    // void XOR_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { REX(r, m); OP(0x32); ModRM(r, m); }
    // void XOR_R8_R8_ALT(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x32); ModRM(r1, r2); }

    // // Opcode 0x33: XOR r16/32/64, r/m16/32/64
    // void XOR_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x33); ModRM(r, m); }
    // void XOR_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(r, m); OP(0x33); ModRM(r, m); }
    // void XOR_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x33); ModRM(r, m); }
    // void XOR_R16_R16_ALT(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x33); ModRM(r1, r2); }
    // void XOR_R32_R32_ALT(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x33); ModRM(r1, r2); }
    // void XOR_R64_R64_ALT(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x33); ModRM(r1, r2); }

    // // Opcode 0x34: XOR AL, imm8
    // void XOR_AL_IMM8(Immediate8 i) { OP(0x34); Emit8(i); }

    // // Opcode 0x35: XOR rAX, imm16/32
    // void XOR_AX_IMM16(Immediate16 i) { OpSize(); OP(0x35); Emit16(i); }
    // void XOR_EAX_IMM32(Immediate32 i) { OP(0x35); Emit32(i); }
    // void XOR_RAX_IMM32(Immediate32 i) { REXW(); OP(0x35); Emit32(i); }

    // // Opcode 0x83: XOR r/m16/32/64, imm8
    // void XOR_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); REX(m); OP(0x83); ModRM(0x06, m); Emit8(i); }
    // void XOR_M32_IMM8(IsMemory32 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x06, m); Emit8(i); }
    // void XOR_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x06, m); Emit8(i); }
    // void XOR_M16_IMM8_LOCK(IsMemory16 auto m, Immediate8 i) { Lock(); OpSize(); REX(m); OP(0x83); ModRM(0x06, m); Emit8(i); }
    // void XOR_M32_IMM8_LOCK(IsMemory32 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x06, m); Emit8(i); }
    // void XOR_M64_IMM8_LOCK(IsMemory64 auto m, Immediate8 i) { Lock(); REX(m); OP(0x83); ModRM(0x06, m); Emit8(i); }

    // void XOR_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x06, r); Emit8(i); }
    // void XOR_R32_IMM8(IsGPR32 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x06, r); Emit8(i); }
    // void XOR_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x06, r); Emit8(i); }
    // void XOR_R16_IMM8_LOCK(IsGPR16 auto r, Immediate8 i) { Lock(); OpSize(); REX(r); OP(0x83); ModRM(0x06, r); Emit8(i); }
    // void XOR_R32_IMM8_LOCK(IsGPR32 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x06, r); Emit8(i); }
    // void XOR_R64_IMM8_LOCK(IsGPR64 auto r, Immediate8 i) { Lock(); REX(r); OP(0x83); ModRM(0x06, r); Emit8(i); }

    // // Opcode 0x81: XOR r/m16/32/64, imm16/32
    // void XOR_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); REX(m); OP(0x81); ModRM(0x06, m); Emit16(i); }
    // void XOR_M32_IMM32(IsMemory32 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x06, m); Emit32(i); }
    // void XOR_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x06, m); Emit32(i); }
    // void XOR_M16_IMM16_LOCK(IsMemory16 auto m, Immediate16 i) { Lock(); OpSize(); REX(m); OP(0x81); ModRM(0x06, m); Emit16(i); }
    // void XOR_M32_IMM32_LOCK(IsMemory32 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x06, m); Emit32(i); }
    // void XOR_M64_IMM32_LOCK(IsMemory64 auto m, Immediate32 i) { Lock(); REX(m); OP(0x81); ModRM(0x06, m); Emit32(i); }

    // void XOR_R16_IMM16(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x06, r); Emit16(i); }
    // void XOR_R32_IMM32(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x06, r); Emit32(i); }
    // void XOR_R64_IMM32(IsGPR64 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x06, r); Emit32(i); }
    // void XOR_R16_IMM16_LOCK(IsGPR16 auto r, Immediate16 i) { Lock(); OpSize(); REX(r); OP(0x81); ModRM(0x06, r); Emit16(i); }
    // void XOR_R32_IMM32_LOCK(IsGPR32 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x06, r); Emit32(i); }
    // void XOR_R64_IMM32_LOCK(IsGPR64 auto r, Immediate32 i) { Lock(); REX(r); OP(0x81); ModRM(0x06, r); Emit32(i); }

    // void CMP(const code::Register& d, const code::Register& s) { Encode(d, s, Op{0x39}, Op8{0x38}); }
    // void CMP(const code::Address& d, const code::Register& s) { Encode(d, s, Op{0x39}, Op8{0x38}); }
    // void CMP(const code::Register& d, const code::Address& s) { Encode(d, s, Op{0x3B}, Op8{0x3A}); }
    // // void CMP(const code::Register& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x07}); }
    // // void CMP(const code::Register& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x07}); }
    // // void CMP(const code::Register& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x07}); }
    // // void CMP(const code::Address& d, code::Immediate8& i) { Encode(d, i, Op{0x83}, Op8{0x80}, Mod{0x07}); }
    // // void CMP(const code::Address& d, code::Immediate16& i) { Encode(d, i, Op{0x81}, Mod{0x07}); }
    // // void CMP(const code::Address& d, code::Immediate32& i) { Encode(d, i, Op{0x81}, Mod{0x07}); }
    // // void CMP0(const code::Register& d) { CMP(d, code::Immediate8::ZERO); }
    // // void CMP0(const code::Address& d) { CMP(d, code::Immediate8::ZERO); }
    // // void CMP1(const code::Register& d) { CMP(d, code::Immediate8::ONE); }
    // // void CMP1(const code::Address& d) { CMP(d, code::Immediate8::ONE); }

    // // Opcode 0x38: CMP r/m8, r8
    // void CMP_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x38); ModRM(m, r); }
    // void CMP_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x38); ModRM(r1, r2); }

    // // Opcode 0x39: CMP r/m16/32/64, r16/32/64
    // void CMP_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x39); ModRM(m, r); }
    // void CMP_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x39); ModRM(r1, r2); }
    // void CMP_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x39); ModRM(m, r); }
    // void CMP_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x39); ModRM(r1, r2); }
    // void CMP_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x39); ModRM(m, r); }
    // void CMP_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x39); ModRM(r1, r2); }

    // // Opcode 0x3A: CMP r8, r/m8
    // void CMP_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { REX(r, m); OP(0x3A); ModRM(r, m); }
    // void CMP_R8_R8_ALT(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x3A); ModRM(r1, r2); }

    // // Opcode 0x3B: CMP r16/32/64, r/m16/32/64
    // void CMP_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x3B); ModRM(r, m); }
    // void CMP_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(r, m); OP(0x3B); ModRM(r, m); }
    // void CMP_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x3B); ModRM(r, m); }
    // void CMP_R16_R16_ALT(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x3B); ModRM(r1, r2); }
    // void CMP_R32_R32_ALT(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x3B); ModRM(r1, r2); }
    // void CMP_R64_R64_ALT(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x3B); ModRM(r1, r2); }

    // // Opcode 0x3C: CMP AL, imm8
    // void CMP_AL_IMM8(Immediate8 i) { OP(0x3C); Emit8(i); }

    // // Opcode 0x3D: CMP rAX, imm16/32
    // void CMP_AX_IMM16(Immediate16 i) { OpSize(); OP(0x3D); Emit16(i); }
    // void CMP_EAX_IMM16(Immediate16 i) { OP(0x3D); Emit16(i); }
    // void CMP_RAX_IMM32(Immediate32 i) { REXW(); OP(0x3D); Emit32(i); }

    // // Opcode 0x83: CMP r/m16/32/64, imm8
    // void CMP_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); REX(m); OP(0x83); ModRM(0x07, m); Emit8(i); }
    // void CMP_M32_IMM8(IsMemory32 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x07, m); Emit8(i); }
    // void CMP_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0x83); ModRM(0x07, m); Emit8(i); }

    // void CMP_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); REX(r); OP(0x83); ModRM(0x07, r); Emit8(i); }
    // void CMP_R32_IMM8(IsGPR32 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x07, r); Emit8(i); }
    // void CMP_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0x83); ModRM(0x07, r); Emit8(i); }

    // // Opcode 0x81: CMP r/m16/32/64, imm16/32
    // void CMP_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); REX(m); OP(0x81); ModRM(0x07, m); Emit16(i); }
    // void CMP_M32_IMM32(IsMemory32 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x07, m); Emit32(i); }
    // void CMP_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0x81); ModRM(0x07, m); Emit32(i); }

    // void CMP_R16_IMM16(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0x81); ModRM(0x07, r); Emit16(i); }
    // void CMP_R32_IMM32(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x07, r); Emit32(i); }
    // void CMP_R64_IMM32(IsGPR64 auto r, Immediate32 i) { REX(r); OP(0x81); ModRM(0x07, r); Emit32(i); }

    // void TEST(const code::Register& d, const code::Register& s) { OpSize(d, s); REX(d, s); OP(d.IsByte() ? 0x84 : 0x85); ModRM(d, s); }
    // void TEST(const code::Address& d, const code::Register& s) { OpSize(d, s); REX(d, s); OP(d.IsByte() ? 0x84 : 0x85); ModRM(d, s); }
    // void TEST(const code::Register& d, const code::Address& s) { TEST(s, d); }
    // // void TEST(const code::Register& d, code::Immediate8& i) { REX(d); OP(0xF6); ModRM(d, 0x00); Emit(i); }
    // // void TEST(const code::Register& d, code::Immediate16& i) { OpSize(); REX(d); OP(0xF7); ModRM(d, 0x00); Emit(i); }
    // // void TEST(const code::Register& d, code::Immediate32& i) { REX(d); OP(0xF7); ModRM(d, 0x00); Emit(i); }
    // // void TEST(const code::Address& d, code::Immediate8& i) { REX(d); OP(0xF6); ModRM(d, 0x00); Emit(i); }
    // // void TEST(const code::Address& d, code::Immediate16& i) { OpSize(); REX(d); OP(0xF7); ModRM(d, 0x00); Emit(i); }
    // // void TEST(const code::Address& d, code::Immediate32& i) { REX(d); OP(0xF7); ModRM(d, 0x00); Emit(i); }
    // // void TEST0(const code::Register& d) { OpSize(d); REX(d); OP(d.IsByte() ? 0x84 : 0x85); ModRM(d, d); }

    // // void TEST0(const code::Address& d) {
    // //   if (d.IsByte()) TEST(d, code::Immediate8::ZERO);
    // //   else if (d.Is16Bit()) TEST(d, code::Immediate16::ZERO);
    // //   else TEST(d, code::Immediate32::ZERO);
    // // }

    // // Opcode 0x84: TEST r/m8, r8
    // void TEST_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { OP(0x84); ModRM(m, r); }
    // void TEST_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { OP(0x84); ModRM(r1, r2); }

    // // Opcode 0x85: TEST r/m16/32/64, r16/32/64
    // void TEST_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x85); ModRM(m, r); }
    // void TEST_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x85); ModRM(r1, r2); }
    // void TEST_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x85); ModRM(m, r); }
    // void TEST_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x85); ModRM(r1, r2); }
    // void TEST_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x85); ModRM(m, r); }
    // void TEST_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x85); ModRM(r1, r2); }

    // // Opcode 0xA8: TEST AL, imm8
    // void TEST_AL_IMM8(Immediate8 i) { OP(0xA8); Emit8(i); }

    // // Opcode 0xA9: TEST rAX, imm16/32
    // void TEST_AX_IMM16(Immediate16 i) { OpSize(); OP(0xA9); Emit16(i); }
    // void TEST_EAX_IMM32(Immediate32 i) { OP(0xA9); Emit32(i); }
    // void TEST_RAX_IMM32(Immediate32 i) { REXW(); OP(0xA9); Emit32(i); }

    // // Opcode 0x8D: LEA r16/32/64, m16/32/64
    // void LEA_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(m, r); OP(0x8D); ModRM(r, m); }
    // void LEA_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(m, r); OP(0x8D); ModRM(r, m); }
    // void LEA_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x8D); ModRM(r, m); }

    // // void Prefix(code::Register& d, code::Register& s) {
    // //   if (d.Is16Bit()) OpSize();
    // // }

    // // void Locked(code::Register& d, code::Register& s) {
    // //   Lock();
    // //   if (d.Is16Bit()) OpSize();
    // // }

    // // void OP8(code::Register& d, code::Register& s, uint8_t code, uint8_t alt) {

    // // }

    // void XCHG(const code::Register& d, const code::Register& s) { Encode(d, s, Op{0x87}, Op8{0x86}); }
    // void XCHG(const code::Register& d, const code::Address& s) { Encode(d, s, Op{0x87}, Op8{0x86}); }
    // void XCHG(const code::Address& s, const code::Register& d) { Encode(d, s, Op{0x87}, Op8{0x86}); }

    // // Opcode 0x86: XCHG r/m8, r8
    // void XCHG_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x86); ModRM(r1, r2); }

    // void XCHG_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x86); ModRM(r, m); }
    // void XCHG_M8_R8_LOCK(IsMemory8 auto m, IsGPR8 auto r) { Lock(); REX(m, r); OP(0x86); ModRM(r, m); }

    // void XCHG_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { REX(r, m); OP(0x86); ModRM(r, m); }
    // void XCHG_R8_M8_LOCK(IsGPR8 auto r, IsMemory8 auto m) { Lock(); REX(r, m); OP(0x86); ModRM(r, m); }

    // // Opcode 0x87: XCHG r/m16/32/64, r16/32/64
    // void XCHG_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x87); ModRM(r1, r2); }
    // void XCHG_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x87); ModRM(r1, r2); }
    // void XCHG_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x87); ModRM(r1, r2); }

    // void XCHG_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x87); ModRM(m, r); }
    // void XCHG_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x87); ModRM(r, m); }
    // void XCHG_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x87); ModRM(r, m); }
    // void XCHG_M16_R16_LOCK(IsMemory16 auto m, IsGPR16 auto r) { Lock(); OpSize(); REX(r, m); OP(0x87); ModRM(r, m); }
    // void XCHG_M32_R32_LOCK(IsMemory32 auto m, IsGPR32 auto r) { Lock(); REX(r, m); OP(0x87); ModRM(r, m); }
    // void XCHG_M64_R64_LOCK(IsMemory64 auto m, IsGPR64 auto r) { Lock(); REX(r, m); OP(0x87); ModRM(r, m); }

    // void XCHG_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); REX(r, m); OP(0x87); ModRM(r, m); }
    // void XCHG_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { REX(r, m); OP(0x87); ModRM(r, m); }
    // void XCHG_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x87); ModRM(r, m); }
    // void XCHG_R16_M16_LOCK(IsGPR16 auto r, IsMemory16 auto m) { Lock(); OpSize(); REX(r, m); OP(0x87); ModRM(r, m); }
    // void XCHG_R32_M32_LOCK(IsGPR32 auto r, IsMemory32 auto m) { Lock(); REX(r, m); OP(0x87); ModRM(r, m); }
    // void XCHG_R64_M64_LOCK(IsGPR64 auto r, IsMemory64 auto m) { Lock(); REX(r, m); OP(0x87); ModRM(r, m); }

    // // 16-bit Versions
    // void XCHG_AX_AX();
    // void XCHG_CX_AX();
    // void XCHG_DX_AX();
    // void XCHG_BX_AX();
    // void XCHG_SP_AX();
    // void XCHG_BP_AX();
    // void XCHG_SI_AX();
    // void XCHG_DI_AX();
    // void XCHG_R8W_AX();
    // void XCHG_R9W_AX();
    // void XCHG_R10W_AX();
    // void XCHG_R11W_AX();
    // void XCHG_R12W_AX();
    // void XCHG_R13W_AX();
    // void XCHG_R14W_AX();
    // void XCHG_R15W_AX();

    // // 32-bit Versions
    // void XCHG_EAX_EAX();
    // void XCHG_ECX_EAX();
    // void XCHG_EDX_EAX();
    // void XCHG_EBX_EAX();
    // void XCHG_ESP_EAX();
    // void XCHG_EBP_EAX();
    // void XCHG_ESI_EAX();
    // void XCHG_EDI_EAX();
    // void XCHG_R8D_EAX();
    // void XCHG_R9D_EAX();
    // void XCHG_R10D_EAX();
    // void XCHG_R11D_EAX();
    // void XCHG_R12D_EAX();
    // void XCHG_R13D_EAX();
    // void XCHG_R14D_EAX();
    // void XCHG_R15D_EAX();

    // // 64-bit Versions
    // void XCHG_RAX_RAX();
    // void XCHG_RCX_RAX();
    // void XCHG_RDX_RAX();
    // void XCHG_RBX_RAX();
    // void XCHG_RSP_RAX();
    // void XCHG_RBP_RAX();
    // void XCHG_RSI_RAX();
    // void XCHG_RDI_RAX();
    // void XCHG_R8_RAX();
    // void XCHG_R9_RAX();
    // void XCHG_R10_RAX();
    // void XCHG_R11_RAX();
    // void XCHG_R12_RAX();
    // void XCHG_R13_RAX();
    // void XCHG_R14_RAX();
    // void XCHG_R15_RAX();

    // void PAUSE();

    // void CBW();
    // void CWDE();
    // void CDQE();

    // void CWD();
    // void CDQ();
    // void CQO();

    // // PUSH rAX, AX, rCX, CX, rDX, DX, rBX, BX, rSP, SP, rBP, BP, rSI, SI, rDI, DI
    // void PUSH_R16(SGPR16 r);
    // void PUSH_R16(EGPR16 r);
    // void PUSH_AX();
    // void PUSH_CX();
    // void PUSH_DX();
    // void PUSH_BX();
    // void PUSH_SP();
    // void PUSH_BP();
    // void PUSH_SI();
    // void PUSH_DI();

    // void PUSH_R8W();
    // void PUSH_R9W();
    // void PUSH_R10W();
    // void PUSH_R11W();
    // void PUSH_R12W();
    // void PUSH_R13W();
    // void PUSH_R14W();
    // void PUSH_R15W();

    // void PUSH_R64(SGPR64 r);
    // void PUSH_R64(EGPR64 r);
    // void PUSH_RAX();
    // void PUSH_RCX();
    // void PUSH_RDX();
    // void PUSH_RBX();
    // void PUSH_RSP();
    // void PUSH_RBP();
    // void PUSH_RSI();
    // void PUSH_RDI();

    // void PUSH_R8();
    // void PUSH_R9();
    // void PUSH_R10();
    // void PUSH_R11();
    // void PUSH_R12();
    // void PUSH_R13();
    // void PUSH_R14();
    // void PUSH_R15();

    // // // PUSH imm16, imm32, imm8
    // // void PUSH(code::Immediate16& i) { OpSize(); OP(0x68); Emit(i); }
    // // void PUSH(code::Immediate32& i) { OP(0x68); Emit(i); }
    // // void PUSH(code::Immediate8& i) { OP(0x6A); Emit(i); }

    // void PUSH_IMM16(Immediate16 i);
    // void PUSH_IMM32(Immediate32 i);
    // void PUSH_IMM8(Immediate8 i);

    // // POP rAX, AX, rCX, CX, rDX, DX, rBX, BX, rSP, SP, rBP, BP, rSI, SI, rDI, DI
    // void POP_AX();
    // void POP_CX();
    // void POP_DX();
    // void POP_BX();
    // void POP_SP();
    // void POP_BP();
    // void POP_SI();
    // void POP_DI();

    // void POP_R8W();
    // void POP_R9W();
    // void POP_R10W();
    // void POP_R11W();
    // void POP_R12W();
    // void POP_R13W();
    // void POP_R14W();
    // void POP_R15W();

    // void POP_RAX();
    // void POP_RCX();
    // void POP_RDX();
    // void POP_RBX();
    // void POP_RSP();
    // void POP_RBP();
    // void POP_RSI();
    // void POP_RDI();

    // void POP_R8();
    // void POP_R9();
    // void POP_R10();
    // void POP_R11();
    // void POP_R12();
    // void POP_R13();
    // void POP_R14();
    // void POP_R15();

    // // POP r/m16
    // void POP_M16(IsMemory16 auto m) { OpSize(); REX(m); OP(0x8F); ModRM(0x00, m); }
    // void POP_R16(IsGPR16 auto r) { OpSize(); REX(r); OP(0x8F); ModRM(0x00, r); }

    // // POP r/m32
    // void POP_M32(IsMemory32 auto m) { REX(m); OP(0x8F); ModRM(0x00, m); }
    // void POP_R32(IsGPR32 auto r) { REX(r); OP(0x8F); ModRM(0x00, r); }

    // // POP r/m64
    // void POP_M64(IsMemory64 auto m) { REX(m); OP(0x8F); ModRM(0x00, m); }
    // void POP_R64(IsGPR64 auto r) { REX(r); OP(0x8F); ModRM(0x00, r); }

    // // Opcode 0x69: IMUL r16/32/64, r/m16/32/64, imm16/32
    // void IMUL_R16_R16_IMM16(IsGPR16 auto r1, IsGPR16 auto r2, Immediate16 i) { OpSize(); REX(r1, r2); OP(0x69); ModRM(r1, r2); Emit16(i); }
    // void IMUL_R32_R32_IMM32(IsGPR32 auto r1, IsGPR32 auto r2, Immediate32 i) { REX(r1, r2); OP(0x69); ModRM(r1, r2); Emit32(i); }
    // void IMUL_R64_R64_IMM32(IsGPR64 auto r1, IsGPR64 auto r2, Immediate32 i) { REX(r1, r2); OP(0x69); ModRM(r1, r2); Emit32(i); }
    // void IMUL_R16_M16_IMM16(IsGPR16 auto r, IsMemory16 auto m, Immediate16 i) { OpSize(); REX(r, m); OP(0x69); ModRM(r, m); Emit16(i); }
    // void IMUL_R32_M32_IMM32(IsGPR32 auto r, IsMemory32 auto m, Immediate32 i) { REX(r, m); OP(0x69); ModRM(r, m); Emit32(i); }
    // void IMUL_R64_M64_IMM32(IsGPR64 auto r, IsMemory64 auto m, Immediate32 i) { REX(r, m); OP(0x69); ModRM(r, m); Emit32(i); }

    // // Opcode 0x6B: IMUL r16/32/64, r/m16/32/64, imm8
    // void IMUL_R16_R16_IMM8(IsGPR16 auto r1, IsGPR16 auto r2, Immediate8 i) { OpSize(); REX(r1, r2); OP(0x6B); ModRM(r1, r2); Emit8(i); }
    // void IMUL_R32_R32_IMM8(IsGPR32 auto r1, IsGPR32 auto r2, Immediate8 i) { REX(r1, r2); OP(0x6B); ModRM(r1, r2); Emit8(i); }
    // void IMUL_R64_R64_IMM8(IsGPR64 auto r1, IsGPR64 auto r2, Immediate8 i) { REX(r1, r2); OP(0x6B); ModRM(r1, r2); Emit8(i); }
    // void IMUL_R16_M16_IMM8(IsGPR16 auto r, IsMemory16 auto m, Immediate8 i) { OpSize(); REX(r, m); OP(0x6B); ModRM(r, m); Emit8(i); }
    // void IMUL_R32_M32_IMM8(IsGPR32 auto r, IsMemory32 auto m, Immediate8 i) { REX(r, m); OP(0x6B); ModRM(r, m); Emit8(i); }
    // void IMUL_R64_M64_IMM8(IsGPR64 auto r, IsMemory64 auto m, Immediate8 i) { REX(r, m); OP(0x6B); ModRM(r, m); Emit8(i); }

    // void JO(code::Relative8& rel) { OP(0x70); Emit(rel); }
    // void JO(code::Relative16& rel) { OpSize(); Escape(0x80); Emit(rel); }
    // void JO(code::Relative32& rel) { Escape(0x80); Emit(rel); }
    // void JNO(code::Relative8& rel) { OP(0x71); Emit(rel); }
    // void JNO(code::Relative16& rel) { OpSize(); Escape(0x81); Emit(rel); }
    // void JNO(code::Relative32& rel) { Escape(0x81); Emit(rel); }
    // void JB(code::Relative8& rel) { OP(0x72); Emit(rel); }
    // void JB(code::Relative16& rel) { OpSize(); Escape(0x82); Emit(rel); }
    // void JB(code::Relative32& rel) { Escape(0x82); Emit(rel); }
    // void JNB(code::Relative8& rel) { OP(0x73); Emit(rel); }
    // void JNB(code::Relative16& rel) { OpSize(); Escape(0x83); Emit(rel); }
    // void JNB(code::Relative32& rel) { Escape(0x83); Emit(rel); }
    // void JZ(code::Relative8& rel) { OP(0x74); Emit(rel); }
    // void JZ(code::Relative16& rel) { OpSize(); Escape(0x84); Emit(rel); }
    // void JZ(code::Relative32& rel) { Escape(0x84); Emit(rel); }
    // void JNZ(code::Relative8& rel) { OP(0x75); Emit(rel); }
    // void JNZ(code::Relative16& rel) { OpSize(); Escape(0x85); Emit(rel); }
    // void JNZ(code::Relative32& rel) { Escape(0x85); Emit(rel); }
    // void JBE(code::Relative8& rel) { OP(0x76); Emit(rel); }
    // void JBE(code::Relative16& rel) { OpSize(); Escape(0x86); Emit(rel); }
    // void JBE(code::Relative32& rel) { Escape(0x86); Emit(rel); }
    // void JNBE(code::Relative8& rel) { OP(0x77); Emit(rel); }
    // void JNBE(code::Relative16& rel) { OpSize(); Escape(0x87); Emit(rel); }
    // void JNBE(code::Relative32& rel) { Escape(0x87); Emit(rel); }
    // void JS(code::Relative8& rel) { OP(0x78); Emit(rel); }
    // void JS(code::Relative16& rel) { OpSize(); Escape(0x88); Emit(rel); }
    // void JS(code::Relative32& rel) { Escape(0x88); Emit(rel); }
    // void JNS(code::Relative8& rel) { OP(0x79); Emit(rel); }
    // void JNS(code::Relative16& rel) { OpSize(); Escape(0x89); Emit(rel); }
    // void JNS(code::Relative32& rel) { Escape(0x89); Emit(rel); }
    // void JP(code::Relative8& rel) { OP(0x7A); Emit(rel); }
    // void JP(code::Relative16& rel) { OpSize(); Escape(0x8A); Emit(rel); }
    // void JP(code::Relative32& rel) { Escape(0x8A); Emit(rel); }
    // void JNP(code::Relative8& rel) { OP(0x7B); Emit(rel); }
    // void JNP(code::Relative16& rel) { OpSize(); Escape(0x8B); Emit(rel); }
    // void JNP(code::Relative32& rel) { Escape(0x8B); Emit(rel); }
    // void JL(code::Relative8& rel) { OP(0x7C); Emit(rel); }
    // void JL(code::Relative16& rel) { OpSize(); Escape(0x8C); Emit(rel); }
    // void JL(code::Relative32& rel) { Escape(0x8C); Emit(rel); }
    // void JNL(code::Relative8& rel) { OP(0x7D); Emit(rel); }
    // void JNL(code::Relative16& rel) { OpSize(); Escape(0x8D); Emit(rel); }
    // void JNL(code::Relative32& rel) { Escape(0x8D); Emit(rel); }
    // void JLE(code::Relative8& rel) { OP(0x7E); Emit(rel); }
    // void JLE(code::Relative16& rel) { OpSize(); Escape(0x8E); Emit(rel); }
    // void JLE(code::Relative32& rel) { Escape(0x8E); Emit(rel); }
    // void JNLE(code::Relative8& rel) { OP(0x7F); Emit(rel); }
    // void JNLE(code::Relative16& rel) { OpSize(); Escape(0x8F); Emit(rel); }
    // void JNLE(code::Relative32& rel) { Escape(0x8F); Emit(rel); }

    // // // Opcode 0x69: IMUL r16/32/64, r/m16/32/64, imm16/32
    // // void IMUL(const code::Register& d, const code::Register& s, code::Immediate8& i) { Encode(d, s, i, Op{0x6B}); }
    // // void IMUL(const code::Register& d, const code::Address& s, code::Immediate8& i) { Encode(d, s, i, Op{0x6B}); }

    // // void IMUL(const code::Register& d, const code::Register& s, code::Immediate16& i) { Encode(d, s, i, Op{0x69}); }
    // // void IMUL(const code::Register& d, const code::Address& s, code::Immediate16& i) { Encode(d, s, i, Op{0x69}); }

    // // void IMUL(const code::Register& d, const code::Register& s, code::Immediate32& i) { Encode(d, s, i, Op{0x69}); }
    // // void IMUL(const code::Register& d, const code::Address& s, code::Immediate32& i) { Encode(d, s, i, Op{0x69}); }

    // void MUL(const code::Address& d) { Encode(d, Op{0xF7}, Op8{0xF6}, Mod{0x04}); }
    // void MUL(const code::Register& d) { Encode(d, Op{0xF7}, Op8{0xF6}, Mod{0x04}); }
    // void IMUL(const code::Address& d) { Encode(d, Op{0xF7}, Op8{0xF6}, Mod{0x05}); }
    // void IMUL(const code::Register& d) { Encode(d, Op{0xF7}, Op8{0xF6}, Mod{0x05}); }
    // void DIV(const code::Address& d) { Encode(d, Op{0xF7}, Op8{0xF6}, Mod{0x06}); }
    // void DIV(const code::Register& d) { Encode(d, Op{0xF7}, Op8{0xF6}, Mod{0x06}); }
    // void IDIV(const code::Address& d) { Encode(d, Op{0xF7}, Op8{0xF6}, Mod{0x07}); }
    // void IDIV(const code::Register& d) { Encode(d, Op{0xF7}, Op8{0xF6}, Mod{0x07}); }

    // void JO_REL8(Relative8 rel) { OP(0x70); Emit(rel - 2); }
    // void JO_REL16(Relative16 rel) { OpSize(); Escape(0x80); Emit(rel - 5); }
    // void JO_REL32(Relative32 rel) { Escape(0x80); Emit(rel - 6); }

    // void JNO_REL8(Relative8 rel) { OP(0x71); Emit(rel - 2); }
    // void JNO_REL16(Relative16 rel) { OpSize(); Escape(0x81); Emit(rel - 5); }
    // void JNO_REL32(Relative32 rel) { Escape(0x81); Emit(rel - 6); }

    // void JB_REL8(Relative8 rel) { OP(0x72); Emit(rel - 2); }
    // void JB_REL16(Relative16 rel) { OpSize(); Escape(0x82); Emit(rel - 5); }
    // void JB_REL32(Relative32 rel) { Escape(0x82); Emit(rel - 6); }

    // void JNB_REL8(Relative8 rel) { OP(0x73); Emit(rel - 2); }
    // void JNB_REL16(Relative16 rel) { OpSize(); Escape(0x83); Emit(rel - 5); }
    // void JNB_REL32(Relative32 rel) { Escape(0x83); Emit(rel - 6); }

    // void JZ_REL8(Relative8 rel) { OP(0x74); Emit(rel - 2); }
    // void JZ_REL16(Relative16 rel) { OpSize(); Escape(0x84); Emit(rel - 5); }
    // void JZ_REL32(Relative32 rel) { Escape(0x84); Emit(rel - 6); }

    // // Opcode 0x75: JNZ (or JNE) r/m8
    // void JNZ_REL8(Relative8 rel) { OP(0x75); Emit(rel - 2); }

    // // Opcode 0x850F: JNZ (or JNE) r/m16/32
    // void JNZ_REL16(Relative16 rel) { OpSize(); Escape(0x85); Emit(rel - 5); }
    // void JNZ_REL32(Relative32 rel) { Escape(0x85); Emit(rel - 6); }

    // // Opcode 0x76: JBE (or JNA) r/m8
    // void JBE_REL8(Relative8 rel) { OP(0x76); Emit(rel - 2); }
    // // Opcode 0x860F: JBE (or JNA) r/m16/32
    // void JBE_REL16(Relative16 rel) { OpSize(); Escape(0x86); Emit(rel - 5); }
    // void JBE_REL32(Relative32 rel) { Escape(0x86); Emit(rel - 6); }

    // // Opcode 0x77: JNBE (or JA) r/m8
    // void JNBE_REL8(Relative8 rel) { OP(0x77); Emit(rel - 2); }
    // // Opcode 0x870F: JNBE (or JA) r/m16/32
    // void JNBE_REL16(Relative16 rel) { OpSize(); Escape(0x87); Emit(rel - 5); }
    // void JNBE_REL32(Relative32 rel) { Escape(0x87); Emit(rel - 6); }

    // // Opcode 0x78: JS r/m8
    // void JS_REL8(Relative8 rel) { OP(0x78); Emit(rel - 2); }

    // // Opcode 0x880F: JS r/m16/32
    // void JS_REL16(Relative16 rel) { OpSize(); Escape(0x88); Emit(rel - 5); }
    // void JS_REL32(Relative32 rel) { Escape(0x88); Emit(rel - 6); }

    // // Opcode 0x79: JNS r/m8
    // void JNS_REL8(Relative8 rel) { OP(0x79); Emit(rel - 2); }
    // // Opcode 0x890F: JNS r/m16/32
    // void JNS_REL16(Relative16 rel) { OpSize(); Escape(0x89); Emit(rel - 5); }
    // void JNS_REL32(Relative32 rel) { Escape(0x89); Emit(rel - 6); }

    // // Opcode 0x7A: JP (or JPE) r/m8
    // void JP_REL8(Relative8 rel) { OP(0x7A); Emit(rel - 2); }
    // // Opcode 0x8A0F: JP (or JPE) r/m16/32
    // void JP_REL16(Relative16 rel) { OpSize(); Escape(0x8A); Emit(rel - 5); }
    // void JP_REL32(Relative32 rel) { Escape(0x8A); Emit(rel - 6); }

    // // Opcode 0x7B: JNP (or JPO) r/m8
    // void JNP_REL8(Relative8 rel) { OP(0x7B); Emit(rel - 2); }
    // // Opcode 0x8B0F: JNP (or JPO) r/m16/32
    // void JNP_REL16(Relative16 rel) { OpSize(); Escape(0x8B); Emit(rel - 5); }
    // void JNP_REL32(Relative32 rel) { Escape(0x8B); Emit(rel - 6); }

    // // Opcode 0x7C: JL (or JNGE) r/m8
    // void JL_REL8(Relative8 rel) { OP(0x7C); Emit(rel - 2); }
    // // Opcode 0x8C0F: JL (or JNGE) r/m16/32
    // void JL_REL16(Relative16 rel) { OpSize(); Escape(0x8C); Emit(rel - 5); }
    // void JL_REL32(Relative32 rel) { Escape(0x8C); Emit(rel - 6); }

    // // Opcode 0x7D: JNL (or JGE) r/m8
    // void JNL_REL8(Relative8 rel) { OP(0x7D); Emit(rel - 2); }
    // // Opcode 0x8D0F: JNL (or JGE) r/m16/32
    // void JNL_REL16(Relative16 rel) { OpSize(); Escape(0x8D); Emit(rel - 5); }
    // void JNL_REL32(Relative32 rel) { Escape(0x8D); Emit(rel - 6); }

    // // Opcode 0x7E: JLE (or JNG) r/m8
    // void JLE_REL8(Relative8 rel) { OP(0x7E); Emit(rel - 2); }

    // // Opcode 0x8E0F: JLE (or JNG) r/m16/32
    // void JLE_REL16(Relative16 rel) { OpSize(); Escape(0x8E); Emit(rel - 5); }
    // void JLE_REL32(Relative32 rel) { Escape(0x8E); Emit(rel - 6); }

    // // Opcode 0x7F: JNLE (or JG) r/m8
    // void JNLE_REL8(Relative8 rel) { OP(0x7F); Emit(rel - 2); }

    // // Opcode 0x8F0F: JNLE (or JG) r/m16/32
    // void JNLE_REL16(Relative16 rel) { OpSize(); Escape(0x8F); Emit(rel - 5); }
    // void JNLE_REL32(Relative32 rel) { Escape(0x8F); Emit(rel - 6); }

    // // void Encode(const code::Register& dest, const code::Register& src, uint8_t opcode) {
    // //   if (dest.Is16Bit()) OpSize();
    // //   REX(dest, src);
    // //   OP(opcode);
    // //   ModRM(dest, src);
    // // }

    // // void Encode(const code::Register& dest, const code::Immediate& src, uint8_t opcode) {
    // //   if (dest.Is16Bit()) OpSize();
    // //   REX(dest, src);
    // //   OP(opcode);
    // //   ModRM(dest, src);
    // // }

    // // void Lockable(const code::Register& dest, const code::Register& src, uint8_t opcode) {
    // //   if (dest.IsAtomic()) Lock();
    // //   Encode(dest, src, opcode);
    // // }

    // template<uint8_t Op>
    // void Unary(std::string_view name, IsRMI auto const& dest, Mod mod) {
      
    // }

    // template<uint8_t Op, uint8_t Op8>
    // void Unary(std::string_view name, IsRMI auto const& dest, Mod mod) {
      
    // }

    // template<uint8_t Op>
    // void UnaryEsc(std::string_view name, IsRMI auto const& dest, Mod mod) {
      
    // }

    // template<uint8_t Op, uint8_t Op8>
    // void UnaryEsc(std::string_view name, IsRMI auto const& dest, Mod mod) {
      
    // }

    // template<uint8_t Op, uint8_t Op8, bool Escape, bool UseOp8>
    // void BinaryHandler(std::string_view name, const List& list, IsRM auto const& dest, IsRMI auto const& src) {
    //   // TODO
    // }

    // template<uint8_t Op>
    // void Binary(std::string_view name, const List& list, IsRM auto const& dest, IsRMI auto const& src) {
    //   return BinaryHandler<Op, 0x00, false, false>(name, list, dest, src);
    // }

    // template<uint8_t Op>
    // void BinaryEsc(std::string_view name, const List& list, IsRM auto const& dest, IsRMI auto const& src) {
    //   return BinaryHandler<Op, 0x00, false, true>(name, list, dest, src);
    // }

    // template<uint8_t Op, uint8_t Op8>
    // void Binary(std::string_view name, const List& list, IsRM auto const& dest, IsRMI auto const& src) {
    //   return BinaryHandler<Op, Op8, false, true>(name, list, dest, src);
    // }

    // template<uint8_t Op, uint8_t Op8, uint8_t Mod>
    // void Binary(std::string_view name, const List& list, IsRM auto const& dest, IsRMI auto const& src) {
    //   return BinaryHandler<Op, Op8, false, true>(name, list, dest, src);
    // }

    // template<uint8_t Op, uint8_t Op8>
    // void BinaryEsc(std::string_view name, const List& list, IsRM auto const& dest, IsRMI auto const& src) {
    //   return BinaryHandler<Op, Op8, true, true>(name, list, dest, src);
    // }

    // static constexpr List MOV_R_I{{S64, S32 | S64}, {S32, S32}, {S16, S16}, {S8, S8}};
    // void MOV(const code::Register& d, const code::Register& s) { Binary<0x89, 0x88>("MOV", SAME, d, s); }
    // void MOV(const code::Address& d, const code::Register& s) { Binary<0x8B, 0x8A>("MOV", SAME, d, s); }
    // void MOV(const code::Register& d, const code::Address& s) { Binary<0x89, 0x88>("MOV", SAME, d, s); }
    // void MOV(const code::Register& d, const code::Immediate& i) { Binary<0xB8, 0xB0>("MOV", MOV_R_I, d, i); }
    // void MOV(const code::Address& d, const code::Immediate& i) { Binary<0xC7, 0xC6>("MOV", IMM, d, i); }
    // // void MOV0(const code::Register& d) { MOV(d, code::Immediate::ZERO); }
    // // void MOV0(const code::Address& d) { MOV(d, code::Immediate::ZERO); }
    // // void MOV1(const code::Register& d) { MOV(d, code::Immediate::ONE); }
    // // void MOV1(const code::Address& d) { MOV(d, code::Immediate::ONE); }

    // // Opcode 0x88: MOV r/m8, r8
    // void MOV_M8_R8(IsMemory8 auto m, IsGPR8 auto r) { REX(m, r); OP(0x88); ModRM(m, r); }
    // void MOV_R8_R8(IsGPR8 auto r1, IsGPR8 auto r2) { REX(r1, r2); OP(0x88); ModRM(r1, r2); }

    // // Opcode 0x89: MOV r/m16/32/64, r16/32/64
    // void MOV_R16_R16(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); REX(r1, r2); OP(0x89); ModRM(r1, r2); }
    // void MOV_R32_R32(IsGPR32 auto r1, IsGPR32 auto r2) { REX(r1, r2); OP(0x89); ModRM(r1, r2); }
    // void MOV_R64_R64(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x89); ModRM(r1, r2); }
    // void MOV_M16_R16(IsMemory16 auto m, IsGPR16 auto r) { OpSize(); REX(m, r); OP(0x89); ModRM(m, r); }
    // void MOV_M32_R32(IsMemory32 auto m, IsGPR32 auto r) { REX(m, r); OP(0x89); ModRM(m, r); }
    // void MOV_M64_R64(IsMemory64 auto m, IsGPR64 auto r) { REX(m, r); OP(0x89); ModRM(m, r); }

    // // Opcode 0x8A: MOV r8, r/m8
    // void MOV_R8_M8(IsGPR8 auto r, IsMemory8 auto m) { OP(0x8A); ModRM(r, m); }
    // void MOV_R8_R8_ALT(IsGPR8 auto r1, IsGPR8 auto r2) { OP(0x8A); ModRM(r1, r2); }

    // // Opcode 0x8B: MOV r16/32/64, r/m16/32/64
    // void MOV_R16_M16(IsGPR16 auto r, IsMemory16 auto m) { OpSize(); OP(0x8B); ModRM(r, m); }
    // void MOV_R32_M32(IsGPR32 auto r, IsMemory32 auto m) { OP(0x8B); ModRM(r, m); }
    // void MOV_R64_M64(IsGPR64 auto r, IsMemory64 auto m) { REX(r, m); OP(0x8B); ModRM(r, m); }
    // void MOV_R16_R16_ALT(IsGPR16 auto r1, IsGPR16 auto r2) { OpSize(); OP(0x8B); ModRM(r1, r2); }
    // void MOV_R32_R32_ALT(IsGPR32 auto r1, IsGPR32 auto r2) { OP(0x8B); ModRM(r1, r2); }
    // void MOV_R64_R64_ALT(IsGPR64 auto r1, IsGPR64 auto r2) { REX(r1, r2); OP(0x8B); ModRM(r1, r2); }

    // // Opcode 0xB0 to 0xB7: MOV r8, imm8
    // void MOV_AL_IMM8(Immediate8 i);
    // void MOV_CL_IMM8(Immediate8 i);
    // void MOV_DL_IMM8(Immediate8 i);
    // void MOV_BL_IMM8(Immediate8 i);
    // void MOV_SPL_IMM8(Immediate8 i);
    // void MOV_BPL_IMM8(Immediate8 i);
    // void MOV_SIL_IMM8(Immediate8 i);
    // void MOV_DIL_IMM8(Immediate8 i);

    // void MOV_R8B_IMM8(Immediate8 i);
    // void MOV_R9B_IMM8(Immediate8 i);
    // void MOV_R10B_IMM8(Immediate8 i);
    // void MOV_R11B_IMM8(Immediate8 i);
    // void MOV_R12B_IMM8(Immediate8 i);
    // void MOV_R13B_IMM8(Immediate8 i);
    // void MOV_R14B_IMM8(Immediate8 i);
    // void MOV_R15B_IMM8(Immediate8 i);

    // // Opcode 0xB8 to 0xBF: MOV r16/32/64, imm16/32/64
    // void MOV_R16_IMM16(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0xB8 + static_cast<uint8_t>(r)); Emit16(i); }
    // void MOV_R32_IMM32(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0xB8 + static_cast<uint8_t>(r)); Emit32(i); }
    // void MOV_R64_IMM64(IsGPR64 auto r, Immediate64 i) { REX(r); OP(0xB8 + static_cast<uint8_t>(r)); Emit64(i); }

    // void MOV_AX_IMM16(Immediate16 i);
    // void MOV_EAX_IMM32(Immediate32 i);
    // void MOV_RAX_IMM64(Immediate64 i);
    // void MOV_CX_IMM16(Immediate16 i);
    // void MOV_ECX_IMM32(Immediate32 i);
    // void MOV_RCX_IMM64(Immediate64 i);
    // void MOV_DX_IMM16(Immediate16 i);
    // void MOV_EDX_IMM32(Immediate32 i);
    // void MOV_RDX_IMM64(Immediate64 i);
    // void MOV_BX_IMM16(Immediate16 i);
    // void MOV_EBX_IMM32(Immediate32 i);
    // void MOV_RBX_IMM64(Immediate64 i);
    // void MOV_SP_IMM16(Immediate16 i);
    // void MOV_ESP_IMM32(Immediate32 i);
    // void MOV_RSP_IMM64(Immediate64 i);
    // void MOV_BP_IMM16(Immediate16 i);
    // void MOV_EBP_IMM32(Immediate32 i);
    // void MOV_RBP_IMM64(Immediate64 i);
    // void MOV_SI_IMM16(Immediate16 i);
    // void MOV_ESI_IMM32(Immediate32 i);
    // void MOV_RSI_IMM64(Immediate64 i);
    // void MOV_DI_IMM16(Immediate16 i);
    // void MOV_EDI_IMM32(Immediate32 i);
    // void MOV_RDI_IMM64(Immediate64 i);

    // // EGPR versions
    // void MOV_R8W_IMM16(Immediate16 i);
    // void MOV_R8D_IMM32(Immediate32 i);
    // void MOV_R8_IMM64(Immediate64 i);
    // void MOV_R9W_IMM16(Immediate16 i);
    // void MOV_R9D_IMM32(Immediate32 i);
    // void MOV_R9_IMM64(Immediate64 i);
    // void MOV_R10W_IMM16(Immediate16 i);
    // void MOV_R10D_IMM32(Immediate32 i);
    // void MOV_R10_IMM64(Immediate64 i);
    // void MOV_R11W_IMM16(Immediate16 i);
    // void MOV_R11D_IMM32(Immediate32 i);
    // void MOV_R11_IMM64(Immediate64 i);
    // void MOV_R12W_IMM16(Immediate16 i);
    // void MOV_R12D_IMM32(Immediate32 i);
    // void MOV_R12_IMM64(Immediate64 i);
    // void MOV_R13W_IMM16(Immediate16 i);
    // void MOV_R13D_IMM32(Immediate32 i);
    // void MOV_R13_IMM64(Immediate64 i);
    // void MOV_R14W_IMM16(Immediate16 i);
    // void MOV_R14D_IMM32(Immediate32 i);
    // void MOV_R14_IMM64(Immediate64 i);
    // void MOV_R15W_IMM16(Immediate16 i);
    // void MOV_R15D_IMM32(Immediate32 i);
    // void MOV_R15_IMM64(Immediate64 i);

    // // Opcode 0xC6: MOV r/m8, imm8
    // void MOV_M8_IMM8(IsGPR8 auto r, Immediate8 i) { OP(0xC6); ModRM(0x00, r); Emit(i); }
    // void MOV_M8_IMM8(IsMemory8 auto m, Immediate8 i) { OP(0xC6); ModRM(0x00, m); Emit(i); }

    // // Opcode 0xC7: MOV r/m16/32/64, imm16/32
    // void MOV_R16_IMM16_ALT(IsGPR16 auto r, Immediate16 i) { OpSize(); REX(r); OP(0xC7); ModRM(0x00, r); Emit16(i); }
    // void MOV_R32_IMM32_ALT(IsGPR32 auto r, Immediate32 i) { REX(r); OP(0xC7); ModRM(0x00, r); Emit32(i); }
    // void MOV_R64_IMM32(IsGPR64 auto r, Immediate32 i) { REX(r); OP(0xC7); ModRM(0x00, r); Emit32(i); }
    // void MOV_M16_IMM16(IsMemory16 auto m, Immediate16 i) { OpSize(); OP(0xC7); ModRM(0x00, m); Emit16(i); }
    // void MOV_M32_IMM32(IsMemory32 auto m, Immediate32 i) { OP(0xC7); ModRM(0x00, m); Emit32(i); }
    // void MOV_M64_IMM32(IsMemory64 auto m, Immediate32 i) { REX(m); OP(0xC7); ModRM(0x00, m); Emit32(i); }

    // static constexpr List MOVSX_R_RM{{S16, S8}, {S32, S16_8}, {S64, S16_8}};
    // void MOVSX(const code::Register& d, const code::Register& s) { BinaryEsc<0xBF, 0xBE>("MOVSX", MOVSX_R_RM, d, s); }
    // void MOVSX(const code::Register& d, const code::Address& s) { BinaryEsc<0xBF, 0xBE>("MOVSX", MOVSX_R_RM, d, s); }

    // static constexpr List MOVZX_R_RM{{S16, S8}, {S32, S16_8}, {S64, S16_8}};
    // void MOVZX(const code::Register& d, const code::Register& s) { BinaryEsc<0xB7, 0xB6>("MOVZX", MOVZX_R_RM, d, s); }
    // void MOVZX(const code::Register& d, const code::Address& s) { BinaryEsc<0x89, 0x88>("MOVZX", MOVZX_R_RM, d, s); }

    // // void MOVZX(const code::Register& d, const code::Register& s) { OpSize(d); REX(d, s); Esc(s.IsByte() ? 0xB6 : 0xB7); ModRM(d, s); }
    // // void MOVZX(const code::Register& d, const code::Address& s) { OpSize(d); REX(d, s); Esc(s.IsByte() ? 0xB6 : 0xB7); ModRM(d, s); }
    
    // // void MOVSX(const code::Register& d, const code::Register& s) { OpSize(d); REX(d, s); Esc(s.IsByte() ? 0xBE : 0xBF); ModRM(d, s); }
    // // void MOVSX(const code::Register& d, const code::Address& s) { OpSize(d); REX(d, s); Esc(s.IsByte() ? 0xBE : 0xBF); ModRM(d, s); }

    // // void MOVSX(code::Register& d, code::Register& s) { OpSize(d, s); REX(d, s); d.IsByte() ? Esc(0xBE) : Esc(0xBF); ModRM(d, s); }
    // // void MOVSX(code::Register& d, code::Address& s) { OpSize(d, s); REX(d, s); d.IsByte() ? Esc(0xBE) : Esc(0xBF); ModRM(d, s); }

    // // // Opcode 0x0F 0xBE/0xBF: MOVSX r16/32/64, r/m8
    // // void MOVSX(code::Register& d, code::Register& s);
    // // void MOVSX(code::Register& d, code::Address& s);

    // void MOVSX_R16_R8(IsGPR16 auto r, IsGPR8 auto m) { OpSize(); REX(r, m); Escape(0xBE); ModRM(r, m); }
    // void MOVSX_R32_R8(IsGPR32 auto r, IsGPR8 auto m) { REX(r, m); Escape(0xBE); ModRM(r, m); }
    // void MOVSX_R64_R8(IsGPR64 auto r, IsGPR8 auto m) { REX(r, m); Escape(0xBE); ModRM(r, m); }
    // void MOVSX_R32_R16(IsGPR32 auto r, IsGPR16 auto m) { REX(r, m); Escape(0xBF); ModRM(r, m); }
    // void MOVSX_R64_R16(IsGPR64 auto r, IsGPR16 auto m) { REX(r, m); Escape(0xBF); ModRM(r, m); }
    // void MOVSX_R16_M8(IsGPR16 auto r, IsMemory8 auto m) { OpSize(); REX(r, m); Escape(0xBE); ModRM(r, m); }
    // void MOVSX_R32_M8(IsGPR32 auto r, IsMemory8 auto m) { REX(r, m); Escape(0xBE); ModRM(r, m); }
    // void MOVSX_R64_M8(IsGPR64 auto r, IsMemory8 auto m) { REX(r, m); Escape(0xBE); ModRM(r, m); }
    // void MOVSX_R32_M16(IsGPR32 auto r, IsMemory16 auto m) { REX(r, m); Escape(0xBF); ModRM(r, m); }
    // void MOVSX_R64_M16(IsGPR64 auto r, IsMemory16 auto m) { REX(r, m); Escape(0xBF); ModRM(r, m); }

    // // Opcode 0x63: MOVSXD r32/64, r/m32
    // void MOVSXD(const code::Register& d, const code::Register& s) { REX(d, s); OP(0x63); ModRM(d, s); }
    // void MOVSXD(const code::Register& d, const code::Address& s) { REX(d, s); OP(0x63); ModRM(d, s); }
    // // void MOVSXD(const code::Register& d, const code::Register& s);
    // // void MOVSXD(const code::Register& d, const code::Address& s);

    // void MOVSXD_R64_M32(IsGPR64 auto r, IsMemory32 auto m) { REX(r, m); OP(0x63); ModRM(r, m); }
    // void MOVSXD_R64_R32(IsGPR64 auto r, IsGPR32 auto m) { REX(r, m); OP(0x63); ModRM(r, m); }

    // void NOP();
    // void PUSHFQ();
    // void POPFQ();

    // void STC();
    // void CLC();
    // void CMC();

    // // Opcode 0xD0: ROL r/m8, 1
    // void ROL_M8_1(IsMemory8 auto m) { OP(0xD0); ModRM(0x00, m); }
    // void ROL_R8_1(IsGPR8 auto r) { OP(0xD0); ModRM(0x00, r); }

    // // Opcode 0xD1: ROL r/m16/32/64, 1
    // void ROL_M16_1(IsMemory16 auto m) { OpSize(); OP(0xD1); ModRM(0x00, m); }
    // void ROL_R16_1(IsGPR16 auto r) { OpSize(); OP(0xD1); ModRM(0x00, r); }
    // void ROL_M32_1(IsMemory32 auto m) { OP(0xD1); ModRM(0x00, m); }
    // void ROL_R32_1(IsGPR32 auto r) { OP(0xD1); ModRM(0x00, r); }
    // void ROL_M64_1(IsMemory64 auto m) { REX(m); OP(0xD1); ModRM(0x00, m); }
    // void ROL_R64_1(IsGPR64 auto r) { REX(r); OP(0xD1); ModRM(0x00, r); }

    // // Opcode 0xC0: ROL r/m8, imm8
    // void ROL_M8_IMM8(IsMemory8 auto m, Immediate8 i) { OP(0xC0); ModRM(0x00, m); Emit8(i); }
    // void ROL_R8_IMM8(IsGPR8 auto r, Immediate8 i) { OP(0xC0); ModRM(0x00, r); Emit8(i); }

    // // Opcode 0xC1: ROL r/m16/32/64, imm8
    // void ROL_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x00, m); Emit8(i); }
    // void ROL_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x00, r); Emit8(i); }
    // void ROL_M32_IMM8(IsMemory32 auto m, Immediate8 i) { OP(0xC1); ModRM(0x00, m); Emit8(i); }
    // void ROL_R32_IMM8(IsGPR32 auto r, Immediate8 i) { OP(0xC1); ModRM(0x00, r); Emit8(i); }
    // void ROL_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0xC1); ModRM(0x00, m); Emit8(i); }
    // void ROL_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0xC1); ModRM(0x00, r); Emit8(i); }

    // // Opcode 0xD0: ROR r/m8, 1
    // void ROR_M8_1(IsMemory8 auto m) { OP(0xD0); ModRM(0x01, m); }
    // void ROR_R8_1(IsGPR8 auto r) { OP(0xD0); ModRM(0x01, r); }

    // // Opcode 0xD1: ROR r/m16/32/64, 1
    // void ROR_M16_1(IsMemory16 auto m) { OpSize(); OP(0xD1); ModRM(0x01, m); }
    // void ROR_R16_1(IsGPR16 auto r) { OpSize(); OP(0xD1); ModRM(0x01, r); }
    // void ROR_M32_1(IsMemory32 auto m) { OP(0xD1); ModRM(0x01, m); }
    // void ROR_R32_1(IsGPR32 auto r) { OP(0xD1); ModRM(0x01, r); }
    // void ROR_M64_1(IsMemory64 auto m) { REX(m); OP(0xD1); ModRM(0x01, m); }
    // void ROR_R64_1(IsGPR64 auto r) { REX(r); OP(0xD1); ModRM(0x01, r); }

    // // Opcode 0xC0: ROR r/m8, imm8
    // void ROR_M8_IMM8(IsMemory8 auto m, Immediate8 i) { OP(0xC0); ModRM(0x01, m); Emit8(i); }
    // void ROR_R8_IMM8(IsGPR8 auto r, Immediate8 i) { OP(0xC0); ModRM(0x01, r); Emit8(i); }

    // // Opcode 0xC1: ROR r/m16/32/64, imm8
    // void ROR_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x01, m); Emit8(i); }
    // void ROR_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x01, r); Emit8(i); }
    // void ROR_M32_IMM8(IsMemory32 auto m, Immediate8 i) { OP(0xC1); ModRM(0x01, m); Emit8(i); }
    // void ROR_R32_IMM8(IsGPR32 auto r, Immediate8 i) { OP(0xC1); ModRM(0x01, r); Emit8(i); }
    // void ROR_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0xC1); ModRM(0x01, m); Emit8(i); }
    // void ROR_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0xC1); ModRM(0x01, r); Emit8(i); }

    // // Opcode 0xD0: RCL r/m8, 1
    // void RCL_M8_1(IsMemory8 auto m) { OP(0xD0); ModRM(0x02, m); }
    // void RCL_R8_1(IsGPR8 auto r) { OP(0xD0); ModRM(0x02, r); }

    // // Opcode 0xD1: RCL r/m16/32/64, 1
    // void RCL_M16_1(IsMemory16 auto m) { OpSize(); OP(0xD1); ModRM(0x02, m); }
    // void RCL_R16_1(IsGPR16 auto r) { OpSize(); OP(0xD1); ModRM(0x02, r); }
    // void RCL_M32_1(IsMemory32 auto m) { OP(0xD1); ModRM(0x02, m); }
    // void RCL_R32_1(IsGPR32 auto r) { OP(0xD1); ModRM(0x02, r); }
    // void RCL_M64_1(IsMemory64 auto m) { REX(m); OP(0xD1); ModRM(0x02, m); }
    // void RCL_R64_1(IsGPR64 auto r) { REX(r); OP(0xD1); ModRM(0x02, r); }

    // // Opcode 0xC0: RCL r/m8, imm8
    // void RCL_M8_IMM8(IsMemory8 auto m, Immediate8 i) { OP(0xC0); ModRM(0x02, m); Emit8(i); }
    // void RCL_R8_IMM8(IsGPR8 auto r, Immediate8 i) { OP(0xC0); ModRM(0x02, r); Emit8(i); }

    // // Opcode 0xC1: RCL r/m16/32/64, imm8
    // void RCL_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x02, m); Emit8(i); }
    // void RCL_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x02, r); Emit8(i); }
    // void RCL_M32_IMM8(IsMemory32 auto m, Immediate8 i) { OP(0xC1); ModRM(0x02, m); Emit8(i); }
    // void RCL_R32_IMM8(IsGPR32 auto r, Immediate8 i) { OP(0xC1); ModRM(0x02, r); Emit8(i); }
    // void RCL_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0xC1); ModRM(0x02, m); Emit8(i); }
    // void RCL_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0xC1); ModRM(0x02, r); Emit8(i); }

    // // Opcode 0xD0: RCR r/m8, 1
    // void RCR_M8_1(IsMemory8 auto m) { OP(0xD0); ModRM(0x03, m); }
    // void RCR_R8_1(IsGPR8 auto r) { OP(0xD0); ModRM(0x03, r); }

    // // Opcode 0xD1: RCR r/m16/32/64, 1
    // void RCR_M16_1(IsMemory16 auto m) { OpSize(); OP(0xD1); ModRM(0x03, m); }
    // void RCR_R16_1(IsGPR16 auto r) { OpSize(); OP(0xD1); ModRM(0x03, r); }
    // void RCR_M32_1(IsMemory32 auto m) { OP(0xD1); ModRM(0x03, m); }
    // void RCR_R32_1(IsGPR32 auto r) { OP(0xD1); ModRM(0x03, r); }
    // void RCR_M64_1(IsMemory64 auto m) { REX(m); OP(0xD1); ModRM(0x03, m); }
    // void RCR_R64_1(IsGPR64 auto r) { REX(r); OP(0xD1); ModRM(0x03, r); }

    // // Opcode 0xC0: RCR r/m8, imm8
    // void RCR_M8_IMM8(IsMemory8 auto m, Immediate8 i) { OP(0xC0); ModRM(0x03, m); Emit8(i); }
    // void RCR_R8_IMM8(IsGPR8 auto r, Immediate8 i) { OP(0xC0); ModRM(0x03, r); Emit8(i); }

    // // Opcode 0xC1: RCR r/m16/32/64, imm8
    // void RCR_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x03, m); Emit8(i); }
    // void RCR_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x03, r); Emit8(i); }
    // void RCR_M32_IMM8(IsMemory32 auto m, Immediate8 i) { OP(0xC1); ModRM(0x03, m); Emit8(i); }
    // void RCR_R32_IMM8(IsGPR32 auto r, Immediate8 i) { OP(0xC1); ModRM(0x03, r); Emit8(i); }
    // void RCR_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0xC1); ModRM(0x03, m); Emit8(i); }
    // void RCR_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0xC1); ModRM(0x03, r); Emit8(i); }

    // // Opcode 0xD0: SHL r/m8, 1
    // void SHL_M8_1(IsMemory8 auto m) { OP(0xD0); ModRM(0x04, m); }
    // void SHL_R8_1(IsGPR8 auto r) { OP(0xD0); ModRM(0x04, r); }

    // // Opcode 0xD1: SHL r/m16/32/64, 1
    // void SHL_M16_1(IsMemory16 auto m) { OpSize(); OP(0xD1); ModRM(0x04, m); }
    // void SHL_R16_1(IsGPR16 auto r) { OpSize(); OP(0xD1); ModRM(0x04, r); }
    // void SHL_M32_1(IsMemory32 auto m) { OP(0xD1); ModRM(0x04, m); }
    // void SHL_R32_1(IsGPR32 auto r) { OP(0xD1); ModRM(0x04, r); }
    // void SHL_M64_1(IsMemory64 auto m) { REX(m); OP(0xD1); ModRM(0x04, m); }
    // void SHL_R64_1(IsGPR64 auto r) { REX(r); OP(0xD1); ModRM(0x04, r); }

    // // Opcode 0xC0: SHL r/m8, imm8
    // void SHL_M8_IMM8(IsMemory8 auto m, Immediate8 i) { OP(0xC0); ModRM(0x04, m); Emit8(i); }
    // void SHL_R8_IMM8(IsGPR8 auto r, Immediate8 i) { OP(0xC0); ModRM(0x04, r); Emit8(i); }

    // // Opcode 0xC1: SHL r/m16/32/64, imm8
    // void SHL_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x04, m); Emit8(i); }
    // void SHL_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x04, r); Emit8(i); }
    // void SHL_M32_IMM8(IsMemory32 auto m, Immediate8 i) { OP(0xC1); ModRM(0x04, m); Emit8(i); }
    // void SHL_R32_IMM8(IsGPR32 auto r, Immediate8 i) { OP(0xC1); ModRM(0x04, r); Emit8(i); }
    // void SHL_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0xC1); ModRM(0x04, m); Emit8(i); }
    // void SHL_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0xC1); ModRM(0x04, r); Emit8(i); }

    // // Opcode 0xD0: SHR r/m8, 1
    // void SHR_M8_1(IsMemory8 auto m) { OP(0xD0); ModRM(0x05, m); }
    // void SHR_R8_1(IsGPR8 auto r) { OP(0xD0); ModRM(0x05, r); }

    // // Opcode 0xD1: SHR r/m16/32/64, 1
    // void SHR_M16_1(IsMemory16 auto m) { OpSize(); OP(0xD1); ModRM(0x05, m); }
    // void SHR_R16_1(IsGPR16 auto r) { OpSize(); OP(0xD1); ModRM(0x05, r); }
    // void SHR_M32_1(IsMemory32 auto m) { OP(0xD1); ModRM(0x05, m); }
    // void SHR_R32_1(IsGPR32 auto r) { OP(0xD1); ModRM(0x05, r); }
    // void SHR_M64_1(IsMemory64 auto m) { REX(m); OP(0xD1); ModRM(0x05, m); }
    // void SHR_R64_1(IsGPR64 auto r) { REX(r); OP(0xD1); ModRM(0x05, r); }

    // // Opcode 0xC0: SHR r/m8, imm8
    // void SHR_M8_IMM8(IsMemory8 auto m, Immediate8 i) { OP(0xC0); ModRM(0x05, m); Emit8(i); }
    // void SHR_R8_IMM8(IsGPR8 auto r, Immediate8 i) { OP(0xC0); ModRM(0x05, r); Emit8(i); }

    // // Opcode 0xC1: SHR r/m16/32/64, imm8
    // void SHR_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x05, m); Emit8(i); }
    // void SHR_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x05, r); Emit8(i); }
    // void SHR_M32_IMM8(IsMemory32 auto m, Immediate8 i) { OP(0xC1); ModRM(0x05, m); Emit8(i); }
    // void SHR_R32_IMM8(IsGPR32 auto r, Immediate8 i) { OP(0xC1); ModRM(0x05, r); Emit8(i); }
    // void SHR_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0xC1); ModRM(0x05, m); Emit8(i); }
    // void SHR_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0xC1); ModRM(0x05, r); Emit8(i); }

    // // Opcode 0xD0: SAR r/m8, 1
    // void SAR_M8_1(IsMemory8 auto m) { OP(0xD0); ModRM(0x07, m); }
    // void SAR_R8_1(IsGPR8 auto r) { OP(0xD0); ModRM(0x07, r); }

    // // Opcode 0xD1: SAR r/m16/32/64, 1
    // void SAR_M16_1(IsMemory16 auto m) { OpSize(); OP(0xD1); ModRM(0x07, m); }
    // void SAR_R16_1(IsGPR16 auto r) { OpSize(); OP(0xD1); ModRM(0x07, r); }
    // void SAR_M32_1(IsMemory32 auto m) { OP(0xD1); ModRM(0x07, m); }
    // void SAR_R32_1(IsGPR32 auto r) { OP(0xD1); ModRM(0x07, r); }
    // void SAR_M64_1(IsMemory64 auto m) { REX(m); OP(0xD1); ModRM(0x07, m); }
    // void SAR_R64_1(IsGPR64 auto r) { REX(r); OP(0xD1); ModRM(0x07, r); }

    // // Opcode 0xC0: SAR r/m8, imm8
    // void SAR_M8_IMM8(IsMemory8 auto m, Immediate8 i) { OP(0xC0); ModRM(0x07, m); Emit8(i); }
    // void SAR_R8_IMM8(IsGPR8 auto r, Immediate8 i) { OP(0xC0); ModRM(0x07, r); Emit8(i); }

    // // Opcode 0xC1: SAR r/m16/32/64, imm8
    // void SAR_M16_IMM8(IsMemory16 auto m, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x07, m); Emit8(i); }
    // void SAR_R16_IMM8(IsGPR16 auto r, Immediate8 i) { OpSize(); OP(0xC1); ModRM(0x07, r); Emit8(i); }
    // void SAR_M32_IMM8(IsMemory32 auto m, Immediate8 i) { OP(0xC1); ModRM(0x07, m); Emit8(i); }
    // void SAR_R32_IMM8(IsGPR32 auto r, Immediate8 i) { OP(0xC1); ModRM(0x07, r); Emit8(i); }
    // void SAR_M64_IMM8(IsMemory64 auto m, Immediate8 i) { REX(m); OP(0xC1); ModRM(0x07, m); Emit8(i); }
    // void SAR_R64_IMM8(IsGPR64 auto r, Immediate8 i) { REX(r); OP(0xC1); ModRM(0x07, r); Emit8(i); }

    // // LOOPNZ (Loop while not zero/not equal)
    // void LOOPNZ(code::Relative8& rel) { OP(0xE0); Emit(rel); }

    // // LOOPZ (Loop while zero/equal)
    // void LOOPZ(code::Relative8& rel) { OP(0xE1); Emit(rel); }

    // // LOOP (Loop with CX/ECX/RCX counter)
    // void LOOP(code::Relative8& rel) { OP(0xE2); Emit(rel); }

    // // JECXZ (Jump short if ECX is zero)
    // void JECXZ(code::Relative8& rel) { OP(0xE3); Emit(rel); }

    // // JRCXZ (Jump short if RCX is zero)
    // void JRCXZ(code::Relative8& rel) { REXW(); OP(0xE3); Emit(rel); }

    // void CALL(code::Relative16& rel) { OpSize(); OP(0xE8); Emit(rel); }
    // void CALL(code::Relative32& rel) { OP(0xE8); Emit(rel); }



    // // LOOPNZ (Loop while not zero/not equal)
    // void LOOPNZ_REL8(Relative8 rel) { OP(0xE0); Emit(rel - 2); }

    // // LOOPZ (Loop while zero/equal)
    // void LOOPZ_REL8(Relative8 rel) { OP(0xE1); Emit(rel - 2); }

    // // LOOP (Loop with CX/ECX/RCX counter)
    // void LOOP_REL8(Relative8 rel) { OP(0xE2); Emit(rel - 2); }

    // // JECXZ (Jump short if ECX is zero)
    // void JECXZ_REL8(Relative8 rel) { OP(0xE3); Emit(rel - 2); }

    // // JRCXZ (Jump short if RCX is zero)
    // void JRCXZ_REL8(Relative8 rel) { REXW(); OP(0xE3); Emit(rel - 3); }

    // // CALL instructions
    // void CALL_REL16(Relative16 rel) { OpSize(); OP(0xE8); Emit(rel - 4); }
    // void CALL_REL32(Relative32 rel) { OP(0xE8); Emit(rel - 5); }
    // // Placeholder16 CALL_REL16() { CALL_REL16(0); return Placeholder16{code}; }
    // // Placeholder32 CALL_REL32() { CALL_REL32(0); return Placeholder32{code}; }

    // // JMP instructions
    // void JMP(code::Relative8& rel) { OP(0xEB); Emit(rel); }
    // void JMP(code::Relative16& rel) { OpSize(); OP(0xE9); Emit(rel); }
    // void JMP(code::Relative32& rel) { OP(0xE9); Emit(rel); }
    // void JMP_REL8(Relative8 rel) { OP(0xEB); Emit(rel - 2); }
    // void JMP_REL16(Relative16 rel) { OpSize(); OP(0xE9); Emit(rel - 4); }
    // void JMP_REL32(Relative32 rel) { OP(0xE9); Emit(rel - 5); }
    // // Placeholder8 JMP_REL8() { JMP_REL8(0); return Placeholder8{code}; }
    // // Placeholder16 JMP_REL16() { JMP_REL16(0); return Placeholder16{code}; }
    // // Placeholder32 JMP_REL32() { JMP_REL32(0); return Placeholder32{code}; }

    // void RET_IMM16(Immediate16 i) { OP(0xC2); Emit16(i); }
    // void RET() { OP(0xC3); }

    // void ENTER(Immediate16 i1, Immediate8 i2) { OP(0xC8); Emit16(i1); Emit8(i2); }
    // void LEAVE() { OP(0xC9); }

    // void IRET() { OP(0xCF); }
    // void IRETD() { OP(0xCF); }
    // void IRETQ() { OP(0xCF); }

    // void NOT(code::Register& d) { Unary<0xF6, 0xF7>("NOT", d, Mod{0x02}); }
    // void NOT(code::Address& d) { Unary<0xF6, 0xF7>("NOT", d, Mod{0x02}); }

    // void NEG(code::Register& d) { Unary<0xF6, 0xF7>("NEG", d, Mod{0x03}); }
    // void NEG(code::Address& d) { Unary<0xF6, 0xF7>("NEG", d, Mod{0x03}); }

    // void INC(code::Register& d) { Unary<0xFE, 0xFF>("INC", d, Mod{0x00}); }
    // void INC(code::Address& d) { Unary<0xFE, 0xFF>("INC", d, Mod{0x00}); }

    // void DEC(code::Register& d) { Unary<0xFE, 0xFF>("DEC", d, Mod{0x01}); }
    // void DEC(code::Address& d) { Unary<0xFE, 0xFF>("DEC", d, Mod{0x01}); }

    // // 0xF6 NOT r/m8
    // void NOT_M8(IsMemory8 auto mem) { OP(0xF6); ModRM(SGPR8::DL, mem); }
    // void NOT_R8(IsGPR8 auto reg) { OP(0xF6); ModRM(SGPR8::DL, reg); }

    // // NOT r/m16
    // void NOT_M16(IsMemory16 auto mem) { OpSize(); OP(0xF7); ModRM(SGPR16::DX, mem); }
    // void NOT_R16(IsGPR16 auto reg) { OpSize(); OP(0xF7); ModRM(SGPR16::DX, reg); }

    // // NOT r/m32
    // void NOT_M32(IsMemory32 auto mem) { OP(0xF7); ModRM(SGPR32::EDX, mem); }
    // void NOT_R32(IsGPR32 auto reg) { OP(0xF7); ModRM(SGPR32::EDX, reg); }

    // // NOT r/m64
    // void NOT_M64(IsMemory64 auto mem) { REXW(); OP(0xF7); ModRM(SGPR64::RDX, mem); }
    // void NOT_R64(IsGPR64 auto reg) { REXW(); OP(0xF7); ModRM(SGPR64::RDX, reg); }

    // // NEG r/m8
    // void NEG_M8(IsMemory8 auto mem) { OP(0xF6); ModRM(SGPR8::BL, mem); }
    // void NEG_R8(IsGPR8 auto reg) { OP(0xF6); ModRM(SGPR8::BL, reg); }

    // // NEG r/m16
    // void NEG_M16(IsMemory16 auto mem) { OpSize(); OP(0xF7); ModRM(SGPR16::BX, mem); }
    // void NEG_R16(IsGPR16 auto reg) { OpSize(); OP(0xF7); ModRM(SGPR16::BX, reg); }

    // // NEG r/m32
    // void NEG_M32(IsMemory32 auto mem) { OP(0xF7); ModRM(SGPR32::EBX, mem); }
    // void NEG_R32(IsGPR32 auto reg) { OP(0xF7); ModRM(SGPR32::EBX, reg); }

    // // NEG r/m64
    // void NEG_M64(IsMemory64 auto mem) { REXW(); OP(0xF7); ModRM(SGPR64::RBX, mem); }
    // void NEG_R64(IsGPR64 auto reg) { REXW(); OP(0xF7); ModRM(SGPR64::RBX, reg); }

    // // MUL r/m8
    // void MUL_R8_M8(IsMemory8 auto mem) { OP(0xF6); ModRM(SGPR8::AH, mem); }
    // void MUL_R8(IsGPR8 auto reg) { OP(0xF6); ModRM(SGPR8::AH, reg); }

    // // MUL r/m16
    // void MUL_R16_M16(IsMemory16 auto mem) { OpSize(); OP(0xF7); ModRM(SGPR16::SI, mem); }
    // void MUL_R16(IsGPR16 auto reg) { OpSize(); OP(0xF7); ModRM(SGPR16::SI, reg); }

    // // MUL r/m32
    // void MUL_R32_M32(IsMemory32 auto mem) { OP(0xF7); ModRM(SGPR32::ESI, mem); }
    // void MUL_R32(IsGPR32 auto reg) { OP(0xF7); ModRM(SGPR32::ESI, reg); }

    // // MUL r/m64
    // void MUL_R64_M64(IsMemory64 auto mem) { REXW(); OP(0xF7); ModRM(SGPR64::RSI, mem); }
    // void MUL_R64(IsGPR64 auto reg) { REXW(); OP(0xF7); ModRM(SGPR64::RSI, reg); }

    // // IMUL r/m8
    // void IMUL_R8_M8(IsMemory8 auto mem) { OP(0xF6); ModRM(SGPR8::CH, mem); }
    // void IMUL_R8(IsGPR8 auto reg) { OP(0xF6); ModRM(SGPR8::CH, reg); }

    // // IMUL r/m16
    // void IMUL_R16_M16(IsMemory16 auto mem) { OpSize(); OP(0xF7); ModRM(SGPR16::DI, mem); }
    // void IMUL_R16(IsGPR16 auto reg) { OpSize(); OP(0xF7); ModRM(SGPR16::DI, reg); }

    // void IMUL_R32_M32(IsMemory32 auto mem) { OP(0xF7); ModRM(SGPR32::EDI, mem); }
    // void IMUL_R32(IsGPR32 auto reg) { OP(0xF7); ModRM(SGPR32::EDI, reg); }

    // void IMUL_R64_M64(IsMemory64 auto mem) { REXW(); OP(0xF7); ModRM(SGPR64::RDI, mem); }
    // void IMUL_R64(IsGPR64 auto reg) { REXW(); OP(0xF7); ModRM(SGPR64::RDI, reg); }

    // void DIV_R8_M8(IsMemory8 auto mem) { OP(0xF6); ModRM(SGPR8::DH, mem); }
    // void DIV_R8(IsGPR8 auto reg) { OP(0xF6); ModRM(SGPR8::DH, reg); }

    // void DIV_R16_M16(IsMemory16 auto mem) { OpSize(); OP(0xF7); ModRM(SGPR16::BX, mem); }
    // void DIV_R16(IsGPR16 auto reg) { OpSize(); OP(0xF7); ModRM(SGPR16::BX, reg); }

    // void DIV_R32_M32(IsMemory32 auto mem) { OP(0xF7); ModRM(SGPR32::EBX, mem); }
    // void DIV_R32(IsGPR32 auto reg) { OP(0xF7); ModRM(SGPR32::EBX, reg); }

    // void DIV_R64_M64(IsMemory64 auto mem) { REXW(); OP(0xF7); ModRM(SGPR64::RBX, mem); }
    // void DIV_R64(IsGPR64 auto reg) { REXW(); OP(0xF7); ModRM(SGPR64::RBX, reg); }

    // void IDIV_R8_M8(IsMemory8 auto mem) { OP(0xF6); ModRM(SGPR8::DH, mem); }
    // void IDIV_R8(IsGPR8 auto reg) { OP(0xF6); ModRM(SGPR8::DH, reg); }

    // void IDIV_R16_M16(IsMemory16 auto mem) { OpSize(); OP(0xF7); ModRM(SGPR16::BX, mem); }
    // void IDIV_R16(IsGPR16 auto reg) { OpSize(); OP(0xF7); ModRM(SGPR16::BX, reg); }

    // void IDIV_R32_M32(IsMemory32 auto mem) { OP(0xF7); ModRM(SGPR32::EBX, mem); }
    // void IDIV_R32(IsGPR32 auto reg) { OP(0xF7); ModRM(SGPR32::EBX, reg); }

    // void IDIV_R64_M64(IsMemory64 auto mem) { REX(mem); OP(0xF7); ModRM(SGPR64::RBX, mem); }
    // void IDIV_R64(IsGPR64 auto reg) { REX(reg); OP(0xF7); ModRM(SGPR64::RBX, reg); }

    // void INC_M8(IsMemory8 auto mem) { REX(mem); OP(0xFE); ModRM(0x00, mem); }
    // void INC_R8(IsGPR8 auto reg) { REX(reg); OP(0xFE); ModRM(0x00, reg); }

    // void INC_M16(IsMemory16 auto mem) { OpSize(); REX(mem); OP(0xFF); ModRM(0x00, mem); }
    // void INC_R16(IsGPR16 auto reg) { OpSize(); REX(reg); OP(0xFF); ModRM(0x00, reg); }

    // void INC_M32(IsMemory32 auto a) { REX(a); OP(0xFF); ModRM(0x00, a); }
    // void INC_R32(IsGPR32 auto a) { REX(a); OP(0xFF); ModRM(0x00, a); }

    // void INC_M64(IsMemory64 auto a) { REX(a); OP(0xFF); ModRM(0x00, a); }
    // void INC_R64(IsGPR64 auto a) { REX(a); OP(0xFF); ModRM(0x00, a); }

    // void DEC_R8(IsGPR8 auto a) { REX(a); OP(0xFE); ModRM(0x01, a); }
    // void DEC_M8(IsMemory8 auto a) { REX(a); OP(0xFE); ModRM(0x01, a); }

    // void DEC_R16(IsGPR16 auto a) { OpSize(); REX(a); OP(0xFF); ModRM(0x01, a); }
    // void DEC_M16(IsMemory16 auto a) { OpSize(); REX(a); OP(0xFF); ModRM(0x01, a); }

    // void DEC_R32(IsGPR32 auto a) { REX(a); OP(0xFF); ModRM(0x01, a); }
    // void DEC_M32(IsMemory32 auto a) { REX(a); OP(0xFF); ModRM(0x01, a); }

    // void DEC_R64(IsGPR64 auto a) { REX(a); OP(0xFF); ModRM(0x01, a); }
    // void DEC_M64(IsMemory64 auto a) { REX(a); OP(0xFF); ModRM(0x01, a); }

    // void CALL_R16(IsGPR16 auto r) { OpSize(); REX(r); OP(0xFF); ModRM(0x02, r); }
    // void CALL_R32(IsGPR32 auto r) { REX(r); OP(0xFF); ModRM(0x02, r); }
    // void CALL_R64(IsGPR64 auto r) { REX(r); OP(0xFF); ModRM(0x02, r); }
    // void CALL_M16(IsMemory16 auto m) { OpSize(); REX(m); OP(0xFF); ModRM(0x02, m); }
    // void CALL_M32(IsMemory32 auto m) { REX(m); OP(0xFF); ModRM(0x02, m); }
    // void CALL_M64(IsMemory64 auto m) { REX(m); OP(0xFF); ModRM(0x02, m); }

    // void JMP_R16(IsGPR16 auto r) { OpSize(); REX(r); OP(0xFF); ModRM(0x04, r); }
    // void JMP_R32(IsGPR32 auto r) { REX(r); OP(0xFF); ModRM(0x04, r); }
    // void JMP_R64(IsGPR64 auto r) { REX(r); OP(0xFF); ModRM(0x04, r); }
    // void JMP_M16(IsMemory16 auto m) { OpSize(); REX(m); OP(0xFF); ModRM(0x04, m); }
    // void JMP_M32(IsMemory32 auto m) { REX(m); OP(0xFF); ModRM(0x04, m); }
    // void JMP_M64(IsMemory64 auto m) { REX(m); OP(0xFF); ModRM(0x04, m); }

    // void PUSH_R16_ALT(IsGPR16 auto r) { OpSize(); REX(r); OP(0xFF); ModRM(0x06, r); }
    // void PUSH_R32(IsGPR32 auto r) { REX(r); OP(0xFF); ModRM(0x06, r); }
    // void PUSH_R64_ALT(IsGPR64 auto r) { REX(r); OP(0xFF); ModRM(0x06, r); }
    // void PUSH_M16(IsMemory16 auto m) { OpSize(); REX(m); OP(0xFF); ModRM(0x06, m); }
    // void PUSH_M32(IsMemory32 auto m) { REX(m); OP(0xFF); ModRM(0x06, m); }
    // void PUSH_M64(IsMemory64 auto m) { REX(m); OP(0xFF); ModRM(0x06, m); }

    // // BSWAP r32
    // void BSWAP_EAX() { Escape(0xC8); }
    // void BSWAP_ECX() { Escape(0xC9); }
    // void BSWAP_EDX() { Escape(0xCA); }
    // void BSWAP_EBX() { Escape(0xCB); }
    // void BSWAP_ESP() { Escape(0xCC); }
    // void BSWAP_EBP() { Escape(0xCD); }
    // void BSWAP_ESI() { Escape(0xCE); }
    // void BSWAP_EDI() { Escape(0xCF); }

    // // BSWAP r64
    // void BSWAP_RAX() { REXW(); Escape(0xC8); }
    // void BSWAP_RCX() { REXW(); Escape(0xC9); }
    // void BSWAP_RDX() { REXW(); Escape(0xCA); }
    // void BSWAP_RBX() { REXW(); Escape(0xCB); }
    // void BSWAP_RSP() { REXW(); Escape(0xCC); }
    // void BSWAP_RBP() { REXW(); Escape(0xCD); }
    // void BSWAP_RSI() { REXW(); Escape(0xCE); }
    // void BSWAP_RDI() { REXW(); Escape(0xCF); }

    // // EGPR BSWAP r64
    // void BSWAP_R8()  { REXWB(); Escape(0xC8); }
    // void BSWAP_R9()  { REXWB(); Escape(0xC9); }
    // void BSWAP_R10() { REXWB(); Escape(0xCA); }
    // void BSWAP_R11() { REXWB(); Escape(0xCB); }
    // void BSWAP_R12() { REXWB(); Escape(0xCC); }
    // void BSWAP_R13() { REXWB(); Escape(0xCD); }
    // void BSWAP_R14() { REXWB(); Escape(0xCE); }
    // void BSWAP_R15() { REXWB(); Escape(0xCF); }

    // void CLFLUSH_M8(IsMemory8 auto mem) { Escape(0xAE); ModRM(0x7, mem); }
    // void CLFLUSH_M16(IsMemory16 auto mem) { Escape(0xAE); ModRM(0x7, mem); }
    // void CLFLUSH_M32(IsMemory32 auto mem) { Escape(0xAE); ModRM(0x7, mem); }
    // void CLFLUSH_M64(IsMemory64 auto mem) { Escape(0xAE); ModRM(0x7, mem); }

    // // Opcode 0x0F 0x40: CMOVO r16/32/64, r/m16/32/64 (Conditional Move - overflow (OF=1))
    // void CMOVO(code::Register& d, code::Register& s) { BinaryEsc<0x40>("CMOVO", SAME_FROM_16, d, s); }
    // void CMOVO(code::Register& d, code::Address& s) { BinaryEsc<0x40>("CMOVO", SAME_FROM_16, d, s); }

    // // Opcode 0x0F 0x41: CMOVNO r16/32/64, r/m16/32/64 (Conditional Move - not overflow (OF=0))
    // void CMOVNO(code::Register& d, code::Register& s) { BinaryEsc<0x41>("CMOVNO", SAME_FROM_16, d, s); }
    // void CMOVNO(code::Register& d, code::Address& s) { BinaryEsc<0x41>("CMOVNO", SAME_FROM_16, d, s); }

    // // Opcode 0x0F 0x42: CMOVB r16/32/64, r/m16/32/64 (Conditional Move - below/not above or equal/carry (CF=1))
    // void CMOVB(code::Register& d, code::Address& s) { BinaryEsc<0x42>("CMOVB", SAME_FROM_16, d, s); }
    // void CMOVB(code::Register& d, code::Register& s) { BinaryEsc<0x42>("CMOVB", SAME_FROM_16, d, s); }

    // void CMOVNAE(auto&&... a) { CMOVB(std::forward<decltype(a)>(a)...); }
    // void CMOVC(auto&&... a) { CMOVB(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x43: CMOVNB r16/32/64, r/m16/32/64 (Conditional Move - not below/above or equal/not carry (CF=0))
    // void CMOVNB(code::Register& d, code::Address& s) { BinaryEsc<0x43>("CMOVNB", SAME_FROM_16, d, s); }
    // void CMOVNB(code::Register& d, code::Register& s) { BinaryEsc<0x43>("CMOVNB", SAME_FROM_16, d, s); }

    // void CMOVAE(auto&&... a) { CMOVNB(std::forward<decltype(a)>(a)...); }
    // void CMOVNC(auto&&... a) { CMOVNB(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x44: CMOVZ r16/32/64, r/m16/32/64 (Conditional Move - zero/equal (ZF=1))
    // void CMOVZ(code::Register& d, code::Address& s) { BinaryEsc<0x44>("CMOVZ", SAME_FROM_16, d, s); }
    // void CMOVZ(code::Register& d, code::Register& s) { BinaryEsc<0x44>("CMOVZ", SAME_FROM_16, d, s); }

    // void CMOVE(auto&&... a) { CMOVZ(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x45: CMOVNZ r16/32/64, r/m16/32/64 (Conditional Move - not zero/not equal (ZF=0))
    // void CMOVNZ(code::Register& d, code::Address& s) { BinaryEsc<0x45>("CMOVNZ", SAME_FROM_16, d, s); }
    // void CMOVNZ(code::Register& d, code::Register& s) { BinaryEsc<0x45>("CMOVNZ", SAME_FROM_16, d, s); }

    // void CMOVNE(auto&&... a) { CMOVNZ(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x46: CMOVBE r16/32/64, r/m16/32/64 (Conditional Move - below or equal/not above (CF=1 OR ZF=1))
    // void CMOVBE(code::Register& d, code::Address& s) { BinaryEsc<0x46>("CMOVBE", SAME_FROM_16, d, s); }
    // void CMOVBE(code::Register& d, code::Register& s) { BinaryEsc<0x46>("CMOVBE", SAME_FROM_16, d, s); }

    // void CMOVNA(auto&&... a) { CMOVBE(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x47: CMOVNBE r16/32/64, r/m16/32/64 (Conditional Move - not below or equal/above (CF=0 AND ZF=0))
    // void CMOVNBE(code::Register& d, code::Address& s) { BinaryEsc<0x47>("CMOVNBE", SAME_FROM_16, d, s); }
    // void CMOVNBE(code::Register& d, code::Register& s) { BinaryEsc<0x47>("CMOVNBE", SAME_FROM_16, d, s); }

    // void CMOVA(auto&&... a) { CMOVNBE(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x48: CMOVS r16/32/64, r/m16/32/64 (Conditional Move - sign (SF=1))
    // void CMOVS(code::Register& d, code::Address& s) { BinaryEsc<0x48>("CMOVS", SAME_FROM_16, d, s); }
    // void CMOVS(code::Register& d, code::Register& s) { BinaryEsc<0x48>("CMOVS", SAME_FROM_16, d, s); }

    // // Opcode 0x0F 0x49: CMOVNS r16/32/64, r/m16/32/64 (Conditional Move - not sign (SF=0))
    // void CMOVNS(code::Register& d, code::Address& s) { BinaryEsc<0x49>("CMOVNS", SAME_FROM_16, d, s); }
    // void CMOVNS(code::Register& d, code::Register& s) { BinaryEsc<0x49>("CMOVNS", SAME_FROM_16, d, s); }

    // // Opcode 0x0F 0x4A: CMOVP r16/32/64, r/m16/32/64 (Conditional Move - parity/parity even (PF=1))
    // void CMOVP(code::Register& d, code::Address& s) { BinaryEsc<0x4A>("CMOVP", SAME_FROM_16, d, s); }
    // void CMOVP(code::Register& d, code::Register& s) { BinaryEsc<0x4A>("CMOVP", SAME_FROM_16, d, s); }

    // void CMOVPE(auto&&... a) { CMOVP(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x4B: CMOVNP r16/32/64, r/m16/32/64 (Conditional Move - not parity/parity odd (PF=0))
    // void CMOVNP(code::Register& d, code::Address& s) { BinaryEsc<0x4B>("CMOVNP", SAME_FROM_16, d, s); }
    // void CMOVNP(code::Register& d, code::Register& s) { BinaryEsc<0x4B>("CMOVNP", SAME_FROM_16, d, s); }

    // void CMOVPO(auto&&... a) { CMOVNP(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x4C: CMOVL r16/32/64, r/m16/32/64 (Conditional Move - less/not greater (SF!=OF))
    // void CMOVL(code::Register& d, code::Address& s) { BinaryEsc<0x4C>("CMOVL", SAME_FROM_16, d, s); }
    // void CMOVL(code::Register& d, code::Register& s) { BinaryEsc<0x4C>("CMOVL", SAME_FROM_16, d, s); }

    // void CMOVNGE(auto&&... a) { CMOVL(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x4D: CMOVNL r16/32/64, r/m16/32/64 (Conditional Move - not less/greater or equal (SF=OF))
    // void CMOVNL(code::Register& d, code::Address& s) { BinaryEsc<0x4D>("CMOVNL", SAME_FROM_16, d, s); }
    // void CMOVNL(code::Register& d, code::Register& s) { BinaryEsc<0x4D>("CMOVNL", SAME_FROM_16, d, s); }

    // void CMOVGE(auto&&... a) { CMOVNL(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x4E: CMOVLE r16/32/64, r/m16/32/64 (Conditional Move - less or equal/not greater ((ZF=1) OR (SF!=OF)))
    // void CMOVLE(code::Register& d, code::Address& s) { BinaryEsc<0x4E>("CMOVLE", SAME_FROM_16, d, s); }
    // void CMOVLE(code::Register& d, code::Register& s) { BinaryEsc<0x4E>("CMOVLE", SAME_FROM_16, d, s); }

    // void CMOVNG(auto&&... a) { CMOVLE(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x4F: CMOVNLE r16/32/64, r/m16/32/64 (Conditional Move - not less nor equal/greater ((ZF=0) AND (SF=OF)))
    // void CMOVNLE(code::Register& d, code::Address& s) { BinaryEsc<0x4F>("CMOVNLE", SAME_FROM_16, d, s); }
    // void CMOVNLE(code::Register& d, code::Register& s) { BinaryEsc<0x4F>("CMOVNLE", SAME_FROM_16, d, s); }

    // void CMOVG(auto&&... a) { CMOVNLE(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x90: SETO r/m8 (Set byte if overflow (OF=1))
    // void SETO(code::Register& d) { UnaryEsc<0x90>("SETO", d, Mod{0x00}); }
    // void SETO(code::Address& d) { UnaryEsc<0x90>("SETO", d, Mod{0x00}); }

    // // Opcode 0x0F 0x91: SETNO r/m8 (Set byte if not overflow (OF=0))
    // void SETNO(code::Register& d) { UnaryEsc<0x91>("SETNO", d, Mod{0x00}); }
    // void SETNO(code::Address& d) { UnaryEsc<0x91>("SETNO", d, Mod{0x00}); }

    // // Opcode 0x0F 0x92: SETB r/m8 (Set byte if below/not above or equal/carry (CF=1))
    // void SETB(code::Register& d) { UnaryEsc<0x92>("SETB", d, Mod{0x00}); }
    // void SETB(code::Address& d) { UnaryEsc<0x92>("SETB", d, Mod{0x00}); }

    // void SETNAE(auto&&... a) { SETB(std::forward<decltype(a)>(a)...); }
    // void SETC(auto&&... a) { SETB(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x93: SETNB r/m8 (Set byte if not below/above or equal/not carry (CF=0))
    // void SETNB(code::Register& d) { UnaryEsc<0x93>("SETNB", d, Mod{0x00}); }
    // void SETNB(code::Address& d) { UnaryEsc<0x93>("SETNB", d, Mod{0x00}); }

    // void SETAE(auto&&... a) { SETNB(std::forward<decltype(a)>(a)...); }
    // void SETNC(auto&&... a) { SETNB(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x94: SETZ r/m8 (Set byte if zero/equal (ZF=1))
    // void SETZ(code::Register& d) { UnaryEsc<0x94>("SETZ", d, Mod{0x00}); }
    // void SETZ(code::Address& d) { UnaryEsc<0x94>("SETZ", d, Mod{0x00}); }

    // void SETE(auto&&... a) { SETZ(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x95: SETNZ r/m8 (Set byte if not zero/not equal (ZF=0))
    // void SETNZ(code::Register& d) { UnaryEsc<0x95>("SETNZ", d, Mod{0x00}); }
    // void SETNZ(code::Address& d) { UnaryEsc<0x95>("SETNZ", d, Mod{0x00}); }

    // void SETNE(auto&&... a) { SETNZ(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x96: SETBE r/m8 (Set byte if below or equal/not above (CF=1 OR ZF=1))
    // void SETBE(code::Register& d) { UnaryEsc<0x96>("SETBE", d, Mod{0x00}); }
    // void SETBE(code::Address& d) { UnaryEsc<0x96>("SETBE", d, Mod{0x00}); }

    // void SETNA(auto&&... a) { SETBE(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x97: SETNBE r/m8 (Set byte if not below or equal/above (CF=0 AND ZF=0))
    // void SETNBE(code::Register& d) { UnaryEsc<0x97>("SETNBE", d, Mod{0x00}); }
    // void SETNBE(code::Address& d) { UnaryEsc<0x97>("SETNBE", d, Mod{0x00}); }

    // void SETA(auto&&... a) { SETNBE(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x98: SETS r/m8 (Set byte if sign (SF=1))
    // void SETS(code::Register& d) { UnaryEsc<0x98>("SETS", d, Mod{0x00}); }
    // void SETS(code::Address& d) { UnaryEsc<0x98>("SETS", d, Mod{0x00}); }

    // // Opcode 0x0F 0x99: SETNS r/m8 (Set byte if not sign (SF=0))
    // void SETNS(code::Register& d) { UnaryEsc<0x99>("SETNS", d, Mod{0x00}); }
    // void SETNS(code::Address& d) { UnaryEsc<0x99>("SETNS", d, Mod{0x00}); }

    // // Opcode 0x0F 0x9A: SETP r/m8 (Set byte if parity/parity even (PF=1))
    // void SETP(code::Register& d) { UnaryEsc<0x9A>("SETP", d, Mod{0x00}); }
    // void SETP(code::Address& d) { UnaryEsc<0x9A>("SETP", d, Mod{0x00}); }

    // void SETPE(auto&&... a) { SETP(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x9B: SETNP r/m8 (Set byte if not parity/parity odd (PF=0))
    // void SETNP(code::Register& d) { UnaryEsc<0x9B>("SETNP", d, Mod{0x00}); }
    // void SETNP(code::Address& d) { UnaryEsc<0x9B>("SETNP", d, Mod{0x00}); }

    // void SETPO(auto&&... a) { SETNP(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x9C: SETL r/m8 (Set byte if less/not greater (SF!=OF))
    // void SETL(code::Register& d) { UnaryEsc<0x9C>("SETL", d, Mod{0x00}); }
    // void SETL(code::Address& d) { UnaryEsc<0x9C>("SETL", d, Mod{0x00}); }

    // void SETNGE(auto&&... a) { SETL(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x9D: SETNL r/m8 (Set byte if not less/greater or equal (SF=OF))
    // void SETNL(code::Register& d) { UnaryEsc<0x9D>("SETNL", d, Mod{0x00}); }
    // void SETNL(code::Address& d) { UnaryEsc<0x9D>("SETNL", d, Mod{0x00}); }

    // void SETGE(auto&&... a) { SETNL(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x9E: SETLE r/m8 (Set byte if less or equal/not greater ((ZF=1) OR (SF!=OF)))
    // void SETLE(code::Register& d) { UnaryEsc<0x9E>("SETLE", d, Mod{0x00}); }
    // void SETLE(code::Address& d) { UnaryEsc<0x9E>("SETLE", d, Mod{0x00}); }

    // void SETNG(auto&&... a) { SETLE(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x9F: SETNLE r/m8 (Set byte if not less nor equal/greater ((ZF=0) AND (SF=OF)))
    // void SETNLE(code::Register& d) { UnaryEsc<0x9F>("SETNLE", d, Mod{0x00}); }
    // void SETNLE(code::Address& d) { UnaryEsc<0x9F>("SETNLE", d, Mod{0x00}); }

    // void SETG(auto&&... a) { SETNLE(std::forward<decltype(a)>(a)...); }

    // // Opcode 0x0F 0x40: CMOVO r16/32/64, r/m16/32/64 (Conditional Move - overflow (OF=1))
    // void CMOVO_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x40); ModRM(d, s); }
    // void CMOVO_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x40); ModRM(d, s); }
    // void CMOVO_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x40); ModRM(d, s); }

    // void CMOVO_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x40); ModRM(d, s); }
    // void CMOVO_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x40); ModRM(d, s); }
    // void CMOVO_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x40); ModRM(d, s); }

    // // Opcode 0x0F 0x41: CMOVNO r16/32/64, r/m16/32/64 (Conditional Move - not overflow (OF=0))
    // void CMOVNO_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x41); ModRM(d, s); }
    // void CMOVNO_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x41); ModRM(d, s); }
    // void CMOVNO_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x41); ModRM(d, s); }

    // void CMOVNO_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x41); ModRM(d, s); }
    // void CMOVNO_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x41); ModRM(d, s); }
    // void CMOVNO_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x41); ModRM(d, s); }

    // // Opcode 0x0F 0x42: CMOVB r16/32/64, r/m16/32/64 (Conditional Move - below/not above or equal/carry (CF=1))
    // void CMOVB_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x42); ModRM(d, s); }
    // void CMOVB_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x42); ModRM(d, s); }
    // void CMOVB_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x42); ModRM(d, s); }

    // void CMOVB_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x42); ModRM(d, s); }
    // void CMOVB_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x42); ModRM(d, s); }
    // void CMOVB_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x42); ModRM(d, s); }

    // // Opcode 0x0F 0x43: CMOVNB r16/32/64, r/m16/32/64 (Conditional Move - not below/above or equal/not carry (CF=0))
    // void CMOVNB_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x43); ModRM(d, s); }
    // void CMOVNB_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x43); ModRM(d, s); }
    // void CMOVNB_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x43); ModRM(d, s); }

    // void CMOVNB_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x43); ModRM(d, s); }
    // void CMOVNB_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x43); ModRM(d, s); }
    // void CMOVNB_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x43); ModRM(d, s); }

    // // Opcode 0x0F 0x44: CMOVZ r16/32/64, r/m16/32/64 (Conditional Move - zero/equal (ZF=1))
    // void CMOVZ_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x44); ModRM(d, s); }
    // void CMOVZ_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x44); ModRM(d, s); }
    // void CMOVZ_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x44); ModRM(d, s); }

    // void CMOVZ_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x44); ModRM(d, s); }
    // void CMOVZ_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x44); ModRM(d, s); }
    // void CMOVZ_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x44); ModRM(d, s); }

    // // Opcode 0x0F 0x45: CMOVNZ r16/32/64, r/m16/32/64 (Conditional Move - not zero/not equal (ZF=0))
    // void CMOVNZ_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x45); ModRM(d, s); }
    // void CMOVNZ_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x45); ModRM(d, s); }
    // void CMOVNZ_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x45); ModRM(d, s); }

    // void CMOVNZ_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x45); ModRM(d, s); }
    // void CMOVNZ_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x45); ModRM(d, s); }
    // void CMOVNZ_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x45); ModRM(d, s); }

    // // Opcode 0x0F 0x46: CMOVBE r16/32/64, r/m16/32/64 (Conditional Move - below or equal/not above (CF=1 OR ZF=1))
    // void CMOVBE_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x46); ModRM(d, s); }
    // void CMOVBE_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x46); ModRM(d, s); }
    // void CMOVBE_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x46); ModRM(d, s); }

    // void CMOVBE_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x46); ModRM(d, s); }
    // void CMOVBE_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x46); ModRM(d, s); }
    // void CMOVBE_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x46); ModRM(d, s); }

    // // Opcode 0x0F 0x47: CMOVNBE r16/32/64, r/m16/32/64 (Conditional Move - not below or equal/above (CF=0 AND ZF=0))
    // void CMOVNBE_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x47); ModRM(d, s); }
    // void CMOVNBE_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x47); ModRM(d, s); }
    // void CMOVNBE_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x47); ModRM(d, s); }

    // void CMOVNBE_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x47); ModRM(d, s); }
    // void CMOVNBE_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x47); ModRM(d, s); }
    // void CMOVNBE_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x47); ModRM(d, s); }

    // // Opcode 0x0F 0x48: CMOVS r16/32/64, r/m16/32/64 (Conditional Move - sign (SF=1))
    // void CMOVS_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x48); ModRM(d, s); }
    // void CMOVS_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x48); ModRM(d, s); }
    // void CMOVS_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x48); ModRM(d, s); }

    // void CMOVS_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x48); ModRM(d, s); }
    // void CMOVS_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x48); ModRM(d, s); }
    // void CMOVS_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x48); ModRM(d, s); }

    // // Opcode 0x0F 0x49: CMOVNS r16/32/64, r/m16/32/64 (Conditional Move - not sign (SF=0))
    // void CMOVNS_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x49); ModRM(d, s); }
    // void CMOVNS_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x49); ModRM(d, s); }
    // void CMOVNS_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x49); ModRM(d, s); }

    // void CMOVNS_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x49); ModRM(d, s); }
    // void CMOVNS_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x49); ModRM(d, s); }
    // void CMOVNS_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x49); ModRM(d, s); }

    // // Opcode 0x0F 0x4A: CMOVP r16/32/64, r/m16/32/64 (Conditional Move - parity/parity even (PF=1))
    // void CMOVP_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x4A); ModRM(d, s); }
    // void CMOVP_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x4A); ModRM(d, s); }
    // void CMOVP_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x4A); ModRM(d, s); }

    // void CMOVP_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x4A); ModRM(d, s); }
    // void CMOVP_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x4A); ModRM(d, s); }
    // void CMOVP_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x4A); ModRM(d, s); }

    // // Opcode 0x0F 0x4B: CMOVNP r16/32/64, r/m16/32/64 (Conditional Move - not parity/parity odd (PF=0))
    // void CMOVNP_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x4B); ModRM(d, s); }
    // void CMOVNP_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x4B); ModRM(d, s); }
    // void CMOVNP_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x4B); ModRM(d, s); }

    // void CMOVNP_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x4B); ModRM(d, s); }
    // void CMOVNP_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x4B); ModRM(d, s); }
    // void CMOVNP_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x4B); ModRM(d, s); }

    // // Opcode 0x0F 0x4C: CMOVL r16/32/64, r/m16/32/64 (Conditional Move - less/not greater (SF!=OF))
    // void CMOVL_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x4C); ModRM(d, s); }
    // void CMOVL_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x4C); ModRM(d, s); }
    // void CMOVL_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x4C); ModRM(d, s); }

    // void CMOVL_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x4C); ModRM(d, s); }
    // void CMOVL_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x4C); ModRM(d, s); }
    // void CMOVL_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x4C); ModRM(d, s); }

    // // Opcode 0x0F 0x4D: CMOVNL r16/32/64, r/m16/32/64 (Conditional Move - not less/greater or equal (SF=OF))
    // void CMOVNL_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x4D); ModRM(d, s); }
    // void CMOVNL_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x4D); ModRM(d, s); }
    // void CMOVNL_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x4D); ModRM(d, s); }

    // void CMOVNL_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x4D); ModRM(d, s); }
    // void CMOVNL_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x4D); ModRM(d, s); }
    // void CMOVNL_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x4D); ModRM(d, s); }

    // // Opcode 0x0F 0x4E: CMOVLE r16/32/64, r/m16/32/64 (Conditional Move - less or equal/not greater ((ZF=1) OR (SF!=OF)))
    // void CMOVLE_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x4E); ModRM(d, s); }
    // void CMOVLE_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x4E); ModRM(d, s); }
    // void CMOVLE_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x4E); ModRM(d, s); }

    // void CMOVLE_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x4E); ModRM(d, s); }
    // void CMOVLE_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x4E); ModRM(d, s); }
    // void CMOVLE_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x4E); ModRM(d, s); }

    // // Opcode 0x0F 0x4F: CMOVNLE r16/32/64, r/m16/32/64 (Conditional Move - not less nor equal/greater ((ZF=0) AND (SF=OF)))
    // void CMOVNLE_R16_R16(IsGPR16 auto d, IsGPR16 auto s) { OpSize(); REX(d, s); Escape(0x4F); ModRM(d, s); }
    // void CMOVNLE_R32_R32(IsGPR32 auto d, IsGPR32 auto s) { REX(d, s); Escape(0x4F); ModRM(d, s); }
    // void CMOVNLE_R64_R64(IsGPR64 auto d, IsGPR64 auto s) { REX(d, s); Escape(0x4F); ModRM(d, s); }

    // void CMOVNLE_R16_M16(IsGPR16 auto d, IsMemory16 auto s) { OpSize(); REX(d, s); Escape(0x4F); ModRM(d, s); }
    // void CMOVNLE_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { REX(d, s); Escape(0x4F); ModRM(d, s); }
    // void CMOVNLE_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { REX(d, s); Escape(0x4F); ModRM(d, s); }

    // // Opcode 0x0F 0x50: MOVMSKPS r32/64, xmm
    // void MOVMSKPS(IsGPR32 auto d, IsXMM auto s) { REX(d, s); Escape(0x50); ModRM(d, s); }
    // void MOVMSKPS(IsGPR64 auto d, IsXMM auto s) { REX(d, s); Escape(0x50); ModRM(d, s); }

    // // Opcode 0x0F 0x51: SQRTPS xmm, xmm/m128
    // void SQRTPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x51); ModRM(d, s); }
    // void SQRTPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x51); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x51: SQRTSS xmm, xmm/m32
    // void SQRTSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x51); ModRM(d, s); }
    // void SQRTSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x51); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x51: SQRTPD xmm, xmm/m128
    // void SQRTPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x51); ModRM(d, s); }
    // void SQRTPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x51); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x51: SQRTSD xmm, xmm/m64
    // void SQRTSD(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x51); ModRM(d, s); }
    // void SQRTSD(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x51); ModRM(d, s); }

    // // Opcode 0x0F 0x52: RSQRTPS xmm, xmm/m128
    // void RSQRTPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x52); ModRM(d, s); }
    // void RSQRTPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x52); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x52: RSQRTSS xmm, xmm/m32
    // void RSQRTSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x52); ModRM(d, s); }
    // void RSQRTSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x52); ModRM(d, s); }

    // // Opcode 0x0F 0x53: RCPPS xmm, xmm/m128
    // void RCPPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x53); ModRM(d, s); }
    // void RCPPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x53); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x53: RCPSS xmm, xmm/m32
    // void RCPSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x53); ModRM(d, s); }
    // void RCPSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x53); ModRM(d, s); }

    // // Opcode 0x0F 0x54: ANDPS xmm, xmm/m128
    // void ANDPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x54); ModRM(d, s); }
    // void ANDPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x54); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x54: ANDPD xmm, xmm/m128
    // void ANDPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x54); ModRM(d, s); }
    // void ANDPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x54); ModRM(d, s); }

    // // Opcode 0x0F 0x55: ANDNPS xmm, xmm/m128
    // void ANDNPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x55); ModRM(d, s); }
    // void ANDNPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x55); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x55: ANDNPD xmm, xmm/m128
    // void ANDNPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x55); ModRM(d, s); }
    // void ANDNPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x55); ModRM(d, s); }

    // // Opcode 0x0F 0x56: ORPS xmm, xmm/m128
    // void ORPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x56); ModRM(d, s); }
    // void ORPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x56); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x56: ORPD xmm, xmm/m128
    // void ORPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x56); ModRM(d, s); }
    // void ORPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x56); ModRM(d, s); }

    // // Opcode 0x0F 0x57: XORPS xmm, xmm/m128
    // void XORPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x57); ModRM(d, s); }
    // void XORPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x57); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x57: XORPD xmm, xmm/m128
    // void XORPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x57); ModRM(d, s); }
    // void XORPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x57); ModRM(d, s); }

    // // Opcode 0x0F 0x58: ADDPS xmm, xmm/m128
    // void ADDPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x58); ModRM(d, s); }
    // void ADDPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x58); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x58: ADDSS xmm, xmm/m32
    // void ADDSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x58); ModRM(d, s); }
    // void ADDSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x58); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x58: ADDPD xmm, xmm/m128
    // void ADDPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x58); ModRM(d, s); }
    // void ADDPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x58); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x58: ADDSD xmm, xmm/m64
    // void ADDSD(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x58); ModRM(d, s); }
    // void ADDSD(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x58); ModRM(d, s); }

    // // Opcode 0x0F 0x59: MULPS xmm, xmm/m128
    // void MULPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x59); ModRM(d, s); }
    // void MULPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x59); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x59: MULSS xmm, xmm/m32
    // void MULSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x59); ModRM(d, s); }
    // void MULSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x59); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x59: MULPD xmm, xmm/m128
    // void MULPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x59); ModRM(d, s); }
    // void MULPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x59); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x59: MULSD xmm, xmm/m64
    // void MULSD(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x59); ModRM(d, s); }
    // void MULSD(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x59); ModRM(d, s); }

    // // Opcode 0x0F 0x5A: CVTPS2PD xmm, xmm/m64
    // void CVTPS2PD(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x5A); ModRM(d, s); }
    // void CVTPS2PD(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x5A); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x5A: CVTPD2PS xmm, xmm/m128
    // void CVTPD2PS(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x5A); ModRM(d, s); }
    // void CVTPD2PS(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x5A); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x5A: CVTSS2SD xmm, xmm/m32
    // void CVTSS2SD(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x5A); ModRM(d, s); }
    // void CVTSS2SD(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x5A); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x5A: CVTSD2SS xmm, xmm/m64
    // void CVTSD2SS(const code::Register& d, const code::Register& s) { Double(); REX(d, s); Escape(0x5A); ModRM(d, s); }
    // void CVTSD2SS(const code::Register& d, const code::Address& s) { Double(); REX(d, s); Escape(0x5A); ModRM(d, s); }

    // void CVTSD2SS(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x5A); ModRM(d, s); }
    // void CVTSD2SS(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x5A); ModRM(d, s); }

    // // Opcode 0x0F 0x5B: CVTDQ2PS xmm, xmm/m128
    // void CVTDQ2PS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x5B); ModRM(d, s); }
    // void CVTDQ2PS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x5B); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x5B: CVTPS2DQ xmm, xmm/m128
    // void CVTPS2DQ(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x5B); ModRM(d, s); }
    // void CVTPS2DQ(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x5B); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x5B: CVTTPS2DQ xmm, xmm/m128
    // void CVTTPS2DQ(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x5B); ModRM(d, s); }
    // void CVTTPS2DQ(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x5B); ModRM(d, s); }

    // // Opcode 0x0F 0x5C: SUBPS xmm, xmm/m128
    // void SUBPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x5C); ModRM(d, s); }
    // void SUBPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x5C); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x5C: SUBSS xmm, xmm/m32
    // void SUBSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x5C); ModRM(d, s); }
    // void SUBSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x5C); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x5C: SUBPD xmm, xmm/m128
    // void SUBPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x5C); ModRM(d, s); }
    // void SUBPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x5C); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x5C: SUBSD xmm, xmm/m64
    // void SUBSD(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x5C); ModRM(d, s); }
    // void SUBSD(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x5C); ModRM(d, s); }

    // // Opcode 0x0F 0x5D: MINPS xmm, xmm/m128
    // void MINPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x5D); ModRM(d, s); }
    // void MINPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x5D); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x5D: MINSS xmm, xmm/m32
    // void MINSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x5D); ModRM(d, s); }
    // void MINSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x5D); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x5D: MINPD xmm, xmm/m128
    // void MINPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x5D); ModRM(d, s); }
    // void MINPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x5D); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x5D: MINSD xmm, xmm/m64
    // void MINSD(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x5D); ModRM(d, s); }
    // void MINSD(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x5D); ModRM(d, s); }

    // // Opcode 0x0F 0x5E: DIVPS xmm, xmm/m128
    // void DIVPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x5E); ModRM(d, s); }
    // void DIVPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x5E); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x5E: DIVSS xmm, xmm/m32
    // void DIVSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x5E); ModRM(d, s); }
    // void DIVSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x5E); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x5E: DIVPD xmm, xmm/m128
    // void DIVPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x5E); ModRM(d, s); }
    // void DIVPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x5E); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x5E: DIVSD xmm, xmm/m64
    // void DIVSD(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x5E); ModRM(d, s); }
    // void DIVSD(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x5E); ModRM(d, s); }

    // // Opcode 0x0F 0x5F: MAXPS xmm, xmm/m128
    // void MAXPS(IsXMM auto d, IsXMM auto s) { REX(d, s); Escape(0x5F); ModRM(d, s); }
    // void MAXPS(IsXMM auto d, IsMemory128 auto s) { REX(d, s); Escape(0x5F); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x5F: MAXSS xmm, xmm/m32
    // void MAXSS(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x5F); ModRM(d, s); }
    // void MAXSS(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x5F); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x5F: MAXPD xmm, xmm/m128
    // void MAXPD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x5F); ModRM(d, s); }
    // void MAXPD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x5F); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x5F: MAXSD xmm, xmm/m64
    // void MAXSD(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x5F); ModRM(d, s); }
    // void MAXSD(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x5F); ModRM(d, s); }

    // // Opcode 0x0F 0x60: PUNPCKLBW xmm, xmm/m128
    // void PUNPCKLBW(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x60); ModRM(d, s); }
    // void PUNPCKLBW(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x60); ModRM(d, s); }

    // // Opcode 0x0F 0x61: PUNPCKLWD xmm, xmm/m128
    // void PUNPCKLWD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x61); ModRM(d, s); }
    // void PUNPCKLWD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x61); ModRM(d, s); }

    // // Opcode 0x0F 0x62: PUNPCKLDQ xmm, xmm/m128
    // void PUNPCKLDQ(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x62); ModRM(d, s); }
    // void PUNPCKLDQ(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x62); ModRM(d, s); }

    // // Opcode 0x0F 0x63: PACKSSWB xmm, xmm/m128
    // void PACKSSWB(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x63); ModRM(d, s); }
    // void PACKSSWB(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x63); ModRM(d, s); }

    // // Opcode 0x0F 0x64: PCMPGTB xmm, xmm/m128
    // void PCMPGTB(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x64); ModRM(d, s); }
    // void PCMPGTB(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x64); ModRM(d, s); }

    // // Opcode 0x0F 0x65: PCMPGTW xmm, xmm/m128
    // void PCMPGTW(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x65); ModRM(d, s); }
    // void PCMPGTW(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x65); ModRM(d, s); }

    // // Opcode 0x0F 0x66: PCMPGTD xmm, xmm/m128
    // void PCMPGTD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x66); ModRM(d, s); }
    // void PCMPGTD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x66); ModRM(d, s); }

    // // Opcode 0x0F 0x67: PACKUSWB xmm, xmm/m128
    // void PACKUSWB(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x67); ModRM(d, s); }
    // void PACKUSWB(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x67); ModRM(d, s); }

    // // Opcode 0x0F 0x68: PUNPCKHBW xmm, xmm/m128
    // void PUNPCKHBW(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x68); ModRM(d, s); }
    // void PUNPCKHBW(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x68); ModRM(d, s); }

    // // Opcode 0x0F 0x69: PUNPCKHWD xmm, xmm/m128
    // void PUNPCKHWD(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x69); ModRM(d, s); }
    // void PUNPCKHWD(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x69); ModRM(d, s); }

    // // Opcode 0x0F 0x6A: PUNPCKHDQ xmm, xmm/m128
    // void PUNPCKHDQ(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x6A); ModRM(d, s); }
    // void PUNPCKHDQ(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x6A); ModRM(d, s); }

    // // Opcode 0x0F 0x6B: PACKSSDW xmm, xmm/m128
    // void PACKSSDW(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x6B); ModRM(d, s); }
    // void PACKSSDW(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x6B); ModRM(d, s); }

    // // Opcode 0x0F 0x6C: PUNPCKLQDQ xmm, xmm/m128
    // void PUNPCKLQDQ(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x6C); ModRM(d, s); }
    // void PUNPCKLQDQ(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x6C); ModRM(d, s); }

    // // Opcode 0x0F 0x6D: PUNPCKHQDQ xmm, xmm/m128
    // void PUNPCKHQDQ(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x6D); ModRM(d, s); }
    // void PUNPCKHQDQ(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x6D); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x6F: MOVDQA xmm, xmm/m128
    // void MOVDQA(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x6F); ModRM(d, s); }
    // void MOVDQA(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x6F); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x6F: MOVDQU xmm, xmm/m128
    // void MOVDQU(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x6F); ModRM(d, s); }
    // void MOVDQU(IsXMM auto d, IsMemory128 auto s) { Single(); REX(d, s); Escape(0x6F); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x70: PSHUFLW xmm, xmm/m128, imm8
    // void PSHUFLW(IsXMM auto d, IsXMM auto s, Immediate8 i) { Double(); REX(d, s); Escape(0x70); ModRM(d, s); Emit8(i); }
    // void PSHUFLW(IsXMM auto d, IsMemory128 auto s, Immediate8 i) { Double(); REX(d, s); Escape(0x70); ModRM(d, s); Emit8(i); }

    // // Opcode 0x66 0x0F 0x70: PSHUFHW xmm, xmm/m128, imm8
    // void PSHUFHW(IsXMM auto d, IsXMM auto s, Immediate8 i) { OpSize(); REX(d, s); Escape(0x70); ModRM(d, s); Emit8(i); }
    // void PSHUFHW(IsXMM auto d, IsMemory128 auto s, Immediate8 i) { OpSize(); REX(d, s); Escape(0x70); ModRM(d, s); Emit8(i); }

    // // Opcode 0x0F 0x70: PSHUFD xmm, xmm/m128, imm8
    // void PSHUFD(IsXMM auto d, IsXMM auto s, Immediate8 i) { OpSize(); REX(d, s); Escape(0x70); ModRM(d, s); Emit8(i); }
    // void PSHUFD(IsXMM auto d, IsMemory128 auto s, Immediate8 i) { OpSize(); REX(d, s); Escape(0x70); ModRM(d, s); Emit8(i); }

    // // Opcode 0x0F 0x71: PSRLW xmm/m128, imm8
    // void PSRLW(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x71); ModRM(2, d); Emit8(i); }
    // void PSRLW(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x71); ModRM(2, s); Emit8(i); }

    // // Opcode 0x0F 0x71: PSRAW xmm/m128, imm8
    // void PSRAW(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x71); ModRM(4, d); Emit8(i); }
    // void PSRAW(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x71); ModRM(4, s); Emit8(i); }

    // // Opcode 0x0F 0x71: PSLLW xmm/m128, imm8
    // void PSLLW(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x71); ModRM(6, d); Emit8(i); }
    // void PSLLW(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x71); ModRM(6, s); Emit8(i); }

    // // Opcode 0x0F 0x72: PSRLD xmm/m128, imm8
    // void PSRLD(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x72); ModRM(2, d); Emit8(i); }
    // void PSRLD(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x72); ModRM(2, s); Emit8(i); }

    // // Opcode 0x0F 0x72: PSRAD xmm/m128, imm8
    // void PSRAD(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x72); ModRM(4, d); Emit8(i); }
    // void PSRAD(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x72); ModRM(4, s); Emit8(i); }

    // // Opcode 0x0F 0x72: PSLLD xmm/m128, imm8
    // void PSLLD(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x72); ModRM(6, d); Emit8(i); }
    // void PSLLD(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x72); ModRM(6, s); Emit8(i); }

    // // Opcode 0x0F 0x73: PSRLQ xmm/m128, imm8
    // void PSRLQ(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x73); ModRM(2, d); Emit8(i); }
    // void PSRLQ(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x73); ModRM(2, s); Emit8(i); }

    // // Opcode 0x0F 0x73: PSRLDQ xmm/m128, imm8
    // void PSRLDQ(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x73); ModRM(3, d); Emit8(i); }
    // void PSRLDQ(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x73); ModRM(3, s); Emit8(i); }

    // // Opcode 0x0F 0x73: PSLLQ xmm/m128, imm8
    // void PSLLQ(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x73); ModRM(6, d); Emit8(i); }
    // void PSLLQ(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x73); ModRM(6, s); Emit8(i); }

    // // Opcode 0x0F 0x73: PSLLDQ xmm/m128, imm8
    // void PSLLDQ(IsXMM auto d, Immediate8 i) { OpSize(); REX(d); Escape(0x73); ModRM(7, d); Emit8(i); }
    // void PSLLDQ(IsMemory128 auto s, Immediate8 i) { OpSize(); REX(s); Escape(0x73); ModRM(7, s); Emit8(i); }

    // // Opcode 0x0F 0x74: PCMPEQB xmm, xmm/m128
    // void PCMPEQB_XMM_XMM(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x74); ModRM(d, s); }
    // void PCMPEQB_XMM_M128(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x74); ModRM(d, s); }

    // // Opcode 0x0F 0x75: PCMPEQW xmm, xmm/m128
    // void PCMPEQW_XMM_XMM(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x75); ModRM(d, s); }
    // void PCMPEQW_XMM_M128(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x75); ModRM(d, s); }

    // // Opcode 0x0F 0x76: PCMPEQD xmm, xmm/m128
    // void PCMPEQD_XMM_XMM(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x76); ModRM(d, s); }
    // void PCMPEQD_XMM_M128(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x76); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x7C: HADDPD xmm, xmm/m128
    // void HADDPD_XMM_XMM(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x7C); ModRM(d, s); }
    // void HADDPD_XMM_M128(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x7C); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x7C: HADDPS xmm, xmm/m128
    // void HADDPS_XMM_XMM(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x7C); ModRM(d, s); }
    // void HADDPS_XMM_M128(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x7C); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x7D: HSUBPD xmm, xmm/m128
    // void HSUBPD_XMM_XMM(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x7D); ModRM(d, s); }
    // void HSUBPD_XMM_M128(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x7D); ModRM(d, s); }

    // // Opcode 0xF2 0x0F 0x7D: HSUBPS xmm, xmm/m128
    // void HSUBPS_XMM_XMM(IsXMM auto d, IsXMM auto s) { Double(); REX(d, s); Escape(0x7D); ModRM(d, s); }
    // void HSUBPS_XMM_M128(IsXMM auto d, IsMemory128 auto s) { Double(); REX(d, s); Escape(0x7D); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x6E: MOVD xmm, r/m32
    // void MOVD_XMM_R32(IsXMM auto d, IsGPR32 auto s) { OpSize(); REX(d, s); Escape(0x6E); ModRM(d, s); }
    // void MOVD_XMM_M128(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0x6E); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x7E: MOVD r/m32, xmm
    // void MOVD_R32_XMM(IsGPR32 auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x7E); ModRM(d, s); }
    // void MOVD_M32_XMM(IsMemory32 auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x7E); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x7E: MOVQ r/m64, xmm
    // void MOVQ_R64_XMM(IsGPR64 auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x7E); ModRM(d, s); }
    // void MOVQ_M64_XMM(IsMemory64 auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x7E); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0xD6: MOVQ xmm, r/m64
    // void MOVQ_XMM_R64(IsXMM auto d, IsGPR64 auto s) { OpSize(); REX(d, s); Escape(0xD6); ModRM(d, s); }
    // void MOVQ_XMM_M128(IsXMM auto d, IsMemory128 auto s) { OpSize(); REX(d, s); Escape(0xD6); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x7F: MOVDQA xmm, xmm/m128
    // void MOVDQA_XMM_XMM_ALT(IsXMM auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x7F); ModRM(d, s); }
    // void MOVDQA_M128_XMM(IsMemory128 auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x7F); ModRM(d, s); }

    // // Opcode 0xF3 0x0F 0x7F: MOVDQU xmm, xmm/m128
    // void MOVDQU_XMM_XMM_ALT(IsXMM auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x7F); ModRM(d, s); }
    // void MOVDQU_M128_XMM(IsMemory128 auto d, IsXMM auto s) { Single(); REX(d, s); Escape(0x7F); ModRM(d, s); }

    // // Opcode 0x66 0x0F 0x50: MOVMSKPD r32/64, xmm
    // void MOVMSKPD_R32_XMM(IsGPR32 auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x50); ModRM(d, s); }
    // void MOVMSKPD_R64_XMM(IsGPR64 auto d, IsXMM auto s) { OpSize(); REX(d, s); Escape(0x50); ModRM(d, s); }

    // void MOVUPS_XMM_XMM(IsXMM auto dest, IsXMM auto src) { Escape(0x10); ModRM(dest, src); }
    // void MOVUPS_XMM_M128(IsXMM auto dest, IsMemory128 auto mem) { Escape(0x10); ModRM(dest, mem); }

    // // Opcode 0x10: MOVSS xmm, xmm/m32
    // void MOVSS_XMM_XMM(IsXMM auto dest, IsXMM auto src) { Single(); Escape(0x10); ModRM(dest, src); }
    // void MOVSS_XMM_M32(IsXMM auto dest, IsMemory32 auto mem) { Single(); Escape(0x10); ModRM(dest, mem); }

    // // Opcode 0x10: MOVUPD xmm, xmm/m128
    // void MOVUPD_XMM_XMM(IsXMM auto dest, IsXMM auto src) { OpSize(); Escape(0x10); ModRM(dest, src); }
    // void MOVUPD_XMM_M128(IsXMM auto dest, IsMemory128 auto mem) { OpSize(); Escape(0x10); ModRM(dest, mem); }

    // // Opcode 0x11: MOVSS xmm/m32, xmm
    // void MOVSS_M32_XMM(IsMemory32 auto dest, IsXMM auto src) { Single(); Escape(0x11); ModRM(dest, src); }
    // void MOVSS_XMM_XMM_ALT(IsXMM auto dest, IsXMM auto src) { Single(); Escape(0x11); ModRM(dest, src); }
    // void MOVUPS_M128_XMM(IsMemory128 auto dest, IsXMM auto src) { Escape(0x11); ModRM(dest, src); }
    // void MOVUPD_M128_XMM(IsMemory128 auto dest, IsXMM auto src) { OpSize(); Escape(0x11); ModRM(dest, src); }

    // // MOVSD (Move Scalar Double-Precision Floating-Point)
    // void MOVSD_XMM_XMM(IsXMM auto dest, IsXMM auto src) { Double(); Escape(0x10); ModRM(dest, src); }
    // void MOVSD_XMM_M64(IsXMM auto dest, IsMemory64 auto mem) { Double(); Escape(0x10); ModRM(dest, mem); }
    // void MOVSD_M64_XMM(IsMemory64 auto mem, IsXMM auto src) { Double(); Escape(0x11); ModRM(mem, src); }

    // // NOTE: Double check the usage of `SO(0x66)`
    // // Unordered Compare Scalar Single-FP Values and Set EFLAGS
    // void UCOMISS_XMM_XMM(IsXMM auto x1, IsXMM auto x2) { Escape(0x2E); ModRM(x1, x2); }
    // void UCOMISS_XMM_M32(IsXMM auto x, IsMemory32 auto m) { Escape(0x2E); ModRM(x, m); }

    // // Unordered Compare Scalar Double-FP Values and Set EFLAGS
    // void UCOMISD_XMM_XMM(IsXMM auto x1, IsXMM auto x2) { Escape(0x2E); ModRM(x1, x2); }
    // void UCOMISD_XMM_M64(IsXMM auto x, IsMemory64 auto m) { OpSize(); Escape(0x2E); ModRM(x, m); }

    // // Compare Scalar Ordered Single-FP Values and Set EFLAGS
    // void COMISS_XMM_XMM(IsXMM auto x1, IsXMM auto x2) { Escape(0x2F); ModRM(x1, x2); }
    // void COMISS_XMM_M32(IsXMM auto x, IsMemory32 auto m) { Escape(0x2F); ModRM(x, m); }

    // // Compare Scalar Ordered Double-FP Values and Set EFLAGS
    // void COMISD_XMM_XMM(IsXMM auto x1, IsXMM auto x2) { Escape(0x2F); ModRM(x1, x2); }
    // void COMISD_XMM_M64(IsXMM auto x, IsMemory64 auto m) { OpSize(); Escape(0x2F); ModRM(x, m); }

    // // Arithmetic Operations
    // void ADDPS_XMM_XMM(IsXMM auto x1, IsXMM auto x2) { Escape(0x58); ModRM(x1, x2); }
    // void ADDPS_XMM_M128(IsXMM auto x, IsMemory128 auto m) { Escape(0x58); ModRM(x, m); }
    // void SUBPS_XMM_XMM(IsXMM auto x1, IsXMM auto x2) { Escape(0x5C); ModRM(x1, x2); }
    // void SUBPS_XMM_M128(IsXMM auto x, IsMemory128 auto m) { Escape(0x5C); ModRM(x, m); }
    // void MULPS_XMM_XMM(IsXMM auto x1, IsXMM auto x2) { Escape(0x59); ModRM(x1, x2); }
    // void MULPS_XMM_M128(IsXMM auto x, IsMemory128 auto m) { Escape(0x59); ModRM(x, m); }
    // void DIVPS_XMM_XMM(IsXMM auto x1, IsXMM auto x2) { Escape(0x5E); ModRM(x1, x2); }
    // void DIVPS_XMM_M128(IsXMM auto x, IsMemory128 auto m) { Escape(0x5E); ModRM(x, m); }

    // // Comparison Operations
    // void CMPPS_XMM_XMM_IMM8(IsXMM auto d, IsXMM auto s, Immediate8 i) { Escape(0xC2); ModRM(d, s); Emit8(i); }
    // void CMPPS_XMM_M128_IMM8(IsXMM auto d, IsMemory128 auto s, Immediate8 i) { Escape(0xC2); ModRM(d, s); Emit8(i); }

    // // Data Movement Operations (Single)
    // void MOVAPS_XMM_XMM(IsXMM auto d, IsXMM auto s) { Escape(0x28); ModRM(d, s); }
    // void MOVAPS_XMM_M128(IsXMM auto d, IsMemory128 auto s) { Escape(0x28); ModRM(d, s); }
    // void MOVAPS_M128_XMM(IsMemory128 auto d, IsXMM auto s) { Escape(0x29); ModRM(d, s); }
    // void MOVLPS_XMM_M64(IsXMM auto d, IsMemory64 auto s) { Escape(0x12); ModRM(d, s); }
    // void MOVLPS_M64_XMM(IsMemory64 auto d, IsXMM auto s) { Escape(0x13); ModRM(d, s); }

    // // Data Movement Operations (Double)
    // void MOVAPD_XMM_XMM(IsXMM auto d, IsXMM auto s) { OpSize(); Escape(0x28); ModRM(d, s); }
    // void MOVAPD_XMM_M128(IsXMM auto d, IsMemory128 auto s) { OpSize(); Escape(0x28); ModRM(d, s); }
    // void MOVAPD_M128_XMM(IsMemory128 auto d, IsXMM auto s) { OpSize(); Escape(0x29); ModRM(d, s); }
    // void MOVLPD_XMM_M64(IsXMM auto d, IsMemory64 auto s) { OpSize(); Escape(0x12); ModRM(d, s); }
    // void MOVLPD_M64_XMM(IsMemory64 auto d, IsXMM auto s) { OpSize(); Escape(0x13); ModRM(d, s); }

    // // Move Aligned Packed Single-Precision Floating-Point Values
    // void MOVAPS(code::Register& d, code::Register& s) { REX(d, s); Esc(0x28); ModRM(d, s); }
    // void MOVAPS(code::Register& d, code::Address& s) { REX(d, s); Esc(0x28); ModRM(d, s); }
    // void MOVAPS(code::Address& d, code::Register& s) { REX(d, s); Esc(0x29); ModRM(d, s); }

    // // Move Aligned Packed Double-Precision Floating-Point Values
    // void MOVAPD(code::Register& d, code::Register& s) { OpSize(); REX(d, s); Esc(0x28); ModRM(d, s); }
    // void MOVAPD(code::Register& d, code::Address& s) { OpSize(); REX(d, s); Esc(0x28); ModRM(d, s); }
    // void MOVAPD(code::Address& d, code::Register& s) { OpSize(); REX(d, s); Esc(0x29); ModRM(d, s); }

    // // // Conversion Operations
    // // void CVT(code::Register& dest, code::Register& src) {
    // //   if (dest.IsFloating() && !src.IsFloating()) {
    // //     // Convert integer src to floating-point dest
    // //     if (dest.IsDouble()) {
    // //       CVTSI2SD(dest, src); // Convert integer src to double-precision floating-point in dest
    // //     } else {
    // //       CVTSI2SS(dest, src); // Convert integer src to single-precision floating-point in dest
    // //     }
    // //   } else if (src.IsFloating() && !dest.IsFloating()) {
    // //     // Convert floating-point src to integer dest
    // //     if (dest.Is64Bit()) {
    // //       if (src.IsDouble()) {
    // //         CVTSD2SI(dest, src); // Convert double-precision floating-point src to integer in dest
    // //       } else {
    // //         CVTSS2SI(dest, src); // Convert single-precision floating-point src to integer in dest
    // //       }
    // //     } else if (dest.Is32Bit()) {
    // //       if (src.IsDouble()) {
    // //         CVTSD2SI32(dest, src); // Convert double-precision floating-point src to 32-bit integer in dest
    // //       } else {
    // //         CVTSS2SI32(dest, src); // Convert single-precision floating-point src to 32-bit integer in dest
    // //       }
    // //     } else {
    // //       throw utility::Error("Unsupported destination size for floating-point to integer conversion");
    // //     }
    // //   } else {
    // //     throw utility::Error("Unsupported conversion between the given source and destination types");
    // //   }
    // // }

    // // Convert Scalar Doubleword Integer to Scalar Single-Precision FP
    // void CVTSI2SS(const code::Register& d, const code::Register& s) { Single(); REX(d, s); Escape(0x2A); ModRM(d, s); }
    // void CVTSI2SS(const code::Register& d, const code::Address& s) { Single(); REX(d, s); Escape(0x2A); ModRM(d, s); }

    // void CVTSI2SS_XMM_R32(IsXMM auto d, IsGPR32 auto s) { Single(); Escape(0x2A); ModRM(d, s); }
    // void CVTSI2SS_XMM_R64(IsXMM auto d, IsGPR64 auto s) { Single(); Escape(0x2A); ModRM(d, s); }
    // void CVTSI2SS_XMM_M32(IsXMM auto d, IsMemory32 auto s) { Single(); Escape(0x2A); ModRM(d, s); }
    // void CVTSI2SS_XMM_M64(IsXMM auto d, IsMemory64 auto s) { Single(); Escape(0x2A); ModRM(d, s); }

    // // Convert Scalar Doubleword Integer to Scalar Double-Precision FP
    // void CVTSI2SD(const code::Register& d, const code::Register& s) { Double(); REX(d, s); Esc(0x2A); ModRM(d, s); }
    // void CVTSI2SD(const code::Register& d, const code::Address& s) { Double(); REX(d, s); Esc(0x2A); ModRM(d, s); }

    // void CVTSI2SD_XMM_R32(IsXMM auto d, IsGPR32 auto s) { Double(); Escape(0x2A); ModRM(d, s); }
    // void CVTSI2SD_XMM_R64(IsXMM auto d, IsGPR64 auto s) { Double(); Escape(0x2A); ModRM(d, s); }
    // void CVTSI2SD_XMM_M32(IsXMM auto d, IsMemory32 auto s) { Double(); Escape(0x2A); ModRM(d, s); }
    // void CVTSI2SD_XMM_M64(IsXMM auto d, IsMemory64 auto s) { Double(); Escape(0x2A); ModRM(d, s); }

    // // Convert Scalar Single-Precision FP to Scalar Doubleword Integer
    // void CVTSS2SI(const code::Register& d, const code::Register& s) { Single(); REX(d, s); Escape(0x2D); ModRM(d, s); }
    // void CVTSS2SI(const code::Register& d, const code::Address& s) { Single(); REX(d, s); Escape(0x2D); ModRM(d, s); }

    // void CVTSS2SI_R32_XMM(IsGPR32 auto d, IsXMM auto s) { Single(); Escape(0x2D); ModRM(d, s); }
    // void CVTSS2SI_R64_XMM(IsGPR64 auto d, IsXMM auto s) { Single(); Escape(0x2D); ModRM(d, s); }
    // void CVTSS2SI_R32_M32(IsGPR32 auto d, IsMemory32 auto s) { Single(); Escape(0x2D); ModRM(d, s); }
    // void CVTSS2SI_R64_M32(IsGPR64 auto d, IsMemory32 auto s) { Single(); Escape(0x2D); ModRM(d, s); }

    // // Convert Scalar Double-Precision FP to Scalar Doubleword Integer
    // void CVTSD2SI(const code::Register& d, const code::Register& s) { Double(); REX(d, s); Escape(0x2D); ModRM(d, s); }
    // void CVTSD2SI(const code::Register& d, const code::Address& s) { Double(); REX(d, s); Escape(0x2D); ModRM(d, s); }

    // void CVTSD2SI_R32_XMM(IsGPR32 auto d, IsXMM auto s) { Double(); Escape(0x2D); ModRM(d, s); }
    // void CVTSD2SI_R64_XMM(IsGPR64 auto d, IsXMM auto s) { Double(); Escape(0x2D); ModRM(d, s); }
    // void CVTSD2SI_R32_M64(IsGPR32 auto d, IsMemory64 auto s) { Double(); Escape(0x2D); ModRM(d, s); }
    // void CVTSD2SI_R64_M64(IsGPR64 auto d, IsMemory64 auto s) { Double(); Escape(0x2D); ModRM(d, s); }

    // // Convert with Trunc. Scalar Single-FP Value to DW Integer
    // void CVTTSS2SI(const code::Register& d, const code::Register& s) { Single(); REX(d, s); Esc(0x2C); ModRM(d, s); }
    // void CVTTSS2SI(const code::Register& d, const code::Address& s) { Single(); REX(d, s); Esc(0x2C); ModRM(d, s); }

    // // Conv. with Trunc. Scalar Double-FP Value to Signed DW Int
    // void CVTTSD2SI(const code::Register& d, const code::Register& s) { Double(); REX(d, s); Esc(0x2C); ModRM(d, s); }
    // void CVTTSD2SI(const code::Register& d, const code::Address& s) { Double(); REX(d, s); Esc(0x2C); ModRM(d, s); }

    // void INT3() { OP(0xCC); }
    // void SYSCALL() { Escape(0x05); }

    // // // Unix only, Windows is stupid
    // // void WRITE_SYSCALL(int fd, std::string_view text) {
    // //   MOV_RAX_IMM64(Immediate64{1});           // syscall number for sys_write
    // //   MOV_RDI_IMM64(Immediate64{fd});          // file descriptor
    // //   MOV_RSI_IMM64(Immediate64{reinterpret_cast<uint64_t>(text.data())}); // pointer to the buffer
    // //   MOV_RDX_IMM64(Immediate64{text.size()}); // number of bytes to write
    // //   SYSCALL();                  // invoke syscall
    // // }
  };
};