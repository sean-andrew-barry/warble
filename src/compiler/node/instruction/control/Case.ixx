export module node.instruction.control._case;

import node.instruction.control;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node {
  export class Declaration;
};

namespace node::instruction::control {
  export class Case : public Control {
  private:
    std::shared_ptr<node::Declaration> result;
  public:
    using Control::Control;

    Case(const lexical::source::Span& span, std::shared_ptr<node::Declaration> result);

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};