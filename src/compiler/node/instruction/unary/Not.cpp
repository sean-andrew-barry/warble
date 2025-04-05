import node.instruction.unary._not;
import visitor;
import updater;

namespace node::instruction::unary {
  void Not::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Not::Update(Updater& updater) {}
  int32_t Not::Size() const { return 0; }
  std::string Not::Name() const { return "Not"; }
  std::string Not::ToString() const { return std::string{Name()}; }
};