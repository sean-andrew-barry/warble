import node.instruction.binary.logical.greater;
import visitor;
import updater;

namespace node::instruction::binary::logical {
  void Greater::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Greater::Update(Updater& updater) {}
  int32_t Greater::Size() const { return 0; }
  std::string Greater::Name() const { return "Greater"; }
  std::string Greater::ToString() const { return std::string{Name()}; }
};