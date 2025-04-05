import node.declaration.rvalue.literal.primitive.numeric.integer;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import code.x64.encoder;
import visitor;
import updater;
import utility.print;

import <cstdint>;
import <limits>;

namespace node::declaration::rvalue::literal::primitive::numeric {
  Integer::Integer(const lexical::source::Span& span, node::Scope& scope, int64_t value) noexcept
    : Numeric{span, scope}, value{value}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Integer::TypeOf() const { return node::scope::Global::Integer(); }

  void Integer::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Integer::Update(Updater& updater) {}

  int32_t Integer::Size() const {
    return 8;

    if (value >= std::numeric_limits<int8_t>::min() && value <= std::numeric_limits<int8_t>::max()) {
      return 1; // Fits in 1 byte
    } else if (value >= std::numeric_limits<int16_t>::min() && value <= std::numeric_limits<int16_t>::max()) {
      return 2; // Fits in 2 bytes
    } else if (value >= std::numeric_limits<int32_t>::min() && value <= std::numeric_limits<int32_t>::max()) {
      return 4; // Fits in 4 bytes
    } else {
      return 8; // Assumes full range of int64_t
    }
  }

  void Integer::Construct(code::x64::Encoder& encoder) {
    switch (Size()) {
      case 8: {
        encoder.MOV_R64_IMM64(code::x64::RAX, value);
        encoder.MOV_M64_R64(code::x64::Memory64{Displacement()}, code::x64::RAX);
        break;
      }
      case 4: {
        encoder.MOV_M32_IMM32(code::x64::Memory32{Displacement()}, static_cast<int32_t>(value));
        break;
      }
      case 2: {
        encoder.MOV_M16_IMM16(code::x64::Memory16{Displacement()}, static_cast<int16_t>(value));
        break;
      }
      case 1: {
        encoder.MOV_M8_IMM8(code::x64::Memory8{Displacement()}, static_cast<int8_t>(value));
        break;
      }
      default: {
        throw std::runtime_error("Unsupported size for integer literal");
      }
    }
  }

  void Integer::Compile(code::x64::Encoder& encoder) {
    auto size = Size();
    // encoder.Write(this, size, value);

    // switch (size) {
    //   case 8: {
    //     encoder.Emit64(value);
    //     // encoder.MOV_R64_IMM64(code::x64::RAX, value);
    //     // encoder.MOV_M64_R64(code::x64::Memory64{Displacement()}, code::x64::RAX);
    //     break;
    //   }
    //   case 4: {
    //     encoder.MOV_M32_IMM32(code::x64::Memory32{Displacement()}, static_cast<int32_t>(value));
    //     break;
    //   }
    //   case 2: {
    //     encoder.MOV_M16_IMM16(code::x64::Memory16{Displacement()}, static_cast<int16_t>(value));
    //     break;
    //   }
    //   case 1: {
    //     encoder.MOV_M8_IMM8(code::x64::Memory8{Displacement()}, static_cast<int8_t>(value));
    //     break;
    //   }
    //   default: {
    //     throw std::runtime_error("Unsupported size for integer literal");
    //   }
    // }
  }

  std::string Integer::Name() const { return utility::Join("Integer(", value, ")"); }
  std::string Integer::ToString() const { return utility::Line(Name()); }
  std::vector<std::type_index> Integer::Signature() const { return {typeid(*this)}; }
};