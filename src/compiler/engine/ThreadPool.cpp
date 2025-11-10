import compiler.engine.ThreadPool;
import compiler.engine.Thread;
import Compiler;

import <iostream>;
import <thread>;
import <cassert>;
import <stdexcept>;

namespace compiler::engine {
  ThreadPool::ThreadPool(Compiler& compiler)
    : compiler{compiler}, active{false}
  {
    auto count = std::thread::hardware_concurrency();
    if (count == 0u) {
      count = 1u;
    }

    const auto thread_count = static_cast<size_t>(count);
    threads.reserve(thread_count);

    for (size_t i = 0u; i < thread_count; ++i) {
      threads.emplace_back(std::make_unique<engine::Thread>(compiler, i));
    }
  }

  ThreadPool::~ThreadPool() {
    for (auto& thread : threads) {
      thread->StartClosing();
    }

    for (auto& thread : extras) {
      thread->StartClosing();
    }
  }

  void ThreadPool::Activate() {
    if (!engine::Thread::IsMain()) throw std::runtime_error("ThreadPool::Activate must ONLY be called by the main thread");
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
    if (engine::Thread::IsMain()) throw std::runtime_error("ThreadPool::Pause must NOT be called by the main thread");
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
    if (engine::Thread::IsMain()) throw std::runtime_error("ThreadPool::Deactivate must NOT be called by the main thread");
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
};