export module visitor.x64machine;

import visitor;
import <vector>;
import <cstdint>;
import <concepts>;
import <variant>;
import <bitset>;
import <array>;
import <type_traits>;
import <optional>;

// struct ASM {
//   virtual ~ASM() = default;
//   virtual void Compile(X64& code) = 0;
// };

// struct AddRTR : public ASM {
//   Registers lhs;
//   Registers rhs;

//   void Compile(X64& code) final {
//     code.EmitREX(false, lhs, rhs); // Handle REX prefix
//     code.Emit8(0x01); // Opcode for ADD r32, r32
//     code.EmitModRM(lhs, rhs); // Emit the ModR/M byte for register-to-register operation
//   }
// };

// struct AddRTI : public ASM {
//   Registers lhs;
//   Immediate rhs;

//   void Compile(X64& code) final {
//     code.EmitImmediateInstruction<0x83, 0x81>(lhs, rhs);
//   }
// };

// struct AddRTM : public ASM {
//   Registers lhs;
//   Memory rhs;

//   void Compile(X64& code) final {}
// };

// struct AddMTI : public ASM {
//   Memory lhs;
//   Immediate rhs;

//   void Compile(X64& code) final {}
// };

// enum class Flags: uint64_t {
//   // W: Write
//   // R: Read
//   // U: Undefined
//   // 0: Clear
//   // 1: Set

//   // OF: Overflow Flag
//   OFW,
//   OFR,
//   OFU,
//   OF0,
//   OF1,

//   // SF: Sign Flag
//   SFW,
//   SFR,
//   SFU,
//   SF0,
//   SF1,

//   // ZF: Zero Flag
//   ZFW,
//   ZFR,
//   ZFU,
//   ZF0,
//   ZF1,

//   // AF: Auxiliary Flag
//   AFW,
//   AFR,
//   AFU,
//   AF0,
//   AF1,

//   // PF: Parity Flag
//   PFW,
//   PFR,
//   PFU,
//   PF0,
//   PF1,

//   // CF: Carry Flag
//   CFW,
//   CFR,
//   CFU,
//   CF0,
//   CF1,

//   // DF: Direction Flag
//   DFW,
//   DFR,
//   DFU,
//   DF0,
//   DF1,

//   // Misc
//   ALT, // Alt flag
//   VOL, // Volatile
//   B16, // Uses 16-bit prefix
//   B64, // Uses REX.W

  
//   FRAX,
//   FEAX,
//   FAX,
//   FAL,
  
//   FRCX,
//   FECX,
//   FCX,
//   FCL,
  
//   FRDX,
//   FEDX,
//   FDX,
//   FDL,
  
//   FRBX,
//   FEBX,
//   FBX,
//   FBL,
  
//   FRSI,
//   FESI,
//   FSI,
//   FSIL,
  
//   FRDI,
//   FEDI,
//   FDI,
//   FDIL,
  
//   FRSP,
//   FESP,
//   FSP,
//   FSPL,
  
//   FRBP,
//   FEBP,
//   FBP,
//   FBPL,
  
//   FR8,
//   FR8D,
//   FR8W,
//   FR8B,
  
//   FR9,
//   FR9D,
//   FR9W,
//   FR9B,
  
//   FR10,
//   FR10D,
//   FR10W,
//   FR10B,
  
//   FR11,
//   FR11D,
//   FR11W,
//   FR11B,
  
//   FR12,
//   FR12D,
//   FR12W,
//   FR12B,
  
//   FR13,
//   FR13D,
//   FR13W,
//   FR13B,
  
//   FR14,
//   FR14D,
//   FR14W,
//   FR14B,
  
//   FR15,
//   FR15D,
//   FR15W,
//   FR15B,

//   COUNT, // Must be last
// };

// constexpr Flags operator|(Flags a, Flags b) {
//   return static_cast<Flags>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
// }

// constexpr uint64_t FLAGS_COUNT = static_cast<uint64_t>(Flags::COUNT);

// template<Flags... Args>
// class Attributes {
// private:
//   constexpr std::bitset<FLAGS_COUNT> Make() {
//     std::bitset<FLAGS_COUNT> bits;
//     (bits.set(static_cast<uint64_t>(Args)), ...);
//     return bits;
//   }

//   std::bitset<FLAGS_COUNT> flags;
// public:
//   constexpr Attributes() : flags{Make()} {}

//   constexpr bool Has(Flags flag) const { return flags[static_cast<uint64_t>(flag)]; }
// };

// // OF: Overflow Flag
// struct OFW {};
// struct OFR {};
// struct OFU {};
// struct OF0 {};
// struct OF1 {};

// // SF: Sign Flag
// struct SFW {};
// struct SFR {};
// struct SFU {};
// struct SF0 {};
// struct SF1 {};

// // ZF: Zero Flag
// struct ZFW {};
// struct ZFR {};
// struct ZFU {};
// struct ZF0 {};
// struct ZF1 {};

// // AF: Auxiliary Flag
// struct AFW {};
// struct AFR {};
// struct AFU {};
// struct AF0 {};
// struct AF1 {};

// // PF: Parity Flag
// struct PFW {};
// struct PFR {};
// struct PFU {};
// struct PF0 {};
// struct PF1 {};

// // CF: Carry Flag
// struct CFW {};
// struct CFR {};
// struct CFU {};
// struct CF0 {};
// struct CF1 {};

// // DF: Direction Flag
// struct DFW {};
// struct DFR {};
// struct DFU {};
// struct DF0 {};
// struct DF1 {};

// // Misc
// struct ALT {}; // Alt flag
// struct VOL {}; // Volatile
// struct B16 {}; // Uses 16-bit prefix
// struct B64 {}; // Uses REX.W
// struct REXW {}; // Uses REX.W

// template<typename... Args>
// struct Tags {
//   template<typename T>
//   static constexpr bool Has() {
//     return (... || std::is_same_v<T, Args>);
//   }
// };

namespace visitor {
  export class X64Machine : public Visitor {
  };
};