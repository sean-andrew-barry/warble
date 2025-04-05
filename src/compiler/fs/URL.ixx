export module fs.url;

import lexical.cursor.string;

import <string>;
import <string_view>;
import <stdexcept>;
import <vector>;
import <utility>;

namespace fs {
  export class Path;

  export class URL {
  private:
    const std::string url;
    std::vector<std::string_view> schemes;
    std::string_view protocol;
    std::string_view username;
    std::string_view password;
    std::string_view hostname;
    std::string_view port;
    std::string_view pathname;
    std::vector<std::string_view> parts;
    std::string_view search;
    std::vector<std::pair<std::string_view, std::string_view>> parameters;
    std::string_view hash;
  private:
    friend class Path;

    std::string ToPathString() const;

    std::string Decode(const std::string_view encoded) const {
      std::string decoded;
      decoded.reserve(encoded.size()); // Reserve enough space to avoid frequent reallocations

      // using IsHex = lexical::cursor::String::IsHex;

      for (size_t i = 0; i < encoded.size(); ++i) {
        if (encoded[i] == '%' && i + 2 < encoded.size() && lexical::cursor::String::IsHex(encoded[i + 1]) && lexical::cursor::String::IsHex(encoded[i + 2])) {
          // Decode percent-encoded character
          int value = (HexCharToInt(encoded[i + 1]) << 4) | HexCharToInt(encoded[i + 2]);
          decoded.push_back(static_cast<char>(value));
          i += 2;
        } else {
          decoded.push_back(encoded[i]);
        }
      }

      return decoded;
    }

    int HexCharToInt(char c) const {
      if (c >= '0' && c <= '9') {
        return c - '0';
      } else if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
      } else if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
      }

      return 0; // Invalid hex character, though IsHex should ensure this doesn't happen
    }

    bool ParseProtocol(lexical::cursor::String& cursor) {
      if (!cursor.IsAlpha()) return false;

      auto start = cursor.cbegin(); // Save the protocol's start position

      // If the first character in a scheme isn't alphabetical, it can't be a protocol
      while (cursor.IsAlpha()) {
        auto scheme_start = cursor.cbegin(); // Save the scheme's scheme_start position

        cursor.Advance(); // Consume the initial alphabetical character

        while (cursor.IsURLProtocol()) {
          cursor.Advance();
        }

        schemes.emplace_back(scheme_start, cursor.cbegin());

        if (!cursor.Match(':')) break; // No more schemes, exit the loop
      }

      // Check for the "//", but don't consume it yet
      if (cursor.Check("//")) {
        protocol = std::string_view{start, cursor.cbegin()}; // Capture the protocol
        cursor.Advance(2); // Advance past the "//"
        return true;
      }

      // If the expected ending syntax wasn't found, reset the cursor and the url string
      cursor.Retreat(start);
      schemes.clear();

      return false;
    }

    bool ParseUsername(lexical::cursor::String& cursor) {
      auto start = cursor.cbegin(); // Save the start position

      while (cursor.IsURLAuthority()) {
        cursor.Advance();
      }

      if (cursor.Match(':')) {
        username = std::string_view{start, cursor.cbegin()};
        return true;
      }

      // If the expected ending syntax wasn't found, reset the cursor and the url string
      cursor.Retreat(start);
      return false;
    }

    bool ParsePassword(lexical::cursor::String& cursor) {
      auto start = cursor.cbegin(); // Save the start position

      while (cursor.IsURLAuthority()) {
        cursor.Advance();
      }

      if (cursor.Match('@')) {
        password = std::string_view{start, cursor.cbegin()};
        return true;
      }

      // If the expected ending syntax wasn't found, reset the cursor and the url string
      cursor.Retreat(start);
      return false;
    }

    bool ParseHostname(lexical::cursor::String& cursor) {
      auto start = cursor.cbegin(); // Save the start position

      // Determine if it's an IPv6 address by checking for '['
      if (cursor.Match('[')) {
        while (cursor.IsHex() || cursor.Check(':')) {
          cursor.Advance(); // Consume valid IPv6 address characters
        }

        if (cursor.Match(']')) {
          hostname = std::string_view{start, cursor.cbegin()};
          return true;
        }

        // If the IPv6 address is not properly closed, reset the cursor and url string
        cursor.Retreat(start);
        return false;
      }

      // For domain names and IPv4 addresses
      while (cursor.IsURLHost()) {
        cursor.Advance(); // Consume valid domain or IPv4 address characters
      }

      // Capture the hostname
      hostname = std::string_view{start, cursor.cbegin()};
      return true;
    }

    bool ParsePort(lexical::cursor::String& cursor) {
      if (!cursor.Match(':')) return false;

      auto start = cursor.cbegin(); // Save the start position

      while (cursor.IsDigit()) {
        cursor.Advance();
      }

      port = std::string_view{start, cursor.cbegin()};
      return true;
    }

    bool ParsePathname(lexical::cursor::String& cursor) {
      // Exit early if it doesn't begin with the initial '/'
      if (!cursor.Check('/')) return false;

      auto start = cursor.cbegin(); // Save the initial start position

      while (cursor.Match('/')) {
        auto part_start = cursor.cbegin(); // Save the start position of the part

        while (cursor.IsURLPathname()) {
          cursor.Advance(); // Consume valid pathname characters
        }

        std::string_view view{part_start, cursor.cbegin()};
        if (!parts.empty() && view == "..") {
          parts.pop_back();
        } else if (view != "." && !view.empty()) {
          // Ignore single dot and empty parts
          parts.push_back(view);
        }
      }

      // Capture the pathname
      pathname = std::string_view{start, cursor.cbegin()};
      return true;
    }

    bool ParseSearch(lexical::cursor::String& cursor) {
      // Exit early if there's no initial '?'
      if (!cursor.Match('?')) return false;

      auto start = cursor.cbegin(); // Save the start position for the full search string

      while (!cursor.Done()) {
        auto key_start = cursor.cbegin(); // Save the start position for the key

        // Parse the key
        while (cursor.IsURLSearch()) {
          cursor.Advance(); // Consume valid key characters
        }

        std::string_view key{key_start, cursor.cbegin()}; // Capture the key

        // Check for the '=' separator
        if (cursor.Match('=')) {
          auto value_start = cursor.cbegin(); // Save the start position for the value

          // Parse the value
          while (cursor.IsURLSearch()) {
            cursor.Advance(); // Consume valid value characters
          }

          std::string_view value{value_start, cursor.cbegin()}; // Capture the value

          // Store the key-value pair in the parameters vector
          parameters.emplace_back(key, value);
        } else {
          parameters.emplace_back(key, "");
        }

        if (!cursor.Match('&')) break; // No more parameters, exit the loop
      }

      // Capture the full search string
      search = std::string_view{start, cursor.cbegin()};
      return true;
    }

    bool ParseHash(lexical::cursor::String& cursor) {
      // Return if there's no initial '#'
      if (!cursor.Match('#')) return false;

      auto start = cursor.cbegin(); // Save the start position

      while (cursor.IsURLHash()) {
        cursor.Advance(); // Consume valid hash characters
      }

      // Capture the hash
      hash = std::string_view{start, cursor.cbegin()};
      return true;
    }

    void Parse();
  public:
    URL(const std::string& url) : url{url} {
      Parse();
    }

    URL(const std::string_view url) : url{url} {
      Parse();
    }

    URL(const Path& path);

    URL(const URL& other) = default;
    URL(URL&& other) noexcept = default;
    URL& operator=(const URL& other) = default;
    URL& operator=(URL&& other) noexcept = default;

    std::string_view HRef() const { return {url.cbegin(), url.cend()}; }
    std::string_view Protocol() const { return protocol; }
    std::string_view Username() const { return username; }
    std::string_view Password() const { return password; }
    std::string_view Hostname() const { return hostname; }
    std::string_view Pathname() const { return pathname; }
    std::string_view Search() const { return search; }
    std::string_view Hash() const { return hash; }
    const std::string& ToString() const { return url; }

    bool IsRelative() const { return pathname.empty() ? false : pathname[0] == '.'; }
    bool IsFile() const { return HasScheme("file"); }
    bool IsBlob() const { return HasScheme("blob"); }
    bool IsHTTP() const { return HasScheme("http"); }
    bool IsHTTPS() const { return HasScheme("https"); }

    bool HasScheme(const std::string_view target) const {
      for (const auto& scheme : schemes) {
        if (scheme == target) return true;
      }

      return false;
    }

    URL Resolve(const URL& target) const {
      std::string resolved;
      // TODO: Possibly want to reserve some space for performance, though it isn't clear to me exactly how much should be reserved

      if (!protocol.empty()) {
        resolved += protocol;
        resolved += "//";
      }

      if (!username.empty()) {
        resolved += username;

        if (!password.empty()) {
          resolved += ':';
          resolved += password;
        }

        resolved += '@';
      }

      if (!hostname.empty()) {
        resolved += hostname;
      }

      auto helper = [](const URL& url, std::string& resolved, size_t start_size){
        for (const auto& part : url.parts) {
          if (part == "..") {
            // Pop characters to remove the last part
            while (resolved.size() > start_size) {
              auto c = resolved.back();
              resolved.pop_back();

              if (c == '/') {
                break;
              }
            }
          } else if (part != "." && !part.empty()) {
            resolved += '/';
            resolved += part;
          }
        }
      };

      auto start_size = resolved.size(); // Remember where the pathname begins so we don't erase into the hostname

      helper(*this, resolved, start_size);
      helper(target, resolved, start_size);

      if (!target.search.empty()) {
        resolved += '?';
        resolved += target.search;
      }

      if (!target.hash.empty()) {
        resolved += '#';
        resolved += target.hash;
      }
      
      return URL{resolved};
    }
  };
};

// // Example usage
// int main() {
//   try {
//     URL url("https://www.example.com/pathname?search=test#hash");
//     std::cout << "Protocol: " << url.protocol() << "\n";
//     std::cout << "Host: " << url.host() << "\n";
//     std::cout << "Pathname: " << url.pathname() << "\n";
//     std::cout << "Search: " << url.search() << "\n";
//     std::cout << "Hash: " << url.hash() << "\n";
//   } catch (const std::invalid_argument& e) {
//     std::cerr << e.what() << '\n';
//   }

//   return 0;
// }
