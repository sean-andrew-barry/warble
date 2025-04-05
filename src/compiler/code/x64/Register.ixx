export module code.x64._register;

import code.x64.operand;
import code._register;
import utility.print;
import <cstdint>;
import <stdexcept>;
import <algorithm>;
import <concepts>;
import <variant>;
import <string>;
import <limits>;

export namespace code::x64 {
  class Encoder;

  enum class Category : uint8_t {
    SGPR8,
    SGPR16,
    SGPR32,
    SGPR64,
    EGPR8,
    EGPR16,
    EGPR32,
    EGPR64,
  };

  struct Register : public Operand {
  protected:
    uint8_t value{std::numeric_limits<uint8_t>::max()};
  public:
    constexpr Register() = default;
    constexpr Register(uint8_t v) : value{v} {}
    constexpr Register(uint32_t v) : value{static_cast<uint8_t>(v)} {}
    constexpr Register(int32_t v) : value{static_cast<uint8_t>(v)} {}

    constexpr bool IsValid() const { return value <= 7; }
    constexpr bool IsNone() const { return value == std::numeric_limits<uint8_t>::max(); }
    constexpr bool IsSP() const { return value == 4; }
    constexpr bool IsBP() const { return value == 5; }
    constexpr bool IsRIP() const { return value == std::numeric_limits<uint8_t>::max() - 1; }
    constexpr int8_t ID() const { return 0; }
    constexpr uint8_t Value() const { return value; }
    // constexpr explicit operator int8_t() const { return value; }
    constexpr explicit operator uint8_t() const { return value; }
    constexpr explicit operator bool() const { return IsValid(); }
    constexpr bool IsExtended() const { return false; }
  };

  struct GPR : public Register {};
  
  struct SGPR : public GPR {
    constexpr uint8_t ID() const { return Value(); }
    constexpr bool IsExtended() const { return false; }
  };

  struct EGPR : public GPR {
    constexpr uint8_t ID() const { return Value() + 8; }
    constexpr bool IsSP() const { return false; }
    constexpr bool IsBP() const { return false; }
    constexpr bool IsRIP() const { return false; }
    constexpr bool IsExtended() const { return true; }
  };
  
  struct SGPR8 : public SGPR {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "AL";
        case 1: return "CL";
        case 2: return "DL";
        case 3: return "BL";
        case 4: return "SPL";
        case 5: return "BPL";
        case 6: return "SIL";
        case 7: return "DIL";
        default: return "NONE";
      }
    }
  };

  inline constexpr SGPR8 AL{0};
  inline constexpr SGPR8 CL{1};
  inline constexpr SGPR8 DL{2};
  inline constexpr SGPR8 BL{3};
  inline constexpr SGPR8 SPL{4};
  inline constexpr SGPR8 BPL{5};
  inline constexpr SGPR8 SIL{6};
  inline constexpr SGPR8 DIL{7};

  struct SGPR16 : public SGPR {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "AX";
        case 1: return "CX";
        case 2: return "DX";
        case 3: return "BX";
        case 4: return "SP";
        case 5: return "BP";
        case 6: return "SI";
        case 7: return "DI";
        default: return "NONE";
      }
    }
  };

  inline constexpr SGPR16 AX{0};
  inline constexpr SGPR16 CX{1};
  inline constexpr SGPR16 DX{2};
  inline constexpr SGPR16 BX{3};
  inline constexpr SGPR16 SP{4};
  inline constexpr SGPR16 BP{5};
  inline constexpr SGPR16 SI{6};
  inline constexpr SGPR16 DI{7};

  struct SGPR32 : public SGPR {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "EAX";
        case 1: return "ECX";
        case 2: return "EDX";
        case 3: return "EBX";
        case 4: return "ESP";
        case 5: return "EBP";
        case 6: return "ESI";
        case 7: return "EDI";
        default: return "NONE";
      }
    }
  };

  inline constexpr SGPR32 EAX{0};
  inline constexpr SGPR32 ECX{1};
  inline constexpr SGPR32 EDX{2};
  inline constexpr SGPR32 EBX{3};
  inline constexpr SGPR32 ESP{4};
  inline constexpr SGPR32 EBP{5};
  inline constexpr SGPR32 ESI{6};
  inline constexpr SGPR32 EDI{7};

  struct SGPR64 : public SGPR {
    std::string ToString() const {
      switch (Value()) {
        case -2: return "RIP";
        case -1: return "NONE";
        case 0: return "RAX";
        case 1: return "RCX";
        case 2: return "RDX";
        case 3: return "RBX";
        case 4: return "RSP";
        case 5: return "RBP";
        case 6: return "RSI";
        case 7: return "RDI";
        default: return "NONE";
      }
    }
  };

  inline constexpr SGPR64 RIP{std::numeric_limits<uint8_t>::max() - 1};
  inline constexpr SGPR64 NONE{std::numeric_limits<uint8_t>::max()};
  inline constexpr SGPR64 RAX{0};
  inline constexpr SGPR64 RCX{1};
  inline constexpr SGPR64 RDX{2};
  inline constexpr SGPR64 RBX{3};
  inline constexpr SGPR64 RSP{4};
  inline constexpr SGPR64 RBP{5};
  inline constexpr SGPR64 RSI{6};
  inline constexpr SGPR64 RDI{7};

  struct EGPR8 : public EGPR {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "R8B";
        case 1: return "R9B";
        case 2: return "R10B";
        case 3: return "R11B";
        case 4: return "R12B";
        case 5: return "R13B";
        case 6: return "R14B";
        case 7: return "R15B";
        default: return "NONE";
      }
    }
  };

  inline constexpr EGPR8 R8B{0};
  inline constexpr EGPR8 R9B{1};
  inline constexpr EGPR8 R10B{2};
  inline constexpr EGPR8 R11B{3};
  inline constexpr EGPR8 R12B{4};
  inline constexpr EGPR8 R13B{5};
  inline constexpr EGPR8 R14B{6};
  inline constexpr EGPR8 R15B{7};

  struct EGPR16 : public EGPR {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "R8W";
        case 1: return "R9W";
        case 2: return "R10W";
        case 3: return "R11W";
        case 4: return "R12W";
        case 5: return "R13W";
        case 6: return "R14W";
        case 7: return "R15W";
        default: return "NONE";
      }
    }
  };
  
  inline constexpr EGPR16 R8W{0};
  inline constexpr EGPR16 R9W{1};
  inline constexpr EGPR16 R10W{2};
  inline constexpr EGPR16 R11W{3};
  inline constexpr EGPR16 R12W{4};
  inline constexpr EGPR16 R13W{5};
  inline constexpr EGPR16 R14W{6};
  inline constexpr EGPR16 R15W{7};

  struct EGPR32 : public EGPR {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "R8D";
        case 1: return "R9D";
        case 2: return "R10D";
        case 3: return "R11D";
        case 4: return "R12D";
        case 5: return "R13D";
        case 6: return "R14D";
        case 7: return "R15D";
        default: return "NONE";
      }
    }
  };

  inline constexpr EGPR32 R8D{0};
  inline constexpr EGPR32 R9D{1};
  inline constexpr EGPR32 R10D{2};
  inline constexpr EGPR32 R11D{3};
  inline constexpr EGPR32 R12D{4};
  inline constexpr EGPR32 R13D{5};
  inline constexpr EGPR32 R14D{6};
  inline constexpr EGPR32 R15D{7};

  struct EGPR64 : public EGPR {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "R8";
        case 1: return "R9";
        case 2: return "R10";
        case 3: return "R11";
        case 4: return "R12";
        case 5: return "R13";
        case 6: return "R14";
        case 7: return "R15";
        default: return "NONE";
      }
    }
  };

  inline constexpr EGPR64 R8{0};
  inline constexpr EGPR64 R9{1};
  inline constexpr EGPR64 R10{2};
  inline constexpr EGPR64 R11{3};
  inline constexpr EGPR64 R12{4};
  inline constexpr EGPR64 R13{5};
  inline constexpr EGPR64 R14{6};
  inline constexpr EGPR64 R15{7};

  struct SSE : public Register {
    constexpr bool IsSP() const { return false; }
    constexpr bool IsBP() const { return false; }
    constexpr bool IsRIP() const { return false; }
  };
  
  struct SSSE : public SSE {
    constexpr uint8_t ID() const { return Value() + 16; }
    constexpr bool IsExtended() const { return false; }
  };

  struct ESSE : public SSE {
    constexpr uint8_t ID() const { return Value() + 24; }
    constexpr bool IsExtended() const { return true; }
  };

  struct SSSE128 : public SSSE {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "XMM0";
        case 1: return "XMM1";
        case 2: return "XMM2";
        case 3: return "XMM3";
        case 4: return "XMM4";
        case 5: return "XMM5";
        case 6: return "XMM6";
        case 7: return "XMM7";
        default: return "NONE";
      }
    }
  };

  inline constexpr SSSE128 XMM0{0};
  inline constexpr SSSE128 XMM1{1};
  inline constexpr SSSE128 XMM2{2};
  inline constexpr SSSE128 XMM3{3};
  inline constexpr SSSE128 XMM4{4};
  inline constexpr SSSE128 XMM5{5};
  inline constexpr SSSE128 XMM6{6};
  inline constexpr SSSE128 XMM7{7};

  struct SSSE256 : public SSSE {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "YMM0";
        case 1: return "YMM1";
        case 2: return "YMM2";
        case 3: return "YMM3";
        case 4: return "YMM4";
        case 5: return "YMM5";
        case 6: return "YMM6";
        case 7: return "YMM7";
        default: return "NONE";
      }
    }
  };

  inline constexpr SSSE256 YMM0{0};
  inline constexpr SSSE256 YMM1{1};
  inline constexpr SSSE256 YMM2{2};
  inline constexpr SSSE256 YMM3{3};
  inline constexpr SSSE256 YMM4{4};
  inline constexpr SSSE256 YMM5{5};
  inline constexpr SSSE256 YMM6{6};
  inline constexpr SSSE256 YMM7{7};

  struct SSSE512 : public SSSE {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "ZMM0";
        case 1: return "ZMM1";
        case 2: return "ZMM2";
        case 3: return "ZMM3";
        case 4: return "ZMM4";
        case 5: return "ZMM5";
        case 6: return "ZMM6";
        case 7: return "ZMM7";
        default: return "NONE";
      }
    }
  };

  inline constexpr SSSE512 ZMM0{0};
  inline constexpr SSSE512 ZMM1{1};
  inline constexpr SSSE512 ZMM2{2};
  inline constexpr SSSE512 ZMM3{3};
  inline constexpr SSSE512 ZMM4{4};
  inline constexpr SSSE512 ZMM5{5};
  inline constexpr SSSE512 ZMM6{6};
  inline constexpr SSSE512 ZMM7{7};

  struct ESSE128 : public ESSE {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "XMM8";
        case 1: return "XMM9";
        case 2: return "XMM10";
        case 3: return "XMM11";
        case 4: return "XMM12";
        case 5: return "XMM13";
        case 6: return "XMM14";
        case 7: return "XMM15";
        default: return "NONE";
      }
    }
  };

  inline constexpr ESSE128 XMM8{0};
  inline constexpr ESSE128 XMM9{1};
  inline constexpr ESSE128 XMM10{2};
  inline constexpr ESSE128 XMM11{3};
  inline constexpr ESSE128 XMM12{4};
  inline constexpr ESSE128 XMM13{5};
  inline constexpr ESSE128 XMM14{6};
  inline constexpr ESSE128 XMM15{7};

  struct ESSE256 : public ESSE {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "YMM8";
        case 1: return "YMM9";
        case 2: return "YMM10";
        case 3: return "YMM11";
        case 4: return "YMM12";
        case 5: return "YMM13";
        case 6: return "YMM14";
        case 7: return "YMM15";
        default: return "NONE";
      }
    }
  };
  
  inline constexpr ESSE256 YMM8{0};
  inline constexpr ESSE256 YMM9{1};
  inline constexpr ESSE256 YMM10{2};
  inline constexpr ESSE256 YMM11{3};
  inline constexpr ESSE256 YMM12{4};
  inline constexpr ESSE256 YMM13{5};
  inline constexpr ESSE256 YMM14{6};
  inline constexpr ESSE256 YMM15{7};

  struct ESSE512 : public ESSE {
    std::string ToString() const {
      switch (Value()) {
        case 0: return "ZMM8";
        case 1: return "ZMM9";
        case 2: return "ZMM10";
        case 3: return "ZMM11";
        case 4: return "ZMM12";
        case 5: return "ZMM13";
        case 6: return "ZMM14";
        case 7: return "ZMM15";
        default: return "NONE";
      }
    }
  };

  inline constexpr ESSE512 ZMM8{0};
  inline constexpr ESSE512 ZMM9{1};
  inline constexpr ESSE512 ZMM10{2};
  inline constexpr ESSE512 ZMM11{3};
  inline constexpr ESSE512 ZMM12{4};
  inline constexpr ESSE512 ZMM13{5};
  inline constexpr ESSE512 ZMM14{6};
  inline constexpr ESSE512 ZMM15{7};

  // struct AGPR : public GPR {
  //   constexpr bool IsStandard() const { return value >= 0 && value <= 7; }
  //   constexpr bool IsExtended() const { return value > 7; }
  //   // constexpr SGPR64 ToSGPR64() const { return SGPR64{value}; }
  //   // constexpr SGPR32 ToSGPR32() const { return SGPR32{value}; }
  //   // constexpr SGPR16 ToSGPR16() const { return SGPR16{value}; }
  //   // constexpr SGPR8  ToSGPR8 () const { return SGPR8{value}; }
  //   // constexpr EGPR64 ToEGPR64() const { return EGPR64{value}; }
  //   // constexpr EGPR32 ToEGPR32() const { return EGPR32{value}; }
  //   // constexpr EGPR16 ToEGPR16() const { return EGPR16{value}; }
  //   // constexpr EGPR8  ToEGPR8 () const { return EGPR8{value}; }

  //   constexpr int8_t Value() const {
  //     if (value > 7) {
  //       return value - 8;
  //     } else {
  //       return value;
  //     }
  //   }

  //   constexpr explicit operator int8_t() const { return Value(); }
  // };

  // inline constexpr AGPR rIP{-2};
  // inline constexpr AGPR rAX{0};
  // inline constexpr AGPR rCX{1};
  // inline constexpr AGPR rDX{2};
  // inline constexpr AGPR rBX{3};
  // inline constexpr AGPR rSP{4};
  // inline constexpr AGPR rBP{5};
  // inline constexpr AGPR rSI{6};
  // inline constexpr AGPR rDI{7};
  // inline constexpr AGPR r8{8};
  // inline constexpr AGPR r9{9};
  // inline constexpr AGPR r10{10};
  // inline constexpr AGPR r11{11};
  // inline constexpr AGPR r12{12};
  // inline constexpr AGPR r13{13};
  // inline constexpr AGPR r14{14};
  // inline constexpr AGPR r15{15};

  // struct AGPR8 : public AGPR {
  //   static constexpr size_t Size = 1;

  //   explicit constexpr operator SGPR8() const {
  //     if (IsStandard()) return SGPR8{Value()};
  //     throw std::runtime_error("Not a standard GPR8");
  //   }

  //   explicit constexpr operator EGPR8() const {
  //     if (IsExtended()) return EGPR8{Value()};
  //     throw std::runtime_error("Not an extended GPR8");
  //   }
  // };

  // struct AGPR16 : public AGPR {
  //   static constexpr size_t Size = 2;

  //   explicit constexpr operator SGPR16() const {
  //     if (IsStandard()) return SGPR16{Value()};
  //     throw std::runtime_error("Not a standard GPR16");
  //   }

  //   explicit constexpr operator EGPR16() const {
  //     if (IsExtended()) return EGPR16{Value()};
  //     throw std::runtime_error("Not an extended GPR16");
  //   }
  // };

  // struct AGPR32 : public AGPR {
  //   static constexpr size_t Size = 4;

  //   explicit constexpr operator SGPR32() const {
  //     if (IsStandard()) return SGPR32{Value()};
  //     throw std::runtime_error("Not a standard GPR32");
  //   }

  //   explicit constexpr operator EGPR32() const {
  //     if (IsExtended()) return EGPR32{Value()};
  //     throw std::runtime_error("Not an extended GPR32");
  //   }
  // };

  // struct AGPR64 : public AGPR {
  //   static constexpr size_t Size = 8;

  //   explicit constexpr operator SGPR64() const {
  //     if (IsStandard()) return SGPR64{Value()};
  //     throw std::runtime_error("Not a standard GPR64");
  //   }

  //   explicit constexpr operator EGPR64() const {
  //     if (IsExtended()) return EGPR64{Value()};
  //     throw std::runtime_error("Not an extended GPR64");
  //   }
  // };

  // template<typename S, typename E>
  // struct AGPR {
  // public:
  //   using Standard = typename S;
  //   using Extended = typename E;
  // protected:
  //   Encoder& encoder;
  //   std::variant<S, E> value;
  //   bool spilled{false};
  // public:
  //   AGPR(Encoder& encoder) : encoder{encoder}, value{S{-1}} {}
  //   AGPR(Encoder& encoder, S value) : encoder{encoder}, value{value} {}
  //   AGPR(Encoder& encoder, E value) : encoder{encoder}, value{value} {}
  //   AGPR(Encoder& encoder, S value, bool spilled) : encoder{encoder}, value{value}, spilled{spilled} {}
  //   AGPR(Encoder& encoder, E value, bool spilled) : encoder{encoder}, value{value}, spilled{spilled} {}

  //   operator std::variant<S, E>() const { return value; }

  //   decltype(auto) Visit(auto&& fn) const {
  //     return std::visit(std::forward<decltype(fn)>(fn), value);
  //   }

  //   decltype(auto) Visit(auto&& fn, AGPR<S, E>&& v) const {
  //     return v.Visit([&](auto&& v2){
  //       return Visit([&](auto&& v1){
  //         return fn(std::forward<decltype(v1)>(v1), std::forward<decltype(v2)>(v2));
  //       });
  //     });
  //   }

  //   constexpr bool IsSpilled() const { return spilled; }
  //   constexpr bool IsValid() const { return Visit([](auto v){ return v.IsValid(); }); }
  //   constexpr bool IsNone() const { return Visit([](auto v){ return v.IsNone(); }); }
  //   constexpr bool IsSP() const { return Visit([](auto v){ return v.IsSP(); }); }
  //   constexpr bool IsBP() const { return Visit([](auto v){ return v.IsBP(); }); }
  //   constexpr bool IsRIP() const { return Visit([](auto v){ return v.IsRIP(); }); }
  //   constexpr int8_t Value() const { return Visit([](auto v){ return v.Value(); }); }
  //   constexpr explicit operator int8_t() const { return Visit([](auto v){ return static_cast<int8_t>(v); }); }
  //   constexpr explicit operator uint8_t() const { return Visit([](auto v){ return static_cast<uint8_t>(v); }); }
  //   constexpr explicit operator bool() const { return Visit([](auto v){ return v.IsValid(); }); }
  // };

  // struct AGPR8 : public AGPR<SGPR8, EGPR8> { using AGPR<SGPR8, EGPR8>::AGPR; ~AGPR8(); };
  // struct AGPR16 : public AGPR<SGPR16, EGPR16> { using AGPR<SGPR16, EGPR16>::AGPR; ~AGPR16(); };
  // struct AGPR32 : public AGPR<SGPR32, EGPR32> { using AGPR<SGPR32, EGPR32>::AGPR; ~AGPR32(); };
  // struct AGPR64 : public AGPR<SGPR64, EGPR64> { using AGPR<SGPR64, EGPR64>::AGPR; ~AGPR64(); };

  // struct AGPR16 : public AGPR {
  //   std::variant<SGPR16, EGPR16> reg;
  //   AGPR16(Encoder& e, SGPR16 r) : AGPR{e}, reg{r} {}
  //   AGPR16(Encoder& e, EGPR16 r) : AGPR{e}, reg{r} {}

  //   operator std::variant<SGPR16, EGPR16>() const { return reg; }
  // };

  // struct AGPR32 : public AGPR {
  //   std::variant<SGPR32, EGPR32> reg;
  //   operator std::variant<SGPR32, EGPR32>() const { return reg; }
  // };

  // struct AGPR64 : public AGPR {
  //   std::variant<SGPR64, EGPR64> reg;
  //   AGPR64(SGPR64 r) : AGPR{}, reg{r} {}
  //   AGPR64(EGPR64 r) : AGPR{}, reg{r} {}
  //   operator std::variant<SGPR64, EGPR64>() const { return reg; }
  // };

  // template<typename T>
  // concept IsAnyRegister = (std::same_as<SGPR, T> || std::same_as<EGPR, T>) || (std::same_as<SGPR8, T> || std::same_as<SGPR16, T> || std::same_as<SGPR32, T> || std::same_as<SGPR64, T>) || (std::same_as<EGPR8, T> || std::same_as<EGPR16, T> || std::same_as<EGPR32, T> || std::same_as<EGPR64, T>) || (std::same_as<SSSE128, T> || std::same_as<SSSE256, T> || std::same_as<SSSE512, T>) || (std::same_as<ESSE128, T> || std::same_as<ESSE256, T> || std::same_as<ESSE512, T>);

  // template<typename T>
  // concept IsAGPR = std::derived_from<T, AGPR> || std::convertible_to<T, AGPR>;

  // Match a specific size of SGPR
  template<typename T>
  concept IsSGPR8 = std::same_as<T, SGPR8>;

  template<typename T>
  concept IsSGPR16 = std::same_as<T, SGPR16>;

  template<typename T>
  concept IsSGPR32 = std::same_as<T, SGPR32>;

  template<typename T>
  concept IsSGPR64 = std::same_as<T, SGPR64>;

  // Match a specific size of EGPR
  template<typename T>
  concept IsEGPR8 = std::same_as<T, EGPR8>;

  template<typename T>
  concept IsEGPR16 = std::same_as<T, EGPR16>;

  template<typename T>
  concept IsEGPR32 = std::same_as<T, EGPR32>;

  template<typename T>
  concept IsEGPR64 = std::same_as<T, EGPR64>;

  // Match any size of SGPR
  template<typename T>
  concept IsSGPR = std::same_as<T, SGPR8> || std::same_as<T, SGPR16> || std::same_as<T, SGPR32> || std::same_as<T, SGPR64>;

  // Match any size of EGPR
  template<typename T>
  concept IsEGPR = std::same_as<T, EGPR8> || std::same_as<T, EGPR16> || std::same_as<T, EGPR32> || std::same_as<T, EGPR64>;

  // // Match any SGPR or EGPR of any size
  // template<typename T>
  // concept IsGPR = std::derived_from<T, AGPR> || std::convertible_to<T, SGPR8> || std::convertible_to<T, SGPR16> || std::convertible_to<T, SGPR32> || std::convertible_to<T, SGPR64> || std::convertible_to<T, EGPR8> || std::convertible_to<T, EGPR16> || std::convertible_to<T, EGPR32> || std::convertible_to<T, EGPR64>;

  // Match SGPR or EGPR of a specific size
  template<typename T>
  concept IsGPR8 = std::same_as<T, SGPR8> || std::same_as<T, EGPR8> || std::same_as<T, code::Register>;

  template<typename T>
  concept IsGPR16 = std::same_as<T, SGPR16> || std::same_as<T, EGPR16> || std::same_as<T, code::Register>;

  template<typename T>
  concept IsGPR32 = std::same_as<T, SGPR32> || std::same_as<T, EGPR32> || std::same_as<T, code::Register>;

  template<typename T>
  concept IsGPR64 = std::same_as<T, SGPR64> || std::same_as<T, EGPR64> || std::same_as<T, code::Register>;

  // Match any SGPR or EGPR of any size
  template<typename T>
  concept IsGPR = std::same_as<T, SGPR8> || std::same_as<T, EGPR8> || std::same_as<T, SGPR16> || std::same_as<T, EGPR16> || std::same_as<T, SGPR32> || std::same_as<T, EGPR32> || std::same_as<T, SGPR64> || std::same_as<T, EGPR64> || std::same_as<T, code::Register>;
  // concept IsGPR = IsGPR8<T> || IsGPR16<T> || IsGPR32<T> || IsGPR64<T>;

  // Match any size of SSSE
  template<typename T>
  concept IsSSSE = std::same_as<T, SSSE128> || std::same_as<T, SSSE256> || std::same_as<T, SSSE512>;

  // Match any size of ESSE
  template<typename T>
  concept IsESSE = std::same_as<T, ESSE128> || std::same_as<T, ESSE256> || std::same_as<T, ESSE512>;

  // Match any size of SSSE or ESSE
  template<typename T>
  concept IsSSE = std::same_as<T, SSSE128> || std::same_as<T, SSSE256> || std::same_as<T, SSSE512> || std::same_as<T, ESSE128> || std::same_as<T, ESSE256> || std::same_as<T, ESSE512>;

  // Standard names for the SSE registers
  template<typename T>
  concept IsXMM = std::same_as<T, SSSE128> || std::same_as<T, ESSE128>;

  template<typename T>
  concept IsYMM = std::same_as<T, SSSE256> || std::same_as<T, ESSE256>;

  template<typename T>
  concept IsZMM = std::same_as<T, SSSE512> || std::same_as<T, ESSE512>;

  template<typename T>
  concept IsRegister = IsGPR<T> || IsSSE<T>;
};