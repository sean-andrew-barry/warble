export module node.scope;

import node;
import node.scopes;
import node.block;
import code.registers;
import code.frame;
// import code.address;
// import node.declaration;
import lexical.source.span;

import <string_view>;
import <string>;
import <vector>;
import <stack>;
import <unordered_map>;
import <memory>;

export class Visitor;
export class Updater;

namespace node {
  namespace scope {
    export class Context;

    namespace context {
      export class Module;
    };

    namespace conditional {
      export class Loop;
    };
  };

  export class Instruction;
  export class Declaration;

  export class Scope : public Node {
  private:
    // node::scope::context::Module& mod;
    std::vector<std::unique_ptr<node::Scope>> scopes; // Can be any Context or Conditional
    std::vector<std::unique_ptr<node::Block>> blocks; // The instruction blocks of this scope
    // node::Block exit; // The special designated exit block, always last

    // std::vector<node::Declaration*> live_stack;
    // code::Frame frame;

    node::Scope* parent{nullptr};
    node::Block* last_body_block{nullptr};
    code::Registers registers;
    // code::Address address;
    int32_t current_offset{0};
    size_t start_position;
  public:
    Scope(const lexical::source::Span& span, node::Scope* parent = nullptr);
    ~Scope() final;

    void OffsetDeclaration(auto declaration) {
      // Scope* context = FindContext();
      // auto size = declaration->Size();
      // context->offset += size;
      // // utility::Print("Offsetting declaration", size, context->offset);
      // declaration->Offset(context->offset);
    }

    void StartPosition(size_t v) { start_position = v; }
    size_t StartPosition() const { return start_position; }

    virtual int32_t Offset(int32_t size);
    int32_t CurrentOffset() const;

    virtual bool Register(std::string&& path, std::string&& specifier, std::vector<std::string>&& modules);
    virtual bool Import(std::string&& specifier);

    // void Add(std::unique_ptr<Node>&& node);
    void Add(std::unique_ptr<node::Scope>&& node);
    void Add(std::unique_ptr<node::Block>&& node);
    void Slide(node::Instruction* instruction);

    virtual bool IsContext() const = 0;
    virtual node::scope::conditional::Loop* FindLoop();
    virtual node::scope::Context* FindContext();
    virtual node::scope::Context& GetContext();
    virtual node::scope::context::Module& GetModule();
    // code::Frame::SizeType Push(node::Declaration& declaration);
    // code::Frame::SizeType Pop(node::Declaration& declaration);

    node::Scope* Parent();

    const std::vector<std::unique_ptr<node::Scope>>& Scopes() const;
    const std::vector<std::unique_ptr<node::Block>>& Blocks() const;
    // const std::vector<node::Declaration*>& LiveStack() const;

    const node::Block& EntryBlock() const;
    const node::Block& LastBodyBlock() const;
    const node::Block& ExitBlock() const;
    
    const node::Block* FirstBlock() const;
    const node::Block* LastBlock() const;
    node::Block* FirstBlock();
    node::Block* LastBlock();

    node::Block& RequiredFirstBlock();
    node::Block& RequiredLastBlock();

    node::Block& EntryBlock();
    node::Block& LastBodyBlock();
    node::Block& ExitBlock();
    code::Registers& Registers();
    code::Frame& Frame();
    // void Registers(code::Registers v);
    // node::Module& Module();

    void Accept(Visitor& visitor) override;
    void Update(Updater& updater) override;
    int32_t Size() const override;
    std::string Name() const override;
    std::string ToString() const override;

    void Compile(code::x64::Encoder& encoder) override;
  };
};