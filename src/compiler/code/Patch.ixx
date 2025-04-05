export module code.patch;

import <cstdint>;
import <vector>;

namespace code {
  export class Address;
  export class Encoder;

  export class Patch {
  private:
    const code::Address& target;
    uint64_t index; // The location in the instruction buffer where this address was written (for patching)
    // uint8_t offset; // The offset from the index to where the placeholder displacement is
    uint8_t size; // The size of the placeholder
    uint8_t shift; // The shift from the offset for sub-byte sizes
  public:
    Patch(const code::Address& target, uint64_t index, uint8_t size, uint8_t shift = 0) noexcept;

    void Apply(code::Encoder& encoder);
  };
};