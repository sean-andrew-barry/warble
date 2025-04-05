import node.instruction.binary.call.character;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void Character::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Character::Update(Updater& updater) {}
  int32_t Character::Size() const { return 0; }
  std::string Character::Name() const { return "Character"; }
  std::string Character::ToString() const { return std::string{Name()}; }
};