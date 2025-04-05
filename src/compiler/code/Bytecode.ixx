export module code.bytecode;

import code;
import code.bytecodes;

import <cstdint>;
import <functional>;
import <string_view>;
import <typeinfo>;
import <bit>;
import <cmath>;
import <limits>;
import <string>;
import <stdexcept>;
import <iostream>;

namespace code {
  export class Bytecode : public Code {
  private:
    code::Bytecodes type;
  public:
    Bytecode(code::Bytecodes t) : type{t} {}

    code::Bytecodes Type() const { return type; }
  };
};