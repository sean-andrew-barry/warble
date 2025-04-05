import node.declaration.lvalue.linked;
import node.declaration;
import node.declaration.rvalue.literal.structured._enum;
import utility.print;
import visitor;
import updater;
import <memory>;

namespace node::declaration::lvalue {
  void Linked::Links(std::shared_ptr<node::declaration::rvalue::literal::structured::Enum> v) {
    links = std::move(v);
  }
  
  std::shared_ptr<node::declaration::rvalue::literal::structured::Enum> Linked::Links() const {
    return links;
  }

  void Linked::Add(std::shared_ptr<node::Declaration>&& link) {
    links->Value(std::move(link));
  }

  bool Linked::Match(const node::declaration::rvalue::literal::structured::Enum& v) const {
    // return v == links;
    return false;
  }

  void Linked::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Linked::Update(Updater& updater) {}

  int32_t Linked::Size() const { return 0; }
  std::string Linked::Name() const { return "Linked"; }
  std::string Linked::ToString() const { return utility::Line("Linked()"); }
};