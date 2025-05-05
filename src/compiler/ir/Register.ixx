export module ir._register;

import <cstdint>;

namespace ir {
  export struct Register {
    uint8_t value;
  };
};

static_assert(sizeof(ir::Register) == 1, "ir::Register must remain 1 byte");