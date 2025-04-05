import node.declaration.rvalue.literal;
import code.x64.encoder;
import node.scope;
import utility.print;
import visitor;
import updater;

namespace node::declaration::rvalue {
  void Literal::Allocate(code::x64::Encoder& encoder) {
    // Get the scope and offset it by the size of this declaration, then set the displacement
    auto displacement = Scope().Offset(Size());
    utility::Debug("Allocating", Size(), displacement);
    Displacement(displacement);
  }

  void Literal::Deallocate(code::x64::Encoder& encoder) {
    utility::Debug("Deallocating", Size(), Displacement());
    // Undo the allocate offset
    Scope().Offset(-Size());
  }

  void Literal::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Literal::Update(Updater& updater) {}
  int32_t Literal::Size() const { return 0; }
  std::string Literal::Name() const { return "Literal"; }
  std::string Literal::ToString() const { return std::string{Name()}; }
};