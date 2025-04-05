export module node.scope.conditional.pattern.cover;

import node.scope.conditional.pattern;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::scope::conditional::pattern {
  export class Cover : public Pattern {
  private:
  public:
    using Pattern::Pattern;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};