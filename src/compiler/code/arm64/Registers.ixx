export module code.arm64.registers;

import <string>;

export namespace code::arm64 {
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

  class W {
  private:
    uint32_t value;

    friend class Encoder;
    W(uint32_t value) : value{value} {}
  public:
    W(const W& other) = default;
    W& operator=(const W& other) = default;

    uint32_t Value() const { return value; }

    std::string ToString() const {
      switch (Value()) {
        case 0: return "W0";
        case 1: return "W1";
        case 2: return "W2";
        case 3: return "W3";
        case 4: return "W4";
        case 5: return "W5";
        case 6: return "W6";
        case 7: return "W7";
        case 8: return "W8";
        case 9: return "W9";
        case 10: return "W10";
        case 11: return "W11";
        case 12: return "W12";
        case 13: return "W13";
        case 14: return "W14";
        case 15: return "W15";
        case 16: return "W16";
        case 17: return "W17";
        case 18: return "W18";
        case 19: return "W19";
        case 20: return "W20";
        case 21: return "W21";
        case 22: return "W22";
        case 23: return "W23";
        case 24: return "W24";
        case 25: return "W25";
        case 26: return "W26";
        case 27: return "W27";
        case 28: return "W28";
        case 29: return "W29";
        case 30: return "W30";
        case 31: return "W31";
        default: return "NONE";
      }
    }
  };

  inline constexpr W W0{0};
  inline constexpr W W1{1};
  inline constexpr W W2{2};
  inline constexpr W W3{3};
  inline constexpr W W4{4};
  inline constexpr W W5{5};
  inline constexpr W W6{6};
  inline constexpr W W7{7};
  inline constexpr W W8{8};
  inline constexpr W W9{9};
  inline constexpr W W10{10};
  inline constexpr W W11{11};
  inline constexpr W W12{12};
  inline constexpr W W13{13};
  inline constexpr W W14{14};
  inline constexpr W W15{15};
  inline constexpr W W16{16};
  inline constexpr W W17{17};
  inline constexpr W W18{18};
  inline constexpr W W19{19};
  inline constexpr W W20{20};
  inline constexpr W W21{21};
  inline constexpr W W22{22};
  inline constexpr W W23{23};
  inline constexpr W W24{24};
  inline constexpr W W25{25};
  inline constexpr W W26{26};
  inline constexpr W W27{27};
  inline constexpr W W28{28};
  inline constexpr W W29{29};
  inline constexpr W W30{30};
  inline constexpr W W31{31};
};