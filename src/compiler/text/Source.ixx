export module compiler.text.Source;

import compiler.ir.Token;

import <cstdint>;
import <string>;
import <vector>;
import <utility>; // for std::pair

namespace compiler::text {
	// The Source class owns the raw UTF-8 source text for a compilation unit.
	// It precomputes line metadata to support fast source mapping and maintains
	// a token buffer produced by the (future) lexer.
	//
	// lines: Each entry is { byte_offset_of_line_start, newline_token_index }.
	//        newline_token_index is an index into the tokens vector for the token
	//        that terminated the line. For the final line (which may lack a
	//        trailing newline) the newline_token_index may point to tokens.size()
	//        or a sentinel value (implementation detail TBD).
	// tokens: Flat token stream for the source.
	export class Source {
	public:
		using LineInfo = std::pair<std::uint32_t, std::uint32_t>;
	private:
		std::string contents;                 // Underlying UTF-8 text
		std::vector<LineInfo> lines;          // Line start + newline token mapping
		std::vector<ir::Token> tokens; // Token stream
	public:
		// Construct a Source from raw UTF-8 text. Line table/token setup is deferred.
		explicit Source(std::string text) : contents(std::move(text)) {}

		Source(const Source&) = default;
		Source(Source&&) noexcept = default;
		Source& operator=(const Source&) = default;
		Source& operator=(Source&&) noexcept = default;
		~Source() = default;

		// Access the complete underlying text.
		[[nodiscard]] const std::string& Text() const noexcept { return contents; }

		// Convenience: number of bytes in the source text.
		[[nodiscard]] std::uint32_t Size() const noexcept { return static_cast<std::uint32_t>(contents.size()); }

		// Convenience: total number of lines (same as lines.size()).
		[[nodiscard]] std::uint32_t LineCount() const noexcept { return static_cast<std::uint32_t>(lines.size()); }

		// Reserve helpers for performance when caller can predict sizes.
		void ReserveLines(std::size_t n) { lines.reserve(n); }
		void ReserveTokens(std::size_t n) { tokens.reserve(n); }

		// Append a line record. Provided for the future tokenizer / line scanner.
		void AddLine(std::uint32_t byte_offset, std::uint32_t newline_token_index) {
			lines.emplace_back(byte_offset, newline_token_index);
		}

		// Append a token.
		void AddToken(ir::Token t) { tokens.push_back(t); }
	};
} // namespace compiler::text

