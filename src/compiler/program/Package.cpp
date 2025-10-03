import compiler.program.Package;
import compiler.program.Module;
import compiler.program.Registration;
import compiler.program.Permission;
import compiler.Program;
import compiler.fs.Path;

import <shared_mutex>;
import <stdexcept>;
import <utility>;

using namespace compiler;

namespace compiler::program {
  std::string Package::Normalize(std::string_view specifier) const {
    // If specifier already starts with '/', treat as absolute within package root; otherwise prefix '/'
    std::string normalized;
    normalized.reserve(root.size() + 1 + specifier.size());

    // Build full path relative to the package root; using fs::Path to collapse '.', '..'
    if (!specifier.empty() && specifier.front() == '/') {
      normalized = root; // root path (as provided when package created)
      normalized += specifier; // append raw specifier starting with '/'
    } else {
      normalized = root;
      if (normalized.empty() || normalized.back() != '/') normalized += '/';
      normalized.append(specifier);
    }

    // Use Path parser to canonicalize path segments (handles '.', '..')
    fs::Path path{normalized};
    return std::string{path.ToString()};
  }

  program::Module* Package::Find(std::string_view specifier) const {
    std::string key = Normalize(specifier);
    std::shared_lock lock{modules_mutex};
    auto it = modules.find(key);
    if (it == modules.end()) return nullptr;
    return it->second.get();
  }

  program::Module& Package::Require(std::string specifier) {
    std::string key = Normalize(specifier);

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
      it->second = std::make_unique<program::Module>(program, *this, key);
    }
    return *it->second;
  }

  program::Registration* Package::FindRegistration(std::string_view alias) const {
    std::shared_lock lock{registrations_mutex};
    auto it = registrations.find(std::string{alias});
    if (it == registrations.end()) return nullptr;
    return it->second.get();
  }

  program::Package* Package::ResolveAlias(std::string_view alias) const {
    auto* reg = FindRegistration(alias);
    return reg ? &reg->Target() : nullptr;
  }

  // Placeholder: Program currently lacks GetOrCreateByRoot; once added, this will call it.
  extern program::Package& GetOrCreateByRoot(program::Program& program, const std::string& root); // forward (to be implemented)

  program::Registration& Package::RegisterPackage(std::string alias, std::string url_or_path, std::vector<program::Permission> grants) {
    // For now just use the provided path as canonical root (normalize later via Program helper)
    // Acquire/create target package at program scope (will implement GetOrCreateByRoot soon).
    // Temporary direct call to Program::Register while GetOrCreateByRoot not implemented.
    program::Package& target = program.Register(alias + "::intrinsic", url_or_path); // distinct internal name

    // Fast path lookup
    {
      std::shared_lock lock{registrations_mutex};
      auto it = registrations.find(alias);
      if (it != registrations.end()) {
        it->second->Merge(std::move(grants));
        return *it->second;
      }
    }

    std::unique_lock lock{registrations_mutex};
    auto [it, inserted] = registrations.try_emplace(alias, nullptr);
    if (!inserted && it->second) {
      it->second->Merge(std::move(grants));
      return *it->second;
    }

    auto reg = std::make_unique<program::Registration>(*this, target, alias, std::move(grants));
    auto& ref = *reg;
    it->second = std::move(reg);
    return ref;
  }
}
