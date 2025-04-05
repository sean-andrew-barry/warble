import node.instruction.control._return;
import node.declaration;
import code.x64.encoder;
import visitor;
import updater;
import utility.print;

namespace node::instruction::control {
  Return::Return(const lexical::source::Span& span, std::shared_ptr<node::Declaration> result)
    : Control{span}, result{std::move(result)}
  {}

  void Return::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Return::Update(Updater& updater) {}
  int32_t Return::Size() const { return 0; }
  // std::string Return::Name() const { return "Return"; }
  // std::string Return::ToString() const { return Name(); }

  std::string Return::Name() const {
    if (result) {
      return utility::Join("return ", result->Name());
    } else {
      return utility::Join("return");
    }
  }
  
  std::string Return::ToString() const { return utility::Line(Name()); }

  void Return::Compile(code::x64::Encoder& encoder) {
    // encoder.Epilogue();
  }
};