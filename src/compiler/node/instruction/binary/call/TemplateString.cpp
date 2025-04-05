import node.instruction.binary.call.template_string;
import visitor;
import updater;

namespace node::instruction::binary::call {
  void TemplateString::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void TemplateString::Update(Updater& updater) {}
  int32_t TemplateString::Size() const { return 0; }
  std::string TemplateString::Name() const { return "TemplateString"; }
  std::string TemplateString::ToString() const { return std::string{Name()}; }
};