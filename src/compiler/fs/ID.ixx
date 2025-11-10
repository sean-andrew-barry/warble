export module compiler.fs.ID;

import <array>;
import <cstddef>;
import <cstdint>;
import <functional>;

namespace compiler::fs {
  export class ID {
  public:
    static constexpr std::size_t StorageSize = 2;
    using Storage = std::array<std::uint64_t, StorageSize>;
  private:
    Storage data;
  public:
    ID() : data{} {}

    [[nodiscard]] const Storage& Data() const { return data; }
    Storage& Data() { return data; }
    
    [[nodiscard]] bool Valid() const {
      for (std::uint64_t value : data) {
        if (value != 0) {
          return true;
        }
      }
      return false;
    }

    friend bool operator==(const ID& lhs, const ID& rhs) = default;
  };
}

export namespace std {
  template<>
  struct hash<compiler::fs::ID> {
    size_t operator()(const compiler::fs::ID& id) const noexcept {
      constexpr std::uint64_t offset = 1469598103934665603ull; // FNV1a offset basis
      constexpr std::uint64_t prime = 1099511628211ull;        // FNV1a prime
      std::uint64_t value = offset;
      for (std::uint64_t chunk : id.Data()) {
        value ^= chunk;
        value *= prime;
      }
      value ^= static_cast<std::uint64_t>(id.Valid());
      value *= prime;
      return static_cast<size_t>(value);
    }
  };
}