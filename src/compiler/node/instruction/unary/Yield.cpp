import node.instruction.unary.yield;
import visitor;
import updater;

namespace node::instruction::unary {
  void Yield::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Yield::Update(Updater& updater) {}
  int32_t Yield::Size() const { return 0; }
  std::string Yield::Name() const { return "Yield"; }
  std::string Yield::ToString() const { return std::string{Name()}; }
};