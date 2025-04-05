import node.scope.conditional.loop._for;
import visitor;
import updater;

namespace node::scope::conditional::loop {
  void For::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void For::Update(Updater& updater) {}
  int32_t For::Size() const { return 0; }
  std::string For::Name() const { return "For"; }
  std::string For::ToString() const { return std::string{Name()}; }
};