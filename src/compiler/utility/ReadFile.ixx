export module utility.read_file;

import utility.print;
import <string>;
import <filesystem>;
import <fstream>;

namespace utility {
  export std::string ReadFile(const std::string& path) {
    std::ifstream file{path, std::ios::ate | std::ios::binary};

    if (file.is_open()) {
      std::string content{};
      content.resize(static_cast<size_t>(file.tellg()));

      file.seekg(0);
      file.read(content.data(), content.size());
      file.close();

      return content;
    }
    else {
      throw utility::Error("Failed to open file at", path);
    }
  }
};