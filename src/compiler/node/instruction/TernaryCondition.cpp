import node.instruction.ternary_condition;
import visitor;
import updater;

namespace node::instruction {
  void TernaryCondition::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void TernaryCondition::Update(Updater& updater) {}
  int32_t TernaryCondition::Size() const { return 0; }
  std::string TernaryCondition::Name() const { return "TernaryCondition"; }
  std::string TernaryCondition::ToString() const { return std::string{Name()}; }
};