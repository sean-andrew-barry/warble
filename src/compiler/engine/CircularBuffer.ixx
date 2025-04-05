export module engine.circular_buffer;

import engine.raw_array;

import <array>;
import <atomic>;
import <optional>;
import <algorithm>;
import <iostream>;

constexpr std::size_t CACHE_LINE_SIZE = 64;

export template <typename T, uint64_t SIZE>
class CircularBuffer
{
public:
  using size_type = uint64_t;
private:
  alignas(CACHE_LINE_SIZE) std::array<T, SIZE> buffer;

  alignas(CACHE_LINE_SIZE) std::atomic<size_type> head{0};
  alignas(CACHE_LINE_SIZE) std::atomic<size_type> tail{0};
  alignas(CACHE_LINE_SIZE) std::atomic<size_type> completed{0};

  static constexpr size_type MaxTail() { return std::numeric_limits<size_type>::max() - std::max(SIZE, size_type{10000}); }
public:
  CircularBuffer() = default;
  ~CircularBuffer() = default;

  CircularBuffer(CircularBuffer&& other) = default; // Move construction
  CircularBuffer(const CircularBuffer& other) = default; // Copy construction

  size_type GetTail() const { return tail.load(std::memory_order_relaxed); }
  size_type GetHead() const { return head.load(std::memory_order_relaxed); }
  size_type GetCompleted() const { return completed.load(std::memory_order_relaxed); }
  size_type GetPending() const { return GetTail() - GetHead(); }
  size_type GetActive() const { return GetHead() - GetCompleted(); }
  size_type Complete() { return completed.fetch_add(1, std::memory_order_relaxed); }

  template <typename... Args>
  bool Emplace(Args&&... args) {
    size_type current_tail = tail.fetch_add(1, std::memory_order_relaxed);
    
    if (MaxTail() > current_tail) {
      size_type current_head = head.load(std::memory_order_acquire);

      if ((current_tail - current_head) < SIZE) {
        size_type index = current_tail % SIZE;
        // Use placement new to construct the element in place.
        new (&buffer[index]) T(std::forward<Args>(args)...);
        tail.notify_one();
        return true;
      }
    }

    // Buffer is either full or in shutdown mode
    // Undo the increment since the task cannot be added
    tail.fetch_sub(1, std::memory_order_relaxed);
    return false;
  }

  bool Push(T element) {
    size_type current_tail = tail.fetch_add(1, std::memory_order_relaxed);
    
    if (MaxTail() > current_tail) {
      size_type current_head = head.load(std::memory_order_acquire);

      if ((current_tail - current_head) < SIZE) {
        buffer[current_tail % SIZE] = std::move(element);
        tail.notify_one();
        return true;
      }
    }

    // Buffer is either full or in shutdown mode.
    // Undo the increment since the task cannot be added.
    tail.fetch_sub(1, std::memory_order_relaxed);
    return false;
  }

  std::optional<T> Pop() {
    size_type old_head = head.fetch_add(1, std::memory_order_relaxed);
    size_type current_tail = tail.load(std::memory_order_acquire);

    if ((current_tail - old_head) < SIZE) {
      T* ptr = &buffer[old_head % SIZE];
      std::optional<T> ret = std::move(*ptr); // Move the element to the optional.
      ptr->~T(); // Manually invoke the destructor for the element.
      return ret;
    }

    // Buffer is empty.
    // Undo the increment since there is nothing to pop.
    head.fetch_sub(1, std::memory_order_relaxed);
    return std::nullopt;
  }

  std::optional<T> Pop() {
    size_type current_head = head.load(std::memory_order_relaxed);
    size_type current_tail = tail.load(std::memory_order_acquire);

    while (current_head < current_tail && MaxTail() > current_tail) {
      if (head.compare_exchange_weak(current_head, current_head + 1, std::memory_order_release, std::memory_order_relaxed)) {
        T* ptr = &buffer[current_head % SIZE];
        std::optional<T> ret = std::move(*ptr); // Move the element to the optional.
        ptr->~T(); // Manually invoke the destructor for the element.
        return ret;
      }

      current_tail = tail.load(std::memory_order_acquire);
    }

    return std::nullopt; // Buffer is empty.
  }

  template<size_type Step>
  RawArray<T, Step> Pop() {
    RawArray<T, Step> ret;

    size_type current_head = head.load(std::memory_order_relaxed);
    
    while (true) {
      size_type current_tail = tail.load(std::memory_order_acquire);
      if (current_tail >= MaxTail()) break;

      size_type available = current_tail - current_head;
      size_type actual_step = std::min(Step, available);

      if (0 >= actual_step) {
        break; // No elements are available
      }

      // TODO: Handle alignment
      // // int offset = number & 0x3;

      size_type next_tail = current_head + actual_step;
      if (head.compare_exchange_weak(current_head, next_tail, std::memory_order_release, std::memory_order_relaxed)) {
        for (size_type i = 0; i < actual_step; ++i) {
          T* ptr = &buffer[(next_tail - actual_step + i) % SIZE];
          ret.Push(std::move(*ptr)); // Move the element
          ptr->~T(); // Manually invoke the destructor
        }

        break;
      }
    }

    return ret;
  }

  void Wait() {
    size_type current_tail = tail.load(std::memory_order_acquire);
    if (MaxTail() > current_tail) {
      tail.wait(tail.load());
    }
  }

  void Shutdown() {
    // Acquire current positions to prevent further task addition or removal
    size_type current_tail = tail.exchange(MaxTail(), std::memory_order_acq_rel);
    if (current_tail >= MaxTail()) {
      // Already shut down
      tail.notify_all();
      return;
    }

    size_type current_head = head.load(std::memory_order_acquire);

    // Manually destruct each remaining task in the buffer
    for (size_type i = current_head; i != current_tail; ++i) {
      T* ptr = &buffer[i % SIZE];
      ptr->~T();
    }

    // Notify all threads waiting for tasks to process the shutdown signal
    tail.notify_all();
  }
};
