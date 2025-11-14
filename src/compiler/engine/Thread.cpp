import compiler.engine.Thread;
import Compiler;
import compiler.utility.OS;
import compiler.utility.Print;

import <cassert>;
import <exception>;
import <iostream>;
import <thread>;
import <atomic>;

namespace compiler::engine {
  thread_local compiler::engine::Thread* Thread::current_thread = nullptr;
  std::thread::id Thread::main_thread_id = std::this_thread::get_id();

  int Thread::Main() {
    compiler::utility::OS::SetThreadPriorityLow(thread.native_handle());

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

  Thread::Thread(Compiler& compiler, size_t index)
    : compiler{compiler}
    , index{index}
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
    // Ask the compiler to perform a unit of work
    if (!compiler.Work(index)) {
      // utility::Debug("Worker", GetID(), "idle");

      // Idle briefly to avoid busy-waiting when there's no work
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }

  void Thread::StartWaiting() { state.store(State::WAITING, std::memory_order_release); state.notify_one(); }
  void Thread::StartWorking() { state.store(State::WORKING, std::memory_order_release); state.notify_one(); }
  void Thread::StartClosing() { state.store(State::CLOSING, std::memory_order_release); state.notify_one(); }

  bool Thread::IsWaiting() const { return state.load(std::memory_order_relaxed) == State::WAITING; }
  bool Thread::IsWorking() const { return state.load(std::memory_order_relaxed) == State::WORKING; }
  bool Thread::IsClosing() const { return state.load(std::memory_order_relaxed) == State::CLOSING; }
};