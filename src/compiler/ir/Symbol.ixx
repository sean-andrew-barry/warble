export module compiler.ir.Symbol;

import <cstdint>;
import <cstddef>;
import <functional>;

namespace compiler::ir {
  // A row index into the `compiler::ir::Symbols` table.
  // This is intended to eventually replace `compiler::ir::Symbol`.
  export class Symbol {
  private:
    uint32_t row;
  public:
    constexpr Symbol() noexcept : row{0} {}
    constexpr explicit Symbol(uint32_t v) noexcept : row{v} {}

    constexpr uint32_t Row() const noexcept { return row; }
    constexpr explicit operator bool() const noexcept { return row != 0; }

    friend constexpr bool operator==(Symbol a, Symbol b) noexcept { return a.row == b.row; }
    friend constexpr bool operator!=(Symbol a, Symbol b) noexcept { return !(a == b); }
  };
};

export namespace std {
  template <>
  struct hash<compiler::ir::Symbol> {
    constexpr size_t operator()(compiler::ir::Symbol idx) const noexcept {
      return static_cast<size_t>(idx.Row());
    }
  };
};
