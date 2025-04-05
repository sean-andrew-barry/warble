import node.scope.conditional.pattern.match;
import visitor;
import updater;

namespace node::scope::conditional::pattern {
  void Match::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Match::Update(Updater& updater) {}
  int32_t Match::Size() const { return 0; }
  std::string Match::Name() const { return "Match"; }
  std::string Match::ToString() const { return std::string{Name()}; }
};