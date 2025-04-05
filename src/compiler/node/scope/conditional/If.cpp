import node.scope.conditional._if;
import visitor;
import updater;

namespace node::scope::conditional {
  void If::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void If::Update(Updater& updater) {}
  int32_t If::Size() const { return 0; }
  std::string If::Name() const { return "If"; }
  std::string If::ToString() const { return std::string{Name()}; }
};