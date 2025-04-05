import node.block;
import node.instruction;
import node.scope;
import node.scope.context;
import lexical.source.span;
import node.instruction.memory.destruct;
import code.encoder;
import code.registers;
import code.frame;
import code.x64.encoder;
import utility.print;
import utility.helpers;
// import node.declaration;
import visitor;
import updater;

import <stdexcept>;

namespace node {
  Block::Block(const lexical::source::Span& span) noexcept
    : Node{span}
  {}

  Block::~Block() = default;

  // void Block::AddLiveDeclarations(const std::vector<node::Declaration*>& live_stack) {
  //   live_declarations.insert(live_stack.begin(), live_stack.end());
  // }

  std::unique_ptr<node::Instruction> Block::Remove(node::Instruction* instruction) {
    for (auto it = instructions.begin(); it != instructions.end(); ++it) {
      if (it->get() == instruction) {
        // Take ownership of the instruction
        auto uptr = std::move(*it);
        // Erase the instruction from the instructions vector
        it = instructions.erase(it);
        return std::move(uptr); // Return the instruction
      }
    }
    
    return nullptr; // Return nullptr if no matching instruction is found
  }

  void Block::Add(std::unique_ptr<node::Instruction>&& instruction) {
    instructions.emplace_back(std::move(instruction));
  }

  code::Registers& Block::Registers() { return registers; }
  // void Block::Registers(code::Registers v) { registers = v; }

  // size_t Block::GetIndex() const {
  //   const auto& nodes = scope.Blocks();

  //   for (size_t i = 0; i < nodes.size(); ++i) {
  //     if (nodes[i].get() == this) {
  //       return i;
  //     }
  //   }

  //   // throw std::runtime_error("Block was not found in the scope");
  //   return 0;
  // }

  bool Block::IsTerminated() const {
    return !instructions.empty() && instructions.back()->IsTerminator();
  }

  // bool Block::IsEntry() const {
  //   return &scope.EntryBlock() == this;
  // }

  // bool Block::IsExit() const {
  //   return &scope.ExitBlock() == this;
  // }

  void Block::Successor(Block* block) { predecessors.push_back(block); }
  void Block::Predecessor(Block* block) { successors.push_back(block); }

  bool Block::RemovePredecessor(Block* block) { return utility::Helpers::FindAndErase(predecessors, block); }
  bool Block::RemoveSuccessor(Block* block) { return utility::Helpers::FindAndErase(successors, block); }

  const std::vector<std::unique_ptr<node::Instruction>>& Block::Instructions() const { return instructions; }
  const std::vector<std::unique_ptr<node::Block*>>& Block::Predecessor() const { return predecessors; }
  const std::vector<std::unique_ptr<node::Block*>>& Block::Successors() const { return successors; }
  // node::scope::Context& Block::Context() { return scope.GetContext(); }

  // void Block::Push(node::Declaration& declaration) {
  //   Context().Push(declaration);
  // }

  // void Block::Pop(node::Declaration& declaration) {
  //   Context().Pop(declaration);
  // }

  void Block::Accept(Visitor& visitor) { visitor.Visit(*this); }
  void Block::Update(Updater& updater) {}
  int32_t Block::Size() const { return 0; }
  std::string Block::Name() const { return "Block"; }
  // std::string Block::ToString() const { return Name(); }

  std::string Block::ToString() const {
    if (instructions.empty()) return "";
    // if (instructions.empty()) return utility::Line(Name(), " {}");

    auto string = utility::Line(Name(), " {");

    {
      utility::Indent i{};

      for (auto& node : instructions) {
        string += node->ToString();
      }
    }

    string += utility::Line("}");
    return string;
  }

  void Block::Declare(code::Encoder& encoder) {
    for (auto& instruction : instructions) {
      instruction->Declare(encoder);
    }
  }

  void Block::Compile(code::Encoder& encoder) {
    // encoder.Register(this);

    for (auto& instruction : instructions) {
      instruction->Compile(encoder);
    }
  }

  void Block::Compile(code::x64::Encoder& encoder) {
    for (auto& instruction : instructions) {
      instruction->Compile(encoder);
    }
  }
};