export module compiler.output.Optimizer;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Instruction;

namespace compiler::output {
  export class Optimizer {
  private:
    program::Module& mod;
  public:
    virtual ~Optimizer() = default;
  };
};