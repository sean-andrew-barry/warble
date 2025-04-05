import node.scope.conditional.pattern.cover;
import visitor;
import updater;

namespace node::scope::conditional::pattern {
  void Cover::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Cover::Update(Updater& updater) {}
  int32_t Cover::Size() const { return 0; }
  std::string Cover::Name() const { return "Cover"; }
  std::string Cover::ToString() const { return std::string{Name()}; }
};