import node.instruction.binary.bitwise._and;
import visitor;
import updater;

namespace node::instruction::binary::bitwise {
  void And::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void And::Update(Updater& updater) {}
  int32_t And::Size() const { return 0; }
  std::string And::Name() const { return "And"; }
  std::string And::ToString() const { return std::string{Name()}; }
};