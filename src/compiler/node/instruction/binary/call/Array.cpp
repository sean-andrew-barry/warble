import node.instruction.binary.call.array;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void Array::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Array::Update(Updater& updater) {}
  int32_t Array::Size() const { return 0; }
  std::string Array::Name() const { return "Array"; }
  std::string Array::ToString() const { return std::string{Name()}; }
};