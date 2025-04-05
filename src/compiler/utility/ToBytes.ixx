export module utility.to_bytes;

import utility.print;

import <algorithm>;
import <array>;
import <concepts>;
import <cstdint>;
import <format>;
import <functional>;
import <iomanip>;
import <iostream>;
import <locale>;
import <ranges>;
import <span>;
import <string_view>;
import <string>;
import <tuple>;
import <type_traits>;
import <typeinfo>;
import <utility>;
import <vector>;
import <cstddef>;
import <bit>;

// decltype(auto) ToBytes(auto value) {
//   std::span<std::byte, sizeof(value)> bytes{reinterpret_cast<std::byte*>(&value), sizeof(value)};
//   return bytes;
// }

// decltype(auto) Cast(auto value) {
//   return std::span<std::byte, sizeof(value)>{reinterpret_cast<std::byte*>(&value), sizeof(value)};
// }

// decltype(const auto) Cast(const auto value) {
//   return std::span<const std::byte, sizeof(value)>{reinterpret_cast<const std::byte*>(&value), sizeof(value)};
// }

// template<typename T, size_t S>
// T From(std::span<std::byte, S> value) {
//   return *reinterpret_cast<T*>(value.data());
// }

template<typename T, size_t S>
T From(std::span<std::byte, S> value) {
  static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
  static_assert(sizeof(T) == S, "Size of T must match the span size");
  return std::bit_cast<T>(*reinterpret_cast<const T*>(value.data()));
}

template<typename T>
concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

export namespace utility {
  // std::span<std::byte, 1> ToBytes(bool v) { return Cast(v); }
  // std::span<std::byte, 1> ToBytes(int8_t v) { return Cast(v); }
  // std::span<std::byte, 1> ToBytes(uint8_t v) { return Cast(v); }
  // std::span<std::byte, 2> ToBytes(int16_t v) { return Cast(v); }
  // std::span<std::byte, 2> ToBytes(uint16_t v) { return Cast(v); }
  // std::span<std::byte, 4> ToBytes(int32_t v) { return Cast(v); }
  // std::span<std::byte, 4> ToBytes(uint32_t v) { return Cast(v); }
  // std::span<std::byte, 4> ToBytes(float v) { return Cast(v); }
  // std::span<std::byte, 8> ToBytes(int64_t v) { return Cast(v); }
  // std::span<std::byte, 8> ToBytes(uint64_t v) { return Cast(v); }
  // std::span<std::byte, 8> ToBytes(double v) { return Cast(v); }

  template<TriviallyCopyable T, size_t S = sizeof(T)>
  constexpr auto ToBytes(T& value) {
    return std::span<std::byte, S>(reinterpret_cast<std::byte*>(&value), S);
  }

  template<TriviallyCopyable T, size_t S = sizeof(T)>
  constexpr auto ToBytes(const T& value) {
    return std::span<const std::byte, S>(reinterpret_cast<const std::byte*>(&value), S);
  }

  template<TriviallyCopyable T>
  constexpr auto ToBytes1(const T& value) { return ToBytes<T, 1>(static_cast<const uint8_t&>(value)); }

  template<TriviallyCopyable T>
  constexpr auto ToBytes2(const T& value) { return ToBytes<T, 2>(static_cast<const uint16_t&>(value)); }

  template<TriviallyCopyable T>
  constexpr auto ToBytes4(const T& value) { return ToBytes<T, 4>(static_cast<const uint32_t&>(value)); }

  template<TriviallyCopyable T>
  constexpr auto ToBytes8(const T& value) { return ToBytes<T, 8>(static_cast<const uint64_t&>(value)); }

  // constexpr auto ToBytes1(const bool& v) { return ToBytes<T, 1>(v); }
  // constexpr auto ToBytes1(const int8_t& v) { return ToBytes<T, 1>(v); }
  // constexpr auto ToBytes1(const uint8_t& v) { return ToBytes<T, 1>(v); }
  // constexpr auto ToBytes2(const int16_t& v) { return ToBytes<T, 2>(v); }
  // constexpr auto ToBytes2(const uint16_t& v) { return ToBytes<T, 2>(v); }
  // constexpr auto ToBytes4(const int32_t& v) { return ToBytes<T, 4>(v); }
  // constexpr auto ToBytes4(const uint32_t& v) { return ToBytes<T, 4>(v); }
  // constexpr auto ToBytes4(const float& v) { return ToBytes<T, 4>(v); }
  // constexpr auto ToBytes8(const int64_t& v) { return ToBytes<T, 8>(v); }
  // constexpr auto ToBytes8(const uint64_t& v) { return ToBytes<T, 8>(v); }
  // constexpr auto ToBytes8(const double& v) { return ToBytes<T, 8>(v); }

  // std::span<std::byte> ToBytes(std::string_view v) { return std::as_bytes(std::span{v.data(), v.size()}); }
  // std::span<std::byte> ToBytes(std::string& v) { return std::as_bytes(std::span{v.data(), v.size()}); }

  // template<typename T>
  // std::span<std::byte> ToBytes(std::vector<T>& v) { return std::as_bytes(std::span{v.data(), v.size()}); }

  // template<typename T, size_t S>
  // std::span<std::byte> ToBytes(std::array<T, S>& v) { return std::as_bytes(std::span{v.data(), v.size()}); }

  constexpr auto ToBytes(std::ranges::contiguous_range auto& v) { return std::as_bytes(std::span{v}); }
  // std::span<std::byte> ToBytes(std::ranges::range auto& v) { return std::as_bytes(std::span{std::ranges::data(v), std::ranges::size(v)}); }

  bool ToBool(std::span<std::byte, 1> v) { return From<bool>(v); }
  int8_t ToInt8(std::span<std::byte, 1> v) { return From<int8_t>(v); }
  uint8_t ToUint8(std::span<std::byte, 1> v) { return From<uint8_t>(v); }
  int16_t ToInt16(std::span<std::byte, 2> v) { return From<int16_t>(v); }
  uint16_t ToUint16(std::span<std::byte, 2> v) { return From<uint16_t>(v); }
  int32_t ToInt32(std::span<std::byte, 4> v) { return From<int32_t>(v); }
  uint32_t ToUint32(std::span<std::byte, 4> v) { return From<uint32_t>(v); }
  float ToFloat(std::span<std::byte, 4> v) { return From<float>(v); }
  int64_t ToInt64_t(std::span<std::byte, 8> v) { return From<int64_t>(v); }
  uint64_t ToUint64_t(std::span<std::byte, 8> v) { return From<uint64_t>(v); }
  double ToDouble(std::span<std::byte, 8> v) { return From<double>(v); }
};

// bool Test() {
//   double d{1.5};
//   auto result_d = utility::ToBytes(d);
//   utility::Print(result_d, utility::ToDouble(result_d), result_d.extent == std::dynamic_extent);
//   // utility::Print(result_d, d);

//   std::string s{"Hello world"};
//   auto result_s = utility::ToBytes(s);
//   utility::Print(result_s, result_s.extent == std::dynamic_extent);

//   return false;
// }

// auto result = Test();