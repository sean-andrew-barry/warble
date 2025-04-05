import node.scope.conditional.loop._do;
import visitor;
import updater;

namespace node::scope::conditional::loop {
  void Do::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Do::Update(Updater& updater) {}
  int32_t Do::Size() const { return 0; }
  std::string Do::Name() const { return "Do"; }
  std::string Do::ToString() const { return std::string{Name()}; }
};