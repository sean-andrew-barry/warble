export module compiler.text.Unicode;

import compiler.text.UnicodeTables;
import compiler.ir.Error;

import <cstdint>;
import <uchar.h>;
import <string>;
import <stdexcept>;
import <cstddef>;
import <array>;
import <charconv>;
import <expected>;

inline constexpr auto LENGTH_TABLE = []{
  std::array<std::uint8_t, 256> table{};

  // ASCII 0x00-0x7F
  for (unsigned i = 0x00; i <= 0x7F; ++i) table[i] = 1;

  // Continuation bytes 0x80-0xBF stay 0 ⇒ “invalid as lead”
  // Two-byte lead 0xC0-0xDF
  for (unsigned i = 0xC0; i <= 0xDF; ++i) table[i] = 2;
  // Three-byte lead 0xE0-0xEF
  for (unsigned i = 0xE0; i <= 0xEF; ++i) table[i] = 3;
  // Four-byte lead 0xF0-0xF7   (0xF8-0xFF are out-of-range in UTF-8)
  for (unsigned i = 0xF0; i <= 0xF7; ++i) table[i] = 4;

  return table;
}();

export namespace compiler::text::Unicode {
  inline constexpr bool IsIdentifierStart(char32_t c) noexcept {
    const auto i = static_cast<uint32_t>(c);
    return i < 0x110000 && ((compiler::text::UnicodeTables::ID_START_SET_BITS[i >> 6] >> (i & 63)) & 1u);
  }

  inline constexpr bool IsIdentifierContinue(char32_t c) noexcept {
    const auto i = static_cast<uint32_t>(c);
    return i < 0x110000 && ((compiler::text::UnicodeTables::ID_CONTINUE_SET_BITS[i >> 6] >> (i & 63)) & 1u);
  }

  inline constexpr bool IsWhiteSpace(char32_t c) noexcept {
    const auto i = static_cast<uint32_t>(c);
    return i < 0x110000 && ((compiler::text::UnicodeTables::WHITESPACE_BITS[i >> 6] >> (i & 63)) & 1u);
  }

  inline constexpr size_t GetCodePointLength(const char8_t lead_byte) noexcept {
    return LENGTH_TABLE[static_cast<uint8_t>(lead_byte)];
  }

  inline constexpr size_t GetCodePointLength(const char32_t code_point) noexcept {
    if      (code_point <= 0x7F) return 1; // ASCII
    else if (code_point <= 0x7FF) return 2; // 2-byte sequence
    else if (code_point <= 0xFFFF) return 3; // 3-byte sequence
    else if (code_point <= 0x10FFFF) return 4; // 4-byte sequence
    else return 0; // code point out of range, error condition
  }

  constexpr size_t GetCodePointLength(const std::string::const_iterator data) {
    return GetCodePointLength(static_cast<char8_t>(*data));
  }

  char32_t GetCodePoint(const char8_t* bytes, const size_t length) {
    switch (length) {
      case 1: return bytes[0];
      case 2: return ((bytes[0] & 0x1F) << 6 ) |  (bytes[1] & 0x3F);
      case 3: return ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6 ) | (bytes[2] & 0x3F);
      case 4: return ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) | ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
      default: return 0;
    }
  }

  char32_t GetCodePoint(const std::string::const_iterator data, const size_t length) {
    switch (length) {
      case 1: return data[0];
      case 2: return ((data[0] & 0x1F) << 6 ) |  (data[1] & 0x3F);
      case 3: return ((data[0] & 0x0F) << 12) | ((data[1] & 0x3F) << 6 ) |  (data[2] & 0x3F);
      case 4: return ((data[0] & 0x07) << 18) | ((data[1] & 0x3F) << 12) | ((data[2] & 0x3F) << 6) | (data[3] & 0x3F);
      default: return 0;
    }
  }

  char32_t GetCodePoint(const std::u8string::const_iterator data, const size_t length) {
    switch (length) {
      case 1: return data[0];
      case 2: return ((data[0] & 0x1F) << 6 ) |  (data[1] & 0x3F);
      case 3: return ((data[0] & 0x0F) << 12) | ((data[1] & 0x3F) << 6 ) |  (data[2] & 0x3F);
      case 4: return ((data[0] & 0x07) << 18) | ((data[1] & 0x3F) << 12) | ((data[2] & 0x3F) << 6) | (data[3] & 0x3F);
      default: return 0;
    }
  }

  std::u32string ToU32(const std::string& string) {
    std::u32string output;

    for (auto it = string.begin(); it != string.end();) {
      if (it > string.end()) {
        throw std::runtime_error("Malformed UTF-8 string: Iterator beyond end");
      }

      size_t length = GetCodePointLength(static_cast<char8_t>(*it));
      if (length == 0 || std::distance(it, string.end()) < static_cast<std::ptrdiff_t>(length)) {
        throw std::runtime_error("Malformed UTF-8 string: Invalid length");
      }

      char32_t code_point = GetCodePoint(it, length);
      output.push_back(code_point);
      it += length;
    }

    return output;
  }

  std::u32string ToU32(const std::u8string& string) {
    std::u32string output;

    for (auto it = string.begin(); it != string.end();) {
      if (it > string.end()) {
        throw std::runtime_error("Malformed UTF-8 string: Iterator beyond end");
      }

      size_t length = GetCodePointLength(static_cast<char8_t>(*it));
      if (length == 0 || std::distance(it, string.end()) < static_cast<std::ptrdiff_t>(length)) {
        throw std::runtime_error("Malformed UTF-8 string: Invalid length");
      }

      char32_t code_point = GetCodePoint(it, length);
      output.push_back(code_point);
      it += length;
    }

    return output;
  }

  std::string FromU8(const std::u8string& string) {
    return std::string{string.cbegin(), string.cend()};
  }

  std::string FromU8(const char8_t cp) {
    // Since char8_t is essentially a byte in UTF-8,
    // we can directly create a string from it.
    return std::string(1, static_cast<char>(cp));
  }

  std::string FromU16(const char16_t cp) {
    std::string result;

    if (cp <= 0x7F) { // 1-byte sequence
      result += static_cast<char>(cp);
    } else if (cp <= 0x7FF) { // 2-byte sequence
      result += static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
      result += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0xFFFF) { // 3-byte sequence, handling non-surrogates only
      result += static_cast<char>(0xE0 | ((cp >> 12) & 0x0F));
      result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      result += static_cast<char>(0x80 | (cp & 0x3F));
    }

    return result;
  }

  std::string FromU32(const char32_t cp) {
    std::string result;

    if (cp <= 0x7F) { // 1-byte sequence
      result += static_cast<char>(cp);
    } else if (cp <= 0x7FF) { // 2-byte sequence
      result += static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
      result += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0xFFFF) { // 3-byte sequence
      result += static_cast<char>(0xE0 | ((cp >> 12) & 0x0F));
      result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      result += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0x10FFFF) { // 4-byte sequence
      result += static_cast<char>(0xF0 | ((cp >> 18) & 0x07));
      result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
      result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      result += static_cast<char>(0x80 | (cp & 0x3F));
    }

    return result;
  }

  std::string FromU32(std::string_view hexDigits) {
    uint32_t temp = 0;
    auto [ptr, ec] = std::from_chars(hexDigits.data(), hexDigits.data() + hexDigits.size(), temp, 16);
    char32_t charCode = static_cast<char32_t>(temp);
    
    // Check for any error in conversion.
    if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
      throw std::runtime_error("Invalid Unicode codepoint escape sequence");
    }

    // Ensuring that the code point is within the valid range of Unicode values.
    if (charCode > 0x10FFFF) {
      throw std::runtime_error("Unicode codepoint out of valid range");
    }

    return FromU32(charCode);
  }

  std::expected<char32_t, ir::Error> HexStringToCodePoint(std::string_view hex_digits) {
    uint32_t temp = 0;
    auto [ptr, ec] = std::from_chars(hex_digits.data(), hex_digits.data() + hex_digits.size(), temp, 16);

    // Check for any error in conversion.
    if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
      return std::unexpected(ir::Error::UnicodeCodePointInvalid);
    }

    // Ensure the value is a valid Unicode scalar value.
    if (temp > 0x10FFFFu) {
      return std::unexpected(ir::Error::UnicodeCodePointOutOfRange);
    }
    if (temp >= 0xD800u && temp <= 0xDFFFu) {
      return std::unexpected(ir::Error::UnicodeSurrogateCodePointIsNotAValidScalarValue);
    }

    return static_cast<char32_t>(temp);
  }
};