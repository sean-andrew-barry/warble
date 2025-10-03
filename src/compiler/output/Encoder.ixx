export module compiler.output.Encoder;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Instruction;
import compiler.utility.Buffer;

namespace compiler::output {
  export class Encoder {
  private:
    program::Module& mod;
    utility::Buffer code;
  public:
    virtual ~Encoder() = default;
    utility::Buffer& Code() { return code; }

    ir::Symbol Get(const ir::Instruction& instruction, uint8_t operand) const {
      return mod.GetSymbol(instruction, operand);
    }

    virtual void Encode(const std::vector<ir::Instruction>& instructions) = 0;
  };
};