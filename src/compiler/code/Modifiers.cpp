import code.modifiers;

import <cstdint>;
import <bitset>;

enum class Mods : uint8_t {
  // NONE,
  LET,
  CONST,
  MUTABLE,
  STATIC,
  PRIVATE,
  PUBLIC,
  PROTECTED,
  EXPORT,
  COMTIME,
  RUNTIME,
  CONST_FUNCTION,
  ASYNC_FUNCTION,
  CASE_FUNCTION,
};

namespace code {
  void Modifiers::Let(bool v) { modifiers.set(static_cast<uint8_t>(Mods::LET), v); }
  void Modifiers::Const(bool v) { modifiers.set(static_cast<uint8_t>(Mods::CONST), v); }
  void Modifiers::Mutable(bool v) { modifiers.set(static_cast<uint8_t>(Mods::MUTABLE), v); }
  void Modifiers::Static(bool v) { modifiers.set(static_cast<uint8_t>(Mods::STATIC), v); }
  void Modifiers::Private(bool v) { modifiers.set(static_cast<uint8_t>(Mods::PRIVATE), v); }
  void Modifiers::Public(bool v) { modifiers.set(static_cast<uint8_t>(Mods::PUBLIC), v); }
  void Modifiers::Protected(bool v) { modifiers.set(static_cast<uint8_t>(Mods::PROTECTED), v); }
  void Modifiers::Export(bool v) { modifiers.set(static_cast<uint8_t>(Mods::EXPORT), v); }
  void Modifiers::Comtime(bool v) { modifiers.set(static_cast<uint8_t>(Mods::COMTIME), v); }
  void Modifiers::Runtime(bool v) { modifiers.set(static_cast<uint8_t>(Mods::RUNTIME), v); }
  void Modifiers::ConstFunction(bool v) { modifiers.set(static_cast<uint8_t>(Mods::CONST_FUNCTION), v); }
  void Modifiers::AsyncFunction(bool v) { modifiers.set(static_cast<uint8_t>(Mods::ASYNC_FUNCTION), v); }
  void Modifiers::CaseFunction(bool v) { modifiers.set(static_cast<uint8_t>(Mods::CASE_FUNCTION), v); }

  bool Modifiers::Let() const { return modifiers.test(static_cast<uint8_t>(Mods::LET)); }
  bool Modifiers::Const() const { return modifiers.test(static_cast<uint8_t>(Mods::CONST)); }
  bool Modifiers::Mutable() const { return modifiers.test(static_cast<uint8_t>(Mods::MUTABLE)); }
  bool Modifiers::Static() const { return modifiers.test(static_cast<uint8_t>(Mods::STATIC)); }
  bool Modifiers::Private() const { return modifiers.test(static_cast<uint8_t>(Mods::PRIVATE)); }
  bool Modifiers::Public() const { return modifiers.test(static_cast<uint8_t>(Mods::PUBLIC)); }
  bool Modifiers::Protected() const { return modifiers.test(static_cast<uint8_t>(Mods::PROTECTED)); }
  bool Modifiers::Export() const { return modifiers.test(static_cast<uint8_t>(Mods::EXPORT)); }
  bool Modifiers::Comtime() const { return modifiers.test(static_cast<uint8_t>(Mods::COMTIME)); }
  bool Modifiers::Runtime() const { return modifiers.test(static_cast<uint8_t>(Mods::RUNTIME)); }
  bool Modifiers::ConstFunction() const { return modifiers.test(static_cast<uint8_t>(Mods::CONST_FUNCTION)); }
  bool Modifiers::AsyncFunction() const { return modifiers.test(static_cast<uint8_t>(Mods::ASYNC_FUNCTION)); }
  bool Modifiers::CaseFunction() const { return modifiers.test(static_cast<uint8_t>(Mods::CASE_FUNCTION)); }

  bool Modifiers::Any() const { return modifiers.any(); }
};