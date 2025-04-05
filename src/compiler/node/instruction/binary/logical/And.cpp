import node.instruction.binary.logical._and;
import utility.print;
import visitor;
import updater;

namespace node::instruction::binary::logical {
  void And::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void And::Update(Updater& updater) {}
  int32_t And::Size() const { return 0; }
  std::string And::Name() const { return "And"; }
  std::string And::ToString() const { return std::string{Name()}; }
};