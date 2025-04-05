export module engine.thread_pool;

import engine.circular_buffer;
import updater;

import <vector>;
import <memory>;
import <atomic>;

export class Node;

namespace engine {
  export class Thread;

  export class ThreadPool {
  private:
    CircularBuffer<Node*, 10000> buffer;
    Updater updater;
    std::vector<std::unique_ptr<engine::Thread>> threads;
    std::vector<std::unique_ptr<engine::Thread>> extras;
    std::atomic<bool> active;
  public:
    ThreadPool();
    ~ThreadPool();

    void Activate();
    void Pause();
    void Deactivate();
    void Work(engine::Thread&, bool waited = false);
    bool Push(Node*);
  };
};