import node.scope.context;
import code.frame;
import node.declaration;
import utility.print;
import visitor;
import updater;
import <cstdint>;
import <cmath>;
import <vector>;

namespace node::scope {
  // void Context::Result(node::Declaration* v) { result = v; }
  // void Context::Case(node::Declaration* c) { cases.push_back(c); }
  // const std::vector<node::Declaration*>& Context::Cases() const { return cases; }

  // Context::Context(const lexical::source::Span& span, node::Scope* parent)
  //   : Scope{span, parent}, frame{} {}

  const std::vector<node::Declaration*>& Context::LiveStack() const { return live_stack; }

  void Context::Push(node::Declaration& declaration) {
    auto prev_displacement = frame.Displacement();

    live_stack.push_back(&declaration);
    frame += declaration.Size();

    // Store the displacement in the declaration's address
    declaration.Address().Displacement(prev_displacement);
  }

  void Context::Pop(node::Declaration& declaration) {
    frame -= declaration.Size();
    live_stack.pop_back();
  }

  int32_t Context::Offset(int32_t size) {
    auto prev_offset = node::Scope::Offset(size);
    auto current_offset = CurrentOffset();

    if (std::abs(current_offset) > largest_offset) {
      largest_offset = std::abs(current_offset);
    }

    // utility::Debug("Context offset is currently", current_offset, "and largest is", largest_offset);

    return prev_offset;
  }

  int32_t Context::LargestOffset() const { return largest_offset; }
  code::Frame& Context::Frame() { return frame; }

  bool Context::IsContext() const { return true; }
  node::scope::conditional::Loop* Context::FindLoop() { return nullptr; } // Cannot find a loop through a context
  node::scope::Context* Context::FindContext() { return this; }
  node::scope::Context& Context::GetContext() { return *this; }

  void Context::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Context::Update(Updater& updater) {}
  int32_t Context::Size() const { return largest_offset; }
  std::string Context::Name() const { return "Context"; }
  std::string Context::ToString() const { return Name(); }
};