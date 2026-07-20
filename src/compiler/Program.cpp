#include "compiler/Program.hpp"
#include "Compiler.hpp"
#include "compiler/fs/File.hpp"
#include "compiler/fs/ID.hpp"
#include "compiler/program/Module.hpp"
#include "compiler/program/Package.hpp"
#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <utility>
namespace compiler {
  Program::Program(Compiler& compiler)
    : compiler{compiler}
  {}

  program::Package& Program::Register(std::filesystem::path&& path) {
    fs::File file{std::move(path)};

    if (!file) {
      throw std::runtime_error("Failed to open package: " + file.Path().string());
    }

    const fs::ID key = file.ID();

    // First shared lock lookup
    {
      std::shared_lock lock{packages_mutex};
      auto it = packages.find(key);
      if (it != packages.end()) {
        return *it->second;
      }
    }

    // Upgrade to unique lock for creation
    std::unique_lock lock{packages_mutex};
    auto [it, inserted] = packages.try_emplace(key, nullptr);
    if (inserted) {
      // Construct the package now that we own the slot
      it->second = std::make_unique<program::Package>(*this, std::move(file));
    }
    
    return *it->second;
  }
};

