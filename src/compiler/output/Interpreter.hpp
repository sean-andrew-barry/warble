#pragma once

#include <cstdint>
#include <bitset>
#include <vector>
#include <string>
#include <stdexcept>
#include "compiler/program/Module.hpp"
#include "compiler/ir/Symbols.hpp"
#include "compiler/ir/Instruction.hpp"
namespace compiler::output {
  class Interpreter {
  private:
    program::Module& mod;
  public:
    virtual ~Interpreter() = default;
  };
};