export module compiler.Engine;

import compiler.engine.ThreadPool;

class Compiler; // forward declaration for constructor parameter

namespace compiler {
  // Engine owns the thread pool and connects it to the Compiler.
  export class Engine {
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