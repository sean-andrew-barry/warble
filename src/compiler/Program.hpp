#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <shared_mutex>
#include <filesystem>
#include "compiler/program/Module.hpp"
#include "compiler/program/Package.hpp"
#include "compiler/fs/ID.hpp"
class Compiler; // forward declaration

namespace compiler {
  class Program {
  private:
    Compiler& compiler;
    mutable std::shared_mutex packages_mutex; // Guards access to package maps
    std::unordered_map<fs::ID, std::unique_ptr<program::Package>> packages;
  public:
    Program(Compiler& compiler);

    program::Package& Register(std::filesystem::path&& path);
  };
};