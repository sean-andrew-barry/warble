export module compiler.ir.Index;

import <cstdint>;
import <bitset>;

namespace compiler::ir {
  export class Index {
  private:
    uint32_t row;
  public:
    Index() : row{0} {}
    Index(uint32_t v) : row{v} {}

    uint32_t Row() const { return row; }
    explicit operator bool() const { return row != 0; }

    friend bool operator==(ir::Index a, ir::Index b) { return a.row == b.row; }
    friend bool operator!=(ir::Index a, ir::Index b) { return !(a == b); }
  };
};