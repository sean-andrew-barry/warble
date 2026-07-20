#pragma once

#include <cstdint>
#include <ranges>
namespace compiler::utility::Macros {
  #ifndef NDEBUG
  constexpr bool DEBUGGING = true;
  #else
  constexpr bool DEBUGGING = false;
  #endif

  #if defined(_WIN32) || defined(_WIN64)
  constexpr bool WINDOWS = true;
  constexpr bool UNIX = false;
  #elif defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__MACH__)
  constexpr bool WINDOWS = false;
  constexpr bool UNIX = true;
  #endif

  #if defined(_MSC_VER) && defined(_M_X64)
  constexpr bool MSVC_X64 = true;
  constexpr bool GNU = false;
  constexpr bool CLANG = false;
  #elif defined(__GNUC__)
  constexpr bool MSVC_X64 = false;
  constexpr bool GNU = true;
  constexpr bool CLANG = false;
  #elif defined(__clang__)
  constexpr bool MSVC_X64 = false;
  constexpr bool CLANG = true;
  #endif

  #if defined(__SIZEOF_INT128__)
  constexpr bool HAS_INT128 = true;
  using UInt128 = unsigned __int128;
  #else
  constexpr bool HAS_INT128 = false;
  using UInt128 = std::ranges::range_difference_t<std::ranges::iota_view<long long, long long>>;
  #endif
};