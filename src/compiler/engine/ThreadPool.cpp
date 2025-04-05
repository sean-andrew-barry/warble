import engine.thread_pool;
import engine.thread;
import node;

import <iostream>;
import <thread>;
import <optional>;
import <cassert>;

namespace engine {
  ThreadPool::ThreadPool() : active{false} {
    // auto count = std::thread::hardware_concurrency();
    // auto count = 1u;
    auto count = 0u;
    threads.reserve(count);
    // extras.reserve(2);

    for (auto i = 0u; i < count; ++i) {
      threads.emplace_back(std::make_unique<Thread>(*this));
    }

    for (auto i = 0u; i < extras.capacity(); ++i) {
      extras.emplace_back(std::make_unique<Thread>(*this));
    }
  }

  ThreadPool::~ThreadPool() {
    for (auto& thread : threads) {
      thread->StartClosing();
    }

    for (auto& thread : extras) {
      thread->StartClosing();
    }

    buffer.Shutdown();
  }

  void ThreadPool::Activate() {
    if (!engine::Thread::IsMain()) throw std::runtime_error("ThreadPool::Activate must only be called by the main thread");
    if (active.load()) throw std::runtime_error("ThreadPool::Activate called when already active");

    // Have the primary workers start, but don't activate the extras
    for (auto& thread : threads) {
      thread->StartWorking();
    }

    // Main thread has handed over control to the thread pool and now goes idle
    active.store(true);
    active.wait(true);
  }

  void ThreadPool::Pause() {
    if (engine::Thread::IsMain()) throw std::runtime_error("ThreadPool::Pause must not be called by the main thread");
    if (!active.load()) throw std::runtime_error("ThreadPool::Pause called when not active");

    for (auto& thread : threads) {
      thread->StartWaiting();
    }

    for (auto& thread : extras) {
      thread->StartWaiting();
    }

    // Alert the main thread it needs to take control again
    active.store(false);
    active.notify_one();
  }

  void ThreadPool::Deactivate() {
    if (engine::Thread::IsMain()) throw std::runtime_error("ThreadPool::Deactivate must not be called by the main thread");
    if (!active.load()) throw std::runtime_error("ThreadPool::Deactivate called when not active");

    for (auto& thread : threads) {
      thread->StartClosing();
    }

    for (auto& thread : extras) {
      thread->StartClosing();
    }

    // Alert the main thread it needs to take control again
    active.store(false);
    active.notify_one();
  }

  void ThreadPool::Work(engine::Thread& thread, bool waited) {
    std::optional<Node*> task = buffer.Pop();

    if (task) {
      (*task)->Update(updater); // Perform the task
      // if (requeue) {
      //   Push(*task); // The task requested to be requeued
      // }
    } else if (!waited) {
      buffer.Wait(); // Go idle until more tasks are added
      return Work(thread, true);
    }
  }

  bool ThreadPool::Push(Node* node) {
    return buffer.Push(node);
  }
};