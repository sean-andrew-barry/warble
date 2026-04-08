export module compiler.utility.Macros;

import <cstdint>;
import <ranges>;

namespace compiler::utility::Macros {
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

  #if defined(_MSC_VER) && defined(_M_X64)
  export constexpr bool MSVC_X64 = true;
  export constexpr bool GNU = false;
  export constexpr bool CLANG = false;
  #elif defined(__GNUC__)
  export constexpr bool MSVC_X64 = false;
  export constexpr bool GNU = true;
  export constexpr bool CLANG = false;
  #elif defined(__clang__)
  export constexpr bool MSVC_X64 = false;
  export constexpr bool CLANG = true;
  #endif

  #if defined(__SIZEOF_INT128__)
  export constexpr bool HAS_INT128 = true;
  export using UInt128 = unsigned __int128;
  #else
  export constexpr bool HAS_INT128 = false;
  export using UInt128 = std::ranges::range_difference_t<std::ranges::iota_view<long long, long long>>;
  #endif
};