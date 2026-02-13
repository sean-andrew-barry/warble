export module compiler.ir.Functions;

import <algorithm>;
import <cstdint>;
import <cstddef>;
import <bitset>;
import <vector>;
import compiler.ir.Function;
import compiler.ir.Symbol;

namespace compiler::ir {
  export class Functions {
  private:
    std::vector<std::bitset<256>> registers; // Virtual registers potentially clobbered by calling this function
    std::vector<ir::Symbol> symbols; // Symbol index (Kind::Function) for this function; sorted ascending by insertion
    std::vector<uint32_t> frame_sizes; // The minimum frame size in bytes
    std::vector<uint32_t> sub_frame_sizes; // Sub frame size in bytes
  public:
    Functions() = default;
    Functions(Functions&&) noexcept = default;
    Functions& operator=(Functions&&) noexcept = default;
    Functions(const Functions&) = delete;
    Functions& operator=(const Functions&) = delete;

    const std::bitset<256>& Registers(ir::Function i) const { return registers[i.Row()]; }
    ir::Symbol Symbol(ir::Function i) const { return symbols[i.Row()]; }
    uint32_t FrameSize(ir::Function i) const { return frame_sizes[i.Row()]; }
    uint32_t SubFrameSize(ir::Function i) const { return sub_frame_sizes[i.Row()]; }

    void Registers(ir::Function i, const std::bitset<256>& v) { registers[i.Row()] = v; }
    void Symbol(ir::Function i, ir::Symbol v) { symbols[i.Row()] = v; }
    void FrameSize(ir::Function i, uint32_t v) { frame_sizes[i.Row()] = v; }
    void SubFrameSize(ir::Function i, uint32_t v) { sub_frame_sizes[i.Row()] = v; }

    bool IsValid(ir::Function i) const {
      return i.Row() < Count();
    }

    // Returns the row that corresponds to the given function symbol index.
    // If not found, returns an invalid Function (row 0).
    ir::Function FindRow(ir::Symbol function) const {
      auto it = std::lower_bound(symbols.begin(), symbols.end(), function,
        [](const ir::Symbol a, const ir::Symbol b) {
          return a.Row() < b.Row();
        }
      );

      if (it != symbols.end() && *it == function) {
        return ir::Function{static_cast<uint32_t>(it - symbols.begin())};
      }

      return ir::Function{};
    }

    ir::Function Add(ir::Symbol function) {
      ir::Function row{static_cast<uint32_t>(symbols.size())};

      registers.emplace_back();
      symbols.push_back(function);
      frame_sizes.push_back(0);

      return row;
    }

    size_t Count() const { return symbols.size(); }

    void Resize(size_t n) {
      registers.resize(n);
      symbols.resize(n);
      frame_sizes.resize(n);
    }
  };
};
