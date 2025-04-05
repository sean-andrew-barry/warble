import node.declaration.rvalue.literal.primitive.numeric.number;
import node.declaration.rvalue.literal.primitive.numeric.integer;
import node.declaration.rvalue.literal.primitive.numeric.decimal;
import node.declaration.rvalue.literal.structured.object;
import node.declaration.rvalue.literal.primitive.boolean;
import node.declaration.rvalue.literal.primitive.character;
import node.declaration.rvalue.literal.primitive.null;
import node.declaration.rvalue.literal.primitive.undefined;
import node.scope.global;
import code.x64.encoder;
import code.x64._register;
import code.x64.memory;
import utility.print;
import visitor;
import updater;

import <cstdint>;
import <limits>;
import <typeinfo>;
import <stdexcept>;

namespace node::declaration::rvalue::literal::primitive::numeric {
  Number::Number(const lexical::source::Span& span, node::Scope& scope, uint64_t value) noexcept
    : Numeric{span, scope}, value{value}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Number::TypeOf() const { return node::scope::Global::Number(); }

  void Number::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Number::Update(Updater& updater) {}

  void Number::Assign(code::x64::Encoder& encoder, node::Declaration& lhs) {
    switch (Size()) {
      case 8: {
        // auto reg = encoder.SelectGPR64();
        // encoder.MOV_R64_M64(reg, code::x64::Memory64{Displacement()});
        // encoder.MOV_M64_R64(code::x64::Memory64{lhs.Displacement()}, reg);
        encoder.MOV_R64_M64(code::x64::RAX, code::x64::Memory64{Displacement()});
        encoder.MOV_M64_R64(code::x64::Memory64{lhs.Displacement()}, code::x64::RAX);
        break;
      }
      case 4: {
        encoder.MOV_R32_M32(code::x64::EAX, code::x64::Memory32{Displacement()});
        encoder.MOV_M32_R32(code::x64::Memory32{lhs.Displacement()}, code::x64::EAX);
        break;
      }
      case 2: {
        encoder.MOV_R16_M16(code::x64::AX, code::x64::Memory16{Displacement()});
        encoder.MOV_M16_R16(code::x64::Memory16{lhs.Displacement()}, code::x64::AX);
        break;
      }
      case 1: {
        encoder.MOV_R8_M8(code::x64::AL, code::x64::Memory8{Displacement()});
        encoder.MOV_M8_R8(code::x64::Memory8{lhs.Displacement()}, code::x64::AL);
        break;
      }
      default: {
        throw std::runtime_error("Unsupported size for numeric literal");
      }
    }
  }

  void Number::Construct(code::x64::Encoder& encoder) {
    switch (Size()) {
      case 8: {
        encoder.MOV_R64_IMM64(code::x64::RAX, value);
        encoder.MOV_M64_R64(code::x64::Memory64{Displacement()}, code::x64::RAX);
        break;
      }
      case 4: {
        encoder.MOV_M32_IMM32(code::x64::Memory32{Displacement()}, static_cast<uint32_t>(value));
        break;
      }
      case 2: {
        encoder.MOV_M16_IMM16(code::x64::Memory16{Displacement()}, static_cast<uint16_t>(value));
        break;
      }
      case 1: {
        encoder.MOV_M8_IMM8(code::x64::Memory8{Displacement()}, static_cast<uint8_t>(value));
        break;
      }
      default: {
        throw std::runtime_error("Unsupported size for numeric literal");
      }
    }
  }

  void Number::Destruct(code::x64::Encoder& encoder) {
  }

  // void Number::Add(code::x64::Encoder& encoder, node::Declaration& rhs) {
  //   auto& type = typeid(rhs);

  //   if (type == typeid(Number)) {
      
  //   } else if (type == typeid(Integer)) {

  //   } else if (type == typeid(Decimal)) {

  //   }
  // }

  void Number::Add(code::x64::Encoder& encoder, node::Declaration& rhs) {
    // Ensure rhs is of type Number
    if (typeid(rhs) != typeid(Number)) {
      throw std::runtime_error("Invalid operation: Addition with different types is not supported.");
    }

    // Load lhs and rhs values into registers
    encoder.MOV_R64_M64(code::x64::RAX, code::x64::Memory64{Displacement()});
    encoder.MOV_R64_M64(code::x64::RCX, code::x64::Memory64{rhs.Displacement()});
    
    // Perform the addition
    encoder.ADD_R64_R64(code::x64::RAX, code::x64::RCX);
    
    // Store the result back into memory
    encoder.MOV_M64_R64(code::x64::Memory64{Displacement()}, code::x64::RAX);
  }

  void Number::Subtract(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Divide(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Multiply(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Modulo(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Not(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Negate(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::And(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Or(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Xor(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Compare(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Move(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Exchange(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Convert(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::RotateLeft(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::RotateRight(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::ShiftLeft(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::ShiftRight(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Increment(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Decrement(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Call(code::x64::Encoder& encoder, node::Declaration& rhs) {

  }

  void Number::Compile(code::x64::Encoder& encoder) {

  }

  int32_t Number::Size() const {
    return 8;

    if (value <= std::numeric_limits<uint8_t>::max()) {
      return 1; // Fits in 1 byte
    } else if (value <= std::numeric_limits<uint16_t>::max()) {
      return 2; // Fits in 2 bytes
    } else if (value <= std::numeric_limits<uint32_t>::max()) {
      return 4; // Fits in 4 bytes
    } else {
      return 8; // Fits in 8 bytes (full range of uint64_t)
    }
  }

  std::string Number::Name() const { return utility::Join("Number(", value, ")"); }
  // std::string Number::Name() const { return utility::Join("Number(", value, ")"); }
  // std::string Number::ToString() const { return Name(); }
  std::string Number::ToString() const { return utility::Line(Name()); }
  std::vector<std::type_index> Number::Signature() const { return {typeid(*this)}; }
};