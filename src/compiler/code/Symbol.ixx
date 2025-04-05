export module code.symbol;

import <cstdint>;
import <string>;
import <vector>;

namespace node {
  export class Declaration;
};

namespace code {
  export class Symbol {
  private:
    std::vector<node::Declaration*> links;
    node::Declaration& source;
    node::Declaration* type;
    node::Declaration* contract;
    uint16_t size; // The size of the declaration in bytes
    uint16_t offset; // Bytes offset from the parent object, 0 for a non-member
  public:
    // Symbol(code::Frame& source, code::Frame& target, uint32_t displacement, uint8_t size) noexcept;
  };
};