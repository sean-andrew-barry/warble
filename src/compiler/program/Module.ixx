export module compiler.program.Module;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <atomic>;

import compiler.Program;
import compiler.program.Package;
import compiler.program.Stages;
import compiler.ir.Symbols;
import compiler.ir.Symbol;
import compiler.ir.Instruction;
import compiler.ir.symbol.Type;

namespace compiler::program {
  export class Module {
  private:
    Program& program;
    program::Package& package;

    const std::string specifier;
    std::string source; // The raw source text of the module
    program::Stages stages;
    std::vector<program::Module*> dependencies; // Every imported module, possibly including self and/or a prelude module first
    
    std::vector<Token> tokens;
    std::vector<uint8_t> widths; // Sequentially marks the width of every variable width token
    std::vector<uint32_t> lines; // Sequentially marks the position of every line start in the source
    std::vector<char32_t> characters; // The UTF-32 characters captured during parsing
    std::vector<ir::Index> enums; // The data backing enum literals
    
    ir::Symbols symbols; // All the symbols defined in this module

    std::vector<ir::Instruction> instructions;
  public:
    const std::vector<char32_t>& Characters() const { return characters; }
    const std::vector<uint32_t>& Lines() const { return lines; }
    const std::vector<lexical::Token>& Tokens() const { return tokens; }
    const std::vector<program::Module*>& Dependencies() const { return dependencies; }
    const ir::Symbols& Symbols() const { return symbols; }
    const std::vector<ir::Instruction>& Instructions() const { return instructions; }
    const std::string& Specifier() const { return specifier; }

    ir::Symbol Symbol(ir::Index index) {
      return symbols.Symbol(index);
    }

    Module(Program& program, program::Package& package, std::string specifier)
      : program{program}
      , package{package}
      , specifier{std::move(specifier)}
    {}

    void AddToken(ir::Token token) { tokens.push_back(token); }
    void AddToken(ir::Token token, uint8_t width) { tokens.push_back(token); widths.push_back(width); }

    void AddLine(std::string::const_iterator iter) {
      lines.push_back(static_cast<uint32_t>(std::distance(source.cbegin(), iter)));
    }

    void AddCharacter(char32_t ch) {
      characters.push_back(ch);
    }

    ir::Symbol AddSymbol(ir::symbol::Type type) {
      return ir::Symbol{*this, symbols.Add(type)};
    }

    void Append(lexical::Token token) { tokens.push_back(token); }
    void ReserveTokens(size_t size) { tokens.reserve(size); }
    void ResizeTokens(size_t size) { tokens.resize(size); }

    void Append(program::Module* mod) { dependencies.push_back(mod); }
    void Append(ir::Instruction& instruction) { instructions.push_back(instruction); }
  };
};