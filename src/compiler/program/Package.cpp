#include "compiler/program/Package.hpp"
#include "compiler/Program.hpp"
#include "compiler/fs/File.hpp"
#include "compiler/fs/ID.hpp"
#include "compiler/program/Module.hpp"
#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <utility>
namespace compiler::program {
  Package::Package(compiler::Program& program, compiler::fs::File&& root)
    : program{program}, root{std::move(root)}
  {}

  std::filesystem::path Package::Name() const { return root.Path().filename(); }
  const std::filesystem::path& Package::Root() const { return root.Path(); }

  compiler::program::Module& Package::Import(std::filesystem::path&& specifier) {
    if (specifier.extension().empty()) {
      specifier.replace_extension(".wbl");
    }

    compiler::fs::File file{root.Path() / specifier};

    if (!file) {
      throw std::runtime_error("Failed to open file: " + file.Path().string());
    }

    compiler::fs::ID key = file.ID();

    // First shared lock lookup
    {
      std::shared_lock lock{modules_mutex};
      auto it = modules.find(key);
      if (it != modules.end()) {
        return *it->second;
      }
    }

    // Upgrade to unique lock for creation
    std::unique_lock lock{modules_mutex};
    auto [it, inserted] = modules.try_emplace(key, nullptr);
    if (inserted) {
      // Construct the module now that we own the slot
      it->second = std::make_unique<compiler::program::Module>(program, *this, std::move(file));
    }
    
    return *it->second;
  }
}
