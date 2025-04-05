export module node.compiler;

import node;
import node.scope;
import <string_view>;
import <string>;
import <memory>;
import <unordered_map>;
import <shared_mutex>;
import <fstream>;

export class Engine;
export class Visitor;
export class Updater;

namespace node::scope::context {
  export class Module;
};

namespace node {
  export class Compiler : public Node {
  private:
    std::shared_mutex mutex;
    std::unordered_map<std::string, node::scope::context::Module> modules;
    Engine& engine;
    node::Compiler* parent;
    std::string path;
    std::string name;
    std::vector<std::string> allowed;
    bool wildcard{false};
  public:
    using Node::Node;

    Compiler(Engine& engine, node::Compiler* parent, std::string&& path, std::string&& name, std::vector<std::string>&& allowed, bool wildcard = false);
    ~Compiler() final;

    node::Compiler* Register(std::string&& path, std::string&& name, std::vector<std::string>&& allowed, bool wildcard);
    node::scope::context::Module* Import(std::string&& specifier);
    bool AllowsImport(const std::string_view specifier) const;
    bool Queue(Node* node);

    node::scope::context::Module* Find(const std::string& name);

    node::Compiler* Parent();
    const std::string& Path() const;
    // const std::string& Name() const;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};