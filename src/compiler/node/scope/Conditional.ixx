export module node.scope.conditional;

import node.scope;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::scope {
  export class Conditional : public Scope {
  private:
    node::Scope& parent; // May be any type of scope
  public:
    // using Scope::Scope;

    Conditional(const lexical::source::Span& span, node::Scope& parent);

    bool IsContext() const final { return false; }

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};