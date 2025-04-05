export module code.riscv.registers;

import <string>;

export namespace code::riscv {
  class X {
  private:
    uint32_t value;

    friend class Encoder;
    X(uint32_t value) : value{value} {}
  public:
    X(const X& other) = default;
    X& operator=(const X& other) = default;

    uint32_t Value() const { return value; }

    std::string ToString() const {
      switch (Value()) {
        case 0: return "X0";
        case 1: return "X1";
        case 2: return "X2";
        case 3: return "X3";
        case 4: return "X4";
        case 5: return "X5";
        case 6: return "X6";
        case 7: return "X7";
        case 8: return "X8";
        case 9: return "X9";
        case 10: return "X10";
        case 11: return "X11";
        case 12: return "X12";
        case 13: return "X13";
        case 14: return "X14";
        case 15: return "X15";
        case 16: return "X16";
        case 17: return "X17";
        case 18: return "X18";
        case 19: return "X19";
        case 20: return "X20";
        case 21: return "X21";
        case 22: return "X22";
        case 23: return "X23";
        case 24: return "X24";
        case 25: return "X25";
        case 26: return "X26";
        case 27: return "X27";
        case 28: return "X28";
        case 29: return "X29";
        case 30: return "X30";
        case 31: return "X31";
        default: return "NONE";
      }
    }
  };

  inline constexpr X X0{0};
  inline constexpr X X1{1};
  inline constexpr X X2{2};
  inline constexpr X X3{3};
  inline constexpr X X4{4};
  inline constexpr X X5{5};
  inline constexpr X X6{6};
  inline constexpr X X7{7};
  inline constexpr X X8{8};
  inline constexpr X X9{9};
  inline constexpr X X10{10};
  inline constexpr X X11{11};
  inline constexpr X X12{12};
  inline constexpr X X13{13};
  inline constexpr X X14{14};
  inline constexpr X X15{15};
  inline constexpr X X16{16};
  inline constexpr X X17{17};
  inline constexpr X X18{18};
  inline constexpr X X19{19};
  inline constexpr X X20{20};
  inline constexpr X X21{21};
  inline constexpr X X22{22};
  inline constexpr X X23{23};
  inline constexpr X X24{24};
  inline constexpr X X25{25};
  inline constexpr X X26{26};
  inline constexpr X X27{27};
  inline constexpr X X28{28};
  inline constexpr X X29{29};
  inline constexpr X X30{30};
  inline constexpr X X31{31};
};