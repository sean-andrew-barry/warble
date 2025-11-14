export module compiler.output.Legalizer;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Instruction;

namespace compiler::output {
  export class Legalizer {
  private:
    program::Module& mod;
  public:
    virtual ~Legalizer() = default;
  };
};