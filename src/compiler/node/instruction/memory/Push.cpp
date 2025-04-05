import node.instruction.memory.push;
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
  Push::Push(const lexical::source::Span& span, node::Declaration* target, node::Declaration* value) noexcept
    : Memory{span}, target{target}, value{value}
  {}

  void Push::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Push::Update(Updater& updater) {}
  int32_t Push::Size() const { return 0; }
  std::string Push::Name() const { return "Push"; }
  std::string Push::ToString() const { return Name(); }

  void Push::Declare(code::Encoder& encoder) {
    Block().Push(*target);
    // target->Displacement(static_cast<int32_t>(displacement));
    encoder.Declare(Block(), *target);

    utility::Debug("Reserving", target->Size(), "bytes at displacement", target->Address().Displacement(), "for declaration", target->Name());
  }

  void Push::Compile(code::Encoder& encoder) {
  }

  void Push::Compile(code::x64::Encoder& encoder) {
  }
};