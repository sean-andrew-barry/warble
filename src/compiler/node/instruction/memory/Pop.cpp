import node.instruction.memory.pop;
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
  Pop::Pop(const lexical::source::Span& span, node::Declaration* target, node::Declaration* value) noexcept
    : Memory{span}, target{target}, value{value}
  {}

  void Pop::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Pop::Update(Updater& updater) {}
  int32_t Pop::Size() const { return 0; }
  std::string Pop::Name() const { return "Pop"; }
  std::string Pop::ToString() const { return Name(); }

  void Pop::Declare(code::Encoder& encoder) {
    // Block().Pop(*target);
    // // target->Displacement(static_cast<int32_t>(displacement));
    // encoder.Declare(Block(), *target);

    // utility::Debug("Reserving", target->Size(), "bytes at displacement", target->Address().Displacement(), "for declaration", target->Name());
  }

  void Pop::Compile(code::Encoder& encoder) {
  }

  void Pop::Compile(code::x64::Encoder& encoder) {
  }
};