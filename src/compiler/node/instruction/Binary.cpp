import node.instruction.binary;
import node.declaration;
import node.declaration.rvalue.temporary;
import code.encoder;
import visitor;
import updater;

namespace node::instruction {
  Binary::Binary(const lexical::source::Span& span, std::shared_ptr<node::declaration::rvalue::Temporary> result, std::shared_ptr<node::Declaration> lhs, std::shared_ptr<node::Declaration> rhs) noexcept
    : Instruction{span}, result{std::move(result)}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}

  void Binary::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Binary::Update(Updater& updater) {}
  int32_t Binary::Size() const { return 0; }
  std::string Binary::Name() const { return "Binary"; }
  std::string Binary::ToString() const { return result->ToString() + " = " + lhs->ToString() + " Binary " + rhs->ToString(); }

  void Binary::Declare(code::Encoder& encoder) {
    // encoder.Declare();
  }

  void Binary::Compile(code::Encoder& encoder) {
    
  }
};