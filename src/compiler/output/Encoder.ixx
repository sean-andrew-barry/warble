export module compiler.output.Encoder;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.program.Module;
import compiler.ir.Symbols;
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

    virtual void Encode(const std::vector<ir::Instruction>& instructions) = 0;
  };
};