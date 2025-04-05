export module node.compiler.registration;

import node;
import <string>;
import <vector>;

namespace node::compiler {
  export class Registration : public Node {
  private:
    const std::string path;
    const std::string name;
    const std::vector<std::string> allowed;
    const node::compiler::Registration* const parent;
    const bool wildcard;
  public:
    using Node::Node;

    Registration(std::string&& path, std::string&& name, std::vector<std::string>&& allowed, node::compiler::Registration* parent, bool wildcard = false) noexcept;

    const std::string& Path() const;
    const std::vector<std::string>& Allowed() const;
    const node::compiler::Registration* Parent() const;

    // void Accept(Visitor& visitor) final;
    // void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};