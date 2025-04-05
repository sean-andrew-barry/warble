export module ir.context._module;

import ir.context;
import ir.codes;
import ir.instruction;
import ir.symbol;
import ir.index;
import lexical.token;
import lexical.tokens;

import <vector>;

namespace ir::context {
  // Forward declaration for the Global
  namespace _module {
    export class Global;
  };

  export class Module : public ir::Context {
  private:
    std::vector<char32_t> characters; // The UTF-32 characters that will be embedded in the global character table
    std::vector<uint32_t> lines; // The line table from the source
    std::vector<lexical::Token> tokens;
    std::vector<ir::context::Module*> dependencies; // The modules used when indexing symbols
    std::vector<ir::Symbol> symbols; // All the symbols defined in this module
    std::vector<ir::Instruction> instructions;
  public:
    Module(ir::context::_module::Global* global)
      : characters{}
      , lines{0} // Line 1 always starts at index 0
      , dependencies{this, global} // Current module is always first, then the global is second
      , symbols{}
      , instructions{}
    {
    }

    const std::vector<char32_t>& Characters() const { return characters; }
    const std::vector<uint32_t>& Lines() const { return lines; }
    const std::vector<lexical::Token>& Tokens() const { return tokens; }
    const std::vector<ir::context::Module*>& Dependencies() const { return dependencies; }
    const std::vector<ir::Symbol>& Symbols() const { return symbols; }
    const std::vector<ir::Instruction>& Instructions() const { return instructions; }

    ir::Symbol& FindSymbol(ir::Index index) {
      return dependencies[index.Dependency()]->symbols[index.Symbol()];
    }

    void Append(char32_t ch) {
      characters.push_back(ch);
      
      // Check for newline or Unicode line/paragraph separators.
      if (ch == U'\n' || ch == U'\u2028' || ch == U'\u2029') {
        lines.push_back(static_cast<uint32_t>(characters.size()));
      }
    }

    void Append(lexical::Token token) { tokens.push_back(token); }
    void ReserveTokens(size_t size) { tokens.reserve(size); }
    void ResizeTokens(size_t size) { tokens.resize(size); }

    void Append(ir::context::Module* mod) { dependencies.push_back(mod); }
    void Append(ir::Symbol* symbol) { symbols.push_back(symbol); }
    void Append(ir::Instruction& instruction) { instructions.push_back(instruction); }
  };
};