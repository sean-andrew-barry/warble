import compiler.fs.Url;
import compiler.fs.Path;
import compiler.utility.Print;
import compiler.utility.OS;

import <string>;
import <stdexcept>;

namespace compiler::fs {
  std::string URL::ToPathString() const {
    if (!IsFile()) {
      throw std::invalid_argument("Only file URLs can be converted to file paths");
    }

    std::string path;
    for (const auto& part : parts) {
      if (!path.empty()) path += utility::OS::FilePathSeparator();
      path += Decode(part);
    }

    return path;
  }

  void URL::Parse() {
    lexical::cursor::String cursor{url};

    // If it begins with a '.' then it is relative and doesn't have a protocol, username, password, or host
    if (!cursor.Check('.')) {
      if (ParseProtocol(cursor)) {
        if (ParseUsername(cursor)) {
          ParsePassword(cursor);
        }
      }

      if (ParseHostname(cursor)) {
        ParsePort(cursor);
      }
    }

    ParsePathname(cursor);
    ParseSearch(cursor);
    ParseHash(cursor);

    if (!cursor.Done()) {
      throw utility::Error("Failed to parse URL string", url);
    }
  }

  URL::URL(const Path& path) : url{path.ToURLString()} {
    Parse();
  }
};