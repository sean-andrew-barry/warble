export module compiler.program.Package;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <atomic>;
import <memory>;
import <shared_mutex>;
import <string_view>;
import <unordered_map>;
import <filesystem>;

import compiler.fs.File;
import compiler.fs.ID;

namespace compiler {
  export class Program;
};

namespace compiler::program {
  export class Module;

  export class Package {
  private:
    compiler::Program& program;
    compiler::fs::File root;

    mutable std::shared_mutex modules_mutex; // Guards modules map
    std::unordered_map<compiler::fs::ID, std::unique_ptr<compiler::program::Module>> modules; // key: normalized specifier
  public:
    Package(compiler::Program& program, compiler::fs::File&& root);

    std::filesystem::path Name() const;
    const std::filesystem::path& Root() const;
    compiler::program::Module& Import(std::filesystem::path&& specifier);
  };
};