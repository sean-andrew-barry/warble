export module compiler.input.wbl.Parser;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Instruction;
import compiler.ir.Token;

namespace compiler::input::wbl {
  export class Parser {
  public:
    virtual ~Parser() final = default;
  };
};