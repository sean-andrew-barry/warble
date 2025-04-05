import node.instruction.binary.logical.lesser;
import visitor;
import updater;

namespace node::instruction::binary::logical {
  void Lesser::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Lesser::Update(Updater& updater) {}
  int32_t Lesser::Size() const { return 0; }
  std::string Lesser::Name() const { return "Lesser"; }
  std::string Lesser::ToString() const { return std::string{Name()}; }
};