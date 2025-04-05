import node.instruction.binary.call.number;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void Number::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Number::Update(Updater& updater) {}
  int32_t Number::Size() const { return 0; }
  std::string Number::Name() const { return "Number"; }
  std::string Number::ToString() const { return std::string{Name()}; }
};