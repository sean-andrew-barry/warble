export module node.declaration.rvalue.temporary;

import node.declaration;
import node.declaration.rvalue;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::declaration::rvalue {
  export class Temporary : public RValue {
  private:
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> type;
  public:
    using RValue::RValue;

    Temporary(const lexical::source::Span& span, node::Scope& scope, std::shared_ptr<node::declaration::rvalue::literal::structured::Object>&& type) noexcept;

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const final;

    void Type(std::shared_ptr<node::declaration::rvalue::literal::structured::Object> v) { type = std::move(v); }
    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Type() const { return type; }

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
  };
};