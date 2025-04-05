import node.scope.conditional.pattern.is;
import visitor;
import updater;

namespace node::scope::conditional::pattern {
  void Is::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Is::Update(Updater& updater) {}
  int32_t Is::Size() const { return 0; }
  std::string Is::Name() const { return "Is"; }
  std::string Is::ToString() const { return std::string{Name()}; }
};