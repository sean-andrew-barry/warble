export module compiler.utility.SuffixAutomaton;

import <cstddef>;
import <functional>;
import <iterator>;
import <unordered_map>;
import <vector>;
import <concepts>;

namespace compiler::utility {
  export template<typename Symbol>
  class SuffixAutomaton {
  public:
    static_assert(std::default_initializable<Symbol>, "Symbol must be default-constructible");
    static_assert(std::copy_constructible<Symbol>, "Symbol must be copy-constructible");
    static_assert(
      requires(Symbol value) {
        { std::hash<Symbol>{}(value) } -> std::convertible_to<std::size_t>;
      },
      "std::hash<Symbol> must return std::size_t"
    );

    struct State {
      int len = 0;                 // Length of the longest string in this state's class
      int link = -1;               // Suffix link
      int first_pos = -1;          // End position of the representative substring
      std::unordered_map<Symbol, int> next; // Transitions
    };
  private:
    std::vector<State> states;
    int last_state;
    std::size_t text_length;

    [[nodiscard]] bool HasTransition(int state_index, const Symbol& symbol) const {
      return states[state_index].next.contains(symbol);
    }
  public:
    explicit SuffixAutomaton(std::size_t reserve_states = 0) : states{}, last_state{0}, text_length{0} {
      states.reserve(reserve_states > 0 ? reserve_states : 2ULL);
      states.emplace_back();
      states[0].len = 0;
      states[0].link = -1;
      states[0].first_pos = -1;
    }

    /// Number of symbols processed so far (length of the logical text).
    [[nodiscard]] std::size_t Length() const noexcept { return text_length; }

    /// Access to states for additional analysis.
    [[nodiscard]] const std::vector<State>& GetStates() const noexcept { return states; }

    /// Extend the automaton by one symbol appended to the text.
    void Extend(const Symbol& symbol) {
      const int current = static_cast<int>(states.size());
      states.emplace_back();

      auto& current_state = states[current];
      current_state.len = states[last_state].len + 1;
      current_state.first_pos = static_cast<int>(text_length);
      ++text_length;

      int previous = last_state;
      while (previous != -1 && !HasTransition(previous, symbol)) {
        states[previous].next[symbol] = current;
        previous = states[previous].link;
      }

      if (previous == -1) {
        current_state.link = 0;
      } else {
        const int target = states[previous].next[symbol];
        if (states[previous].len + 1 == states[target].len) {
          current_state.link = target;
        } else {
          const int clone = static_cast<int>(states.size());
          states.push_back(states[target]);
          auto& clone_state = states[clone];
          clone_state.len = states[previous].len + 1;

          while (previous != -1 && states[previous].next[symbol] == target) {
            states[previous].next[symbol] = clone;
            previous = states[previous].link;
          }

          states[target].link = clone;
          current_state.link = clone;
        }
      }

      last_state = current;
    }

    /// Check whether the subsequence [first, last) appears in the text.
    template<std::input_iterator InputIt>
    bool Find(InputIt first, InputIt last, std::size_t& out_start, std::size_t& out_end) const {
      int state_index = 0;
      std::size_t pattern_length = 0;

      for (auto it = first; it != last; ++it) {
        const Symbol& symbol = *it;
        auto transition = states[state_index].next.find(symbol);
        if (transition == states[state_index].next.end()) {
            return false;
        }
        state_index = transition->second;
        ++pattern_length;
      }

      if (pattern_length == 0) {
        return false; // Treat empty pattern as "not found"
      }

      const int end_position = states[state_index].first_pos;
      const std::size_t end = static_cast<std::size_t>(end_position);
      const std::size_t start = end + 1 - pattern_length;

      out_start = start;
      out_end = end;
      return true;
    }

    /// Convenience overload: check containment without returning indices.
    template<std::input_iterator InputIt>
    bool Contains(InputIt first, InputIt last) const {
      std::size_t start = 0;
      std::size_t end = 0;
      return Find(first, last, start, end);
    }
  };
}
