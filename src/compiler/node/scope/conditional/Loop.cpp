import node.scope.conditional.loop;
import visitor;
import updater;

namespace node::scope::conditional {
  void Loop::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Loop::Update(Updater& updater) {}
  int32_t Loop::Size() const { return 0; }
  std::string Loop::Name() const { return "Loop"; }
  std::string Loop::ToString() const { return std::string{Name()}; }
};