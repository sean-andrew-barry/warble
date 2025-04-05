export module node.scope.context;

import node.scope;
import code.frame;
import <string_view>;
import <string>;
import <cstdint>;
import <vector>;

export class Visitor;
export class Updater;

namespace node {
  export class Declaration;
};

namespace node::scope {
  export class Context : public Scope {
  private:
    code::Frame frame;
    std::vector<node::Declaration*> live_stack;
    // node::Declaration* result; // The return of the function
    // std::vector<node::Declaration*> cases;
    int32_t largest_offset{0};
  public:
    using Scope::Scope;

    // Context(const lexical::source::Span& span, node::Scope* parent);

    // void Result(node::Declaration* result);
    // void Case(node::Declaration* c);
    // node::Declaration* Result() const;
    // const std::vector<node::Declaration*>& Cases() const;

    const std::vector<node::Declaration*>& LiveStack() const;
    void Push(node::Declaration& declaration);
    void Pop(node::Declaration& declaration);

    int32_t Offset(int32_t size) override;
    int32_t LargestOffset() const;
    code::Frame& Frame();

    bool IsContext() const final;
    node::scope::conditional::Loop* FindLoop() final;
    node::scope::Context* FindContext() final;
    node::scope::Context& GetContext() final;

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;
  };
};