export module node.declaration.lvalue.phi;

import node.declaration.lvalue;
import <string>;
import <vector>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::declaration::lvalue {
  export class Phi : public LValue {
  private:
    std::shared_ptr<node::declaration::LValue> source;
    std::vector<std::shared_ptr<node::declaration::LValue>> declarations;
  public:
    using LValue::LValue;

    Phi(const lexical::source::Span& span, node::Scope& scope, std::shared_ptr<node::declaration::LValue>&& source) noexcept;

    void Add(std::shared_ptr<node::declaration::LValue>&& v);

    void Declarations(std::vector<std::shared_ptr<node::declaration::LValue>>&& v);
    const std::vector<std::shared_ptr<node::declaration::LValue>>& Declarations() const;

    bool Match(const std::string_view v) const final;
    bool Match(const std::string& v) const final;
    bool Match(const node::declaration::rvalue::literal::structured::Enum& v) const final;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};