import node.instruction.binary.call.string;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void String::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void String::Update(Updater& updater) {}
  int32_t String::Size() const { return 0; }
  std::string String::Name() const { return "String"; }
  std::string String::ToString() const { return std::string{Name()}; }
};