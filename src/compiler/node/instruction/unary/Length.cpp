import node.instruction.unary.length;
import visitor;
import updater;

namespace node::instruction::unary {
  void Length::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Length::Update(Updater& updater) {}
  int32_t Length::Size() const { return 0; }
  std::string Length::Name() const { return "Length"; }
  std::string Length::ToString() const { return std::string{Name()}; }
};