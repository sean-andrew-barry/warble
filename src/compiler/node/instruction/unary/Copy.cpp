import node.instruction.unary.copy;
import visitor;
import updater;

namespace node::instruction::unary {
  void Copy::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Copy::Update(Updater& updater) {}
  int32_t Copy::Size() const { return 0; }
  std::string Copy::Name() const { return "Copy"; }
  std::string Copy::ToString() const { return std::string{Name()}; }
};