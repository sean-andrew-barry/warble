export module node.scope.context.function;

import node.scope.context;
import <string_view>;
import <string>;
import <vector>;

export class Visitor;
export class Updater;

namespace node {
  export class Declaration;
};

namespace node::scope::context {
  export class Function : public Context {
  private:
    node::Scope& parent; // May be any type of scope

    // std::vector<node::Declaration*> parameters;
    // std::vector<node::Declaration*> captures;
    // bool is_arrow = false;
  public:
    // using Context::Context;

    Function(const lexical::source::Span& span, node::Scope& parent);

    // void IsArrow(bool b);
    // bool IsArrow() const;
    // void Parameter(node::Declaration* parameter);
    // void Capture(node::Declaration* capture);
    // node::Declaration* Value() const;
    // const std::vector<node::Declaration*>& Parameters() const;
    // const std::vector<node::Declaration*>& Captures() const;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
    void Compile(code::x64::Encoder& encoder) override;
  };
};