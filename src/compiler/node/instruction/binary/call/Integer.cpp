import node.instruction.binary.call.integer;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void Integer::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Integer::Update(Updater& updater) {}
  int32_t Integer::Size() const { return 0; }
  std::string Integer::Name() const { return "Integer"; }
  std::string Integer::ToString() const { return std::string{Name()}; }
};