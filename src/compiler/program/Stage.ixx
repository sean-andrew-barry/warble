export module compiler.program.Stage;

import <cstdint>;

namespace compiler::program {
  export enum class Stage : uint8_t {
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
}