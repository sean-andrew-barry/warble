export module code.x64.immediate;

// import code.immediate;
import code.x64.operand;
import <cstdint>;
import <stdexcept>;
import <algorithm>;
import <concepts>;

export namespace code::x64 {
  template<typename T>
  struct Immediate : public Operand {
  private:
    T value;
  public:
    // explicit Immediate(auto val) : value{static_cast<T>(val)} {}
    // Immediate(uint8_t val) : value{static_cast<T>(val)} {}
    // Immediate(uint16_t val) : value{static_cast<T>(val)} {}
    // Immediate(uint32_t val) : value{static_cast<T>(val)} {}
    // Immediate(uint64_t val) : value{static_cast<T>(val)} {}
    // Immediate(int8_t val) : value{static_cast<T>(val)} {}
    // Immediate(int16_t val) : value{static_cast<T>(val)} {}
    // Immediate(int32_t val) : value{static_cast<T>(val)} {}
    // Immediate(int64_t val) : value{static_cast<T>(val)} {}
    Immediate(T val) : value{val} {}
    // Immediate(code::Immediate i) : value{static_cast<T>(i.Value())} {}

    T Value() const { return value; }

    explicit operator T() const { return value; }

    // std::string ToString() const { return utility::Join("Memory8(", this->base, ", ", this->index, ", ", this->scale, ", ", this->displacement, ")"); }
  };

  struct Immediate8 : public Immediate<uint8_t> { using Immediate::Immediate; };
  struct Immediate16 : public Immediate<uint16_t> { using Immediate::Immediate; };
  struct Immediate32 : public Immediate<uint32_t> { using Immediate::Immediate; };
  struct Immediate64 : public Immediate<uint64_t> { using Immediate::Immediate; };

  struct SignedImmediate8 : public Immediate<int8_t> { using Immediate::Immediate; };
  struct SignedImmediate16 : public Immediate<int16_t> { using Immediate::Immediate; };
  struct SignedImmediate32 : public Immediate<int32_t> { using Immediate::Immediate; };
  struct SignedImmediate64 : public Immediate<int64_t> { using Immediate::Immediate; };

  template<typename T>
  concept IsImmediate = std::same_as<Immediate8, T>
       || std::same_as<Immediate16, T>
       || std::same_as<Immediate32, T>
       || std::same_as<Immediate64, T>
       || std::same_as<SignedImmediate8, T>
       || std::same_as<SignedImmediate16, T>
       || std::same_as<SignedImmediate32, T>
       || std::same_as<SignedImmediate64, T>;
};