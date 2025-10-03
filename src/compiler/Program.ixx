export module compiler.Program;

import <memory>;
import <string>;
import <string_view>;
import <unordered_map>;
import <shared_mutex>;

import compiler.program.Module;
import compiler.program.Package;

namespace compiler {
  export class Program {
  private:
    mutable std::shared_mutex mutex; // Guards access to packages map
    std::unordered_map<std::string, std::unique_ptr<program::Package>> packages; // legacy name keyed
    std::unordered_map<std::string, std::unique_ptr<program::Package>> root_packages; // canonical root keyed
  public:
    Program() = default;

    program::Package& Register(std::string name, std::string root_path);
    program::Package* Find(std::string_view name) const;
    program::Package& GetOrCreateByRoot(const std::string& canonical_root);

    // Build graph entrypoints
    program::Module& Import(std::string_view package_name, std::string specifier);

    // Scheduling/execution to be added later
    // void BuildAll();
  };
};