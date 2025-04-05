export module lexical.unicode;

import lexical.code_points;

import <cstdint>;
import <uchar.h>;
import <unordered_set>;
import <string>;
import <stdexcept>;
import <cstddef>;
import <charconv>;

const auto ID_START_SET = []{
  std::unordered_set<char32_t> set;

  set.reserve(ID_START.size() + EMOJI.size());
  set.insert(ID_START.cbegin(), ID_START.cend());
  set.insert(EMOJI.cbegin(), EMOJI.cend());

  return set;
}();

const auto ID_CONTINUE_SET = []{
  std::unordered_set<char32_t> set;

  set.reserve(ID_START.size() + EMOJI.size() + ID_CONTINUE.size());
  set.insert(ID_START.cbegin(), ID_START.cend());
  set.insert(EMOJI.cbegin(), EMOJI.cend());
  set.insert(ID_CONTINUE.cbegin(), ID_CONTINUE.cend());

  return set;
}();

const auto WHITESPACE_SET = []{
  std::unordered_set<char32_t> set;

  set.reserve(WHITESPACE.size());
  set.insert(WHITESPACE.cbegin(), WHITESPACE.cend());

  return set;
}();

export namespace lexical::Unicode {
  bool IsIdentifierStart(char32_t c) {
    return ID_START_SET.find(c) != ID_START_SET.end();
  }

  bool IsIdentifierContinue(char32_t c) {
    return ID_CONTINUE_SET.find(c) != ID_CONTINUE_SET.end();
  }

  bool IsWhiteSpace(char32_t c) {
    return WHITESPACE_SET.find(c) != WHITESPACE_SET.end();
  }

  constexpr size_t GetCodePointLength(const char8_t lead_byte) {
    if      ((lead_byte & 0x80) == 0   ) return 1; // Lead byte: 0xxxxxxx
    else if ((lead_byte & 0xE0) == 0xC0) return 2; // Lead byte: 110xxxxx
    else if ((lead_byte & 0xF0) == 0xE0) return 3; // Lead byte: 1110xxxx
    else if ((lead_byte & 0xF8) == 0xF0) return 4; // Lead byte: 11110xxx
    else return 0; // Error condition (invalid UTF-8 lead byte)
  }

  constexpr size_t GetCodePointLength(const char32_t code_point) {
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

  // char32_t TakeCodePoint(std::string::const_iterator& data) {
  //   switch (GetCodePointLength(static_cast<char8_t>(data[0]))) {
  //     case 1: return data[0];
  //     case 2: return ((data[0] & 0x1F) << 6 ) |  (data[1] & 0x3F);
  //     case 3: return ((data[0] & 0x0F) << 12) | ((data[1] & 0x3F) << 6 ) |  (data[2] & 0x3F);
  //     case 4: return ((data[0] & 0x07) << 18) | ((data[1] & 0x3F) << 12) | ((data[2] & 0x3F) << 6) | (data[3] & 0x3F);
  //     default: return 0;
  //   }
  // }

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
};