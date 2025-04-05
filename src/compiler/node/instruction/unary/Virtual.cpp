import node.instruction.unary._virtual;
import visitor;
import updater;

namespace node::instruction::unary {
  void Virtual::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Virtual::Update(Updater& updater) {}
  int32_t Virtual::Size() const { return 0; }
  std::string Virtual::Name() const { return "Virtual"; }
  std::string Virtual::ToString() const { return std::string{Name()}; }
};