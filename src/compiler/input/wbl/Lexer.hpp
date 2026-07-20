#pragma once

#include <cstdint>
#include <bitset>
#include <vector>
#include <string>
#include <stdexcept>
#include "compiler/program/Module.hpp"
#include "compiler/ir/Symbols.hpp"
#include "compiler/ir/Symbol.hpp"
#include "compiler/ir/Instruction.hpp"

namespace compiler::input::wbl {
  class Lexer {
  public:
    virtual ~Lexer() final = default;
  };
};