export module compiler.utility.OS;

import compiler.utility.Macros;

import <thread>;
import <string>;
import <memory>;
import <cstdint>;
import <vector>;
import <filesystem>;
import <array>;
import <cstddef>;

export namespace compiler::utility::OS {
  void Setup();

  consteval const char FilePathSeparator() {
    if constexpr (compiler::utility::Macros::WINDOWS) {
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

  // Platform-typed native handle used only through fs::File abstraction.
  #if defined(_WIN32) || defined(_WIN64)
    using NativeHandle = void*; // HANDLE
    constexpr NativeHandle InvalidNativeHandle = nullptr;
  #else
    using NativeHandle = int;   // POSIX file descriptor
    constexpr NativeHandle InvalidNativeHandle = -1;
  #endif

  // Open / close opaque native handle (implementation interprets platform specifics internally).
  NativeHandle OpenNativeFile(const std::filesystem::path& path);
  void CloseNativeFile(NativeHandle handle);
  // Compute file ID from an already-open native handle.
  bool GetFileID(NativeHandle handle, std::array<std::uint64_t, 2>& out_id);
  bool ReadFileToString(NativeHandle handle, std::string& out);
};