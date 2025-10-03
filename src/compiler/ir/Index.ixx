export module compiler.ir.Index;

import <stdexcept>;
import <cstdint>;

namespace compiler::ir {
  export class Index {
  private:
    uint32_t value;
  public:
    Index() : value{0} {}
    Index(uint32_t v) : value{v} {}

    uint32_t Value() const { return value; }
  };
};