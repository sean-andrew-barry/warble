export module node.declaration.forward;

import node.declaration;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration {
  export class Forward : public Declaration {
  private:
    std::string name;
  public:
    using Declaration::Declaration;

    Forward(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept;
    Forward(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};