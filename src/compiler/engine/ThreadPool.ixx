export module compiler.engine.ThreadPool;

import <vector>;
import <memory>;
import <atomic>;

export class Compiler; // forward declaration

namespace compiler::engine {
  export class Thread;

  export class ThreadPool {
  private:
    Compiler& compiler;
    std::vector<std::unique_ptr<compiler::engine::Thread>> threads;
    std::vector<std::unique_ptr<compiler::engine::Thread>> extras;
    std::atomic<bool> active;
  public:
    ThreadPool(Compiler& compiler);
    ~ThreadPool();

    void Activate();
    void Pause();
    void Deactivate();

    const std::vector<std::unique_ptr<compiler::engine::Thread>>& Threads() const { return threads; }
  };
}