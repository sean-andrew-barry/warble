import node.instruction.binary.access.member.optional;
import visitor;
import updater;

namespace node::instruction::binary::access::member {
  void Optional::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Optional::Update(Updater& updater) {}
  int32_t Optional::Size() const { return 0; }
  std::string Optional::Name() const { return "Optional"; }
  std::string Optional::ToString() const { return std::string{Name()}; }
};