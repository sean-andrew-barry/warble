import node.instruction.binary.logical.not_equal;
import visitor;
import updater;

namespace node::instruction::binary::logical {
  void NotEqual::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void NotEqual::Update(Updater& updater) {}
  int32_t NotEqual::Size() const { return 0; }
  std::string NotEqual::Name() const { return "NotEqual"; }
  std::string NotEqual::ToString() const { return std::string{Name()}; }
};