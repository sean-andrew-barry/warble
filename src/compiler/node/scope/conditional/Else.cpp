import node.scope.conditional._else;
import visitor;
import updater;

namespace node::scope::conditional {
  void Else::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Else::Update(Updater& updater) {}
  int32_t Else::Size() const { return 0; }
  std::string Else::Name() const { return "Else"; }
  std::string Else::ToString() const { return std::string{Name()}; }
};