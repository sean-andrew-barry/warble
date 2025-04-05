export module code.frame;

import code.registers;

import <cstdint>;

namespace code {
  export class Frame {
  private:
    uint32_t displacement{0}; // Represents the current stack displacement
    uint32_t maximum{0}; // Represents the maximum stack displacement
    code::Registers registers; // The registers that are POTENTIALLY written to inside this frame
  public:
    constexpr Frame() = default;
    constexpr Frame(const Frame& other) = default;
    constexpr Frame& operator=(const Frame& other) = default;

    constexpr uint32_t Displacement() const { return displacement; }
    constexpr uint32_t Maximum() const { return maximum; }
    constexpr code::Registers Registers() const { return registers; }

    constexpr Frame& operator+=(uint32_t size) {
      displacement += size;
      if (displacement > maximum) {
        maximum = displacement;
      }
      
      return *this;
    }

    constexpr Frame& operator-=(uint32_t size) {
      displacement -= size;
      return *this;
    }

    // constexpr bool operator>(const Frame& other) {
    //   return true; // TODO
    // }
  };
};