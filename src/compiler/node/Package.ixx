export module node.package;

import node;
import <string_view>;
import <string>;
import <vector>;
import <memory>;
import <unordered_map>;
import <shared_mutex>;
import <fstream>;

export class Visitor;
export class Updater;
export class Engine;

namespace node::scope::context {
  export class Module;
};

namespace node {
  // Different libraries are divided into Packages.
  // Packages contain Modules. Access to Modules must be thread safe, allowing workers to find or create
  // Modules at any time as they parse import statements.
  export class Package : public Node {
  public:
    static Package* STD;
  private:
    std::shared_mutex packages_mutex;
    std::shared_mutex modules_mutex;
    Engine& engine;
    node::Package* parent;
    std::vector<std::unique_ptr<node::Package>> packages; // Direct children of this package
    std::vector<std::unique_ptr<node::scope::context::Module>> modules; // Modules in this package
    std::string path;
    std::string name;
    std::vector<std::pair<std::string, std::string>> allowed;
  public:
    using Node::Node;

    Package(Engine& engine, node::Package* parent, std::string&& path, std::string&& specifier, std::vector<std::pair<std::string, std::string>>&& allowed);
    ~Package() final;

    node::Package& Register(std::string&& path, std::string&& name, std::vector<std::pair<std::string, std::string>>&& allowed);
    node::scope::context::Module& Import(std::string&& specifier, std::string&& source);
    bool AllowsImport(const std::string& specifier) const;
    const std::string& Path() const;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    void Compile(code::x64::Encoder& encoder) final;
  };
};