import node.instruction.binary.assign.add;
import visitor;
import updater;

namespace node::instruction::binary::assign {
  void Add::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Add::Update(Updater& updater) {}
  int32_t Add::Size() const { return 0; }
  std::string Add::Name() const { return "Add"; }
  std::string Add::ToString() const { return std::string{Name()}; }
};