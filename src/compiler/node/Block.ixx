export module node.block;

import node;
import lexical.source.span;
import code.registers;
import code.frame;
// import node.scope;
// import node._module;

import <string_view>;
import <string>;
import <vector>;
import <memory>;
import <unordered_set>;

export class Visitor;
export class Updater;

export namespace code {
  class Encoder;
};

export namespace code::x64 {
  class Encoder;
};

namespace node {
  export class Instruction;
  export class Declaration;
  export class Scope;

  namespace scope {
    export class Context;
  };

  namespace instruction::memory {
    export class Destruct;
  };

  export class Block : public Node {
  private:
    std::vector<std::unique_ptr<node::Instruction>> instructions;
    std::vector<Block*> predecessors;
    std::vector<Block*> successors;
    // node::Scope& scope;
    code::Registers registers;
    // code::Offset offset;
  public:
    using Node::Node;

    Block(const lexical::source::Span& span) noexcept;
    ~Block() final;

    std::unique_ptr<node::Instruction> Remove(node::Instruction* instruction);

    // void AddLiveDeclarations(const std::vector<node::Declaration*>& live_stack);
    void Add(std::unique_ptr<node::Instruction>&& instruction);

    template<typename T, typename... Args>
    T* Instruct(const lexical::source::Span& span, Args&&... args) {
      auto uptr = std::make_unique<T>(span, std::forward<Args>(args)...);
      T* ptr = uptr.get();
      instructions.emplace_back(std::move(uptr));
      return ptr;
    }

    code::Registers& Registers();
    // size_t GetIndex() const;
    bool IsTerminated() const;
    // bool IsEntry() const;
    // bool IsExit() const;

    void Successor(Block* block);
    void Predecessor(Block* block);

    bool RemovePredecessor(Block* block);
    bool RemoveSuccessor(Block* block);

    const std::vector<std::unique_ptr<node::Instruction>>& Instructions() const;
    const std::vector<std::unique_ptr<node::Block*>>& Predecessor() const;
    const std::vector<std::unique_ptr<node::Block*>>& Successors() const;
    // node::scope::Context& Context();
    // void Push(node::Declaration& declaration);
    // void Pop(node::Declaration& declaration);

    void Accept(Visitor& visitor) final;
    void Update(Updater& updater) final;
    int32_t Size() const final;
    std::string Name() const final;
    std::string ToString() const final;

    virtual void Declare(code::Encoder& encoder) final;
    virtual void Compile(code::Encoder& encoder) final;
    virtual void Compile(code::x64::Encoder& encoder) final;
  };
};