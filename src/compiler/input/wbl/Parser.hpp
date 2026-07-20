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
#include "compiler/ir/Token.hpp"

namespace compiler::input::wbl {
  class Parser {
  public:
    virtual ~Parser() final = default;
  };
};