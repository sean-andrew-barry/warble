import node.scope.conditional.pattern._default;
import visitor;
import updater;

namespace node::scope::conditional::pattern {
  void Default::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Default::Update(Updater& updater) {}
  int32_t Default::Size() const { return 0; }
  std::string Default::Name() const { return "Default"; }
  std::string Default::ToString() const { return std::string{Name()}; }
};