export module utility.to_string;

import lexical.unicode;

import <algorithm>;
import <array>;
import <concepts>;
import <cstdint>;
import <format>;
import <functional>;
import <iomanip>;
import <iostream>;
import <locale>;
import <mutex>;
import <ranges>;
import <span>;
import <sstream>;
import <string_view>;
import <string>;
import <tuple>;
import <type_traits>;
import <typeinfo>;
import <utility>;
import <vector>;
import <memory>;

// template<typename T>
// concept IsRange = requires(T t) {
//   std::ranges::begin(t);  // Checks if begin() can be called
//   std::ranges::end(t);    // Checks if end() can be called
// };

// std::locale LOCALE = [](){
//   std::locale locale;

//   try {
//     locale = std::locale("en_US.UTF-8");
//     std::cout << "Locale set to en_US.UTF-8" << std::endl;
//   } catch (const std::runtime_error&) {
//     std::cout << "en_US.UTF-8 not available, falling back to C locale" << std::endl;
//     // Fallback to the "C" locale if the specified locale is not available
//     locale = std::locale("C");
//   }

//   return locale;
// }();

std::locale& GetLocale() {
  static std::locale locale;
  static bool initialized = false;

  if (!initialized) {
    try {
      locale = std::locale("en_US.UTF-8");
      std::cout << "Locale set to en_US.UTF-8" << std::endl;
    } catch (const std::runtime_error&) {
      // Fallback to the "C" locale if the specified locale is not available
      std::cout << "en_US.UTF-8 not available, falling back to C locale" << std::endl;
      locale = std::locale("C");
    }
    initialized = true;
  }

  return locale;
}

// This is treated as the highest priority conversion
template<typename T>
concept HasToStringView = requires(T t) {
  { t.ToStringView() } -> std::convertible_to<std::string_view>;
};

template<typename T>
concept HasToStringViewPtr = requires(T t) {
  { t->ToStringView() } -> std::convertible_to<std::string_view>;
};

// template<typename T>
// concept HasToString = requires(T t) {
//   { t.ToString() } -> std::convertible_to<std::string>;
// };

template<typename T>
concept HasToString = requires(T t) {
  { t.ToString() } -> std::convertible_to<std::string>;
};

template<typename T>
concept HasToStringPtr = requires(T t) {
  { t->ToString() } -> std::convertible_to<std::string>;
};

template<typename T>
concept IsSimpleCallable = requires(T t) { t(); };

template<typename T>
concept IsRange = std::ranges::range<T>;

template<typename T>
concept IsFormattable = std::formattable<T>;

template<typename T>
concept IsScopedEnum = std::is_scoped_enum_v<T>;

template<typename T>
concept IsPointer = std::is_pointer_v<T>;


template<typename T>
concept IfHasToStringView = HasToStringView<T>;

template<typename T>
concept IfHasToString = (HasToString<T>) && !(HasToStringView<T>);

// template<typename T>
// concept IfHasToString = HasToString<T>;

template<typename T>
concept IfIsSimpleCallable = IsSimpleCallable<T> && !(HasToStringView<T> || IfHasToString<T>);

template<typename T>
concept IfIsRange = IsRange<T> && !(HasToStringView<T> || IfHasToString<T> || IfIsSimpleCallable<T>);

template<typename T>
concept IfIsFormattable = IsFormattable<T> && !(HasToStringView<T> || IfHasToString<T> || IfIsSimpleCallable<T> || IfIsRange<T>);

template<typename T>
concept IfIsFallback = !(HasToStringView<T> || IfHasToString<T> || IfIsSimpleCallable<T> || IfIsRange<T> || IfIsFormattable<T>);

std::string RemovePrefix(std::string input) {
  constexpr std::string_view class_prefix = "class ";
  constexpr std::string_view struct_prefix = "struct ";

  // Find and remove "class " prefix
  size_t pos = input.find(class_prefix);
  if (pos != std::string::npos) {
    input.erase(pos, class_prefix.length());
  }
  else {
    // Find and remove "struct " prefix
    pos = input.find(struct_prefix);
    if (pos != std::string::npos) {
      input.erase(pos, struct_prefix.length());
    }
  }

  return input;
}

// Convert a single byte to its hex representation
std::pair<char, char> ToHex(std::byte b) {
  constexpr char hex_map[] = "0123456789ABCDEF";

  return {
    hex_map[(std::to_integer<unsigned int>(b) >> 4) & 0xF],
    hex_map[std::to_integer<unsigned int>(b) & 0xF],
  };
}

void ToHex(std::string& str, const std::byte b) {
  constexpr char hex_map[] = "0123456789ABCDEF";

  str += hex_map[(std::to_integer<unsigned int>(b) >> 4) & 0xF];
  str += hex_map[std::to_integer<unsigned int>(b) & 0xF];
}

// decltype(auto) ToBytes(auto value) {
//   std::span<std::byte, sizeof(value)> bytes{reinterpret_cast<std::byte*>(&value), sizeof(value)};
//   return bytes;
// }

decltype(auto) ToBytes(const auto value) {
  std::span<const std::byte, sizeof(value)> bytes{reinterpret_cast<const std::byte*>(&value), sizeof(value)};
  return bytes;
}

export namespace utility {
  std::string ToString() { return std::string{}; }
  const std::string& ToString(const std::string& value) { return value; }
  std::string ToString(std::string&& value) { return std::move(value); }
  std::string ToString(const std::string_view value) { return std::string{value}; }
  std::string ToString(const bool value) { return value ? "true" : "false"; }
  std::string ToString(const char value) { return std::string{value}; }
  std::string ToString(const char* value) { return std::string(value); }
  // std::string ToString(const std::type_info& value) { return RemovePrefix(value.name()); }
  std::string ToString(const std::type_info& value) { return value.name(); }
  std::string ToString(const char8_t code_point) { return lexical::Unicode::FromU8(code_point); }
  std::string ToString(const char16_t code_point) { return lexical::Unicode::FromU16(code_point); }
  std::string ToString(const char32_t code_point) { return lexical::Unicode::FromU32(code_point); }

  std::string ToString(uint8_t v) { return std::format(GetLocale(), "{:L}", static_cast<uint16_t>(v)); }
  std::string ToString(uint16_t v) { return std::format(GetLocale(), "{:L}", v); }
  std::string ToString(uint32_t v) { return std::format(GetLocale(), "{:L}", v); }
  std::string ToString(uint64_t v) { return std::format(GetLocale(), "{:L}", v); }
  std::string ToString(int8_t v) { return std::format(GetLocale(), "{:L}", static_cast<uint16_t>(v)); }
  std::string ToString(int16_t v) { return std::format(GetLocale(), "{:L}", v); }
  std::string ToString(int32_t v) { return std::format(GetLocale(), "{:L}", v); }
  std::string ToString(int64_t v) { return std::format(GetLocale(), "{:L}", v); }
  std::string ToString(float v) { return std::format(GetLocale(), "{:L}", v); }
  std::string ToString(double v) { return std::format(GetLocale(), "{:L}", v); }
  std::string ToString(long double v) { return std::format(GetLocale(), "{:L}", v); }

  template<size_t S = std::dynamic_extent>
  std::string ToString(const std::span<std::byte, S> bytes) {
    // Allocate a string with the expected length: 2 chars per byte, plus the "0x" prefix.
    std::string result;
    result.reserve(2 + bytes.size() * 2);
    result.append("0x");

    for (auto b : bytes) {
      auto pair = ToHex(b);
      result.push_back(pair.first);
      result.push_back(pair.second);
    }

    return result;
  }

  std::string ToString(const void* v) {
    std::string result = std::format("{:#018x}", reinterpret_cast<uintptr_t>(v));

    std::transform(result.begin() + 2, result.end(), result.begin() + 2, ::toupper);
    return result;
  }

  std::string ToString(const std::byte v) {
    std::string result{};
    ToHex(result, v);
    return result;
  }

  std::string ToString(std::nullptr_t v) { return "nullptr"; }

  std::string ToString(IsPointer auto const p) {
    if constexpr (requires { p->ToStringView(); }) {
      return p ? p->ToStringView() : "nullptr";
    } else if constexpr (requires { p->ToString(); }) {
      return p ? p->ToString() : "nullptr";
    } else {
      std::string result = std::format("{:#018x}", reinterpret_cast<uintptr_t>(p));

      std::transform(result.begin() + 2, result.end(), result.begin() + 2, ::toupper);
      return result;
    }
  }

  template<typename T>
  std::string ToString(const std::unique_ptr<T>& p) {
    if (p) return utility::ToString(*p);
    else return "nullptr";
  }

  template<typename T>
  std::string ToString(const std::shared_ptr<T>& p) {
    if (p) return utility::ToString(*p);
    else return "nullptr";
  }

  template<typename T1, typename T2>
  std::string ToString(const std::pair<T1, T2>& p) {
    return '(' + utility::ToString(p.first) + ", " + utility::ToString(p.second) + ')';
  }

  template<typename... Args>
  std::string ToString(const std::tuple<Args...>& tuple) {
    std::string output;
    output += '(';

    int count = 0;
    std::apply([&output, &count](const auto&... args) {
      (output.append(++count == 1 ? "" : ", ").append(utility::ToString(args)), ...);
    }, tuple);

    output += ')';
    return output;
  }

  std::string ToString(IfHasToStringView auto const& value) { return utility::ToString(value.ToStringView()); }
  std::string ToString(IfHasToString auto const& value) { return value.ToString(); }

  // std::string ToString(IfHasToString auto const& value) {
  //   if constexpr (requires { value.ToString(); }) {
  //     return value.ToString();
  //   } else if constexpr (requires { value->ToString(); }) {
  //     return value->ToString();
  //   } else {
  //     return "Unsupported type";
  //   }
  // }

  std::string ToString(IfIsSimpleCallable auto const& value) { return utility::ToString(value()); }

  // Convert that is compatible with std::ranges
  std::string ToString(IfIsRange auto const& range) {
    std::string str;

    auto it = std::ranges::begin(range);
    auto end = std::ranges::end(range);
    
    if (it != end) {  // Handle the first element separately to avoid a trailing comma
      str += utility::ToString(*it);
      ++it;
    }

    for (; it != end; ++it) {
      str += ", " + utility::ToString(*it);
    }

    return str;
  }

  std::string ToString(IfIsFormattable auto const& value) { return std::format("{}", value); }
  std::string ToString(IfIsFallback auto const& value) { return utility::ToString(typeid(value)); } // Catch all unknown values
};