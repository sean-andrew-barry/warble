import node.instruction.binary.math;
import node.declaration.rvalue.temporary;
import visitor;
import updater;

namespace node::instruction::binary {
  Math::Math(const lexical::source::Span& span, std::shared_ptr<node::declaration::rvalue::Temporary> result, std::shared_ptr<node::Declaration> lhs, std::shared_ptr<node::Declaration> rhs) noexcept
    : Binary{span, std::move(result), std::move(lhs), std::move(rhs)}
  {
    Result()->Type(LHS()->TypeOf());
  }

  void Math::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Math::Update(Updater& updater) {}
  int32_t Math::Size() const { return 0; }
  std::string Math::Name() const { return "Math"; }
  std::string Math::ToString() const { return std::string{Name()}; }
};