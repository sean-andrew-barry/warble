import node.instruction.unary.expected;
import visitor;
import updater;

namespace node::instruction::unary {
  void Expected::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Expected::Update(Updater& updater) {}
  int32_t Expected::Size() const { return 0; }
  std::string Expected::Name() const { return "Expected"; }
  std::string Expected::ToString() const { return std::string{Name()}; }
};