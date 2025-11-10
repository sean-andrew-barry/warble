export module compiler.engine.thread.Block;

import <array>;
import <atomic>;
import <cstddef>;

namespace compiler::engine::thread {
  export template<typename T, std::size_t Size>
  class Block {
  private:
    std::array<T, Size> tasks;
    Block* prev;
    Block* next;
    std::atomic<std::size_t> head; // steal end (thieves CAS here)
    std::atomic<std::size_t> tail; // owner end (owner pushes/pops here)
  public:
    Block() : prev(nullptr), next(nullptr), head(0), tail(0) {}

    void SetPrev(Block* prev_block) { prev = prev_block; }
    void SetNext(Block* next_block) { next = next_block; }

    Block* GetPrev() const { return prev; }
    Block* GetNext() const { return next; }

    // Owner push at tail (publish-after-write; no atomic RMW on hot path)
    bool Push(const T& task_value) {
      std::size_t t = tail.load(std::memory_order_relaxed);
      std::size_t h = head.load(std::memory_order_acquire);

      if (t - h >= Size) {
        return false; // Block is full
      }

      tasks[t % Size] = task_value;                         // write element
      tail.store(t + 1, std::memory_order_release);         // publish
      return true;
    }

    bool Push(T&& task_value) {
      std::size_t t = tail.load(std::memory_order_relaxed);
      std::size_t h = head.load(std::memory_order_acquire);

      if (t - h >= Size) {
        return false; // Block is full
      }

      tasks[t % Size] = std::move(task_value);              // write element
      tail.store(t + 1, std::memory_order_release);         // publish
      return true;
    }

    // Owner pop from tail (LIFO). No RMW unless resolving size==1 race.
    bool Pop(T& out) {
      // Tentatively take from tail - 1
      std::size_t t = tail.load(std::memory_order_relaxed);
      if (t == head.load(std::memory_order_acquire)) {
        return false; // empty fast-path
      }

      t -= 1;
      tail.store(t, std::memory_order_relaxed); // provisional shrink

      std::size_t h = head.load(std::memory_order_acquire);
      if (h <= t) {
        // We own the slot at t
        out = std::move(tasks[t % Size]);
        return true;
      } else {
        // Raced with a steal or was empty; restore and handle size==1 case
        tail.store(h, std::memory_order_relaxed);
        // If size was exactly 1, either we or a thief should get it.
        if (h < t + 1) {
          // Try to claim the last item by advancing head (classic fix-up).
          if (head.compare_exchange_strong(
                  h, h + 1,
                  std::memory_order_acq_rel,
                  std::memory_order_acquire)) {
            out = std::move(tasks[h % Size]);
            return true;
          }
        }
        return false;
      }
    }

    // Thief steals from head (CAS on head).
    bool Steal(T& out) {
      for (;;) {
        std::size_t h = head.load(std::memory_order_acquire);
        std::size_t t = tail.load(std::memory_order_acquire);

        if (h >= t) {
          return false; // empty
        }

        // Read the element at h; acquire above pairs with owner's release on tail.
        // Claim by moving head forward by 1.
        if (head.compare_exchange_weak(
                h, h + 1,
                std::memory_order_acq_rel,
                std::memory_order_acquire)) {
          out = std::move(tasks[h % Size]);
          return true;
        }
        // lost the race; retry
      }
    }

    bool IsEmpty() const {
      return head.load(std::memory_order_acquire) ==
             tail.load(std::memory_order_acquire);
    }

    bool IsFull() const {
      std::size_t t = tail.load(std::memory_order_acquire);
      std::size_t h = head.load(std::memory_order_acquire);
      return (t - h) >= Size;
    }
  };
} // namespace compiler::engine::thread
