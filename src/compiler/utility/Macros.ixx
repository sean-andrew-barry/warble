export module utility.macros;

namespace utility {
  #ifndef NDEBUG
  export constexpr bool DEBUGGING = true;
  #else
  export constexpr bool DEBUGGING = false;
  #endif

  #if defined(_WIN32) || defined(_WIN64)
  export constexpr bool WINDOWS = true;
  export constexpr bool UNIX = false;
  #elif defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__MACH__)
  export constexpr bool WINDOWS = false;
  export constexpr bool UNIX = true;
  #endif
};