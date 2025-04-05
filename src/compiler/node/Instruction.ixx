export module node.instruction;

import node;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

export namespace node {
  class Block;
};

namespace node {
  export class Instruction : public Node {
  public:
    using Node::Node;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;

    virtual bool IsAssignment() const { return false; }
    virtual bool IsTerminator() const { return false; }
  };
};