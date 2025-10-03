export module ir.symbol.Registers;

import <cstdint>;
import <bitset>;

namespace ir::symbol {
  export struct Registers {
    std::bitset<64> value;
  };
};