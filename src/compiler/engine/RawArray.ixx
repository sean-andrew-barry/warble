export module engine.raw_array;

import <type_traits>;
import <cstddef>;
import <new>; // for placement new
import <utility>;
import <iterator>;
import <stdexcept>;

export template<typename T, std::size_t N>
class RawArray {
private:
  using StorageType = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  StorageType storage[N];
  std::size_t length{0};

  // Helper to get a pointer to a specific element
  T* ptr(std::size_t index) { return reinterpret_cast<T*>(&storage[index]); }
  const T* ptr(std::size_t index) const { return reinterpret_cast<const T*>(&storage[index]); }
public:
  RawArray() = default;

  ~RawArray() {
    // Manually call destructor for constructed elements
    for (std::size_t i = 0; i < length; ++i) {
      ptr(i)->~T();
    }
  }

  // Disable copy and move semantics
  RawArray(const RawArray&) = delete;
  RawArray& operator=(const RawArray&) = delete;

  template<typename... Args>
  void Emplace(Args&&... args) {
    if (length >= N) throw std::out_of_range("Exceeded capacity of RawArray.");
    new(&storage[length++]) T(std::forward<Args>(args)...);
  }

  void Push(const T& value) {
    if (length >= N) throw std::out_of_range("Exceeded capacity of RawArray.");
    new(&storage[length++]) T(value);
  }

  void Push(T&& value) {
    if (length >= N) throw std::out_of_range("Exceeded capacity of RawArray.");
    new(&storage[length++]) T(std::move(value));
  }

  // Iterator support
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
  iterator end() { return iterator{ptr(length)}; }
};
