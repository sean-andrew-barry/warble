import node.instruction.phi;
import node.declaration;
import visitor;
import updater;

namespace node::instruction {
  Phi::Phi(const lexical::source::Span& span, node::Block& block, std::vector<node::Declaration*>&& declarations, node::Declaration* result) noexcept
    : Instruction{span, block}, declarations{std::move(declarations)}, result{result}
  {}

  void Phi::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Phi::Update(Updater& updater) {}
  int32_t Phi::Size() const { return 0; }
  std::string Phi::Name() const { return "Phi"; }
  std::string Phi::ToString() const { return std::string{Name()}; }
  bool Phi::IsTerminator() const { return false; }
};