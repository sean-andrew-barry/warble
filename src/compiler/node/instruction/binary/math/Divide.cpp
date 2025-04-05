import node.instruction.binary.math.divide;
import visitor;
import updater;

namespace node::instruction::binary::math {
  void Divide::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Divide::Update(Updater& updater) {}
  int32_t Divide::Size() const { return 0; }
  std::string Divide::Name() const { return "Divide"; }
  std::string Divide::ToString() const { return std::string{Name()}; }
};