import lexical.source.span;
import lexical.token;

namespace lexical::source {
  Span::Span() noexcept
    : string{}, begin{}, end{} {}

  // Span::Span(std::string::const_iterator string, std::vector<lexical::Token>::const_iterator begin) noexcept
  //   : string{string}, begin{begin}, end{} {}

  Span::Span(std::string::const_iterator string, std::vector<lexical::Token>::const_iterator begin, std::vector<lexical::Token>::const_iterator end) noexcept
    : string{string}, begin{begin}, end{end} {}

  std::string_view Span::View() const {
    return std::string_view{string + begin->Index(), string + end->Index() + end->Width()};
  }
};