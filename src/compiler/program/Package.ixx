export module compiler.program.Package;

import <cstdint>;
import <bitset>;
import <vector>;
import <string>;
import <atomic>;
import <memory>;
import <shared_mutex>;
import <string_view>;

import compiler.fs.Path;
import compiler.program.Registration;
import compiler.program.Permission;
import compiler.Program;
import compiler.program.Module;

namespace compiler::program {
  export class Package {
  private:
    Program& program;
    const std::string name;
    const std::string root;

    mutable std::shared_mutex modules_mutex; // Guards modules map
    std::unordered_map<std::string, std::unique_ptr<program::Module>> modules; // key: normalized specifier

    mutable std::shared_mutex registrations_mutex; // Guards registrations
    std::unordered_map<std::string, std::unique_ptr<program::Registration>> registrations; // alias -> registration
  public:
    Package(Program& program, std::string name, std::string root)
      : program{program}, name{std::move(name)}, root{std::move(root)} {}

    const std::string& Name() const { return name; }
    const std::string& Root() const { return root; }

      // Normalize a user specifier to a canonical key relative to root
    std::string Normalize(std::string_view specifier) const;

      // Thread-safe lookup (shared lock)
    program::Module* Find(std::string_view specifier) const;
    program::Module& Require(std::string specifier);

    // Registration (package alias) management
    program::Registration* FindRegistration(std::string_view alias) const;
    program::Package* ResolveAlias(std::string_view alias) const; // Returns target package or nullptr
    program::Registration& RegisterPackage(std::string alias, std::string url_or_path, std::vector<program::Permission> grants);
  };
};