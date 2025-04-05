export module node.declaration.lvalue.named.parameter;

import node.declaration.lvalue.named;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::declaration::lvalue::named {
  export class Phi;

  export class Parameter : public Named {
  private:
    std::vector<Symbol> types; // Parameters are templates that can have many types
  public:
    using Named::Named;

    Parameter(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept;
    Parameter(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept;

    void Name(std::string_view v);
    void Name(std::string&& v);

    void Phi(std::shared_ptr<node::declaration::lvalue::Phi>&& v);
    std::shared_ptr<node::declaration::lvalue::Phi> Phi() const;

    bool Match(const std::string_view v) const final;
    bool Match(const std::string& v) const final;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};