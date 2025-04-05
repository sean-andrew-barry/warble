import node.instruction.binary.nullish;
import visitor;
import updater;

namespace node::instruction::binary {
  void Nullish::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Nullish::Update(Updater& updater) {}
  int32_t Nullish::Size() const { return 0; }
  std::string Nullish::Name() const { return "Nullish"; }
  std::string Nullish::ToString() const { return std::string{Name()}; }
};