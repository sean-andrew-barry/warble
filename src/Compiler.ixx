export module Compiler;

import compiler.Engine;
import <memory>;
import <vector>;
import <string>;

export class Compiler {
private:
  std::vector<std::string> commands;
  std::unique_ptr<compiler::Engine> engine;
public:
  Compiler(std::vector<std::string>&& commands) : commands{std::move(commands)} {
    // Spawn an Engine instance. The Compiler now owns the commands; Engine no longer takes them.
    engine = std::make_unique<compiler::Engine>();
  }
};