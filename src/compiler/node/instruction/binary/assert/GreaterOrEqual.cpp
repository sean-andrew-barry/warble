import node.instruction.binary.assert.greater_or_equal;
import visitor;
import updater;

namespace node::instruction::binary::assert {
  void GreaterOrEqual::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void GreaterOrEqual::Update(Updater& updater) {}
  int32_t GreaterOrEqual::Size() const { return 0; }
  std::string GreaterOrEqual::Name() const { return "GreaterOrEqual"; }
  std::string GreaterOrEqual::ToString() const { return std::string{Name()}; }
};