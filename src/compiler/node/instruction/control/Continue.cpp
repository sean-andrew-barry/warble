import node.instruction.control._continue;
import visitor;
import updater;
import utility.print;

namespace node::instruction::control {
  void Continue::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Continue::Update(Updater& updater) {}
  int32_t Continue::Size() const { return 0; }
  std::string Continue::Name() const { return utility::Join("continue"); }
  std::string Continue::ToString() const { return utility::Line(Name()); }
};