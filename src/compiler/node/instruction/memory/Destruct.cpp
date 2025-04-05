import node.instruction.memory.destruct;
import node.declaration;
import node.scope;
import node.block;
import code.encoder;
import code.x64.encoder;
import utility.print;
import visitor;
import updater;

namespace node::instruction::memory {
  Destruct::Destruct(const lexical::source::Span& span, std::shared_ptr<node::Declaration> target) noexcept
    : Memory{span}, target{target}
  {}

  void Destruct::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Destruct::Update(Updater& updater) {}
  int32_t Destruct::Size() const { return 0; }
  std::string Destruct::Name() const { return "Destruct"; }
  // std::string Destruct::ToString() const { return Name(); }
  std::string Destruct::ToString() const { return utility::Line(Name(), " (", target ? target->Name() : "null", ")"); }

  void Destruct::Declare(code::Encoder& encoder) {
    // Block().Pop(*target); // Old

    // target->Scope().Pop(*target); // Better
  }

  void Destruct::Compile(code::Encoder& encoder) {
    // for (auto& instruction : instructions) {
    //   instruction->Compile(encoder);
    // }
  }

  void Destruct::Compile(code::x64::Encoder& encoder) {
    // // utility::Debug("Compiling Destruct instruction for", target->Name());
    // // target->Destruct(encoder);
    // // target->Deallocate(encoder);

    // auto& scope = target->Scope();
    // auto size = target->Size();
    // auto displacement = scope.Offset(-size);

    // // // Get the scope and offset it by the size of this declaration, then set the displacement
    // // auto displacement = Scope().Offset(Size());
    // utility::Debug("Releasing", size, "bytes at displacement", displacement, "for declaration", target->Name());
  }
};