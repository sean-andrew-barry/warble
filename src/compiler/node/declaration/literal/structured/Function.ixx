export module node.declaration.literal.structured.function;

import node.declaration.literal.structured;
import <string_view>;
import <string>;

export class Visitor;
export class Updater;

namespace node::scope::context {
  export class Function;
};

namespace node::declaration::literal::structured {
  export class Function : public Structured {
  private:
    node::scope::context::Function& function_scope;
    std::vector<node::Declaration*> parameters;
    std::vector<node::Declaration*> captures;
    size_t offset;
    bool is_arrow = false;
  public:
    using Structured::Structured;

    Function(const lexical::source::Span& span, node::Scope& scope, node::scope::context::Function& function_scope) noexcept;
    Function(const lexical::source::Span& span, node::Scope& scope, node::scope::context::Function& function_scope, std::vector<node::Declaration*>&& parameters, std::vector<node::Declaration*>&& captures, bool is_arrow = false) noexcept;

    void IsArrow(bool b);
    bool IsArrow() const;
    void Parameter(node::Declaration* parameter);
    void Capture(node::Declaration* capture);
    node::Declaration* Value() const;

    const node::scope::context::Function& FunctionScope() const;
    const std::vector<node::Declaration*>& Parameters() const;
    const std::vector<node::Declaration*>& Captures() const;
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    std::vector<std::type_index> Signature() const final;

    void Construct(code::x64::Encoder& encoder) final;
    void Call(code::x64::Encoder& encoder, node::Declaration& rhs) final;
    // void Compile(code::x64::Encoder& encoder) override;
  };
};