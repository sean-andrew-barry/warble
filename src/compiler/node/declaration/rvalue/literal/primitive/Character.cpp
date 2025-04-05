import node.declaration.rvalue.literal.primitive.character;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import visitor;
import updater;
import utility.print;

namespace node::declaration::rvalue::literal::primitive {
  Character::Character(const lexical::source::Span& span, node::Scope& scope, std::string&& value)
    : Primitive{span, scope}, value{std::move(value)}
  {}

  Character::Character(const lexical::source::Span& span, node::Scope& scope, std::string&& value, char32_t raw)
    : Primitive{span, scope}, value{std::move(value)}, raw{raw}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Character::TypeOf() const { return node::scope::Global::Character(); }

  void Character::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Character::Update(Updater& updater) {}
  int32_t Character::Size() const { return 4; }
  std::string Character::Name() const { return utility::Join("Character '", value, "'"); }
  std::string Character::ToString() const { return utility::Line(Name()); }
  std::vector<std::type_index> Character::Signature() const { return {typeid(*this)}; }
};