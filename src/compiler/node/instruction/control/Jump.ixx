export module node.instruction.control.jump;

import node.instruction.control;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node {
  export class Block;
  export class Declaration;
};

namespace node::instruction::control {
  export class Jump : public Control {
  private:
    node::Block* target;
    std::shared_ptr<node::Declaration> condition;
  public:
    using Control::Control;

    Jump(const lexical::source::Span& span, node::Block* target) noexcept;
    Jump(const lexical::source::Span& span, std::shared_ptr<node::Declaration> condition) noexcept;
    Jump(const lexical::source::Span& span, node::Block* target, std::shared_ptr<node::Declaration> condition) noexcept;
    Jump(const lexical::source::Span& span, std::shared_ptr<node::Declaration> condition, node::Block* target) noexcept;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};