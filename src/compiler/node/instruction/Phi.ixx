export module node.instruction.phi;

import node.instruction;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node {
  export class Declaration;
};

namespace node::instruction {
  export class Phi : public Instruction {
  private:
    std::vector<node::Declaration*> declarations;
    node::Declaration* result;
  public:
    using Instruction::Instruction;

    Phi(const lexical::source::Span& span, node::Block& block, std::vector<node::Declaration*>&& declarations, node::Declaration* result) noexcept;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    bool IsTerminator() const final;
  };
};