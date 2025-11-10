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
import compiler.Program;
import compiler.program.Module;

namespace compiler::program {
  export class Package {
  private:
    Program& program;
    fs::File root;

    mutable std::shared_mutex modules_mutex; // Guards modules map
    std::unordered_map<fs::ID, std::unique_ptr<program::Module>> modules; // key: normalized specifier
  public:
    Package(Program& program, fs::File&& root);

    std::filesystem::path Name() const;
    const std::filesystem::path& Root() const;
    program::Module& Import(std::filesystem::path&& specifier);
  };
};