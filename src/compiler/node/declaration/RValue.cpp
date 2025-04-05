import node.declaration.rvalue;
import node.declaration;
import visitor;
import updater;

namespace node::declaration {
  void RValue::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void RValue::Update(Updater& updater) {}
  int32_t RValue::Size() const { return 0; }
  std::string RValue::Name() const { return "RValue"; }
  std::string RValue::ToString() const { return Name(); }
};