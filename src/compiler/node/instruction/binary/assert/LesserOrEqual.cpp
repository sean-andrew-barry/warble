import node.instruction.binary.assert.lesser_or_equal;
import visitor;
import updater;

namespace node::instruction::binary::assert {
  void LesserOrEqual::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void LesserOrEqual::Update(Updater& updater) {}
  int32_t LesserOrEqual::Size() const { return 0; }
  std::string LesserOrEqual::Name() const { return "LesserOrEqual"; }
  std::string LesserOrEqual::ToString() const { return std::string{Name()}; }
};