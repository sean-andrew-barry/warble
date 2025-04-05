import node.instruction.binary.logical;
import node.declaration.rvalue.temporary;
import node.declaration.rvalue.literal.structured.object;
import node.declaration.rvalue.literal.primitive.boolean;
import node.scope.global;
import visitor;
import updater;

namespace node::instruction::binary {
  Logical::Logical(const lexical::source::Span& span, std::shared_ptr<node::declaration::rvalue::Temporary> result, std::shared_ptr<node::Declaration> lhs, std::shared_ptr<node::Declaration> rhs) noexcept
    : Binary{span, std::move(result), std::move(lhs), std::move(rhs)}
  {
    Result()->Type(node::scope::Global::Boolean());
  }

  void Logical::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Logical::Update(Updater& updater) {}
  int32_t Logical::Size() const { return 0; }
  std::string Logical::Name() const { return "Logical"; }
  std::string Logical::ToString() const { return std::string{Name()}; }
};