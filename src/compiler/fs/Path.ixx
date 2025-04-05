export module fs.path;

import lexical.cursor.string;

import <string>;
import <string_view>;
import <vector>;
import <stdexcept>;
import <utility>;

namespace fs {
  export class URL;

  export class Path {
  private:
    const std::string path;
    std::vector<std::string_view> schemes;
    std::string_view drive;
    std::string_view pathname;
    std::vector<std::string_view> parts;
    std::string_view extension;
  private:
    friend class URL;

    std::string ToURLString() const;

    bool ParseDrive(lexical::cursor::String& cursor) {
      if (!cursor.IsAlpha()) return false;

      auto start = cursor.cbegin(); // Save the start position

      do {
        cursor.Advance(); // Consume valid characters
      } while (cursor.IsAlpha());

      if (cursor.Match(':')) {
        // Capture the drive
        drive = std::string_view{start, cursor.cbegin()};
        return true;
      } else {
        cursor.Retreat(start);
        return false;
      }
    }

    bool ParsePathname(lexical::cursor::String& cursor) {
      if (cursor.IsSlash()) {
        cursor.Advance(); // Consume the slash
      }

      auto start = cursor.cbegin(); // Save the initial start position

      while (!cursor.Done()) {
        auto part_start = cursor.cbegin(); // Save the start position of the part

        while (cursor.IsFilePath()) {
          cursor.Advance(); // Consume valid characters
        }

        std::string_view view{part_start, cursor.cbegin()};
        if (!parts.empty() && view == "..") {
          parts.pop_back();
        } else if (view != "." && !view.empty()) {
          // Ignore single dot and empty parts
          parts.push_back(view);
        }

        if (cursor.IsSlash()) {
          cursor.Advance(); // Consume the slash
        } else {
          break;
        }
      };

      // Capture the pathname
      pathname = std::string_view{start, cursor.cbegin()};
      return true;
    }

    bool ParseExtension() {
      if (parts.empty()) return false;

      std::string_view& last_part = parts.back();

      for (auto it = last_part.crbegin(); it != last_part.crend(); ++it) {
        if (*it == '.') {
          size_t pos = last_part.size() - 1 - std::distance(last_part.crbegin(), it);
          extension = last_part.substr(pos + 1); // Capture the extension without the dot
          last_part = last_part.substr(0, pos); // Shrink the last part to exclude the extension
          return true;
        }
      }

      return false;
    }

    void Parse();
  public:
    Path(const std::string& path) : path{path} {
      Parse();
    }

    Path(const std::string_view path) : path{path} {
      Parse();
    }

    Path(const URL& url);

    Path(const Path& other) = default;
    Path(Path&& other) noexcept = default;
    Path& operator=(const Path& other) = default;
    Path& operator=(Path&& other) noexcept = default;

    std::string_view Drive() const { return drive; }
    std::string_view Pathname() const { return pathname; }
    std::string_view Extension() const { return extension; }
    const std::string& ToString() const { return path; }

    bool IsRelative() const { return pathname.empty() ? false : pathname[0] == '.'; }

    Path Resolve(const Path& target) const;
  };
};