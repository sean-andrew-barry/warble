import node.instruction.binary.range;
import visitor;
import updater;

namespace node::instruction::binary {
  void Range::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Range::Update(Updater& updater) {}
  int32_t Range::Size() const { return 0; }
  std::string Range::Name() const { return "Range"; }
  std::string Range::ToString() const { return std::string{Name()}; }
};