#pragma once

#include <cstdint>
#include <bitset>
namespace ir::symbol {
  struct Registers {
    std::bitset<64> value;
  };
};