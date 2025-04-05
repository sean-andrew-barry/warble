export module utility.buffer;

import utility.to_bytes;

import <cstdint>;
import <vector>;
import <span>;
import <ranges>;
import <bit>;
import <algorithm>;
import <limits>;

namespace utility {
  export class Buffer {
  private:
    std::vector<std::byte> bytes;
    std::endian endian;
  public:
    constexpr Buffer(std::endian endian) : bytes{}, endian{endian} {}
    constexpr Buffer(const Buffer& other) = default;
    constexpr Buffer& operator=(const Buffer& other) = default;

    constexpr void Reserve(size_t size) { bytes.reserve(size); }
    constexpr size_t Size() const { return bytes.size(); }
    constexpr size_t Capacity() const { return bytes.capacity(); }
    constexpr const std::vector<std::byte>& Bytes() const { return bytes; }

    // Endianness can be ignored for a single byte
    void Emit(std::byte byte) { bytes.push_back(byte); }
    void Emit(std::byte byte, size_t index) { bytes[index] = byte; }

    void Emit(std::byte byte, size_t index, uint8_t shift) {
      if (shift >= 8) return; // No operation needed if shift is 8 or more
      std::byte mask = static_cast<std::byte>((1 << shift) - 1);
      bytes[index] = (bytes[index] & ~mask) | (byte & mask);
    }

    template<size_t S>
    void Emit(std::span<std::byte, S> data) {
      if (std::endian::native == endian) {
        bytes.append_range(data);
      } else {
        bytes.append_range(data | std::views::reverse);
      }
    }

    template<size_t S>
    void Emit(std::span<std::byte, S> data, size_t index) {
      if (std::endian::native == endian) {
        std::ranges::copy(data.cbegin(), data.cend(), bytes.begin() + index);
      } else {
        std::ranges::copy_backward(data.cbegin(), data.cend(), bytes.begin() + index + data.size());
      }
    }

    template<size_t S>
    void Emit(std::span<std::byte, S> data, size_t index, uint8_t shift) {
      if (shift == 0) {
        // A shift of zero has no effect, so we can ignore it
        return Emit<S>(data, index);
      }

      if (std::endian::native == endian) {
        // Apply shift to the first byte if endianness matches the native endianness
        Emit(data[0], index, shift);
        auto rest = data.last<data.size() - 1>(); // Process the rest of the span after the first byte
        ++index;

        if (std::endian::native == endian) {
          std::ranges::copy(rest.cbegin(), rest.cend(), bytes.begin() + index);
        } else {
          std::ranges::copy_backward(rest.cbegin(), rest.cend(), bytes.begin() + index + rest.size());
        }
      } else {
        // Apply shift to the last byte if endianness does not match the native endianness
        Emit(data[data.size() - 1], index + data.size() - 1, shift);
        auto rest = data.first<data.size() - 1>(); // Process all but the last byte

        if (std::endian::native == endian) {
          std::ranges::copy(rest.cbegin(), rest.cend(), bytes.begin() + index);
        } else {
          std::ranges::copy_backward(rest.cbegin(), rest.cend(), bytes.begin() + index + rest.size());
        }
      }
    }

    // void Emit(std::ranges::contiguous_range auto& v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }
    // void Emit(bool v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit(int8_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit(uint8_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit(int16_t v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }
    // void Emit(uint16_t v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }
    // void Emit(int32_t v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }
    // void Emit(uint32_t v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }
    // void Emit(int64_t v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }
    // void Emit(uint64_t v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }
    // void Emit(float v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }
    // void Emit(double v, auto&&... args) { Emit(utility::ToBytes(v), std::forward<decltype(args)>(args)...); }

    // void Emit8(bool v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(int8_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(uint8_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(int16_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(uint16_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(int32_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(uint32_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(int64_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(uint64_t v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(float v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }
    // void Emit8(double v, auto&&... args) { Emit(static_cast<std::byte>(v), std::forward<decltype(args)>(args)...); }

    // void Emit16(bool v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(int8_t v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(uint8_t v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(int16_t v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(uint16_t v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(int32_t v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(uint32_t v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(int64_t v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(uint64_t v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(float v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit16(double v, auto&&... args) { Emit(static_cast<uint16_t>(v), std::forward<decltype(args)>(args)...); }

    // void Emit32(bool v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(int8_t v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(uint8_t v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(int16_t v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(uint16_t v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(int32_t v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(uint32_t v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(int64_t v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(uint64_t v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(float v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit32(double v, auto&&... args) { Emit(static_cast<uint32_t>(v), std::forward<decltype(args)>(args)...); }

    // void Emit64(bool v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(int8_t v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(uint8_t v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(int16_t v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(uint16_t v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(int32_t v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(uint32_t v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(int64_t v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(uint64_t v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(float v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }
    // void Emit64(double v, auto&&... args) { Emit(static_cast<uint64_t>(v), std::forward<decltype(args)>(args)...); }

    void Emit(std::ranges::contiguous_range auto& v) { Emit(utility::ToBytes(v)); }
    void Emit(bool v) { Emit(static_cast<std::byte>(v)); }
    void Emit(int8_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit(uint8_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit(int16_t v) { Emit(utility::ToBytes(v)); }
    void Emit(uint16_t v) { Emit(utility::ToBytes(v)); }
    void Emit(int32_t v) { Emit(utility::ToBytes(v)); }
    void Emit(uint32_t v) { Emit(utility::ToBytes(v)); }
    void Emit(int64_t v) { Emit(utility::ToBytes(v)); }
    void Emit(uint64_t v) { Emit(utility::ToBytes(v)); }
    void Emit(float v) { Emit(utility::ToBytes(v)); }
    void Emit(double v) { Emit(utility::ToBytes(v)); }

    void Emit8(bool v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(int8_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(uint8_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(int16_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(uint16_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(int32_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(uint32_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(int64_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(uint64_t v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(float v) { Emit(static_cast<std::byte>(v)); }
    void Emit8(double v) { Emit(static_cast<std::byte>(v)); }

    void Emit16(bool v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(int8_t v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(uint8_t v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(int16_t v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(uint16_t v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(int32_t v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(uint32_t v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(int64_t v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(uint64_t v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(float v) { Emit(static_cast<uint16_t>(v)); }
    void Emit16(double v) { Emit(static_cast<uint16_t>(v)); }

    void Emit32(bool v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(int8_t v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(uint8_t v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(int16_t v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(uint16_t v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(int32_t v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(uint32_t v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(int64_t v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(uint64_t v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(float v) { Emit(static_cast<uint32_t>(v)); }
    void Emit32(double v) { Emit(static_cast<uint32_t>(v)); }

    void Emit64(bool v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(int8_t v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(uint8_t v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(int16_t v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(uint16_t v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(int32_t v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(uint32_t v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(int64_t v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(uint64_t v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(float v) { Emit(static_cast<uint64_t>(v)); }
    void Emit64(double v) { Emit(static_cast<uint64_t>(v)); }

    void Emit(std::ranges::contiguous_range auto& v, size_t i) { Emit(utility::ToBytes(v), i); }
    void Emit(bool v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit(int8_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit(uint8_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit(int16_t v, size_t i) { Emit(utility::ToBytes(v), i); }
    void Emit(uint16_t v, size_t i) { Emit(utility::ToBytes(v), i); }
    void Emit(int32_t v, size_t i) { Emit(utility::ToBytes(v), i); }
    void Emit(uint32_t v, size_t i) { Emit(utility::ToBytes(v), i); }
    void Emit(int64_t v, size_t i) { Emit(utility::ToBytes(v), i); }
    void Emit(uint64_t v, size_t i) { Emit(utility::ToBytes(v), i); }
    void Emit(float v, size_t i) { Emit(utility::ToBytes(v), i); }
    void Emit(double v, size_t i) { Emit(utility::ToBytes(v), i); }

    void Emit8(bool v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(int8_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(uint8_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(int16_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(uint16_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(int32_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(uint32_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(int64_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(uint64_t v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(float v, size_t i) { Emit(static_cast<std::byte>(v), i); }
    void Emit8(double v, size_t i) { Emit(static_cast<std::byte>(v), i); }

    void Emit16(bool v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(int8_t v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(uint8_t v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(int16_t v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(uint16_t v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(int32_t v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(uint32_t v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(int64_t v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(uint64_t v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(float v, size_t i) { Emit(static_cast<uint16_t>(v), i); }
    void Emit16(double v, size_t i) { Emit(static_cast<uint16_t>(v), i); }

    void Emit32(bool v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(int8_t v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(uint8_t v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(int16_t v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(uint16_t v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(int32_t v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(uint32_t v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(int64_t v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(uint64_t v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(float v, size_t i) { Emit(static_cast<uint32_t>(v), i); }
    void Emit32(double v, size_t i) { Emit(static_cast<uint32_t>(v), i); }

    void Emit64(bool v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(int8_t v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(uint8_t v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(int16_t v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(uint16_t v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(int32_t v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(uint32_t v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(int64_t v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(uint64_t v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(float v, size_t i) { Emit(static_cast<uint64_t>(v), i); }
    void Emit64(double v, size_t i) { Emit(static_cast<uint64_t>(v), i); }

    void Emit(std::ranges::contiguous_range auto& v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }
    void Emit(bool v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit(int8_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit(uint8_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit(int16_t v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }
    void Emit(uint16_t v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }
    void Emit(int32_t v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }
    void Emit(uint32_t v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }
    void Emit(int64_t v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }
    void Emit(uint64_t v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }
    void Emit(float v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }
    void Emit(double v, size_t i, uint8_t s) { Emit(utility::ToBytes(v), i, s); }

    void Emit8(bool v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(int8_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(uint8_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(int16_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(uint16_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(int32_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(uint32_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(int64_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(uint64_t v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(float v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }
    void Emit8(double v, size_t i, uint8_t s) { Emit(static_cast<std::byte>(v), i, s); }

    void Emit16(bool v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(int8_t v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(uint8_t v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(int16_t v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(uint16_t v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(int32_t v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(uint32_t v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(int64_t v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(uint64_t v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(float v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }
    void Emit16(double v, size_t i, uint8_t s) { Emit(static_cast<uint16_t>(v), i, s); }

    void Emit32(bool v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(int8_t v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(uint8_t v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(int16_t v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(uint16_t v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(int32_t v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(uint32_t v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(int64_t v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(uint64_t v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(float v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }
    void Emit32(double v, size_t i, uint8_t s) { Emit(static_cast<uint32_t>(v), i, s); }

    void Emit64(bool v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(int8_t v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(uint8_t v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(int16_t v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(uint16_t v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(int32_t v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(uint32_t v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(int64_t v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(uint64_t v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(float v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
    void Emit64(double v, size_t i, uint8_t s) { Emit(static_cast<uint64_t>(v), i, s); }
  };
};