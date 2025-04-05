export module engine.atomic_deque;

import engine.atomic_block;

import <atomic>;
import <vector>;
import <memory>;
import <cstddef>;
import <iterator>;

export template<typename T, std::size_t BLOCK_SIZE = 1024>
class AtomicDeque {
public:
  using Block = AtomicBlock<T, BLOCK_SIZE>;
private:
  std::atomic<Block*> head; // Pointer to the current head block for appending
  std::atomic<Block*> tail; // Pointer to the tail block for consistent iteration
public:
  AtomicDeque() : head(new Block()), tail(head.load()) {}

  ~AtomicDeque() {
    // Clean up all blocks upon destruction
    Block* t = tail.load();
    if (t) delete t;
  }

  template<typename... Args>
  void Emplace(Args&&... args) {
    while (true) {
      Block* h = head.load(std::memory_order_acquire);
      auto index = h->currentIndex.fetch_add(1, std::memory_order_relaxed);

      if (index >= BLOCK_SIZE) {
        head = h->Next();
      } else {
        h->Emplace(index, std::forward<Args>(args)...);
        return;
      }
    }
  }

  template<typename... Args>
  void Push(Args&&... args) {
    while (true) {
      Block* h = head.load(std::memory_order_acquire);
      auto index = h->currentIndex.fetch_add(1, std::memory_order_relaxed);

      if (index >= BLOCK_SIZE) {
        head = h->Next();
      } else {
        h->Push(index, std::forward<Args>(args)...);
        return;
      }
    }
  }

  class iterator {
  private:
    Block* block;
    Block::iterator iter;
    Block::iterator end;
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

    iterator(Block* block) : block(block), iter(block->begin()), end(block->end()) {}

    reference operator*() { return *iter; }
    pointer operator->() { return iter; }

    iterator& operator++() {
      // When the current block is exhausted
      if (++iter == end) {
        // Attempt to load the next block
        Block* next = block->next.load(std::memory_order_acquire);

        if (next != nullptr) {
          iter = next->begin();
          end = next->end();
          block = next;
        }
      }

      return *this;
    }

    bool operator!=(const iterator& other) const { return iter != other.end; }
  };

  iterator begin() { return iterator(tail.load(std::memory_order_acquire)); }
  iterator end() { return iterator(head.load(std::memory_order_acquire)); }
};