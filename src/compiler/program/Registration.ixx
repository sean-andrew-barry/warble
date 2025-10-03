export module compiler.program.Registration;

import <string>;
import <string_view>;
import <vector>;
import <memory>;

import compiler.program.Package; // forward declare only
import compiler.program.Permission;

namespace compiler::program {
  export class Package;

  // A registration is owned by the package that performed a 'register' statement.
  // It provides a local alias, permissions, and a pointer to the canonical target package.
  export class Registration {
  private:
    Package& owner;   // The package containing the register statement
    Package& target;  // The canonical shared package referenced
    std::string alias; // Local alias string
    std::vector<Permission> grants; // Permission assertions
  public:
    Registration(Package& owner, Package& target, std::string alias,
         std::vector<Permission>&& grants)
      : owner{owner}
      , target{target}
      , alias{std::move(alias)}
      , grants{std::move(grants)} {}

    Package& Owner() const { return owner; }
    Package& Target() const { return target; }
    const std::string& Alias() const { return alias; }
    const std::vector<Permission>& Grants() const { return grants; }

    // Merge additional grants (simple append; de-dup can be added later)
    void Merge(std::vector<Permission>&& extra) {
      grants.insert(grants.end(), std::make_move_iterator(extra.begin()), std::make_move_iterator(extra.end()));
    }
  };
};
