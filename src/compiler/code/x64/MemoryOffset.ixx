export module code.x64.memory_offset;

import code.x64.operand;
import <cstdint>;
import <stdexcept>;
import <algorithm>;
import <concepts>;

export namespace code::x64 {
  template<typename T>
  class MemoryOffset : public Operand {
    T value;
  public:
    MemoryOffset(T val) : value{val} {}

    T Value() const { return value; }
  };

  struct MemoryOffset8 : public MemoryOffset<uint8_t> { using MemoryOffset::MemoryOffset; };
  struct MemoryOffset16 : public MemoryOffset<uint16_t> { using MemoryOffset::MemoryOffset; };
  struct MemoryOffset32 : public MemoryOffset<uint32_t> { using MemoryOffset::MemoryOffset; };
  struct MemoryOffset64 : public MemoryOffset<uint64_t> { using MemoryOffset::MemoryOffset; };

  template<typename T>
  concept IsMemoryOffset = std::same_as<MemoryOffset8, T> || std::same_as<MemoryOffset16, T> || std::same_as<MemoryOffset32, T> || std::same_as<MemoryOffset64, T>;
};