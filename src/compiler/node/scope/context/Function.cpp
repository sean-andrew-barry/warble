import node.scope.context.function;
import node.scope;
import code.x64.encoder;
import node.instruction.control._return;
import visitor;
import updater;
import utility.print;
import <typeinfo>;

namespace node::scope::context {
  Function::Function(const lexical::source::Span& span, node::Scope& parent)
    : Context{span}
    , parent{parent}
  {
  }

  void Function::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Function::Update(Updater& updater) {}
  int32_t Function::Size() const { return LargestOffset(); }
  std::string Function::Name() const { return "Function Scope"; }
  // std::string Function::ToString() const { return Name(); }

  std::string Function::ToString() const {
    if (Blocks().empty()) return utility::Line(Name(), " {}");

    auto string = utility::Line(Name(), " {");

    {
      utility::Indent i{};

      for (auto& node : Blocks()) {
        string += node->ToString();
      }

      for (auto& node : Scopes()) {
        string += node->ToString();
      }
    }

    string += utility::Line("}");
    
    return string;
  }

  // void Function::IsArrow(bool b) { is_arrow = b; }
  // bool Function::IsArrow() const { return is_arrow; }
  // void Function::Parameter(node::Declaration* parameter) { parameters.push_back(parameter); }
  // void Function::Capture(node::Declaration* capture) { captures.push_back(capture); }
  // const std::vector<node::Declaration*>& Function::Parameters() const { return parameters; }
  // const std::vector<node::Declaration*>& Function::Captures() const { return captures; }

  void Function::Compile(code::x64::Encoder& encoder) {
    // // Store the current position in the machine code buffer
    // StartPosition(encoder.Size());

    // Offset(8);
    // encoder.Prologue();

    // // Allocate stack space for local variables
    // code::Placeholder32 sub = encoder.SUB_R64_IMM32(code::x64::RSP);

    // // Compile the body of the function
    // Context::Compile(encoder);

    // Offset(-8);

    // // auto& instructions = LastBodyBlock().Instructions();
    // // if (instructions.empty() || typeid(instructions.back()) != typeid(node::instruction::control::Return)) {
    // //   encoder.Epilogue();
    // // }

    // encoder.Epilogue();

    // uint32_t size = static_cast<uint32_t>(encoder.Align16(Size()));
    // utility::Debug("Function scope started at instruction offset", StartPosition(), "with a size of", size, Size());

    // sub.Emit(size);
  }
};