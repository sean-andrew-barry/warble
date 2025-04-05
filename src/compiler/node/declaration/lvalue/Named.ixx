export module node.declaration.lvalue.named;

import node.declaration.lvalue;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::declaration::lvalue {
  export class Phi;

  export class Named : public LValue {
  private:
    std::string name;
    std::shared_ptr<node::declaration::lvalue::Phi> phi;
  public:
    using LValue::LValue;

    Named(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept;
    Named(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept;

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