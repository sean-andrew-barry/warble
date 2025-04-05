import node.scope.context._module;
import node.scope.context.function;
import node.package;
import node.declaration.literal.primitive;
import visitor;
import updater;
import node.declaration;
import node.compiler;
import node.compiler.registration;
import lexical.lexer;
import lexical.parser;
import utility.print;
import code.x64.encoder;

import <stdexcept>;
import <typeinfo>;
import <memory>;

// #include "../../../utility/Macros.hpp"

namespace node::scope::context {
  // void Module::DoSomethingStatic(void* instance) {
  //   utility::Print("Successfully called static do_something function");
  //   // static_cast<Module*>(instance)->DoSomething();
  // }

  // void Module::DoSomething(Module* mod) {
  //   utility::Print("Successfully called do_something function");
  // }

  Module::Module(node::Package& parent, std::string&& s, std::string&& c)
    : Context{lexical::source::Span{}}
    , parent{parent}
    , specifier{std::move(s)}
    , source{std::move(c)}
  {
    // functions["do_something"] = &Module::DoSomething;
    // functions["do_something"] = Module::DoSomethingStatic;
    // functions["do_something"] = [this](){
    //   utility::Print("Successfully called do_something function");
    // };

    // // Print("Got path: ", path);
    // content = std::move(read(path));

    // std::string initial{content};
    // if constexpr (!DEBUGGING) {
    //   const size_t size = 200'000'000;
    //   content.reserve(size + content.size());

    //   while (content.size() <= size) {
    //     content += initial;
    //   }
    // }

    // start = Span{std::string_view{content}, 1, 1};
    // span = Span{content.cbegin(), content.size(), 1, 1};
    // root = std::make_unique<Scope>(span, *this);
    // root->SetStart(start);
  }

  Module::~Module() = default;

  void Module::AddFunction(node::scope::context::Function* function) {
    functions.push_back(function);
  }

  void Module::AddPrimitive(node::declaration::literal::Primitive* primitive) {
    primitives.push_back(primitive);
  }

  // // const std::function<void()>& Module::FindFunction(const std::string& name) const {
  // Module::Function Module::FindFunction(const std::string& name) {
  //   auto iter = functions.find(name);

  //   if (iter == functions.end()) {
  //     throw utility::Error("Unknown function name", name);
  //   }
  //   else {
  //     return iter->second;
  //     // return iter->second.target<void(*)()>();
  //   }
  // }

  node::Package& Module::Register(std::string&& path, std::string&& name, std::vector<std::pair<std::string, std::string>>&& allowed) {
    return parent.Register(std::move(path), std::move(name), std::move(allowed));
  }

  node::scope::context::Module& Module::Import(std::string&& specifier, std::string&& source) {
    return parent.Import(std::move(specifier), std::move(source));
  }

  // bool Module::Register(std::string&& path, std::string&& specifier, std::vector<std::string>&& allowed) {
  //   return parent.Register(std::move(path), std::move(specifier), std::move(allowed), wildcard);
  // }

  // bool Module::Import(std::string&& specifier) {
  //   auto* mod = compiler.Import(std::move(specifier));
  //   imports.push_back(mod);

  //   return true;
  // }

  node::Package& Module::Parent() const { return parent; }
  const std::string& Module::Specifier() const { return specifier; }
  const lexical::Source& Module::Source() const { return source; }

  bool Module::Parse() {
    lexical::Lexer lexer{*this, source};

    while (lexer.Valid()) {
      bool statement = lexer.Statement();

      if (!statement) {
        if (lexer.Done()) break;
        
        return false;
      }
    }

    lexical::Parser parser{*this, source};

    while (parser.Valid()) {
      Node* node = parser.Statement(*this);
      if (!node) {
        utility::Debug("Parsing failed!");
        return false;
      }
    }
    
    return true;
  }

  void Module::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Module::Update(Updater& updater) {}
  int32_t Module::Size() const { return LargestOffset(); }
  std::string Module::Name() const { return utility::Join("Module ", specifier); }

  std::string Module::ToString() const {
    if (Blocks().empty()) return utility::Line(Name(), " {}");

    auto string = utility::Line(Name(), " {");

    {
      utility::Indent i{};

      for (auto& node : Blocks()) {
        // string += utility::Line(typeid(*node));
        string += node->ToString();
      }

      for (auto& node : Scopes()) {
        // string += utility::Line(typeid(*node));
        string += node->ToString();
      }
    }

    string += utility::Line("}");
    
    return string;
  }

  // std::string Module::ToString() const { return std::string{Name()}; }

  void Module::Compile(code::x64::Encoder& encoder) {
    // // Store a jump as the very first instruction.
    // // This will take us past the functions, to the module entry point.
    // auto start = encoder.Size();
    // encoder.JMP_REL32(0);
    // // code::Placeholder32 call = encoder.JMP_REL32();
    // encoder.INT3();
    // // encoder.RET();

    // for (auto& primitive : primitives) {
    //   primitive->Compile(encoder);
    // }

    // for (auto& function : functions) {
    //   function->Compile(encoder);
    // }

    // call.Emit(encoder.Size() - start); // Set the real value in the placeholder
    // utility::Debug("Call distance is", encoder.Size() - start);
    // StartPosition(encoder.Size());

    // // Generate prologue: Push RBP and set RBP to RSP
    // Offset(8);
    // encoder.PUSH_RBP();
    // encoder.MOV_R64_R64(code::x64::RBP, code::x64::RSP);

    // // Allocate stack space for local variables
    // code::Placeholder32 sub = encoder.SUB_R64_IMM32(code::x64::RSP);

    // // Compile the body of the module
    // Context::Compile(encoder);

    // // Generate epilogue: Reset RSP to RBP, pop RBP, and return
    // encoder.MOV_R64_R64(code::x64::RSP, code::x64::RBP);
    // encoder.POP_RBP();
    // Offset(-8);
    // // encoder.RET();

    // uint32_t size = static_cast<uint32_t>(encoder.Align16(Size()));
    // utility::Debug("Module scope started at instruction offset", StartPosition(), "with a size of", size, Size());

    // sub.Emit(size);
  }
};