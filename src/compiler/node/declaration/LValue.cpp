import node.declaration.lvalue;
import node.declaration;
import visitor;
import updater;
import <memory>;

namespace node::declaration {
  void LValue::Contract(std::shared_ptr<node::Declaration> v) { contract = std::move(v); }
  void LValue::Value(std::shared_ptr<node::Declaration> v) { value = std::move(v); }

  std::shared_ptr<node::Declaration> LValue::Contract() const { return contract; }
  std::shared_ptr<node::Declaration> LValue::Value() const { return value; }

  void LValue::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void LValue::Update(Updater& updater) {}
  int32_t LValue::Size() const { return 0; }
  std::string LValue::Name() const { return "LValue"; }
  std::string LValue::ToString() const { return Name(); }
};