#pragma once

#include "compiler/engine/ThreadPool.hpp"
class Compiler; // forward declaration

namespace compiler {
  // Engine owns the thread pool and connects it to the Compiler.
  class Engine {
  private:
    engine::ThreadPool thread_pool;
  public:
    explicit Engine(Compiler& compiler);

    void Activate();
    void Pause();
    void Deactivate();

    size_t ThreadCount() const;
  };
}