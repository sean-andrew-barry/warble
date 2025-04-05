import node._module;
import utility.print;
import visitor;
import updater;

namespace node {
  void Module::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Module::Update(Updater& updater) {}
  int32_t Module::Size() const { return 0; }
  std::string Module::Name() const { return "Module"; }
  std::string Module::ToString() const { return std::string{Name()}; }
};