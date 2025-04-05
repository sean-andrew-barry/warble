export module lexical.token;

import lexical.tokens;
import <cstdint>;
import <string_view>;
import <string>;

namespace lexical {
  export class Token {
  private:
    // This Token type will be 8 bytes in total
    lexical::Tokens type = Tokens::NONE;
    int8_t code_point = 0;
    uint16_t width = 0; // Width of this token
    uint32_t index = 0; // Position in the source text
  public:
    constexpr Token() noexcept : type{Tokens::NONE} {}

    constexpr Token(lexical::Tokens type) noexcept : type{type} {}

    constexpr Token(lexical::Tokens type, uint16_t width, uint32_t index) noexcept
      : type{type}, width{width}, index{index} {}

    constexpr uint8_t Raw() const { return static_cast<uint8_t>(type); }
    constexpr lexical::Tokens Type() const { return type; }
    constexpr uint16_t Width() const { return width; }
    constexpr uint32_t Index() const { return index; }

    constexpr void Type(lexical::Tokens v) { type = v; }
    constexpr void Width(uint16_t v) { width = v; }
    constexpr void Index(uint32_t v) { index = v; }

    std::string_view Name() const;
    std::string ToString() const;

    bool IsTrivial() const;
    bool IsLiteral() const;
    bool IsEscape() const;
    bool IsContextual() const;
    bool IsImplicit() const;
    bool IsBinary() const;
    bool IsUnaryPrefix() const;
    bool IsUnaryPostfix() const;
    bool IsUnary() const;
    bool IsStatement() const;
    bool IsModifier() const;
    bool IsType() const;
    bool IsSymbol() const;
    bool IsMisc() const;
    bool IsIdentifiable() const;
    bool IsDeclarable() const;
    bool IsRightAssociative() const;

    int Precedence() const;
  };
};