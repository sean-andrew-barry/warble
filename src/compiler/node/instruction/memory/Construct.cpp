import node.instruction.memory.construct;
import node.declaration.rvalue.literal.structured.object;
import node.declaration;
import node.scope;
import node.block;
import code.encoder;
import code.address;
import code.x64.encoder;
import utility.print;
import visitor;
import updater;

namespace node::instruction::memory {
  Construct::Construct(const lexical::source::Span& span, std::shared_ptr<node::Declaration> target, std::shared_ptr<node::Declaration> value) noexcept
    : Memory{span}, target{target}, value{value}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Construct::TypeOf() const { return value->TypeOf(); }

  void Construct::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Construct::Update(Updater& updater) {}
  int32_t Construct::Size() const { return 0; }
  std::string Construct::Name() const { return "Construct"; }
  // std::string Construct::ToString() const { return Name(); }
  std::string Construct::ToString() const { return utility::Line(Name(), " (", target ? target->Name() : "null", ", ", value ? value->Name() : "null", ")"); }

  // std::string Construct::ToString() const {
  //   auto string = utility::Line("Block {");

  //   {
  //     utility::Indent i{};

  //     for (auto& node : instructions) {
  //       string += node->ToString();
  //     }
  //   }

  //   string += utility::Line("}");
  //   return string;
  // }

  void Construct::Declare(code::Encoder& encoder) {
    // Block().Push(*target);

    // // target->Displacement(static_cast<int32_t>(displacement));
    // encoder.Declare(Block(), *target);

    // utility::Debug("Reserving", target->Size(), "bytes at displacement", target->Address().Displacement(), "for declaration", target->Name());
  }

  void Construct::Compile(code::Encoder& encoder) {
    // for (auto& instruction : instructions) {
    //   instruction->Compile(encoder);
    // }
  }

  void Construct::Compile(code::x64::Encoder& encoder) {
    // // utility::Debug("Compiling Construct instruction for", target->Name());
    // // if (value) {
    // //   value->Allocate(encoder);
    // //   // value->Construct(encoder);
    // // }
    
    // // target->Allocate(encoder);
    // // // target->Construct(encoder);
    // auto& scope = target->Scope();
    // auto size = target->Size();
    // auto displacement = scope.Offset(size);
    // target->Displacement(displacement); // Save the displacement

    // // // Get the scope and offset it by the size of this declaration, then set the displacement
    // // auto displacement = Scope().Offset(Size());
    // utility::Debug("Reserving", size, "bytes at displacement", displacement, "for declaration", target->Name());

    // target->Construct(encoder);

    // // Displacement(displacement);

    // // utility::Debug("Deallocating", Size(), Displacement());
    // // // Undo the allocate offset
    // // Scope().Offset(-Size());
  }
};