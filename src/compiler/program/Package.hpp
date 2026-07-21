#pragma once

#include <cstdint>
#include <bitset>
#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>
#include <filesystem>

#include "compiler/fs/File.hpp"
#include "compiler/fs/ID.hpp"

namespace compiler {
  class Program;
};

namespace compiler::program {
  class Module;

  class Package {
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