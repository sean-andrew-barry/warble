export module node.declaration.typed;

import node.declaration;
import node.declaration.modifiers;
import <string_view>;
import <string>;
import <bitset>;

export class Visitor;
export class Updater;

namespace node::declaration {
  export class Typed : public Declaration {
  private:
    std::string name;
    node::Declaration* type;
    node::Declaration* value;
    node::Declaration* link;
    std::bitset<16> modifiers;
  public:
    using Declaration::Declaration;
    using Declaration::Name;

    Typed(const lexical::source::Span& span, node::Scope& scope, const std::string& name) noexcept;
    Typed(const lexical::source::Span& span, node::Scope& scope, std::string_view name) noexcept;
    Typed(const lexical::source::Span& span, node::Scope& scope, node::Declaration* value) noexcept;
    Typed(const lexical::source::Span& span, node::Scope& scope, std::string& name, node::Declaration* value) noexcept;

    void Name(std::string&& v);
    void Name(std::string_view v);
    void Type(node::Declaration* v);
    void Value(node::Declaration* v);
    void Link(node::Declaration* v);

    void SetModifier(node::declaration::Modifiers modifier);
    bool HasModifier(node::declaration::Modifiers modifier) const;

    node::Declaration* Type() const;
    node::Declaration* Value() const;
    node::Declaration* Link() const;

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;
    void Construct(code::x64::Encoder& encoder) final;
  };
};