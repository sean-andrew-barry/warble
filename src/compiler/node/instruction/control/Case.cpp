import node.instruction.control._case;
import node.declaration;
import visitor;
import updater;

namespace node::instruction::control {
  Case::Case(const lexical::source::Span& span, std::shared_ptr<node::Declaration> result)
    : Control{span}, result{std::move(result)}
  {}

  void Case::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Case::Update(Updater& updater) {}
  int32_t Case::Size() const { return 0; }
  std::string Case::Name() const { return "Case"; }
  std::string Case::ToString() const { return std::string{Name()}; }
};