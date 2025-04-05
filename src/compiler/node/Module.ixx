export module node._module;

import node;
import utility.print;
import <string_view>;

export class Visitor;
export class Updater;

namespace node {
  export class Module : public Node {
  private:
  public:
    using Node::Node;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};