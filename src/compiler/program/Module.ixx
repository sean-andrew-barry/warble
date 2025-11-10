export module compiler.program.Module;

import <cstdint>;
import <vector>;
import <string>;
import <string_view>;
import <atomic>;
import <filesystem>;

import compiler.Program;
import compiler.program.Package;
import compiler.ir.Token;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Instruction;
import compiler.utility.DualBuffer;
import compiler.fs.File;

namespace compiler::program {
  export class Module {
  private:
    static constexpr bool IsOdd(std::size_t v) noexcept { return (v & 1u) != 0u; }
    static constexpr bool IsEven(std::size_t v) noexcept { return (v & 1u) == 0u; }
  public:
    enum class Stage : size_t {
      Created, // The zero state, the module has been created but not yet processed
      Reading, // Loading the source text
      Read,
      Lexing, // Building the characters/lines/tokens and discovering dependencies
      Lexed,
      Parsing, // Iterating tokens to build the symbols and instructions
      Parsed,
      Validating, // Ensuring semantic correctness, including checking for cyclic dependencies
      Validated,
      Allocating, // Allocating registers and stack frame layout
      Allocated,
      Optimizing, // Refining the IR
      Optimized,
      Encoding, // Generating the machine code
      Encoded,
      Merging, // Merging into the Program to form the final binary
      Merged,
    };
  private:
    Program& program;
    program::Package& package;
    fs::File file;
    
    std::atomic<size_t> cycle{0}; // The current processing cycle for dependency tracking
    std::string source; // The raw source text of the module
    std::vector<program::Module*> dependencies; // Every imported module, possibly including self and/or a prelude module first
    
    std::vector<ir::Token> tokens;
    std::vector<std::pair<uint32_t, uint32_t>> lines; // The token index for each line start and the `characters` checkpoint
    std::vector<uint32_t> data; // The UTF-32 characters and bigint limbs captured during lexing 
    
    ir::Symbols symbols; // All the symbols defined in this module

    std::vector<ir::Instruction> instructions;
  private:
    static Stage RequiredDependencyStageFor(Stage next);
    static bool StageAtLeast(size_t dep_cycle, Stage required);
  private:
    bool Reading();
    bool Lexing();
    bool Parsing();
    bool Validating();
    bool Allocating();
    bool Optimizing();
    bool Encoding();
    bool Merging();

    bool Step(Stage stage);
  public:
    Module(Program& program, program::Package& package, fs::File&& file);

    bool Run(utility::DualBuffer<program::Module*>& buffer);

    // Register a package with the program
    program::Package& Register(std::filesystem::path&& specifier);

    // Import a module from the package
    program::Module& Import(std::filesystem::path&& specifier);
  };
}