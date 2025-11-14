export module compiler.text.cursor.Token;

import compiler.text.Cursor;
import compiler.ir.Token;
import <vector>;
import <bitset>;
import <initializer_list>;
import <string_view>;
import <stdexcept>;
import <unordered_map>;
import <utility>;
import <array>;
import <format>;
import <cstddef>;

namespace compiler::text::cursor {
  inline constexpr ir::Token end{ir::Token::None};
  constexpr ir::Token END() noexcept { return end; }

  export class Token : public compiler::text::Cursor<std::vector<ir::Token>, END> {
  private:
  public:
    using Super::Super;
    using Super::Peek;

    constexpr void Advance(std::size_t n = 1) {
      while (n > 0 && Valid()) {
        Super::Advance(1);
        --n;
      }
    }

   void Retreat(Iterator /*to*/) {
      throw std::runtime_error("Retreat is not valid in a Token cursor");
    }

    constexpr decltype(auto) Take() {
      auto current = Super::Peek();
      Advance(1); // Take has to be overridden so that we call the correct Advance
      
      return current;
    }

    constexpr bool Check(const ir::Token type) const {
      return Super::Peek() == type;
    }

    constexpr bool Match(const ir::Token type) {
      if (Super::Peek() == type) {
        Advance();
        return true;
      } else {
        return false;
      }
    }
  };
};