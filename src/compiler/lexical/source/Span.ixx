export module lexical.source.span;

import <vector>;
import <string>;
import <string_view>;
import <ranges>;

namespace lexical {
  export class Token;
};

namespace lexical::source {
  export class Span {
  public:
    using StringIterator = std::string::const_iterator;
    using TokenIterator = std::vector<lexical::Token>::const_iterator;
  private:
    StringIterator string;
    TokenIterator begin;
    TokenIterator end;
    // std::ranges::subrange<TokenIterator> range;
  public:
    Span() noexcept;
    // Span(StringIterator string, TokenIterator begin) noexcept;
    Span(StringIterator string, TokenIterator begin, TokenIterator end) noexcept;

    StringIterator String() const { return string; }
    TokenIterator Begin() const { return begin; }
    TokenIterator End() const { return end; }

    void String(StringIterator iter) { string = iter; }
    void Begin(TokenIterator iter) { begin = iter; }
    void End(TokenIterator iter) { end = iter; }

    Span ToBegin() const { return Span{string, begin, begin}; }
    Span ToEnd() const { return Span{string, end, end}; }

    std::ranges::subrange<TokenIterator> Range() const { return std::ranges::subrange<TokenIterator>{begin, end}; }

    std::string_view View() const;
  };
};