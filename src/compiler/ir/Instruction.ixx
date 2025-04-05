export module ir.instruction;

import ir.codes;
import ir.index;
import <array>;

namespace ir {
  export struct Instruction {
    ir::Codes code;
    std::array<uint8_t, 3> registers;
    std::array<ir::Index, 3> operands;
  };
};