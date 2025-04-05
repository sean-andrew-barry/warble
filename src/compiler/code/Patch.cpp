import code.patch;
import code.address;
import code.encoder;

import <cstdint>;
import <vector>;

namespace code {
  Patch::Patch(const code::Address& target, uint64_t index, uint8_t size, uint8_t shift) noexcept
    : target{target}, index{index}, size{size}, shift{shift} {}

  void Patch::Apply(code::Encoder& encoder) {
    // auto position = source.Position();
    // encoder.Emit();
    // uint32_t source_offset = source.AbsoluteOffset();
    // uint32_t target_offset = target.AbsoluteOffset();

    // int32_t displacement = static_cast<int32_t>(target_offset - (source_offset + index + size));

    // // Convert displacement to bytes and insert it into the code buffer
    // for (uint32_t i = 0; i < size; ++i) {
    //   code[source_offset + index + i] = std::byte((displacement >> (i * 8)) & 0xFF);
    // }
  }
};