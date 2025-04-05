import node.instruction.binary.call.tuple;
import node.declaration;
import visitor;
import updater;
import utility.print;

namespace node::instruction::binary::call {
  void Tuple::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Tuple::Update(Updater& updater) {}
  int32_t Tuple::Size() const { return 0; }
  std::string Tuple::Name() const { return "Tuple"; }
  std::string Tuple::ToString() const { return utility::Line("Call: ", LHS()->Name(), " => ", RHS()->Name()); }
};