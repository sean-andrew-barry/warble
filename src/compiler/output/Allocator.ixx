export module compiler.output.Allocator;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Instruction;

namespace compiler::output {
  export class Allocator {
  private:
    program::Module& mod;
  public:
    virtual ~Allocator() = default;
  };
};