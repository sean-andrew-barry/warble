import node.scope.conditional.loop._while;
import visitor;
import updater;

namespace node::scope::conditional::loop {
  void While::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void While::Update(Updater& updater) {}
  int32_t While::Size() const { return 0; }
  std::string While::Name() const { return "While"; }
  std::string While::ToString() const { return std::string{Name()}; }
};