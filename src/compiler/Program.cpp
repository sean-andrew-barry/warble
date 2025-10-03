import compiler.Program;
import compiler.program.Package;
import compiler.program.Module;

import <shared_mutex>;
import <utility>;
import <stdexcept>;

namespace compiler {
  program::Package& Program::Register(std::string name, std::string root_path) {
    // Fast path: check if exists under shared lock
    {
      std::shared_lock lock{mutex};
      auto it = packages.find(name);
      if (it != packages.end()) {
        return *it->second;
      }
    }
  
    // Upgrade to unique lock to insert
    std::unique_lock lock{mutex};
    auto [it, inserted] = packages.try_emplace(name, nullptr);
    if (!inserted) {
      // Another thread inserted between locks
      return *it->second;
    }
    it->second = std::make_unique<program::Package>(*this, it->first, std::move(root_path));
    return *it->second;
  }
  
  program::Package* Program::Find(std::string_view name) const {
    std::shared_lock lock{mutex};
    auto it = packages.find(std::string{name});
    if (it == packages.end()) return nullptr;
    return it->second.get();
  }
  
  program::Module& Program::Import(std::string_view package_name, std::string specifier) {
    auto* pkg = Find(package_name);
    if (!pkg) {
      throw std::runtime_error("Package not found");
    }
    return pkg->Require(std::move(specifier));
  }
  
  program::Package& Program::GetOrCreateByRoot(const std::string& canonical_root) {
    // Fast path: shared lock
    {
      std::shared_lock lock{mutex};
      auto it = root_packages.find(canonical_root);
      if (it != root_packages.end()) return *it->second;
    }
  
    std::unique_lock lock{mutex};
    auto [it, inserted] = root_packages.try_emplace(canonical_root, nullptr);
    if (inserted) {
      it->second = std::make_unique<program::Package>(*this, canonical_root, canonical_root);
    }
    return *it->second;
  }
};

