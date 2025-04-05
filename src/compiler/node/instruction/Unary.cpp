import node.instruction.unary;
import node.declaration;
import node.declaration.rvalue.temporary;
import visitor;
import updater;

namespace node::instruction {
  Unary::Unary(const lexical::source::Span& span, std::shared_ptr<node::declaration::rvalue::Temporary> result, std::shared_ptr<node::Declaration> value) noexcept
    : Instruction{span}, result{std::move(result)}, value{std::move(value)} {}

  void Unary::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Unary::Update(Updater& updater) {}
  int32_t Unary::Size() const { return 0; }
  std::string Unary::Name() const { return "Unary"; }
  std::string Unary::ToString() const { return result->ToString() + " = " + value->ToString(); }
};