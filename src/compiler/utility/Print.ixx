export module compiler.utility.Print;

import compiler.text.Builder;
import compiler.utility.Macros;

import <mutex>;
import <atomic>;
import <iostream>;
import <source_location>;
import <utility>;
import <filesystem>;
import <stdexcept>;
import <stacktrace>;
import <iomanip>;
import <sstream>;

namespace compiler::utility {
  std::mutex cout_mutex;

  // Doesn't append a new line
  export void Write(auto&&... args) {
    compiler::text::Builder builder{std::forward<decltype(args)>(args)...};
    std::scoped_lock lock{cout_mutex};
    std::cout << builder.ToString();
    std::cout.flush();
  }

  export void Print(auto&&... args) {
    compiler::text::Builder builder{std::forward<decltype(args)>(args)...};
    std::scoped_lock lock{cout_mutex};
    std::cout << builder.ToString() << '\n';
    std::cout.flush();
  }

  export void Debug(auto&&... args) {
    if constexpr (!utility::Macros::DEBUGGING) return;

    // auto message = Merge(std::forward<decltype(args)>(args)...);
    compiler::text::Builder builder{std::forward<decltype(args)>(args)...};
    std::scoped_lock lock{cout_mutex};
    std::cout << builder.ToString() << '\n';
    std::cout.flush();
  }
}