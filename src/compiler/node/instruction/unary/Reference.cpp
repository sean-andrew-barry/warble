import node.instruction.unary.reference;
import visitor;
import updater;

namespace node::instruction::unary {
  void Reference::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Reference::Update(Updater& updater) {}
  int32_t Reference::Size() const { return 0; }
  std::string Reference::Name() const { return "Reference"; }
  std::string Reference::ToString() const { return std::string{Name()}; }
};