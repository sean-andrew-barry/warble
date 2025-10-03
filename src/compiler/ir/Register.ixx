export module compiler.ir.Register;

import <cstdint>;

namespace compiler::ir {
  export struct Register {
    uint8_t value;
  };
};

static_assert(sizeof(compiler::ir::Register) == 1, "ir::Register must remain 1 byte");