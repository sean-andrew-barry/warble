import node.declaration.literal.structured.string;
import visitor;
import updater;
import utility.print;

namespace node::declaration::literal::structured {
  void String::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void String::Update(Updater& updater) {}
  int32_t String::Size() const { return 0; }
  std::string String::Name() const { return "String"; }
  std::string String::ToString() const { return utility::Line(Name(), " \"", value, "\""); }
  std::vector<std::type_index> String::Signature() const { return {typeid(*this)}; }
  // node::Declaration* String::Value() const { return nullptr; }
};