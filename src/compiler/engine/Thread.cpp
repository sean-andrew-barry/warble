import engine.thread;
import engine.thread_pool;
import node;
import utility.print;
import utility.os;

import <cassert>;
import <iostream>;

namespace engine {
  thread_local engine::Thread* Thread::current_thread = nullptr;
  std::thread::id Thread::main_thread_id = std::this_thread::get_id();

  int Thread::Main() {
    utility::OS::SetThreadPriorityLow(thread.native_handle());

    try {
      // We'll stay in this loop until told to close
      while (true) {
        auto current = state.load(std::memory_order_relaxed);

        if (current == State::WAITING) {
          // If WAITING, then wait until the state is changed from outside
          state.wait(State::WAITING); 
        } else if (current == State::CLOSING) {
          // If CLOSING, then exit the main loop
          break;
        } else if (current == State::WORKING) {
          Work();
        }
      }

      return 0;
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      return -1;
    }
  }

  engine::Thread* Thread::Get() {
    return Thread::current_thread;
  }

  bool Thread::IsMain() {
    return main_thread_id == std::this_thread::get_id();
  }

  Thread::Thread(engine::ThreadPool& thread_pool)
    : thread_pool{thread_pool}
    , state{State::WAITING}
    , thread{[this]{
      Thread::current_thread = this;

      utility::Debug("Worker", GetID(), "begin");

      auto result = Main();
      assert(result == 0);

      utility::Debug("Worker", GetID(), "end");

      Thread::current_thread = nullptr;
    }}
  {
  }

  Thread::~Thread() {
    // StartClosing(); // Make sure it's in closing state

    if (thread.joinable()) {
      thread.join();
    }
  }

  void Thread::Work() {
    // Go to the thread pool and attempt to do a task
    thread_pool.Work(*this);
  }

  void Thread::StartWaiting() { state.store(State::WAITING, std::memory_order_release); state.notify_one(); }
  void Thread::StartWorking() { state.store(State::WORKING, std::memory_order_release); state.notify_one(); }
  void Thread::StartClosing() { state.store(State::CLOSING, std::memory_order_release); state.notify_one(); }

  bool Thread::IsWaiting() const { return state.load(std::memory_order_relaxed) == State::WAITING; }
  bool Thread::IsWorking() const { return state.load(std::memory_order_relaxed) == State::WORKING; }
  bool Thread::IsClosing() const { return state.load(std::memory_order_relaxed) == State::CLOSING; }
};