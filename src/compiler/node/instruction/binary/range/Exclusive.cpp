import node.instruction.binary.range.exclusive;
import visitor;
import updater;

namespace node::instruction::binary::range {
  void Exclusive::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Exclusive::Update(Updater& updater) {}
  int32_t Exclusive::Size() const { return 0; }
  std::string Exclusive::Name() const { return "Exclusive"; }
  std::string Exclusive::ToString() const { return std::string{Name()}; }
};