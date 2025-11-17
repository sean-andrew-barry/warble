export module compiler.ir.Index;

import <cstdint>;
import <cstddef>;
import <functional>;

namespace compiler::ir {
  export class Index {
  private:
    uint32_t row;
  public:
    constexpr Index() noexcept : row{0} {}
    constexpr explicit Index(uint32_t v) noexcept : row{v} {}

    constexpr uint32_t Row() const noexcept { return row; }
    constexpr explicit operator bool() const noexcept { return row != 0; }

    friend constexpr bool operator==(Index a, Index b) noexcept { return a.row == b.row; }
    friend constexpr bool operator!=(Index a, Index b) noexcept { return !(a == b); }
  };
};

export namespace std {
  template <>
  struct hash<compiler::ir::Index> {
    constexpr size_t operator()(compiler::ir::Index idx) const noexcept {
      return static_cast<size_t>(idx.Row());
    }
  };
};