export module compiler.engine.Thread;

import <functional>;
import <thread>;
import <string>;
import <atomic>;
import <memory>;
import <iostream>;
import <cassert>;
import <chrono>;
import <cstddef>;

export class Compiler; // forward declaration

namespace compiler::engine {
  export class Thread {
  private:
    static thread_local engine::Thread* current_thread;
    static std::thread::id main_thread_id;

    enum class State {
      WAITING,
      WORKING,
      CLOSING,
    };

    std::thread thread;
    Compiler& compiler; // Thread checks in with Compiler for work
    size_t index;      // Index of this thread in the thread pool
    std::atomic<State> state;

    int Main();
  public:
    static compiler::engine::Thread* Get();
    static bool IsMain();

    Thread(Thread&& other) noexcept = delete;  // Disable move-construction
    Thread(const Thread&) = delete;            // Copy constructor
    Thread &operator=(const Thread&) = delete; // Copy assignment
    Thread &operator=(Thread&&) = delete;      // Move assignment

    Thread(Compiler& compiler, size_t index);
    ~Thread();

    void Work();
    
    bool IsWaiting() const;
    bool IsWorking() const;
    bool IsClosing() const;
    
    void StartWaiting();
    void StartWorking();
    void StartClosing();
    
    size_t Index() const noexcept { return index; }
    std::thread::id GetID() const noexcept { return thread.get_id(); }
  };
}