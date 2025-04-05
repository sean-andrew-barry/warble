import node.scope.conditional._else_if;
import visitor;
import updater;

namespace node::scope::conditional {
  void ElseIf::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void ElseIf::Update(Updater& updater) {}
  int32_t ElseIf::Size() const { return 0; }
  std::string ElseIf::Name() const { return "ElseIf"; }
  std::string ElseIf::ToString() const { return std::string{Name()}; }
};