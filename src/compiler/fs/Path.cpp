import fs.path;
import fs.url;
import utility.print;
import utility.os;

import <string>;

namespace fs {
  std::string Path::ToURLString() const {
    std::string url;
    url += "file://";

    if (!drive.empty()) {
      url += '/';
      url += drive;
    }

    for (const auto& part : parts) {
      url += '/';
      url += part;
    }

    if (!extension.empty()) {
      url += '.';
      url += extension;
    }

    return url;
  }

  void Path::Parse() {
    lexical::cursor::String cursor{path};

    // If it begins with a '.' then it is relative and doesn't have a drive
    if (!cursor.Check('.')) {
      ParseDrive(cursor);
    }

    ParsePathname(cursor);
    ParseExtension();

    if (!cursor.Done()) {
      throw utility::Error("Failed to parse path string", path);
    }
  }

  Path::Path(const URL& url) : path{url.ToPathString()} {
    Parse();
  }

  Path Path::Resolve(const Path& target) const {
    std::string resolved;
    // TODO: Possibly want to reserve some space for performance, though it isn't clear to me exactly how much should be reserved

    if (!drive.empty()) {
      resolved += drive;
    }

    auto helper = [](const Path& path, std::string& resolved, size_t start_size){
      for (const auto& part : path.parts) {
        if (part == "..") {
          // Pop characters to remove the last part
          while (resolved.size() > start_size) {
            auto c = resolved.back();
            resolved.pop_back();

            if (c == '/' || c == '\\') {
              break;
            }
          }
        } else if (part != "." && !part.empty()) {
          resolved += utility::OS::FilePathSeparator();
          resolved += part;
        }
      }
    };

    auto start_size = resolved.size(); // Remember where the pathname begins so we don't erase into the drive

    helper(*this, resolved, start_size);
    helper(target, resolved, start_size);

    if (!target.extension.empty()) {
      resolved += target.extension;
    }
    
    return Path{resolved};
  }
};