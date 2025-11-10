export module Compiler;

import compiler.Engine;
import compiler.Program;
import compiler.program.Package;
import compiler.program.Module;
import compiler.utility.DualBuffer;
import <memory>;
import <vector>;
import <string>;
import <filesystem>;
import <stdexcept>;

export class Compiler {
private:
  std::vector<std::string> commands;
  compiler::Engine engine; // Compiler owns a single shared Engine
  std::unique_ptr<compiler::Program> program;
  std::vector<compiler::utility::DualBuffer<compiler::program::Module*>> work_buffers;
public:
  // Compiler owns the commands and the Engine
  Compiler(std::vector<std::string>&& cmds)
    : commands{std::move(cmds)}, engine{*this}
  {
    if (commands.size() < 2) {
      throw std::invalid_argument("Compiler requires root and entry paths");
    }

    std::filesystem::path root_path{commands[0]};
    std::filesystem::path entry_path{commands[1]};

    program = std::make_unique<compiler::Program>(*this);

    compiler::program::Package& pkg = program->Register(std::move(root_path));
    compiler::program::Module& mod = pkg.Import(std::move(entry_path));

    auto thread_count = engine.ThreadCount();
    if (thread_count == 0u) {
      thread_count = 1u;
    }

    work_buffers.reserve(thread_count);
    for (auto i = 0u; i < thread_count; ++i) {
      work_buffers.emplace_back();
    }

    work_buffers[0].Reschedule(&mod);
  }

  // Hand control to the thread pool to perform work
  void Run() { engine.Activate(); }

  bool Work(size_t index) {
    // The index tells us which thread is checking in for work
    if (index >= work_buffers.size()) {
      return false;
    }

    auto& buffer = work_buffers[index];

    if (buffer.Empty()) return false; // No work to do right now

    auto mod = buffer.Take();
    return mod->Run(buffer);
  }
};