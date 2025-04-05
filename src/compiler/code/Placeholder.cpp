import code.placeholder;
import code.encoder;

namespace code {
  void Placeholder8::Emit(code::Encoder& encoder, int8_t value) { encoder.Emit8(value + offset, index, shift); }
  void Placeholder16::Emit(code::Encoder& encoder, int16_t value) { encoder.Emit16(value + offset, index, shift); }
  void Placeholder32::Emit(code::Encoder& encoder, int32_t value) { encoder.Emit32(value + offset, index, shift); }
  void Placeholder64::Emit(code::Encoder& encoder, int64_t value) { encoder.Emit64(value + offset, index, shift); }
};