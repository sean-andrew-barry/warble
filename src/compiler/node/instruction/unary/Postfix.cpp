import node.instruction.unary.postfix;
import visitor;
import updater;

namespace node::instruction::unary {
  void Postfix::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Postfix::Update(Updater& updater) {}
  int32_t Postfix::Size() const { return 0; }
  std::string Postfix::Name() const { return "Postfix"; }
  std::string Postfix::ToString() const { return std::string{Name()}; }
};