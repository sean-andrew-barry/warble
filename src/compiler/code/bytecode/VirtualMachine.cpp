import code.bytecode.virtual_machine;
import code.bytecode;
import code.bytecode.instructions;
import code.bytecodes;
import utility.print;

import <cstddef>;
import <array>;
import <vector>;
import <string>;
import <stdexcept>;
import <type_traits>;
import <algorithm>;

namespace code::bytecode {
  void VirtualMachine::NextExtended() {
    throw std::runtime_error("Extended instruction set isn't implemented yet");
  }

  void VirtualMachine::Next() {
    // Encode<code::bytecode::Load8>(code::Registers::RBP, code::Registers::RSP, 0);

    switch (reinterpret_cast<const code::Bytecode&>(*ip).Type()) {
      case code::bytecode::Extended::Type(): return NextExtended(); // Special handling for any instructions beyond 256 (currently none)
      case code::bytecode::NoOperation::Type(): return Decode<code::bytecode::NoOperation>();
      case code::bytecode::DirectJump::Type(): return Decode<code::bytecode::DirectJump>();
      case code::bytecode::ShortJump::Type(): return Decode<code::bytecode::ShortJump>();
      case code::bytecode::NearJump::Type(): return Decode<code::bytecode::NearJump>();
      case code::bytecode::JumpIfEqual::Type(): return Decode<code::bytecode::JumpIfEqual>();
      case code::bytecode::JumpIfZero::Type(): return Decode<code::bytecode::JumpIfZero>();
      case code::bytecode::JumpIfNotEqual::Type(): return Decode<code::bytecode::JumpIfNotEqual>();
      case code::bytecode::JumpIfNotZero::Type(): return Decode<code::bytecode::JumpIfNotZero>();
      case code::bytecode::JumpIfGreater::Type(): return Decode<code::bytecode::JumpIfGreater>();
      case code::bytecode::JumpIfGreaterOrEqual::Type(): return Decode<code::bytecode::JumpIfGreaterOrEqual>();
      case code::bytecode::JumpIfLess::Type(): return Decode<code::bytecode::JumpIfLess>();
      case code::bytecode::JumpIfLessOrEqual::Type(): return Decode<code::bytecode::JumpIfLessOrEqual>();
      case code::bytecode::JumpIfSign::Type(): return Decode<code::bytecode::JumpIfSign>();
      case code::bytecode::JumpIfNotSign::Type(): return Decode<code::bytecode::JumpIfNotSign>();
      case code::bytecode::JumpIfAbove::Type(): return Decode<code::bytecode::JumpIfAbove>();
      case code::bytecode::JumpIfAboveOrEqual::Type(): return Decode<code::bytecode::JumpIfAboveOrEqual>();
      case code::bytecode::JumpIfBelow::Type(): return Decode<code::bytecode::JumpIfBelow>();
      case code::bytecode::JumpIfBelowOrEqual::Type(): return Decode<code::bytecode::JumpIfBelowOrEqual>();
      case code::bytecode::Push::Type(): return Decode<code::bytecode::Push>();
      case code::bytecode::Pop::Type(): return Decode<code::bytecode::Pop>();
      case code::bytecode::Allocate::Type(): return Decode<code::bytecode::Allocate>();
      case code::bytecode::AllocateLarge::Type(): return Decode<code::bytecode::AllocateLarge>();
      case code::bytecode::Deallocate::Type(): return Decode<code::bytecode::Deallocate>();
      case code::bytecode::DeallocateLarge::Type(): return Decode<code::bytecode::DeallocateLarge>();
      case code::bytecode::Call::Type(): return Decode<code::bytecode::Call>();
      case code::bytecode::Return::Type(): return Decode<code::bytecode::Return>();
      case code::bytecode::Load8::Type(): return Decode<code::bytecode::Load8>();
      case code::bytecode::Load16::Type(): return Decode<code::bytecode::Load16>();
      case code::bytecode::Load32::Type(): return Decode<code::bytecode::Load32>();
      case code::bytecode::Load64::Type(): return Decode<code::bytecode::Load64>();
      case code::bytecode::Store8::Type(): return Decode<code::bytecode::Store8>();
      case code::bytecode::Store16::Type(): return Decode<code::bytecode::Store16>();
      case code::bytecode::Store32::Type(): return Decode<code::bytecode::Store32>();
      case code::bytecode::Store64::Type(): return Decode<code::bytecode::Store64>();
      case code::bytecode::StoreImmediate8::Type(): return Decode<code::bytecode::StoreImmediate8>();
      case code::bytecode::StoreImmediate16::Type(): return Decode<code::bytecode::StoreImmediate16>();
      case code::bytecode::StoreImmediate32::Type(): return Decode<code::bytecode::StoreImmediate32>();
      case code::bytecode::StoreImmediate64::Type(): return Decode<code::bytecode::StoreImmediate64>();
      case code::bytecode::Move::Type(): return Decode<code::bytecode::Move>();
      case code::bytecode::AddU8::Type(): return Decode<code::bytecode::AddU8>();
      case code::bytecode::AddU16::Type(): return Decode<code::bytecode::AddU16>();
      case code::bytecode::AddU32::Type(): return Decode<code::bytecode::AddU32>();
      case code::bytecode::AddU64::Type(): return Decode<code::bytecode::AddU64>();
      case code::bytecode::AddI8::Type(): return Decode<code::bytecode::AddI8>();
      case code::bytecode::AddI16::Type(): return Decode<code::bytecode::AddI16>();
      case code::bytecode::AddI32::Type(): return Decode<code::bytecode::AddI32>();
      case code::bytecode::AddI64::Type(): return Decode<code::bytecode::AddI64>();
      case code::bytecode::AddF32::Type(): return Decode<code::bytecode::AddF32>();
      case code::bytecode::AddF64::Type(): return Decode<code::bytecode::AddF64>();
      case code::bytecode::SubtractU8::Type(): return Decode<code::bytecode::SubtractU8>();
      case code::bytecode::SubtractU16::Type(): return Decode<code::bytecode::SubtractU16>();
      case code::bytecode::SubtractU32::Type(): return Decode<code::bytecode::SubtractU32>();
      case code::bytecode::SubtractU64::Type(): return Decode<code::bytecode::SubtractU64>();
      case code::bytecode::SubtractI8::Type(): return Decode<code::bytecode::SubtractI8>();
      case code::bytecode::SubtractI16::Type(): return Decode<code::bytecode::SubtractI16>();
      case code::bytecode::SubtractI32::Type(): return Decode<code::bytecode::SubtractI32>();
      case code::bytecode::SubtractI64::Type(): return Decode<code::bytecode::SubtractI64>();
      case code::bytecode::SubtractF32::Type(): return Decode<code::bytecode::SubtractF32>();
      case code::bytecode::SubtractF64::Type(): return Decode<code::bytecode::SubtractF64>();
      case code::bytecode::MultiplyU8::Type(): return Decode<code::bytecode::MultiplyU8>();
      case code::bytecode::MultiplyU16::Type(): return Decode<code::bytecode::MultiplyU16>();
      case code::bytecode::MultiplyU32::Type(): return Decode<code::bytecode::MultiplyU32>();
      case code::bytecode::MultiplyU64::Type(): return Decode<code::bytecode::MultiplyU64>();
      case code::bytecode::MultiplyI8::Type(): return Decode<code::bytecode::MultiplyI8>();
      case code::bytecode::MultiplyI16::Type(): return Decode<code::bytecode::MultiplyI16>();
      case code::bytecode::MultiplyI32::Type(): return Decode<code::bytecode::MultiplyI32>();
      case code::bytecode::MultiplyI64::Type(): return Decode<code::bytecode::MultiplyI64>();
      case code::bytecode::MultiplyF32::Type(): return Decode<code::bytecode::MultiplyF32>();
      case code::bytecode::MultiplyF64::Type(): return Decode<code::bytecode::MultiplyF64>();
      case code::bytecode::DivideU8::Type(): return Decode<code::bytecode::DivideU8>();
      case code::bytecode::DivideU16::Type(): return Decode<code::bytecode::DivideU16>();
      case code::bytecode::DivideU32::Type(): return Decode<code::bytecode::DivideU32>();
      case code::bytecode::DivideU64::Type(): return Decode<code::bytecode::DivideU64>();
      case code::bytecode::DivideI8::Type(): return Decode<code::bytecode::DivideI8>();
      case code::bytecode::DivideI16::Type(): return Decode<code::bytecode::DivideI16>();
      case code::bytecode::DivideI32::Type(): return Decode<code::bytecode::DivideI32>();
      case code::bytecode::DivideI64::Type(): return Decode<code::bytecode::DivideI64>();
      case code::bytecode::DivideF32::Type(): return Decode<code::bytecode::DivideF32>();
      case code::bytecode::DivideF64::Type(): return Decode<code::bytecode::DivideF64>();
      case code::bytecode::ModuloU8::Type(): return Decode<code::bytecode::ModuloU8>();
      case code::bytecode::ModuloU16::Type(): return Decode<code::bytecode::ModuloU16>();
      case code::bytecode::ModuloU32::Type(): return Decode<code::bytecode::ModuloU32>();
      case code::bytecode::ModuloU64::Type(): return Decode<code::bytecode::ModuloU64>();
      case code::bytecode::ModuloI8::Type(): return Decode<code::bytecode::ModuloI8>();
      case code::bytecode::ModuloI16::Type(): return Decode<code::bytecode::ModuloI16>();
      case code::bytecode::ModuloI32::Type(): return Decode<code::bytecode::ModuloI32>();
      case code::bytecode::ModuloI64::Type(): return Decode<code::bytecode::ModuloI64>();
      case code::bytecode::ModuloF32::Type(): return Decode<code::bytecode::ModuloF32>();
      case code::bytecode::ModuloF64::Type(): return Decode<code::bytecode::ModuloF64>();
      case code::bytecode::NegateI8::Type(): return Decode<code::bytecode::NegateI8>();
      case code::bytecode::NegateI32::Type(): return Decode<code::bytecode::NegateI32>();
      case code::bytecode::NegateI16::Type(): return Decode<code::bytecode::NegateI16>();
      case code::bytecode::NegateI64::Type(): return Decode<code::bytecode::NegateI64>();
      case code::bytecode::NegateF32::Type(): return Decode<code::bytecode::NegateF32>();
      case code::bytecode::NegateF64::Type(): return Decode<code::bytecode::NegateF64>();
      case code::bytecode::EqualU64::Type(): return Decode<code::bytecode::EqualU64>();
      case code::bytecode::EqualF32::Type(): return Decode<code::bytecode::EqualF32>();
      case code::bytecode::EqualF64::Type(): return Decode<code::bytecode::EqualF64>();
      case code::bytecode::NotEqualU64::Type(): return Decode<code::bytecode::NotEqualU64>();
      case code::bytecode::NotEqualF32::Type(): return Decode<code::bytecode::NotEqualF32>();
      case code::bytecode::NotEqualF64::Type(): return Decode<code::bytecode::NotEqualF64>();
      case code::bytecode::Greater::Type(): return Decode<code::bytecode::Greater>();
      case code::bytecode::Lesser::Type(): return Decode<code::bytecode::Lesser>();
      case code::bytecode::GreaterOrEqual::Type(): return Decode<code::bytecode::GreaterOrEqual>();
      case code::bytecode::LesserOrEqual::Type(): return Decode<code::bytecode::LesserOrEqual>();
      case code::bytecode::LogicalAnd::Type(): return Decode<code::bytecode::LogicalAnd>();
      case code::bytecode::LogicalOr::Type(): return Decode<code::bytecode::LogicalOr>();
      case code::bytecode::LogicalNot::Type(): return Decode<code::bytecode::LogicalNot>();
      case code::bytecode::BitwiseAnd::Type(): return Decode<code::bytecode::BitwiseAnd>();
      case code::bytecode::BitwiseOr::Type(): return Decode<code::bytecode::BitwiseOr>();
      case code::bytecode::BitwiseExclusiveOr::Type(): return Decode<code::bytecode::BitwiseExclusiveOr>();
      case code::bytecode::BitwiseNot::Type(): return Decode<code::bytecode::BitwiseNot>();
      default: throw std::runtime_error("Invalid instruction code " + std::to_string(static_cast<uint8_t>(*ip)));
    }
  }

  void VirtualMachine::Run() {
    ip = code.begin();

    while (ip != code.end()) {
      // Print("Executing");
      Next();
      // break;
    }
  }
};