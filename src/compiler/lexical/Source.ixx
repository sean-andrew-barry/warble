export module lexical.source;

import lexical.token;
import lexical.tokens;
import lexical.source.span;
import utility.print;

import <string>;
import <vector>;
import <bit>;
import <cstdint>;
import <limits>;
import <stdexcept>;
import <iomanip>;
import <iostream>;
import <algorithm>;

namespace lexical {
  export class Source {
  private:
    std::string text;
    std::vector<lexical::Token> tokens;
    std::vector<uint32_t> lines; // The line table, each element represents the UTF-32 index of a new line
    uint32_t index = 0;
    uint32_t line = 1;
    uint32_t column = 1;
    uint64_t wasted = 0;
  private:
    void Refresh() {
      if (tokens.empty()) return;

      // Restore the position from the last token
      const auto& token = tokens.back();
      index = token.Index();
      line = token.Line();
      column = token.Column();

      utility::Debug("Refreshing token", token, token.Width(), index, line, column, std::string_view{text.cbegin() + index, text.cbegin() + index + token.Width()}, "wasted", wasted);
    }
  public:
    Source(std::string&& t)
      : text{std::move(t)}
    {
      size_t estimate = text.size() / 2;
      size_t next = std::bit_ceil(estimate);

      tokens.reserve(next);
      lines.push_back(0); // Line 1 starts at offset 0
    }

    const std::string& Text() const { return text; }
    const std::vector<lexical::Token>& Tokens() const { return tokens; }
    const std::vector<uint32_t>& Lines() const { return lines; }

    void NewPush(lexical::Tokens type, uint32_t width, uint32_t offset = 0) {
      if (width > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("The width of an individual token cannot exceed " + std::to_string(std::numeric_limits<uint16_t>::max()) + " characters");
      }

      if (offset > std::numeric_limits<uint8_t>::max()) {
        throw std::runtime_error("The unicode offset of an individual token cannot exceed " + std::to_string(std::numeric_limits<uint8_t>::max()) + " characters");
      }

      tokens.emplace_back(type, static_cast<uint8_t>(offset), static_cast<uint16_t>(width), index);

      index += width;

      if (type == lexical::Tokens::NEWLINE) {
        lines.push_back(index + 1);
      }
    }

    void Push(lexical::Tokens type, uint32_t width, uint32_t offset = 0) {
      if (width > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("The width of an individual token cannot exceed " + std::to_string(std::numeric_limits<uint16_t>::max()) + " characters");
      }

      if (offset > std::numeric_limits<uint8_t>::max()) {
        throw std::runtime_error("The unicode offset of an individual token cannot exceed " + std::to_string(std::numeric_limits<uint8_t>::max()) + " characters");
      }

      tokens.emplace_back(type, static_cast<uint8_t>(offset), static_cast<uint16_t>(width), index, line, column);

      index += width;

      if (type == lexical::Tokens::NEWLINE) {
        line += 1;
        column = 1;
      }
      else {
        column += width;
      }
    }

    void Pop() {
      tokens.pop_back();
      // Refresh();
    }

    void Rollback(size_t size) {
      if (tokens.size() == size) return;
      // std::cout << "Rolling back to " << size << ", " << tokens.size() << std::endl;

      // utility::Debug("Rolling back to", size, tokens.size());
      // if (size == 0) return;
      // if (tokens.empty()) return;
      wasted += tokens.size() - size;

      while (tokens.size() > size + 1) {
        // Refresh();
        tokens.pop_back();

        // if (tokens.size() == size) {
        //   Refresh();
        // }
      }

      Refresh();
      tokens.pop_back();
    }

    size_t Size() const { return tokens.size(); }
    // std::vector<lexical::Token>::const_iterator End() const { return tokens.cend(); }
    lexical::Token& Back() { return tokens.back(); }

    std::string_view Read(const Token& token) const {
      return std::string_view{text.data() + token.Index(), token.Width()};
    }

    std::string_view Read(const Token& start, const Token& end) const {
      return std::string_view{text.cbegin() + start.Index(), text.cbegin() + end.Index() + end.Width()};
    }

    std::string_view Read() const {
      return Read(tokens.back());
    }

    std::string_view Read(size_t i) const {
      return Read(tokens.at(i));
    }

    std::string_view Read(size_t start, size_t end) const {
      return Read(tokens.at(start), tokens.at(end));
    }

    std::string_view ReadLastSignificant() const {
      for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        if (!it->IsTrivial()) {
          return Read(*it);
        }
      }

      return "";
    }

    // Given an offset and the line table, return the 1-indexed line and column.
    std::pair<uint32_t, uint32_t> GetLineColumn(uint32_t offset) {
      // upper_bound finds the first element greater than offset.
      auto it = std::upper_bound(lines.begin(), lines.end(), offset);
      uint32_t line = std::distance(lines.begin(), it);
      // Adjust to get the starting offset of the current line.
      uint32_t line_start = lines[line - 1];
      uint32_t column = offset - line_start;
      return {line, column + 1}; // 1-index columns for user-friendly reporting.
    }
  };
};