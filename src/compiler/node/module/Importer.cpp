import node._module.importer;
import visitor;
import updater;

namespace node::_module {
  void Importer::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Importer::Update(Updater& updater) {}
  int32_t Importer::Size() const { return 0; }
  std::string Importer::Name() const { return "Importer"; }
  std::string Importer::ToString() const { return std::string{Name()}; }
};