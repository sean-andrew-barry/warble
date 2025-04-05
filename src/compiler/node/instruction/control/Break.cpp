import node.instruction.control._break;
import visitor;
import updater;
import utility.print;

namespace node::instruction::control {
  void Break::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Break::Update(Updater& updater) {}
  int32_t Break::Size() const { return 0; }
  std::string Break::Name() const { return utility::Join("break"); }
  std::string Break::ToString() const { return utility::Line(Name()); }
};