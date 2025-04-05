import node.declaration.rvalue.literal.structured.string;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import visitor;
import updater;
import utility.print;
import <memory>;

namespace node::declaration::rvalue::literal::structured {
  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> String::TypeOf() const { return node::scope::Global::String(); }

  void String::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void String::Update(Updater& updater) {}
  int32_t String::Size() const { return 0; }
  std::string String::Name() const { return "String"; }
  std::string String::ToString() const { return utility::Line(Name(), " \"", value, "\""); }
  std::vector<std::type_index> String::Signature() const { return {typeid(*this)}; }
  // node::Declaration* String::Value() const { return nullptr; }
};