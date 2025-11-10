import compiler.utility.OS;
import compiler.utility.Macros;
import <thread>;
import <iostream>;
import <vector>;
import <cstdint>;
import <memory>;
import <stdexcept>;
import <string>;
import <filesystem>;
import <limits>;
import <utility>;
import <cstring>;
import <array>;

namespace compiler::utility::OS {
  bool SetThreadPriorityLow(std::thread::native_handle_type handle) { return SetThreadPriority(handle, Priority::LOW); }
  bool SetThreadPriorityNormal(std::thread::native_handle_type handle) { return SetThreadPriority(handle, Priority::NORMAL); }
  bool SetThreadPriorityHigh(std::thread::native_handle_type handle) { return SetThreadPriority(handle, Priority::HIGH); }
};

// Windows Implementation
#if defined(_WIN32) || defined(_WIN64)
// Annoyingly these have to be #includes because the compiler won't allow the modules unless they are already imported from a module file.
#include <windows.h>

struct Setup {
  Setup() {
    ::SetConsoleOutputCP(CP_UTF8);
    ::SetConsoleCP(CP_UTF8);
    // std::cout << "Setting up UTF8" << std::endl;
  }
};

// Global static instance
static const Setup setup{};

namespace compiler::utility::OS {
  void Setup() {
    SetConsoleOutputCP(CP_UTF8);
    // std::cout << "Setting up UTF8" << std::endl;
  }

  bool CanSetPriority() {
    return true;
  }

  bool SetThreadPriority(std::thread::native_handle_type handle, utility::OS::Priority priority) {
    int winPriority;
    
    switch (priority) {
      case utility::OS::Priority::LOW: winPriority = THREAD_PRIORITY_BELOW_NORMAL; break;
      case utility::OS::Priority::NORMAL: winPriority = THREAD_PRIORITY_NORMAL; break;
      case utility::OS::Priority::HIGH: winPriority = THREAD_PRIORITY_ABOVE_NORMAL; break;
      default: return false;
    }

    return ::SetThreadPriority(handle, winPriority) != 0;
  }

  // template <typename T>
  // T* CustomAllocator<T>::allocate(std::size_t n) {
  //   if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
  //     throw std::bad_alloc();
  //   }
  //   return static_cast<T*>(::HeapAlloc(GetProcessHeap(), 0, n * sizeof(T)));
  // }

  // template <typename T>
  // void CustomAllocator<T>::deallocate(T* p, std::size_t) noexcept {
  //   ::HeapFree(GetProcessHeap(), 0, p);
  // }

  template<typename T>
  T* ExecutableMemoryAllocator<T>::allocate(std::size_t n) {
    std::size_t size = n * sizeof(T);
    void* ptr = ::VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!ptr) {
      throw std::bad_alloc();
    }
    return static_cast<T*>(ptr);
  }

  template<typename T>
  void ExecutableMemoryAllocator<T>::deallocate(T* ptr, std::size_t) noexcept {
    if (ptr) {
      ::VirtualFree(ptr, 0, MEM_RELEASE);
    }
  }

  // Explicitly instantiate the allocator for std::byte
  template struct ExecutableMemoryAllocator<std::byte>;

  std::vector<std::byte, ExecutableMemoryAllocator<std::byte>> Allocate(std::size_t size) {
    std::vector<std::byte, ExecutableMemoryAllocator<std::byte>> code(size);
    return code;
  }

  void Execute(std::vector<std::byte, ExecutableMemoryAllocator<std::byte>>& code) {
    // Change memory protection to executable only
    DWORD oldProtect;
    if (!::VirtualProtect(static_cast<void*>(code.data()), code.size(), PAGE_EXECUTE_READ, &oldProtect)) {
      throw std::runtime_error("Failed to change memory protection.");
    }

    ExecuteFunction(code.data());
  }

  void ExecutableMemoryDeleter::operator()(void* ptr) {
    if (ptr) {
      ::VirtualFree(ptr, 0, MEM_RELEASE);
    }
  }

  // Function to allocate executable memory and return a unique_ptr with custom deleter
  std::unique_ptr<void, ExecutableMemoryDeleter> AllocateExecutableMemory(size_t size) {
    void* ptr = ::VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (!ptr) {
      std::cerr << "Memory allocation failed." << std::endl;
      return nullptr;
    }

    return std::unique_ptr<void, ExecutableMemoryDeleter>{ptr, {size}};
  }

  std::unique_ptr<void, ExecutableMemoryDeleter> AllocateExecutableMemory(const std::vector<std::byte>& data) {
    auto executableMemory = AllocateExecutableMemory(data.size());
    std::memcpy(executableMemory.get(), data.data(), data.size());

    // Change memory protection to executable only
    DWORD oldProtect;
    ::VirtualProtect(executableMemory.get(), data.size(), PAGE_EXECUTE_READ, &oldProtect);

    return executableMemory;
  }

  // C function for executing the machine code with SEH
  extern "C" void ExecuteFunction(void* func) {
    __try {
      using FuncType = void(*)();
      FuncType function = reinterpret_cast<FuncType>(func);
      function();
      std::cout << "Executed successfully!" << std::endl;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      std::cerr << "Execution failed: Invalid machine code encountered." << std::endl;
    }
  }

  void Execute(std::unique_ptr<void, ExecutableMemoryDeleter> uptr) {
    ExecuteFunction(uptr.get());
  }

  // Open/close opaque native handles and compute ID from them (Windows)
  NativeHandle OpenNativeFile(const std::filesystem::path& path) {
    std::wstring wpath = path.wstring();
    HANDLE h = ::CreateFileW(
      wpath.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      nullptr,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_SEQUENTIAL_SCAN,
      nullptr
    );
    if (h == INVALID_HANDLE_VALUE) return InvalidNativeHandle;
    return reinterpret_cast<NativeHandle>(h);
  }

  void CloseNativeFile(NativeHandle handle) {
    if (handle != InvalidNativeHandle) {
      ::CloseHandle(static_cast<HANDLE>(handle));
    }
  }

  bool GetFileID(NativeHandle handle, std::array<std::uint64_t, 2>& out_id) {
    out_id.fill(0);
    if (handle == InvalidNativeHandle) {
      return false;
    }

    BY_HANDLE_FILE_INFORMATION info{};
    if (!::GetFileInformationByHandle(static_cast<HANDLE>(handle), &info)) {
      return false;
    }

    out_id[0] = static_cast<std::uint64_t>(info.dwVolumeSerialNumber);
    out_id[1] = (static_cast<std::uint64_t>(info.nFileIndexHigh) << 32) | static_cast<std::uint64_t>(info.nFileIndexLow);
    return true;
  }

  bool ReadFileToString(NativeHandle handle, std::string& out) {
    if (handle == InvalidNativeHandle) {
      return false;
    }

    HANDLE h = static_cast<HANDLE>(handle);

    LARGE_INTEGER size{};
    if (!::GetFileSizeEx(h, &size) || size.QuadPart < 0) {
      return false;
    }

    LARGE_INTEGER origin{};
    if (!::SetFilePointerEx(h, origin, nullptr, FILE_BEGIN)) {
      return false;
    }

    const auto file_size = static_cast<std::uint64_t>(size.QuadPart);
    if (file_size == 0) {
      out.clear();
      return true;
    }

    if (file_size > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
      return false;
    }

    const std::size_t target = static_cast<std::size_t>(file_size);
    std::string buffer(target, '\0');

    std::size_t total_read = 0;
    char* data = buffer.data();

    while (total_read < target) {
      std::size_t remaining = target - total_read;
      DWORD to_read = remaining > static_cast<std::size_t>(std::numeric_limits<DWORD>::max())
        ? std::numeric_limits<DWORD>::max()
        : static_cast<DWORD>(remaining);

      DWORD chunk_read = 0;
      if (!::ReadFile(h, data + total_read, to_read, &chunk_read, nullptr)) {
        ::SetFilePointerEx(h, origin, nullptr, FILE_BEGIN);
        return false;
      }

      if (chunk_read == 0) {
        buffer.resize(total_read);
        break;
      }

      total_read += chunk_read;
    }

    out = std::move(buffer);
    return true;
  }
};

// POSIX Implementation
#elif defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__MACH__)
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <csignal>
#include <cstring>
#include <setjmp.h>

namespace compiler::utility::OS {
  // Global jump buffer for signal handling
  jmp_buf jumpBuffer;

  void Setup() {
  }

  bool CanSetPriority() {
    // Simplification: assuming POSIX systems can set thread priority,
    // though real-world application may involve permissions and policies.
    return true;
  }

  bool SetThreadPriority(std::thread::native_handle_type handle, utility::OS::Priority priority) {
    int policy;
    sched_param param;
    if (pthread_getschedparam(handle, &policy, &param) != 0) {
      return false;
    }

    // Simplified approach: adjust priority within current policy's range
    int minPriority = sched_get_priority_min(policy);
    int maxPriority = sched_get_priority_max(policy);
    switch (priority) {
      case utility::OS::Priority::LOW: param.sched_priority = minPriority + (maxPriority - minPriority) / 3; break;
      case utility::OS::Priority::NORMAL: param.sched_priority = (minPriority + maxPriority) / 2; break;
      case utility::OS::Priority::HIGH: param.sched_priority = maxPriority - (maxPriority - minPriority) / 3; break;
      default: return false;
    }

    return pthread_setschedparam(handle, policy, &param) == 0;
  }

  template<typename T>
  T* ExecutableMemoryAllocator<T>::allocate(std::size_t n) {
    std::size_t size = n * sizeof(T);
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (ptr == MAP_FAILED) {
      throw std::bad_alloc();
    }
    return static_cast<T*>(ptr);
  }

  template<typename T>
  void ExecutableMemoryAllocator<T>::deallocate(T* ptr, std::size_t n) noexcept {
    if (ptr) {
      ::munmap(ptr, n * sizeof(T));
    }
  }

  // Explicitly instantiate the allocator for std::byte
  template struct ExecutableMemoryAllocator<std::byte>;

  std::vector<std::byte, ExecutableMemoryAllocator<std::byte>> Allocate(std::size_t size) {
    std::vector<std::byte, ExecutableMemoryAllocator<std::byte>> code(size);
    return code;
  }

  void Execute(std::vector<std::byte, ExecutableMemoryAllocator<std::byte>>& code) {
    // Change memory protection to executable only
    if (::mprotect(code.data(), code.size(), PROT_READ | PROT_EXEC) != 0) {
      throw std::runtime_error("Failed to change memory protection.");
    }

    using ExecuteFunctionType = void(*)();
    ExecuteFunctionType func = reinterpret_cast<ExecuteFunctionType>(code.data());

    // Setup signal handling
    struct sigaction sa;
    sa.sa_handler = [](int sig) { longjmp(jumpBuffer, sig); };
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);

    if (setjmp(jumpBuffer) == 0) {
      // Call the function
      func();
      std::cout << "Executed successfully!" << std::endl;
    } else {
      std::cerr << "Execution failed: Invalid machine code encountered." << std::endl;
    }
  }

  void ExecutableMemoryDeleter::operator()(void* ptr) {
    if (ptr) {
      ::munmap(ptr, size);
    }
  }

  // Allocate executable memory with a given size
  std::unique_ptr<void, ExecutableMemoryDeleter> AllocateExecutableMemory(size_t size) {
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);

    if (ptr == MAP_FAILED) {
      std::cerr << "Memory allocation failed." << std::endl;
      return nullptr;
    }

    return std::unique_ptr<void, ExecutableMemoryDeleter>{ptr, {size}};
  }

  // Allocate executable memory and initialize with data from a vector
  std::unique_ptr<void, ExecutableMemoryDeleter> AllocateExecutableMemory(const std::vector<std::byte>& data) {
    auto executableMemory = AllocateExecutableMemory(data.size());
    if (executableMemory) {
      std::memcpy(executableMemory.get(), data.data(), data.size());
      ::mprotect(executableMemory.get(), data.size(), PROT_READ | PROT_EXEC);
    }

    return executableMemory;
  }

  void Execute(std::unique_ptr<void, ExecutableMemoryDeleter> uptr) {
    using FuncType = void(*)();
    // Cast the allocated memory to the function pointer type
    FuncType func = reinterpret_cast<FuncType>(uptr.get());

    // Setup signal handling
    struct sigaction sa;
    sa.sa_handler = [](int sig) { longjmp(jumpBuffer, sig); };
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);

    if (setjmp(jumpBuffer) == 0) {
      // Call the function
      func();
      std::cout << "Executed successfully!" << std::endl;
    } else {
      std::cerr << "Execution failed: Invalid machine code encountered." << std::endl;
    }
  }

  // Open/close typed native handles and compute ID from them (POSIX)
  NativeHandle OpenNativeFile(const std::filesystem::path& path) {
    int fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) return InvalidNativeHandle;
    return fd;
  }

  void CloseNativeFile(NativeHandle handle) {
    if (handle != InvalidNativeHandle) {
      ::close(handle);
    }
  }

  bool GetFileID(NativeHandle handle, std::array<std::uint64_t, 2>& out_id) {
    out_id.fill(0);
    if (handle == InvalidNativeHandle) {
      return false;
    }

    struct stat st{};
    if (::fstat(handle, &st) != 0) {
      return false;
    }

    out_id[0] = static_cast<std::uint64_t>(st.st_dev);
    out_id[1] = static_cast<std::uint64_t>(st.st_ino);
    return true;
  }

  bool ReadFileToString(NativeHandle handle, std::string& out) {
    if (handle == InvalidNativeHandle) {
      return false;
    }

    struct stat st{};
    if (::fstat(handle, &st) != 0 || st.st_size < 0) {
      return false;
    }

    if (::lseek(handle, 0, SEEK_SET) == -1) {
      return false;
    }

    const auto file_size = static_cast<std::uint64_t>(st.st_size);
    if (file_size == 0) {
      out.clear();
      return true;
    }

    if (file_size > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
      return false;
    }

#if defined(POSIX_FADV_SEQUENTIAL)
    ::posix_fadvise(handle, 0, 0, POSIX_FADV_SEQUENTIAL);
#endif

    const std::size_t target = static_cast<std::size_t>(file_size);
    std::string buffer(target, '\0');
    std::size_t total_read = 0;
    char* data = buffer.data();

    while (total_read < target) {
      ssize_t chunk = ::read(handle, data + total_read, target - total_read);
      if (chunk < 0) {
        if (errno == EINTR) {
          continue;
        }
        ::lseek(handle, 0, SEEK_SET);
        return false;
      }

      if (chunk == 0) {
        buffer.resize(total_read);
        break;
      }

      total_read += static_cast<std::size_t>(chunk);
    }

    out = std::move(buffer);
    return true;
  }
};

#endif