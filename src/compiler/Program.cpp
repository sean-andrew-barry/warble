import compiler.Program;
import Compiler;
import compiler.fs.File;
import compiler.fs.ID;
import compiler.program.Module;
import compiler.program.Package;

import <filesystem>;
import <memory>;
import <shared_mutex>;
import <stdexcept>;
import <utility>;

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

