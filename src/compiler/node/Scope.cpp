import node.scope;
import node.scope.context;
import node.scope.context._module;
import node.block;
import node.declaration;
import node.instruction;
import code.encoder;
import code.x64.encoder;
import code.registers;
import utility.print;
import visitor;
import updater;

import <ranges>;
import <cstdint>;
import <typeinfo>;

namespace node {
  Scope::Scope(const lexical::source::Span& span, node::Scope* parent)
    : Node{span}, parent{parent}
  {
    // nodes.emplace_back(std::make_unique<node::Block>(span.ToBegin(), *this));
    // nodes.emplace_back(std::make_unique<node::Block>(span, *this));
    // nodes.emplace_back(std::make_unique<node::Block>(span.ToEnd(), *this));

    // last_body_block = static_cast<node::Block*>(nodes[1].get());
  }

  Scope::~Scope() = default;

  int32_t Scope::Offset(int32_t size) {
    int32_t prev = current_offset;
    current_offset -= size;

    // utility::Debug("Previous Scope offset is", prev, "current is", current_offset);

    if (parent) return parent->Offset(size);
    else return prev;
  }

  int32_t Scope::CurrentOffset() const { return current_offset; }

  bool Scope::Register(std::string&& path, std::string&& specifier, std::vector<std::string>&& modules) {
    if (!parent) {
      throw utility::Error("Scope doesn't have a parent for Register when importing, but the parent should always have been the Module", path, specifier, modules);
    }

    // return parent->Register(std::move(path), std::move(specifier), std::move(modules));
    return false;
  }

  bool Scope::Import(std::string&& specifier) {
    if (!parent) {
      throw utility::Error("Scope doesn't have a parent for Import, but the parent should always have been the Module", specifier);
    }

    // return parent->Import(std::move(specifier));
    return false;
  }

  // void Scope::Add(std::unique_ptr<Node>&& node) {
  //   nodes.insert(nodes.cend() - 1, std::move(node));
  // }

  void Scope::Add(std::unique_ptr<node::Scope>&& node) {
    scopes.push_back(std::move(node));
    // scopes.insert(scopes.cend() - 1, std::move(node));
  }

  void Scope::Add(std::unique_ptr<node::Block>&& node) {
    last_body_block = node.get();
    blocks.push_back(std::move(node));
    // nodes.insert(nodes.cend() - 1, std::move(node));
  }

  void Scope::Slide(node::Instruction* instruction) {
    if (!instruction) return;
    
    // Iterate over all nodes in the scope
    for (auto& block : blocks | std::views::reverse) {
      // Attempt to remove the destructor from the block
      auto uptr = block->Remove(instruction);
      if (uptr) {
        // Append the instruction to the last body block
        blocks.back()->Add(std::move(uptr));
        return;
      }
    }
  }

  node::scope::conditional::Loop* Scope::FindLoop() { return parent ? parent->FindLoop() : nullptr; }
  node::scope::Context* Scope::FindContext() { return parent ? parent->FindContext() : nullptr; }

  node::scope::Context& Scope::GetContext() {
    if (!parent) throw utility::Error("A scope that isn't a context must have a parent set");

    return parent->GetContext();
  }

  node::scope::context::Module& Scope::GetModule() {
    if (!parent) throw utility::Error("A scope that isn't a module must have a parent set");

    return parent->GetModule();
  }
  // void Scope::Push(node::Declaration* declaration) { return parent ? parent->Push(declaration) : nullptr; }
  // void Scope::Pop(node::Declaration* declaration) { return parent ? parent->Pop(declaration) : nullptr; }

  // code::Frame::SizeType Scope::Push(node::Declaration& declaration) {
  //   auto prev_size = frame.Size();

  //   live_stack.push_back(&declaration);
  //   frame += declaration.Size();

  //   return parent ? parent->Push(declaration) : prev_size;
  // }

  // code::Frame::SizeType Scope::Pop(node::Declaration& declaration) {
  //   auto prev_size = frame.Size();

  //   frame -= declaration.Size();
  //   live_stack.pop_back();

  //   return parent ? parent->Pop(declaration) : prev_size;
  // }

  node::Scope* Scope::Parent() { return parent; }

  const std::vector<std::unique_ptr<node::Scope>>& Scope::Scopes() const { return scopes; }
  const std::vector<std::unique_ptr<node::Block>>& Scope::Blocks() const { return blocks; }
  // const std::vector<node::Declaration*>& Scope::LiveStack() const { return live_stack; }
  
  const node::Block* Scope::FirstBlock() const { return blocks.empty() ? nullptr : blocks.front().get(); }
  const node::Block* Scope::LastBlock() const { return blocks.empty() ? nullptr : blocks.back().get(); }

  node::Block* Scope::FirstBlock() { return blocks.empty() ? nullptr : blocks.front().get(); }
  node::Block* Scope::LastBlock() { return blocks.empty() ? nullptr : blocks.back().get(); }

  node::Block& Scope::RequiredFirstBlock() {
    if (blocks.empty()) {
      blocks.emplace_back(std::make_unique<node::Block>());
    }

    return *blocks.front();
  }

  node::Block& Scope::RequiredLastBlock() {
    if (blocks.empty()) {
      blocks.emplace_back(std::make_unique<node::Block>());
    }

    return *blocks.back();
  }

  const node::Block& Scope::EntryBlock() const { return *blocks.front(); }
  const node::Block& Scope::LastBodyBlock() const { return *last_body_block; }
  const node::Block& Scope::ExitBlock() const { return *blocks.back(); }
  // const node::Block& Scope::EntryBlock() const { return static_cast<node::Block&>(*nodes.front()); }
  // const node::Block& Scope::LastBodyBlock() const { return *last_body_block; }
  // const node::Block& Scope::ExitBlock() const { return static_cast<node::Block&>(*nodes.back()); }
  
  node::Block& Scope::EntryBlock() { return *blocks.front(); }
  node::Block& Scope::LastBodyBlock() { return *last_body_block; }
  node::Block& Scope::ExitBlock() { return *blocks.back(); }
  // node::Block& Scope::EntryBlock() { return static_cast<node::Block&>(*nodes.front()); }
  // node::Block& Scope::LastBodyBlock() { return *last_body_block; }
  // node::Block& Scope::ExitBlock() { return static_cast<node::Block&>(*nodes.back()); }
  code::Registers& Scope::Registers() { return registers; }
  code::Frame& Scope::Frame() { return GetContext().Frame(); }
  // void Scope::Registers(code::Registers v) { registers = v; }
  // node::Module& Scope::Module() { return mod; }

  void Scope::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Scope::Update(Updater& updater) {}
  int32_t Scope::Size() const { return 0; }
  std::string Scope::Name() const { return "Scope"; }
  std::string Scope::ToString() const { return std::string{Name()}; }

  void Scope::Compile(code::x64::Encoder& encoder) {
    node::Block* previous{nullptr};

    // for (auto& node : nodes) {
    //   if (typeid(*node) == typeid(node::Block)) {
    //     auto& block = static_cast<node::Block&>(*node);
    //     block.Compile(encoder);

    //     if (previous) {
    //       encoder.Collide(block, *previous);
    //     }
        
    //     previous = &block;
    //   }
    // }
  }
};