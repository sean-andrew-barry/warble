#pragma once

#include <cstdint>
#include <bitset>
#include <vector>
#include <string>
#include <stdexcept>
#include "compiler/program/Module.hpp"
#include "compiler/ir/Symbols.hpp"
#include "compiler/ir/Instruction.hpp"
#include "compiler/utility/Buffer.hpp"
namespace compiler::output {
  class Encoder {
  private:
    program::Module& mod;
    utility::Buffer code;
  public:
    virtual ~Encoder() = default;
    utility::Buffer& Code() { return code; }

    virtual void Encode(const std::vector<ir::Instruction>& instructions) = 0;
  };
};