export module code.placeholder;

import <cstdint>;

namespace code {
  export class Encoder;

  export template<typename T>
  class Placeholder {
  public:
    static constexpr size_t Size = sizeof(T);
  protected:
    size_t index;
    int8_t offset;
    uint8_t shift;
  public:
    Placeholder(size_t index)
      : index{index - Size}, offset{0}, shift{0} {}

    Placeholder(size_t index, int8_t offset, uint8_t shift)
      : index{index - Size}, offset{offset}, shift{shift} {}

    size_t Index() const { return index; }
    uint8_t Shift() const { return shift; }
  };

  export struct Placeholder8 : public Placeholder<int8_t> {
    using Placeholder::Placeholder;
    void Emit(code::Encoder& encoder, int8_t value);
  };

  export struct Placeholder16 : public Placeholder<int16_t> {
    using Placeholder::Placeholder;
    void Emit(code::Encoder& encoder, int16_t value);
  };

  export struct Placeholder32 : public Placeholder<int32_t> {
    using Placeholder::Placeholder;
    void Emit(code::Encoder& encoder, int32_t value);
  };

  export struct Placeholder64 : public Placeholder<int64_t> {
    using Placeholder::Placeholder;
    void Emit(code::Encoder& encoder, int64_t value);
  };
};