import compiler.Engine;
import <thread>;
import <chrono>;

namespace compiler {
  Engine::Engine(Compiler& compiler)
    : thread_pool{compiler}
  {}

  void Engine::Activate() {
    thread_pool.Activate();
  }

  void Engine::Pause() {
    thread_pool.Pause();
  }

  void Engine::Deactivate() {
    thread_pool.Deactivate();
  }

  size_t Engine::ThreadCount() const { return thread_pool.Threads().size(); }
}