import node.scope.context.function.inline_function;
import visitor;
import updater;

namespace node::scope::context::function {
  void Inline::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Inline::Update(Updater& updater) {}
  int32_t Inline::Size() const { return 0; }
  std::string Inline::Name() const { return "Inline"; }
  std::string Inline::ToString() const { return std::string{Name()}; }
};