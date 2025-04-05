import node.instruction;
import node.block;
import visitor;
import updater;

namespace node {
  void Instruction::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Instruction::Update(Updater& updater) {}
  int32_t Instruction::Size() const { return 0; }
  std::string Instruction::Name() const { return "Instruction"; }
  std::string Instruction::ToString() const { return Name(); }
};