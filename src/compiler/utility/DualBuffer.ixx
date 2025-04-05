export module utility.dual_buffer;

import <vector>;
import <utility>;

namespace utility {
  export template <typename T>
  class DualBuffer {
    private:
      std::vector<T> active;
      std::vector<T> inactive;
      std::size_t index; // Index into the active buffer
    public:
      DualBuffer() : index(0) {}

      void Push(const T& element) { inactive.push_back(element); }
      void Push(T&& element) { inactive.push_back(std::move(element)); }

      T& Current() { return active[index]; }

      void Advance() { ++index; }

      void Reset() {
        index = 0;
        active.clear();
        active.swap(inactive);
      }

      std::size_t ActiveSize() const { return active.size(); }
      std::size_t InactiveSize() const { return inactive.size(); }
      bool AtEnd() const { return index >= active.size(); }
  };
}
