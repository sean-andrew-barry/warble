import node.instruction.binary.call.object;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void Object::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Object::Update(Updater& updater) {}
  int32_t Object::Size() const { return 0; }
  std::string Object::Name() const { return "Object"; }
  std::string Object::ToString() const { return std::string{Name()}; }
};