export module node.declaration.rvalue.literal.structured.function;

import node.declaration.rvalue.literal.structured;
import <string_view>;
import <string>;
import <memory>;

export class Visitor;
export class Updater;

namespace node::scope::context {
  export class Function;
};

namespace node::declaration::rvalue::literal::structured {
  export class Object;
  
  export class Function : public Structured {
  private:
    std::vector<Symbol> captures;
    std::vector<Symbol> parameters;
    node::Block entry;
    size_t offset;
    bool is_arrow = false;
  public:
    using Structured::Structured;

    Function(const lexical::source::Span& span) noexcept;
    Function(const lexical::source::Span& span, std::vector<Symbol>&& parameters, std::vector<Symbol>&& captures, bool is_arrow = false) noexcept;

    std::shared_ptr<node::declaration::rvalue::literal::structured::Object> TypeOf() const final;

    void IsArrow(bool b);
    bool IsArrow() const;
    void Parameter(std::shared_ptr<node::Declaration>&& parameter);
    void Capture(std::shared_ptr<node::Declaration>&& capture);
    std::shared_ptr<node::Declaration> Value() const;

    const node::scope::context::Function& FunctionScope() const;
    const std::vector<std::shared_ptr<node::Declaration>>& Parameters() const;
    const std::vector<std::shared_ptr<node::Declaration>>& Captures() const;
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