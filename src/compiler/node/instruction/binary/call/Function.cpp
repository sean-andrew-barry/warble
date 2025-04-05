import node.instruction.binary.call.function;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void Function::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Function::Update(Updater& updater) {}
  int32_t Function::Size() const { return 0; }
  std::string Function::Name() const { return "Function"; }
  std::string Function::ToString() const { return std::string{Name()}; }
};