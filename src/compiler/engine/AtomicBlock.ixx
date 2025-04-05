export module engine.atomic_block;

import <atomic>;
import <vector>;
import <memory>;
import <new>;
import <cstddef>;
import <iterator>;

export template<typename T, std::size_t BLOCK_SIZE = 1024>
struct AtomicBlock {
  using Block = AtomicBlock<T, BLOCK_SIZE>;
  using StorageType = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

  StorageType storage[BLOCK_SIZE]; // Uninitialized memory for elements
  std::atomic<size_t> currentIndex{0};
  std::atomic<Block*> next{nullptr};

  AtomicBlock() = default;

  ~AtomicBlock() {
    // Manually call destructor for constructed elements
    size_t length = currentIndex.load(std::memory_order_acquire);
    for (std::size_t i = 0; i < length; ++i) {
      ptr(i)->~T();
    }

    Block* n = next.load();
    if (n) delete n;
  }

  // Helper to get a pointer to a specific element
  T* ptr(std::size_t index) { return reinterpret_cast<T*>(&storage[index]); }
  const T* ptr(std::size_t index) const { return reinterpret_cast<const T*>(&storage[index]); }

  void Connect(Block* newBlock) {
    Block* current = this;

    while (true) {
      Block* expected = nullptr;
      
      // Attempt to set the new block as the next block.
      if (current->next.compare_exchange_weak(expected, newBlock, std::memory_order_release, std::memory_order_relaxed)) {
        // Successfully linked the new block.
        return;
      }

      // If the exchange failed, `expected` now contains the value of `current->next`.
      // Move to the next block in the chain.
      current = expected;
    }
  }

  Block* Next() {
    Block* n = next.load();
    if (n) return n;

    Connect(new Block());

    return next.load();
  }

  template<typename... Args>
  void Emplace(std::size_t index, Args&&... args) {
    new(&storage[index]) T(std::forward<Args>(args)...);
  }

  void Push(std::size_t index, const T& value) {
    new(&storage[index]) T(value);
  }

  void Push(std::size_t index, T&& value) {
    new(&storage[index]) T(std::move(value));
  }

  class iterator {
  private:
    T* ptr;
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

    iterator(T* ptr) : ptr{ptr} {}

    reference operator*() const { return *ptr; }
    pointer operator->() { return ptr; }

    // Prefix increment
    iterator& operator++() {
      ++ptr;
      return *this;
    }

    // Postfix increment
    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const iterator& a, const iterator& b) { return a.ptr == b.ptr; }
    friend bool operator!=(const iterator& a, const iterator& b) { return a.ptr != b.ptr; }
  };

  iterator begin() { return iterator{ptr(0)}; }
  iterator end() { return iterator{ptr(currentIndex.load(std::memory_order_acquire))}; }
};