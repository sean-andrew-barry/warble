import node.instruction.binary.math.subtract;
import visitor;
import updater;

namespace node::instruction::binary::math {
  void Subtract::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Subtract::Update(Updater& updater) {}
  int32_t Subtract::Size() const { return 0; }
  std::string Subtract::Name() const { return "Subtract"; }
  std::string Subtract::ToString() const { return std::string{Name()}; }
};