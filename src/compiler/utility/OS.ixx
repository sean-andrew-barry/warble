export module utility.os;

import utility.macros;

import <thread>;
import <memory>;
import <cstdint>;
import <vector>;

export namespace utility::OS {
  void Setup();

  consteval const char FilePathSeparator() {
    if constexpr (utility::WINDOWS) {
      return '\\';
    } else {
      return '/';
    }
  }

  enum class Priority {
    LOW,
    NORMAL,
    HIGH,
  };

  // Checks if the current OS supports setting thread priorities
  bool CanSetPriority();

  // Sets the priority of a thread
  bool SetThreadPriority(std::thread::native_handle_type handle, Priority priority);

  bool SetThreadPriorityLow(std::thread::native_handle_type handle);
  bool SetThreadPriorityNormal(std::thread::native_handle_type handle);
  bool SetThreadPriorityHigh(std::thread::native_handle_type handle);

  // Custom allocator for executable memory
  template<typename T>
  struct ExecutableMemoryAllocator {
    using value_type = T;

    ExecutableMemoryAllocator() = default;

    template<typename U>
    constexpr ExecutableMemoryAllocator(const ExecutableMemoryAllocator<U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n);

    void deallocate(T* p, std::size_t) noexcept;
  };

  template<typename T, typename U>
  bool operator==(const ExecutableMemoryAllocator<T>&, const ExecutableMemoryAllocator<U>&) { return true; }

  template<typename T, typename U>
  bool operator!=(const ExecutableMemoryAllocator<T>&, const ExecutableMemoryAllocator<U>&) { return false; }

  std::vector<std::byte, ExecutableMemoryAllocator<std::byte>> Allocate(std::size_t size);
  void Execute(std::vector<std::byte, ExecutableMemoryAllocator<std::byte>>& code);

  struct ExecutableMemoryDeleter {
    size_t size; // Added to keep track of the size for munmap in POSIX
    void operator()(void* ptr);
  };

  std::unique_ptr<void, ExecutableMemoryDeleter> AllocateExecutableMemory(size_t size);
  std::unique_ptr<void, ExecutableMemoryDeleter> AllocateExecutableMemory(const std::vector<std::byte>& data);
  extern "C" void ExecuteFunction(void* func);
  void Execute(std::unique_ptr<void, ExecutableMemoryDeleter> uptr);
};