export module engine.thread.block;

import <array>;
import <atomic>;
import <cstddef>;

namespace engine::thread {
  export template<typename T, std::size_t Size>
  class Block {
  private:
    std::array<T, Size> tasks;
    Block* prev;
    Block* next;
    std::atomic<std::size_t> head;
    std::atomic<std::size_t> tail;
  public:
    Block() : prev(nullptr), next(nullptr), head(0), tail(0) {}

    void SetPrev(Block* prev_block) { prev = prev_block; }
    void SetNext(Block* next_block) { next = next_block; }

    Block* GetPrev() const { return prev; }
    Block* GetNext() const { return next; }

    bool Push(const T& task) {
      std::size_t current_tail = tail.fetch_add(1, std::memory_order_acquire);

      if (current_tail - head.load(std::memory_order_acquire) >= Size) {
        // Roll back the tail increment
        tail.fetch_sub(1, std::memory_order_release);
        return false; // Block is full
      }

      tasks[current_tail % Size] = task;
      return true;
    }

    bool Push(T&& task) {
      std::size_t current_tail = tail.fetch_add(1, std::memory_order_acquire);

      if (current_tail - head.load(std::memory_order_acquire) >= Size) {
        // Roll back the tail increment
        tail.fetch_sub(1, std::memory_order_release);
        return false; // Block is full
      }

      tasks[current_tail % Size] = std::move(task);
      return true;
    }

    bool Pop(T& task) {
      std::size_t current_head = head.fetch_add(1, std::memory_order_acquire);

      if (current_head == tail.load(std::memory_order_acquire)) {
        // Roll back the head increment
        head.fetch_sub(1, std::memory_order_release);
        return false; // Block is empty
      }

      task = std::move(tasks[current_head % Size]);
      return true;
    }

    bool Steal(T& task) {
      std::size_t current_tail = tail.load(std::memory_order_relaxed);
      std::size_t current_head = head.load(std::memory_order_acquire);

      while (current_tail > current_head) {
        std::size_t new_tail = current_tail - 1;

        if (tail.compare_exchange_weak(current_tail, new_tail, std::memory_order_acquire, std::memory_order_relaxed)) {
          task = std::move(tasks[new_tail % Size]);
          return true;
        }

        // Update `current_head` in case it changes during the loop.
        current_head = head.load(std::memory_order_acquire);
      }

      return false; // Block is empty
    }

    bool IsEmpty() const {
      return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
    }

    bool IsFull() const {
      return tail.load(std::memory_order_acquire) - head.load(std::memory_order_acquire) >= Size;
    }
  };
} // namespace engine::thread