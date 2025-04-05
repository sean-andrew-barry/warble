import node.declaration.literal.primitive.numeric.decimal;
import code.x64.encoder;
import visitor;
import updater;
import utility.print;

import <cstdint>;
import <limits>;
import <cmath>;

namespace node::declaration::literal::primitive::numeric {
  Decimal::Decimal(const lexical::source::Span& span, node::Scope& scope, double value) noexcept
    : Numeric{span, scope}, value{value}
  {}

  void Decimal::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Decimal::Update(Updater& updater) {}

  int32_t Decimal::Size() const {
    return 8;

    // Check if the value can be represented as a float without losing precision
    float f = static_cast<float>(value);

    if (std::fabs(f - value) <= std::numeric_limits<float>::epsilon() * std::fabs(f)) {
      return 4; // Can be represented as 32-bit float
    } else {
      return 8; // Requires 64-bit double to maintain precision
    }
  }

  void Decimal::Construct(code::x64::Encoder& encoder) {
    switch (Size()) {
      case 8: {
        // Move the double value into a general-purpose register, then to an XMM register, and then to memory
        uint64_t intValue;
        std::memcpy(&intValue, &value, sizeof(double));  // Bitwise copy of the double value to an integer representation
        encoder.MOV_R64_IMM64(code::x64::RAX, intValue);
        encoder.MOVQ_XMM_R64(code::x64::XMM0, code::x64::RAX);
        encoder.MOVSD_M64_XMM(code::x64::Memory64{Displacement()}, code::x64::XMM0);
        break;
      }
      case 4: {
        // Move the float value into a general-purpose register, then to an XMM register, and then to memory
        uint32_t intValue;
        float floatValue = static_cast<float>(value);
        std::memcpy(&intValue, &floatValue, sizeof(float));  // Bitwise copy of the float value to an integer representation
        encoder.MOV_R32_IMM32(code::x64::EAX, intValue);
        encoder.MOVD_XMM_R32(code::x64::XMM0, code::x64::EAX);
        encoder.MOVSS_M32_XMM(code::x64::Memory32{Displacement()}, code::x64::XMM0);
        break;
      }
      default: {
        throw std::runtime_error("Unsupported size for decimal literal");
      }
    }
  }

  std::string Decimal::Name() const { return utility::Join("Decimal(", value, ")"); }
  std::string Decimal::ToString() const { return utility::Line(Name()); }
  std::vector<std::type_index> Decimal::Signature() const { return {typeid(*this)}; }
};