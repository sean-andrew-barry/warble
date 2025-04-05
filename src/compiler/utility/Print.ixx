export module utility.print;

import utility.to_string;
import utility.macros;
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

namespace utility {
  std::mutex cout_mutex;
  std::atomic<int> indent_count{0};
  // static thread_local int indent_count{0};
  std::source_location last_source = std::source_location::current();

  std::string simplify_path(const std::string& full_path) {
    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path full_path_obj{full_path};

    // Obtain the relative path with respect to the current directory
    std::filesystem::path relative_path = std::filesystem::relative(full_path_obj, cwd);

    return relative_path.string(); // Convert the path object to a string
  }

  export struct Indent {
    Indent() {
      ++indent_count;
    }

    ~Indent() {
      --indent_count;
    }
  };

  void IndentHelper(std::string& output) {
    // auto count = indent_count.load();
    auto count = indent_count.load();
    while (count > 0) {
      output += "  ";
      --count;
    }
  }

  template<typename... Args>
  constexpr std::string SimpleHelper(Args&&... args) {
    std::string output;
    output.reserve(200); // Reserve an estimated amount of space to avoid multiple reallocations

    IndentHelper(output);

    // Append each argument
    (output.append(utility::ToString(std::forward<Args>(args)) + ' '), ...);

    return output;
  }

  constexpr std::string Merge(auto&&... args) {
    std::string output;
    output.reserve(sizeof...(args) * 5); // Reserve an estimated amount of space to avoid multiple reallocations

    IndentHelper(output);

    // Append each argument
    (output.append(utility::ToString(std::forward<decltype(args)>(args)) + ' '), ...);

    return output;
  }

  template<typename... Args>
  constexpr std::string Helper(const std::source_location& location, Args&&... args) {
    std::lock_guard<std::mutex> lock{cout_mutex};

    std::string output;
    output.reserve(200); // Reserve an estimated amount of space to avoid multiple reallocations
    
    if (
      last_source.line() != location.line() ||
      last_source.column() != location.column() ||
      last_source.file_name() != location.file_name()
    ) {
      output += '\n';
      output += simplify_path(location.file_name()) + '(' +
                std::to_string(location.line()) + ':' +
                std::to_string(location.column()) + "):\n";
    }


    last_source = location;

    IndentHelper(output);

    // Append each argument
    (output.append(utility::ToString(std::forward<Args>(args)) + ' '), ...);

    return output;
  }

  // export template<typename... Args>
  // struct Print {
  //   constexpr explicit Print(Args&&... args) {
  //     std::cout << SimpleHelper(std::forward<Args>(args)...) + '\n';
  //   }
  // };

  // export template<typename... Args>
  // Print(Args&&...) -> Print<Args...>;

  // Doesn't append a new line
  export void Write(auto&&... args) {
    std::cout << Merge(std::forward<decltype(args)>(args)...);
  }

  export void Print(auto&&... args) {
    std::cout << Merge(std::forward<decltype(args)>(args)...) + '\n';
  }

  export std::string Join(auto&&... args) {
    std::string output;
    output.reserve(sizeof...(args) * 5); // Reserve an estimated amount of space to avoid multiple reallocations

    // Append each argument
    (output.append(utility::ToString(std::forward<decltype(args)>(args))), ...);

    return output;
  }

  export std::string Line(auto&&... args) {
    std::string output;
    output.reserve(sizeof...(args) * 5); // Reserve an estimated amount of space to avoid multiple reallocations

    IndentHelper(output);
    
    // Append each argument
    (output.append(utility::ToString(std::forward<decltype(args)>(args))), ...);

    output += '\n';

    return output;
  }

  export std::string JoinWith(std::string separator, auto&&... args) {
    std::string output;
    output.reserve(sizeof...(args) * 5); // Reserve an estimated amount of space to avoid multiple reallocations

    // Append each argument
    (output.append(utility::ToString(std::forward<decltype(args)>(args)) + separator), ...);

    return output;
  }

  // export template<typename... Args>
  // struct Print {
  //   constexpr explicit Print(Args&&... args, std::source_location location = std::source_location::current()) {
  //     std::cout << Helper(location, std::forward<Args>(args)...) + '\n';
  //   }
  // };

  // export template<typename... Args>
  // Print(Args&&...) -> Print<Args...>;

  // export template<typename... Args>
  // struct Log {
  //   constexpr explicit Log(Args&&... args) {
  //     std::cout << SimpleHelper(std::forward<Args>(args)...) + '\n';
  //   }
  // };

  // export template<typename... Args>
  // Log(Args&&...) -> Log<Args...>;

  // export template<typename... Args>
  // struct Debug {
  //   constexpr Debug(Args&&... args, std::source_location location = std::source_location::current()) {
  //     if constexpr (!utility::DEBUGGING) return;

  //     std::cout << Helper(location, std::forward<Args>(args)...) + '\n';
  //   }
  // };

  // export template<typename... Args>
  // Debug(Args&&...) -> Debug<Args...>;

  export void Debug(auto&&... args) {
    if constexpr (!utility::DEBUGGING) return;

    std::cout << Merge(std::forward<decltype(args)>(args)...) + '\n';
  }

  // export template<typename... Args>
  // struct Error : public std::runtime_error {
  // private:
  //   std::stacktrace trace;
  // public:
  //   // constexpr Error(Args&&... args)
  //   // {
  //   // }
  //   constexpr Error(Args&&... args, std::source_location location = std::source_location::current(), std::stacktrace trace = std::stacktrace::current())
  //     : std::runtime_error{Helper(location, std::forward<Args>(args)...)}, trace{trace}
  //   {
  //     // throw std::runtime_error{Helper(location, std::forward<Args>(args)...)};
  //   }
  // };

  // export template<typename... Args>
  // Error(Args&&...) -> Error<Args...>;

  export std::runtime_error Error(auto&&... args) {
    return std::runtime_error{JoinWith(" ", std::forward<decltype(args)>(args)...)};
  }

  export class Quoted {
  private:
    std::string output;
    bool first{true};
  private:
    void Escape(auto&& value) {
      if (first) first = false;
      else output += ' ';

      for (const char c : utility::ToString(value)) {
        switch (c) {
          case '\n': {
            output += "\\n";
            break;
          }
          case '\r': {
            output += "\\r";
            break;
          }
          case '\t': {
            output += "\\t";
            break;
          }
          case '\b': {
            output += "\\b";
            break;
          }
          case '\f': {
            output += "\\f";
            break;
          }
          case '"': {
            output += "\\\"";
            break;
          }
          default: {
            output += c;
          }
        }
      }
    }
  public:
    constexpr explicit Quoted(auto&&... args) {
      output += '"';
      (Escape(std::forward<decltype(args)>(args)), ...);
      output += '"';
    }

    std::string ToString() const { return output; }
  };
};