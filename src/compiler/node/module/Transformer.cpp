import node._module.transformer;
import visitor;
import updater;

namespace node::_module {
  void Transformer::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Transformer::Update(Updater& updater) {}
  int32_t Transformer::Size() const { return 0; }
  std::string Transformer::Name() const { return "Transformer"; }
  std::string Transformer::ToString() const { return std::string{Name()}; }
};