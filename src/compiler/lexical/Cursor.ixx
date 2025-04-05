export module lexical.cursor;

import <ranges>;
import <span>;
import <stdexcept>;

namespace lexical {
  export template<typename T, auto END = [](){ return T::value_type{}; }>
  class Cursor {
  public:
    using Super = Cursor<T, END>; // Convenience for derived classes
    using Value = typename T::value_type;
    using Iterator = typename T::const_iterator;
  private:
    // Iterator begin;
    // Iterator current;
    // Iterator end;
    std::ranges::subrange<Iterator> range;
  public:
    constexpr Cursor(const T& t) : range{t.cbegin(), t.cend()} {}

    constexpr void Advance(size_t n = 1) { range.advance(n); }
    constexpr void Retreat(size_t n = 1) { range.advance(-static_cast<int64_t>(n)); }
    constexpr void Move(int64_t n) { range.advance(n); }

    constexpr void Retreat(typename T::const_iterator to) {
      if (to > cend()) {
        throw std::out_of_range("Retreat position is out of valid range");
      }

      range = std::ranges::subrange(to, cend());
    }

    constexpr decltype(auto) Take() {
      decltype(auto) current = Peek();
      Advance(1);
      return current;
    }

    constexpr const std::span<const Value> Span(size_t count) {
      size_t c = std::min(count, range.size());
      auto begin = range.begin();
      Advance(c);
      return std::span{begin, c};
    }

    constexpr decltype(auto) Peek() const { return Valid() ? range.front() : END(); }
    constexpr decltype(auto) Peek(size_t n) const { return Fits(n) ? range.next(n).front() : END(); }

    constexpr bool Done() const { return range.empty(); }
    constexpr bool Valid() const { return !range.empty(); }
    constexpr bool Fits(size_t n) const { return range.size() >= n; }
    constexpr size_t Size() const { return range.size(); }
    constexpr std::ranges::subrange<typename T::const_iterator> Remaining() const { return range; }

    constexpr explicit operator bool() const { return Valid(); }
    constexpr decltype(auto) cbegin() const { return range.cbegin(); }
    constexpr decltype(auto) cend() const { return range.cend(); }
  };
};