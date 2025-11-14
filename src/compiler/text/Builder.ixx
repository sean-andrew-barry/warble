export module compiler.text.Builder;

import compiler.text.Unicode;

import <algorithm>;
import <concepts>;
import <cctype>;
import <cstddef>;
import <cstdint>;
import <format>;
import <locale>;
import <memory>;
import <optional>;
import <ranges>;
import <span>;
import <stdexcept>;
import <string>;
import <string_view>;
import <thread>;
import <tuple>;
import <type_traits>;
import <typeinfo>;
import <utility>;

export namespace compiler::ir {
  enum class Token : uint8_t;
  enum class Error : uint8_t;
  enum class Opcode : uint8_t;
  class Index;
  class Instruction;
}

export namespace compiler::input {
  class Lexer;
}

const std::locale& Locale() {
  // Magic-static initialization is thread-safe since C++11 and guarantees
  // that the lambda runs exactly once across threads for this variable.
  static const std::locale locale = [] {
    try {
      return std::locale("en_US.UTF-8");
    } catch (const std::runtime_error&) {
      return std::locale("C");
    }
  }();

  return locale;
}

namespace compiler::text {
  export class Builder {
  private:
    std::string output;
  public:
    Builder(auto&&... args) {
      (Append(std::forward<decltype(args)>(args)), ...);
    }

    const std::string& ToString() const { return output; }

    void Append() { output.append(std::string{}); }
    void Append(const std::string& v) { output.append(v); }
    void Append(std::string&& v) { output.append(std::move(v)); }
    void Append(std::string_view v) { output.append(std::string{v}); }
    void Append(const bool v) { output.append(v ? "true" : "false"); }
    void Append(const char v) { output.append(std::string{v}); }
    void Append(const char* v) { output.append(v ? std::string{v} : "nullptr"); }
    void Append(char* v) { Append(static_cast<const char*>(v)); }
    void Append(const std::type_info& v) { output.append(v.name()); }
    void Append(const char8_t c) { output.append(text::Unicode::FromU8(c)); }
    void Append(const char16_t c) { output.append(text::Unicode::FromU16(c)); }
    void Append(const char32_t c) { output.append(text::Unicode::FromU32(c)); }
    void Append(uint8_t v) { output.append(std::format(Locale(), "{:L}", static_cast<uint16_t>(v))); }
    void Append(uint16_t v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(uint32_t v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(uint64_t v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(int8_t v) { output.append(std::format(Locale(), "{:L}", static_cast<int16_t>(v))); }
    void Append(int16_t v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(int32_t v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(int64_t v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(float v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(double v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(long double v) { output.append(std::format(Locale(), "{:L}", v)); }
    void Append(std::thread::id v) { output.append(std::format(Locale(), "{}", v)); }
    void Append(std::nullptr_t) { output.append("nullptr"); }

    void Append(const std::byte v) {
      static constexpr char hex_map[] = "0123456789ABCDEF";
      const unsigned x = std::to_integer<unsigned>(v);
      output.push_back(hex_map[(x >> 4) & 0xF]);
      output.push_back(hex_map[x & 0xF]);
    }

    void Append(const void* ptr) {
      if (!ptr) {
        output += "nullptr";
      } else {
        // total width = "0X" (2) + 2 hex chars per byte of uintptr_t
        const int w = 2 + 2 * static_cast<int>(sizeof(uintptr_t));
        output += std::format("{:#0{}X}", reinterpret_cast<uintptr_t>(ptr), w);
      }
    }

    void Append(compiler::ir::Token t);
    void Append(compiler::ir::Error e);
    void Append(compiler::ir::Opcode opcode);
    void Append(compiler::ir::Index index);
    void Append(const compiler::ir::Instruction& instruction);
    void Append(const compiler::input::Lexer& lexer);
    void Append(compiler::ir::Instruction& instruction);
    void Append(compiler::input::Lexer& lexer);

    template<typename T>
    void Append(const std::unique_ptr<T>& p) {
      if (p) Append(*p);
      else output += "nullptr";
    }

    template<typename T>
    void Append(const std::shared_ptr<T>& p) {
      if (p) Append(*p);
      else output += "nullptr";
    }

    template<typename T>
    void Append(const std::weak_ptr<T>& p) {
      if (auto locked = p.lock()) {
        Append(*locked);
      } else {
        output += "expired";
      }
    }

    template<typename T>
    void Append(const std::optional<T>& value) {
      if (value) Append(*value);
      else output += "nullopt";
    }

    template<typename T1, typename T2>
    void Append(const std::pair<T1, T2>& p) {
      output += '(';
      Append(p.first);
      output += ", ";
      Append(p.second);
      output += ')';
    }

    template<typename... Args>
    void Append(const std::tuple<Args...>& tuple) {
      output.push_back('(');

      std::apply([&](const auto&... elems) {
        bool first = true;
        (( first ? first = false : static_cast<void>(output.append(", ")), Append(elems)), ...);
      }, tuple);

      output.push_back(')');
    }

    template<typename T>
    requires std::is_pointer_v<std::remove_cvref_t<T>>
    void Append(T value) {
      if (value == nullptr) {
        output.append("nullptr");
        return;
      }

      if constexpr (requires { output.append(value->ToStringView()); }) {
        output.append(value->ToStringView());
      } else if constexpr (requires { output.append(value->ToString()); }) {
        output.append(value->ToString());
      } else {
        Append(reinterpret_cast<const void*>(value));
      }
    }

    template<typename T>
    requires (!std::is_pointer_v<std::remove_reference_t<T>>)
    void Append(T&& value) {
      using V = std::remove_cvref_t<T>;

      if constexpr (requires { output.append(value.ToStringView()); }) {
        output.append(value.ToStringView());
      } else if constexpr (requires { output.append(value.ToString()); }) {
        output.append(value.ToString());
      } else if constexpr (std::ranges::input_range<V>) {
        auto&& wrapped = value;
        bool first = true;

        output.push_back('[');
        for (auto&& element : wrapped) {
          if (!first) output.append(", ");
          first = false;
          Append(std::forward<decltype(element)>(element));
        }
        output.push_back(']');
      } else if constexpr (std::is_enum_v<V>) {
        Append(static_cast<std::underlying_type_t<V>>(value));
      } else if constexpr (std::formattable<V, char>) {
        output.append(std::format("{}", std::forward<T>(value)));
      } else {
        Append(typeid(value));
      }
    }
  };
}