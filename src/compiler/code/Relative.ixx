export module code.relative;

import <cstdint>;
import <limits>;

namespace code {
  export template<typename T>
  class Relative {
  public:
    static constexpr size_t Size = sizeof(T);
  private:
    size_t index;
    T value;
  public:
    constexpr Relative() = default;
    constexpr Relative(T value) : index{std::numeric_limits<size_t>::max()}, value{value} {}

    constexpr void Index(size_t v) { index = v; }
    constexpr size_t Index() const { return index; }

    constexpr void Value(T v) { value = v; }
    constexpr T Value() const { return value; }

    constexpr explicit operator bool() const { return index != std::numeric_limits<size_t>::max(); }
  };

  export struct Relative8 : public Relative<int8_t> {
    using Relative::Relative;
  };

  export struct Relative16 : public Relative<int16_t> {
    using Relative::Relative;
  };

  export struct Relative32 : public Relative<int32_t> {
    using Relative::Relative;
  };

  export struct Relative64 : public Relative<int64_t> {
    using Relative::Relative;
  };
};