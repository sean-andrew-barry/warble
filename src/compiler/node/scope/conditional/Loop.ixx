export module node.scope.conditional.loop;

import node.scope.conditional;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::scope::conditional {
  export class Loop : public Conditional {
  private:
  public:
    using Conditional::Conditional;

    node::scope::conditional::Loop* FindLoop() final { return this; }

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};