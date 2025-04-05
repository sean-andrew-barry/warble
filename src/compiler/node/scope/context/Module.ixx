export module node.scope.context._module;

import node;
import node.scope.context;
import lexical.source;
import utility.print;
import <string_view>;
import <string>;
import <vector>;
import <memory>;
import <functional>;
import <unordered_map>;
import <utility>;

// #include "../../../utility/Macros.hpp"

export class Visitor;
export class Updater;

namespace node {
  export class Compiler;
  export class Declaration;
};

namespace node::declaration::literal {
  export class Primitive;
};

namespace node {
  export class Package;
};

namespace node::declaration::rvalue::literal::structured {
  export class Object;
};

namespace node::scope::context {
  export class Function;

  export class Module : public Context {
  private:
    node::Package& parent;
    const std::string specifier;
    lexical::Source source;

    std::vector<node::scope::context::Module*> imports;
    std::vector<node::Declaration*> exports;
    std::vector<node::scope::context::Function*> functions;
    std::vector<node::declaration::literal::Primitive*> primitives;

    // code::Placeholder32 placeholder;
    // std::vector<Node*> registry; // Nodes that have been registered for recompilation
    // std::atomic<State> state;

    // std::unordered_map<std::string, std::function<void()>> functions;
    // std::unordered_map<std::string, Function> functions;

    // extern "C" void TestCall(node::scope::context::Module* mod) {
    //   utility::Print("Successfully called TestCall", reinterpret_cast<const void*>(mod));
    // }
    
    // static void DoSomething(Module* mod);
  public:
    using Context::Context;

    Module(node::Package& parent, std::string&& specifier, std::string&& content);
    ~Module() final;

    node::Package& Register(std::string&& path, std::string&& name, std::vector<std::pair<std::string, std::string>>&& allowed);
    node::scope::context::Module& Import(std::string&& specifier, std::string&& source);

    void AddFunction(node::scope::context::Function* function);
    void AddPrimitive(node::declaration::literal::Primitive* primitive);

    node::Package& Parent() const;
    const std::string& Specifier() const;
    const lexical::Source& Source() const;

    bool Parse();
    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;
    void Compile(code::x64::Encoder& encoder) final;
  };
};