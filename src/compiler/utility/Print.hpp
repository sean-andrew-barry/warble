#pragma once

#include "compiler/text/Builder.hpp"
#include "compiler/utility/Macros.hpp"
#include <mutex>
#include <atomic>
#include <iostream>
#include <source_location>
#include <utility>
#include <filesystem>
#include <stdexcept>
#include <stacktrace>
#include <iomanip>
#include <sstream>
namespace compiler::utility {
  inline std::mutex cout_mutex;

  // Doesn't append a new line
  void Write(auto&&... args) {
    compiler::text::Builder builder{std::forward<decltype(args)>(args)...};
    std::scoped_lock lock{cout_mutex};
    std::cout << builder.ToString();
    std::cout.flush();
  }

  void Print(auto&&... args) {
    compiler::text::Builder builder{std::forward<decltype(args)>(args)...};
    std::scoped_lock lock{cout_mutex};
    std::cout << builder.ToString() << '\n';
    std::cout.flush();
  }

  auto Error(auto&&... args) {
    compiler::text::Builder builder{std::forward<decltype(args)>(args)...};
    std::scoped_lock lock{cout_mutex};
    return std::runtime_error(builder.ToString());
  }

  void Debug(auto&&... args) {
    if constexpr (!utility::Macros::DEBUGGING) return;

    // auto message = Merge(std::forward<decltype(args)>(args)...);
    compiler::text::Builder builder{std::forward<decltype(args)>(args)...};
    std::scoped_lock lock{cout_mutex};
    std::cout << builder.ToString() << '\n';
    std::cout.flush();
  }
}