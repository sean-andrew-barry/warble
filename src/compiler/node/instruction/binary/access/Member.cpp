import node.instruction.binary.access.member;
import visitor;
import updater;

namespace node::instruction::binary::access {
  void Member::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Member::Update(Updater& updater) {}
  int32_t Member::Size() const { return 0; }
  std::string Member::Name() const { return "Member"; }
  std::string Member::ToString() const { return Name(); }
};