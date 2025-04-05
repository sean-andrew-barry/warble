export module node.scope.context.function.inline_function;

import node.scope.context.function;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::scope::context::function {
  export class Inline : public Function {
  private:
  public:
    using Function::Function;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};