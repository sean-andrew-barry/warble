export module code.state;

import code.frame;

import <cstdint>;
import <limits>;
import <span>;
import <bit>;

namespace code {
  // The State is a simple wrapper around a Frame that forwards operations to it
  // optionally using a specific index
  export class State {
  private:
    code::Frame& frame;
    uint64_t index{std::numeric_limits<uint64_t>::max()};
  public:
    State(code::Frame& frame) : frame{frame} {}
    State(const State& other) : frame{other.frame}, index{frame.Index()} {}

    bool operator==(code::Frame& other) const { return &frame == &other; }
    bool operator!=(code::Frame& other) const { return &frame != &other; }
    bool IsIndexed() const { return index != std::numeric_limits<uint64_t>::max(); }

    code::Frame& Frame() { return frame; }

    void Emit(std::byte byte) {
      if (IsIndexed()) {
        frame.Emit(byte, index);
      } else {
        frame.Emit(byte);
      }
    }

    template<size_t S>
    void Emit(std::span<std::byte, S> bytes, std::endian endian) {
      if (IsIndexed()) {
        frame.Emit(bytes, endian, index);
      } else {
        frame.Emit(bytes, endian);
      }
    }
  };
};