export module compiler.text.cursor.String;

import compiler.text.Cursor;
import compiler.text.Unicode;
import <string>;
import <string_view>;
import <bitset>;
import <cstdint>;
import <ranges>;
import <execution>;
import <iterator>;
import <algorithm>;
import <cassert>;

namespace compiler::text::cursor {
  constexpr char END() { return '\0'; }

  export class String : public compiler::text::Cursor<std::string, END> {
  private:
  public:
    using Super::Super;
    using Super::Peek;

    char32_t PeekCodePoint() const {
      char8_t ch = Super::Peek();
      size_t length = compiler::text::Unicode::GetCodePointLength(ch);

      if (length > 0 && Size() >= length) {
        char32_t code_point = compiler::text::Unicode::GetCodePoint(reinterpret_cast<const char8_t*>(&*cbegin()), length);

        return code_point;
      } else {
        return '\0';
      }
    }

    char32_t CodePoint() {
      char8_t ch = Super::Peek();
      size_t length = compiler::text::Unicode::GetCodePointLength(ch);

      if (length > 0 && Size() >= length) {
        char32_t code_point = compiler::text::Unicode::GetCodePoint(reinterpret_cast<const char8_t*>(&*cbegin()), length);
        Advance(length);

        return code_point;
      } else {
        return '\0';
      }
    }

    constexpr bool Fits(const std::string_view text) const {
      // If it doesn't go past the end, it can match
      return Size() >= text.size();
    }

    constexpr bool Check(const char c) const {
      return Super::Peek() == c;
    }

    constexpr bool Check(const unsigned char c) const {
      return static_cast<unsigned char>(Super::Peek()) == c;
    }

    constexpr bool Check(const std::string_view text) const {
      if (!Fits(text)) return false; // If it doesn't fit, it can't match

      auto temp = cbegin();
      for (size_t i = 0; i < text.size(); ++i, ++temp) {
        if (text[i] != *temp) {
          return false;
        }
      }

      return true; // All characters matched
    }

    constexpr bool Match(const char c) {
      auto iter = cbegin();
      if (iter != cend() && *iter == c) {
        Advance();
        return true;
      } else {
        return false;
      }
    }

    constexpr bool Match(const unsigned char c) {
      auto iter = cbegin();
      if (iter != cend() && static_cast<unsigned char>(*iter) == c) {
        Advance();
        return true;
      } else {
        return false;
      }
    }

    constexpr bool Match(const std::string_view text) {
      if (!Check(text)) return false; // Compare the text to the content

      Advance(text.size());

      return true;
    }

    // ────────────────────────────────────────────────────────────────
    //  Count a run of *one* repeated character
    // ────────────────────────────────────────────────────────────────
    constexpr uint32_t Count(const unsigned char ch) const noexcept {
      // `find_if_not` turns into a single auto-vectorised loop that
      // stops as soon as a mismatch is observed.
      const iterator first_not = std::find_if_not(cbegin(), cend(), [=](unsigned char c) { return c == ch; });

      return static_cast<uint32_t>(first_not - cbegin());
    }

    // ────────────────────────────────────────────────────────────────
    //  Count a run of *repeated pattern*  (e.g. "/*" or "\r\n")
    // ────────────────────────────────────────────────────────────────
    constexpr uint32_t Count(const std::string_view pat) const noexcept {
      assert(!pat.empty()); // an empty pattern is ill-defined

      uint32_t n = 0;
      const size_t sz = pat.size();
      auto cur = cbegin();
      auto end = cend();

      // Because `std::equal` on contiguous iterators is specified to
      // use `memcmp` when the iterators’ value type is trivially copyable
      // (like `char`), the inner comparison is already the most
      // efficient available on the platform (hand-written SIMD inside
      // libc or the compiler’s builtin).
      while (static_cast<size_t>(end - cur) >= sz && std::equal(cur, cur + sz, pat.begin())) {
        ++n;
        cur += sz;
      }

      return n;
    }

    constexpr uint32_t Consume(const char ch) noexcept {
      uint32_t count = Count(ch);
      Advance(count);
      return count;
    }

    constexpr uint32_t Consume(const std::string_view pat) noexcept {
      uint32_t count = Count(pat);
      Advance(count * pat.size());
      return count;
    }

    constexpr std::string_view Capture(const std::string_view text) {
      if (!Check(text)) return std::string_view{cbegin(), cbegin()};

      std::string_view captured{cbegin(), cbegin() + text.size()};

      Advance(text.size());

      return captured;
    }
  };
};