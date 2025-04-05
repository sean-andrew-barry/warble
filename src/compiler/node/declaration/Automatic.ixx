export module node.declaration.automatic;

import node.declaration;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::declaration {
  export class Automatic : public Declaration {
  private:
    node::Declaration* value;
  public:
    using Declaration::Declaration;

    Automatic(const lexical::source::Span& span, node::Scope& scope, node::Declaration* value) noexcept;

    void Value(node::Declaration* v) { value = v; }
    node::Declaration* Value() const { return value; }

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;

    void Add(code::x64::Encoder& encoder, node::Declaration& lhs, node::Declaration& rhs) override;
  };
};