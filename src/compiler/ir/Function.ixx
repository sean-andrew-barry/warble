export module compiler.ir.Function;

import <cstdint>;
import <cstddef>;
import <functional>;

namespace compiler::ir {
  // A row index into the `compiler::ir::Functions` table.
  export class Function {
  private:
    uint32_t row;
  public:
    constexpr Function() noexcept : row{0} {}
    constexpr explicit Function(uint32_t v) noexcept : row{v} {}

    constexpr uint32_t Row() const noexcept { return row; }
    constexpr explicit operator bool() const noexcept { return row != 0; }

    friend constexpr bool operator==(Function a, Function b) noexcept { return a.row == b.row; }
    friend constexpr bool operator!=(Function a, Function b) noexcept { return !(a == b); }
  };
};

export namespace std {
  template <>
  struct hash<compiler::ir::Function> {
    constexpr size_t operator()(compiler::ir::Function idx) const noexcept {
      return static_cast<size_t>(idx.Row());
    }
  };
};
