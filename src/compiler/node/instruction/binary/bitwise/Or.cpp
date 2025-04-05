import node.instruction.binary.bitwise._or;
import visitor;
import updater;

namespace node::instruction::binary::bitwise {
  void Or::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Or::Update(Updater& updater) {}
  int32_t Or::Size() const { return 0; }
  std::string Or::Name() const { return "Or"; }
  std::string Or::ToString() const { return std::string{Name()}; }
};