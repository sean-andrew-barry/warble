export module engine.thread;

import <functional>;
import <thread>;
import <vector>;
import <string>;
import <atomic>;
import <memory>;
import <iostream>;
import <cassert>;
import <chrono>;

export class Node;

namespace engine {
  export class ThreadPool;

  export class Thread
  {
  private:
    static thread_local engine::Thread* current_thread;
    static std::thread::id main_thread_id;

    enum class State {
      WAITING,
      WORKING,
      CLOSING,
    };

    std::thread thread;
    std::vector<Node*> overflow{};
    engine::ThreadPool& thread_pool;
    std::atomic<State> state;

    int Main();
  public:
    static engine::Thread* Get();
    static bool IsMain();

    Thread(Thread&& other) noexcept = delete;  // Disable move-construction
    Thread(const Thread&) = delete;            // Copy constructor
    Thread &operator=(const Thread&) = delete; // Copy assignment
    Thread &operator=(Thread&&) = delete;      // Move assignment

    Thread(engine::ThreadPool& thread_pool);
    ~Thread();

    void Work();

    bool IsWaiting() const;
    bool IsWorking() const;
    bool IsClosing() const;

    void StartWaiting();
    void StartWorking();
    void StartClosing();

    std::thread::id GetID() const noexcept { return thread.get_id(); }
  };
};