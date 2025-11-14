export module compiler.Program;

import <memory>;
import <string>;
import <string_view>;
import <unordered_map>;
import <shared_mutex>;
import <filesystem>;

import compiler.program.Module;
import compiler.program.Package;
import compiler.fs.ID;

export class Compiler; // forward declaration

namespace compiler {
  export class Program {
  private:
    Compiler& compiler;
    mutable std::shared_mutex packages_mutex; // Guards access to package maps
    std::unordered_map<fs::ID, std::unique_ptr<program::Package>> packages;
  public:
    Program(Compiler& compiler) : compiler{compiler} {}

    program::Package& Register(std::filesystem::path&& path);
  };
};