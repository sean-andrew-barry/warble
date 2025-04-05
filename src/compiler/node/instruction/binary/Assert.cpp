import node.instruction.binary.assert;
import visitor;
import updater;

namespace node::instruction::binary {
  void Assert::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Assert::Update(Updater& updater) {}
  int32_t Assert::Size() const { return 0; }
  std::string Assert::Name() const { return "Assert"; }
  std::string Assert::ToString() const { return std::string{Name()}; }
};