export module compiler.input.wbl.Lexer;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <stdexcept>;

import compiler.program.Module;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Instruction;

namespace compiler::input::wbl {
  export class Lexer {
  public:
    virtual ~Lexer() final = default;
  };
};