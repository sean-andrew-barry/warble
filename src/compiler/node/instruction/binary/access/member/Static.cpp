import node.instruction.binary.access.member._static;
import visitor;
import updater;

namespace node::instruction::binary::access::member {
  void Static::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Static::Update(Updater& updater) {}
  int32_t Static::Size() const { return 0; }
  std::string Static::Name() const { return "Static"; }
  std::string Static::ToString() const { return std::string{Name()}; }
};