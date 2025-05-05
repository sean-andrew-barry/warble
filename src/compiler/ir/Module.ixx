export module ir._module;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <atomic>;

import ir.program;
import ir.package;
import ir.symbols;
import ir.symbol;
import ir.block;

// ────────────────────────────────────────────────────────────────
//  Symbol view: non-owning, trivially-cheap accessor
// ────────────────────────────────────────────────────────────────
namespace ir {
  export class Module {
  public:
    enum class Stage : uint8_t {
      Initializing,
      Initialized,
      Tokenizing, // Building the characters/lines/tokens/widths and discovering dependencies
      Tokenized,
      Parsing, // Iterating tokens to build the symbols/blocks
      Parsed,
      Optimizing, // Refining the IR
      Optimized,
      Encoding, // Generating the machine code
      Encoded,
      Merging, // Merging into the Program to form the final binary
      Merged,
    };
  private:
    ir::Program& program;
    ir::Package& package;

    const std::string specifier;
    ir::Index index; // The symbol representing this module
    std::atomic<Stage> stage;
    
    std::vector<char32_t> characters; // The UTF-32 characters captured during parsing
    std::vector<uint32_t> lines; // The line table from the source
    std::vector<Token> tokens;
    std::vector<uint8_t> widths;
    
    std::vector<ir::Module*> dependencies; // The modules used when indexing symbols
    std::vector<ir::Index> enums; // The data backing enum literals
    Symbols symbols; // All the symbols defined in this module
    std::vector<ir::Block> blocks;
    ir::Block* active;
  public:
  };
};