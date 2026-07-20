#pragma once

#include "compiler/text/Cursor.hpp"
#include "compiler/ir/Token.hpp"
#include <vector>
#include <bitset>
#include <initializer_list>
#include <string_view>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <array>
#include <format>
#include <cstddef>
namespace compiler::text::cursor {
  inline constexpr auto token_end = [] { return ir::Token::None; };

  class Token : public compiler::text::Cursor<std::vector<ir::Token>, token_end> {
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