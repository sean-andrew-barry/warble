import node.instruction.control.jump;
import node.block;
import node.declaration;
import visitor;
import updater;

namespace node::instruction::control {
  Jump::Jump(const lexical::source::Span& span, node::Block* target) noexcept
    : Control{span}, target{target}
  {}

  Jump::Jump(const lexical::source::Span& span, std::shared_ptr<node::Declaration> condition) noexcept
    : Control{span}, condition{condition}
  {}

  Jump::Jump(const lexical::source::Span& span, node::Block* target, std::shared_ptr<node::Declaration> condition) noexcept
    : Control{span}, target{target}, condition{condition}
  {}

  Jump::Jump(const lexical::source::Span& span, std::shared_ptr<node::Declaration> condition, node::Block* target) noexcept
    : Control{span}, condition{condition}, target{target}
  {}

  void Jump::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Jump::Update(Updater& updater) {}
  int32_t Jump::Size() const { return 0; }
  std::string Jump::Name() const { return "Jump"; }
  std::string Jump::ToString() const { return std::string{Name()}; }
};