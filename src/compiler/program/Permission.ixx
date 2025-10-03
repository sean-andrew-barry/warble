export module compiler.program.Permission;

import <string>;
import <vector>;

namespace compiler::program {
  export class Package;

  // Represents allowed specifiers (or wildcard) in a target package namespace.
  export class Permission {
  private:
    Package& target;                             // Direct reference to target package
    std::vector<std::string> allowed_specifiers; // normalized or raw specifiers
    bool wildcard_all{false};
  public:
    Permission(Package& target,
               std::vector<std::string> allowed_specifiers,
               bool wildcard_all = false)
      : target{target}
      , allowed_specifiers{std::move(allowed_specifiers)}
      , wildcard_all{wildcard_all} {}

    Package& Target() const { return target; }
    const std::vector<std::string>& AllowedSpecifiers() const { return allowed_specifiers; }
    bool WildcardAll() const { return wildcard_all; }
  };
};
