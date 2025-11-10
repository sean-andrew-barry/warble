export module compiler.utility.DualBuffer;

import <vector>;
import <utility>;

namespace compiler::utility {
  export template <typename T>
  class DualBuffer {
  private:
    std::vector<T> active;
    std::vector<T> inactive;
    std::size_t index; // Index into the active buffer
  public:
    DualBuffer() : index{0} {}

    void Reschedule(const T& element) { active.push_back(element); }
    void Reschedule(T&& element) { active.push_back(std::move(element)); }
    void Complete(const T& element) { inactive.push_back(element); }
    void Complete(T&& element) { inactive.push_back(std::move(element)); }

    T& Take() { return active[index++]; }

    void Reset() {
      index = 0;
      active.clear();
      active.swap(inactive);
    }

    bool Empty() {
      if (AtEnd()) {
        Reset();
      }
      
      return AtEnd();
    }

    std::size_t ActiveSize() const { return active.size(); }
    std::size_t InactiveSize() const { return inactive.size(); }
    bool AtEnd() const { return index >= active.size(); }
  };
}
