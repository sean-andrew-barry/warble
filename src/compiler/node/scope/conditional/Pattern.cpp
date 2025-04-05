import node.scope.conditional.pattern;
import visitor;
import updater;

namespace node::scope::conditional {
  void Pattern::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Pattern::Update(Updater& updater) {}
  int32_t Pattern::Size() const { return 0; }
  std::string Pattern::Name() const { return "Pattern"; }
  std::string Pattern::ToString() const { return std::string{Name()}; }
};