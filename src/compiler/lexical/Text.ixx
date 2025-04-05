export module lexical.text;

import lexical.unicode;
// import Concepts;

import <bitset>;
import <utility>;
import <string_view>;
import <string>;
import <charconv>;
import <utility>;
import <cassert>;
import <cstddef>;
import <stdexcept>;
import <format>;

export namespace lexical::Text {
  template<typename T>
  std::pair<T, std::errc> ParseNumber(std::string_view view, int base = 10) {
    T value{};
    auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), value, base);
    return {value, ec};
  }

  auto ParseU64(std::string_view view, int base = 10) { return ParseNumber<uint64_t>(view, base); }
  auto ParseI64(std::string_view view, int base = 10) { return ParseNumber<int64_t>(view, base); }
  auto ParseI32(std::string_view view, int base = 10) { return ParseNumber<int32_t>(view, base); }

  std::pair<double, std::errc> ParseF64(std::string_view view) {
    double value{0.0};
    auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), value);
    return {value, ec};
  }

  std::string ParseEscapeUnicodeCodepoint(std::string_view view) {
    // Ensuring the format is at least \u{X}
    assert(view.size() >= 4); 

    // Extract the hexadecimal part between { and }.
    std::string_view hex = view.substr(3, view.size() - 4);
    return Unicode::FromU32(hex);
  }

  std::string ParseEscapeUnicodeShort(std::string_view view) {
    assert(view.size() >= 6); // Ensuring the format is \uXXXX
    
    // Extract the four hex digits.
    std::string_view hex = view.substr(2, 4);
    return Unicode::FromU32(hex);
  }

  std::string ParseEscapeHexCode(std::string_view view) {
    assert(view.size() >= 4); // Ensuring the format is \xXX
    
    // Extract the two hex digits.
    std::string_view hexDigits = view.substr(2, 2);
    
    auto [value, ec] = ParseI32(hexDigits, 16);
    
    // Check for any error in conversion.
    if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
      throw std::runtime_error("Invalid hex escape sequence");
    }
    
    // Convert the integer code to a character.
    char character = static_cast<char>(value);
    
    return std::string(1, character);
  }
};
