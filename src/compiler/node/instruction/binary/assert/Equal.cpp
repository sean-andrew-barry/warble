import node.instruction.binary.assert.equal;
import visitor;
import updater;

namespace node::instruction::binary::assert {
  void Equal::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Equal::Update(Updater& updater) {}
  int32_t Equal::Size() const { return 0; }
  std::string Equal::Name() const { return "Equal"; }
  std::string Equal::ToString() const { return std::string{Name()}; }
};