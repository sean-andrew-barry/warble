import node.instruction.unary.await;
import visitor;
import updater;

namespace node::instruction::unary {
  void Await::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Await::Update(Updater& updater) {}
  int32_t Await::Size() const { return 0; }
  std::string Await::Name() const { return "Await"; }
  std::string Await::ToString() const { return std::string{Name()}; }
};