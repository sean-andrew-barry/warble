export module code.cost;

import <cstdint>;
import <limits>;
import <bitset>;

namespace code {
  export class Cost {
  private:
    std::bitset<64> boundaries;
    float throughput{1.0}; // The number of instructions that can be executed per cycle
    uint8_t min_latency{1}; // The number of cycles the instruction takes to complete
    uint8_t max_latency{1}; // The number of cycles the instruction takes to complete
    uint8_t uops{1}; // Number of micro-operations
    std::bitset<8> flags;
  public:
    static constexpr std::bitset<8> ALU{0x01}; // Arithmetic logic unit
    static constexpr std::bitset<8> AGU{0x02}; // Address generation unit
    static constexpr std::bitset<8> LSU{0x04}; // Load/Store unit
    static constexpr std::bitset<8> FADD{0x08}; // Floating-point addition
    static constexpr std::bitset<8> FMUL{0x10}; // Floating-point multiplication
    static constexpr std::bitset<8> FMISC{0x20}; // Floating Point Miscellaneous unit
    static constexpr std::bitset<8> LOCK{0x40}; // Instruction uses locking
    static constexpr std::bitset<8> BRANCH{0x80}; // Instruction involves branch prediction
    static constexpr std::bitset<8> FPU{FADD | FMUL | FMISC}; // Any of the floating-point units
    static constexpr std::bitset<8> FANY{0}; // IDK how to represent this yet...
    static constexpr std::bitset<8> UNKNOWN{0}; // Means I have no clue
  public:
    constexpr Cost() = default;
    
    constexpr Cost(float throughput, uint8_t min_latency, uint8_t max_latency, uint8_t uops)
      : throughput{throughput}, min_latency{min_latency}, max_latency{max_latency}, uops{uops} {}

    constexpr Cost(float throughput, uint8_t min_latency, uint8_t max_latency, uint8_t uops, std::bitset<8> flags)
      : throughput{throughput}, min_latency{min_latency}, max_latency{max_latency}, uops{uops}, flags{flags} {}

    constexpr Cost(const Cost& other) = default;
    constexpr Cost& operator=(const Cost& other) = default;

    constexpr void Throughput(float v) { throughput = v; }
    constexpr float Throughput() const { return throughput; }

    constexpr void MinLatency(uint8_t v) { min_latency = v; }
    constexpr uint8_t MinLatency() const { return min_latency; }

    constexpr void MaxLatency(uint8_t v) { max_latency = v; }
    constexpr uint8_t MaxLatency() const { return max_latency; }

    // A helper to set both at once, since often they are the same.
    constexpr void Latency(uint8_t v) { MinLatency(v); MaxLatency(v); }

    // constexpr void SetALU(bool v) { flags.set(static_cast<uint8_t>(Flags::ALU), v); }
    // constexpr void SetLSU(bool v) { flags.set(static_cast<uint8_t>(Flags::LSU), v); }
    // constexpr void SetFPU(bool v) { flags.set(static_cast<uint8_t>(Flags::FPU), v); }
    // constexpr void SetBPU(bool v) { flags.set(static_cast<uint8_t>(Flags::BPU), v); }
    // constexpr void SetSIMD(bool v) { flags.set(static_cast<uint8_t>(Flags::SIMD), v); }
    // constexpr void SetLOCK(bool v) { flags.set(static_cast<uint8_t>(Flags::LOCK), v); }

    // constexpr bool IsALU() const { return flags.test(static_cast<uint8_t>(Flags::ALU)); }
    // constexpr bool IsLSU() const { return flags.test(static_cast<uint8_t>(Flags::LSU)); }
    // constexpr bool IsFPU() const { return flags.test(static_cast<uint8_t>(Flags::FPU)); }
    // constexpr bool IsBPU() const { return flags.test(static_cast<uint8_t>(Flags::BPU)); }
    // constexpr bool IsSIMD() const { return flags.test(static_cast<uint8_t>(Flags::SIMD)); }
    // constexpr bool IsLOCK() const { return flags.test(static_cast<uint8_t>(Flags::LOCK)); }
  };
};