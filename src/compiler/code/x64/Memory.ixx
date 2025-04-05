export module code.x64.memory;

import code.x64.operand;
import code.x64._register;
import code.address;
import utility.print;
import <cstdint>;
import <stdexcept>;
import <algorithm>;
import <concepts>;
import <string>;
// import <optional>;

export namespace code::x64 {
  // Effective Address = base + (index * scale) + displacement
  template<IsGPR B = SGPR64, IsGPR I = B>
  class Memory : public Operand {
  public:
    using BaseGPR = B;
    using IndexGPR = I;
  protected:
    B base;
    I index;
    int32_t displacement;
    uint8_t scale; // Scale must be one of {1, 2, 4, 8}
  public:
    // using Memory<B, I>::Memory;

    Memory(int32_t displacement)
      : base{RBP}, index{}, scale{1}, displacement{displacement}
    {}

    Memory(B base, int32_t displacement)
      : base{base}, index{}, scale{1}, displacement{displacement}
    {}

    Memory(B base, I index, uint8_t scale, int32_t displacement)
      : base{base}, index{index}, scale{scale}, displacement{displacement}
    {
      if (scale != 1 && scale != 2 && scale != 4 && scale != 8) {
        throw std::invalid_argument("Scale must be one of 1, 2, 4, or 8");
      }
    }

    Memory(uint32_t displacement)
      : base{RBP}, index{}, scale{1}, displacement{static_cast<int32_t>(displacement)}
    {}

    Memory(B base, uint32_t displacement)
      : base{base}, index{}, scale{1}, displacement{static_cast<int32_t>(displacement)}
    {}

    Memory(B base, I index, uint8_t scale, uint32_t displacement)
      : base{base}, index{index}, scale{scale}, displacement{static_cast<int32_t>(displacement)}
    {
      if (scale != 1 && scale != 2 && scale != 4 && scale != 8) {
        throw std::invalid_argument("Scale must be one of 1, 2, 4, or 8");
      }
    }

    B Base() const { return base; }
    I Index() const { return index; }
    uint8_t Scale() const { return scale; }
    int32_t Displacement() const { return displacement; }

    // Function to return the log base 2 of the scale factor for SIB encoding
    uint8_t ScaleLog2() const {
      switch (scale) {
        case 1: return 0; // 2^0 = 1
        case 2: return 1; // 2^1 = 2
        case 4: return 2; // 2^2 = 4
        case 8: return 3; // 2^3 = 8
        default: throw std::runtime_error("Invalid scale factor");
      }
    }
  };

  template<IsGPR B = SGPR64, IsGPR I = B>
  struct Memory8 : public Memory<B, I> {
    using Memory<B, I>::Memory;

    std::string ToString() const { return utility::Join("Memory8(", this->base, ", ", this->index, ", ", this->scale, ", ", this->displacement, ")"); }
  };

  template<IsGPR B = SGPR64, IsGPR I = B>
  struct Memory16 : public Memory<B, I> {
    using Memory<B, I>::Memory;

    std::string ToString() const { return utility::Join("Memory16(", this->base, ", ", this->index, ", ", this->scale, ", ", this->displacement, ")"); }
  };

  template<IsGPR B = SGPR64, IsGPR I = B>
  struct Memory32 : public Memory<B, I> {
    using Memory<B, I>::Memory;

    std::string ToString() const { return utility::Join("Memory32(", this->base, ", ", this->index, ", ", this->scale, ", ", this->displacement, ")"); }
  };

  template<IsGPR B = SGPR64, IsGPR I = B>
  struct Memory64 : public Memory<B, I> {
    using Memory<B, I>::Memory;

    std::string ToString() const { return utility::Join("Memory64(", this->base, ", ", this->index, ", ", this->scale, ", ", this->displacement, ")"); }
  };

  template<IsGPR B = SGPR64, IsGPR I = B>
  struct Memory128 : public Memory<B, I> {
    using Memory<B, I>::Memory;

    std::string ToString() const { return utility::Join("Memory128(", this->base, ", ", this->index, ", ", this->scale, ", ", this->displacement, ")"); }
  };

  template<typename T>
  concept IsMemory = requires(T t) {
    { t.Base() } -> std::convertible_to<typename T::BaseGPR>;
    { t.Index() } -> std::convertible_to<typename T::IndexGPR>;
    { t.Displacement() } -> std::convertible_to<int32_t>;
    { t.Scale() } -> std::convertible_to<uint8_t>;
  };

  template<typename T>
  concept IsMemory8 = IsMemory<T> && std::is_base_of_v<Memory8<typename T::BaseGPR, typename T::IndexGPR>, T> || std::same_as<T, code::Address>;

  template<typename T>
  concept IsMemory16 = IsMemory<T> && std::is_base_of_v<Memory16<typename T::BaseGPR, typename T::IndexGPR>, T> || std::same_as<T, code::Address>;

  template<typename T>
  concept IsMemory32 = IsMemory<T> && std::is_base_of_v<Memory32<typename T::BaseGPR, typename T::IndexGPR>, T> || std::same_as<T, code::Address>;

  template<typename T>
  concept IsMemory64 = IsMemory<T> && std::is_base_of_v<Memory64<typename T::BaseGPR, typename T::IndexGPR>, T> || std::same_as<T, code::Address>;

  template<typename T>
  concept IsMemory128 = IsMemory<T> && std::is_base_of_v<Memory128<typename T::BaseGPR, typename T::IndexGPR>, T> || std::same_as<T, code::Address>;
};