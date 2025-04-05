import node.declaration.rvalue.literal.structured.function;
import node.declaration.rvalue.literal.structured.object;
import node.scope.global;
import node.scope.context.function;
import node.scope;
import code.x64.encoder;
import code.x64._register;
import code.x64.memory;
import visitor;
import updater;
import utility.print;
import <typeinfo>;
import <typeindex>;
import <memory>;

namespace node::declaration::rvalue::literal::structured {
  Function::Function(const lexical::source::Span& span, node::Scope& scope, node::scope::context::Function& function_scope) noexcept
    : Structured{span, scope}, function_scope{function_scope}
  {}
  
  Function::Function(const lexical::source::Span& span, node::Scope& scope, node::scope::context::Function& function_scope, std::vector<std::shared_ptr<node::Declaration>>&& parameters, std::vector<std::shared_ptr<node::Declaration>>&& captures, bool is_arrow) noexcept
    : Structured{span, scope}, function_scope{function_scope}, parameters{std::move(parameters)}, captures{std::move(captures)}, is_arrow{is_arrow}
  {}

  std::shared_ptr<node::declaration::rvalue::literal::structured::Object> Function::TypeOf() const { return node::scope::Global::Function(); }

  void Function::IsArrow(bool b) { is_arrow = b; }
  bool Function::IsArrow() const { return is_arrow; }

  void Function::Parameter(std::shared_ptr<node::Declaration>&& parameter) { parameters.push_back(std::move(parameter)); }
  void Function::Capture(std::shared_ptr<node::Declaration>&& capture) { captures.push_back(std::move(capture)); }

  const node::scope::context::Function& Function::FunctionScope() const { return function_scope; }
  const std::vector<std::shared_ptr<node::Declaration>>& Function::Parameters() const { return parameters; }
  const std::vector<std::shared_ptr<node::Declaration>>& Function::Captures() const { return captures; }

  void Function::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Function::Update(Updater& updater) {}

  int32_t Function::Size() const {
    return 8; // It's a pointer

    // int size = 0;
    // for (auto c : captures) {
    //   size += c ? c->Size() : 0;
    // }

    // // for (auto p : parameters) {
    // //   size += p ? p->Size() : 0;
    // // }

    // return size;
  }

  std::vector<std::type_index> Function::Signature() const {
    std::vector<std::type_index> signature{};

    for (auto c : captures) {
      signature.emplace_back(typeid(c));
    }

    for (auto p : parameters) {
      signature.emplace_back(typeid(p));
    }
    
    return signature;
  }

  std::string Function::Name() const { return "Function Literal"; }
  // std::string Function::ToString() const { return std::string{Name()}; }

  std::string Function::ToString() const {
    return utility::Line(Name());
    // auto string = utility::Line("Function {");

    // // {
    // //   utility::Indent i{};

    // //   for (auto& node : Nodes()) {
    // //     string += node->ToString();
    // //   }
    // // }

    // string += utility::Line("}");
    
    // return string;
  }

  std::shared_ptr<node::Declaration> Function::Value() const { return nullptr; }

  void Function::Construct(code::x64::Encoder& encoder) {
    // // Store the current position in the machine code buffer
    // offset = encoder.Size();
    // utility::Print("Function literal at instruction offset", offset);

    // // Generate prologue: Push RBP and set RBP to RSP
    // encoder.PUSH_RBP();
    // encoder.MOV_R64_R64(code::x64::RBP, code::x64::RSP);
    // uint32_t size = static_cast<uint32_t>(Size() + function_scope.Size());
    // utility::Print("Function literal allocating", size, "bytes for stack frame");

    // // Allocate stack space for local variables
    // encoder.SUB_R64_IMM32(code::x64::RSP, size);

    // // Compile the body of the function
    // function_scope.Compile(encoder);

    // // Generate epilogue: Reset RSP to RBP, pop RBP, and return
    // encoder.MOV_R64_R64(code::x64::RSP, code::x64::RBP);
    // encoder.POP_RBP();
    // encoder.RET();
  }

  void Function::Call(code::x64::Encoder& encoder, node::Declaration& rhs) {
    // // Load the parameter base address into RDI
    // encoder.LEA_R64_M64(code::x64::RDI, code::x64::Memory64{rhs.Displacement()});

    // utility::Print("Encoding function literal call at displacement", rhs.Displacement(), "and offset", offset - encoder.Size());

    // // Call the function using the stored offset
    // encoder.CALL_REL32(static_cast<int32_t>(offset - encoder.Size()));
  }

  // void Function::Compile(code::x64::Encoder& encoder) {
  // }
};