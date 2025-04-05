export module code.flags;

import code.flag;

namespace code {
  export struct Flags {
    // Arithmetic Operations
    static inline constexpr Flag ADD{0};
    static inline constexpr Flag SUBTRACT{1};
    static inline constexpr Flag MULTIPLY{2};
    static inline constexpr Flag DIVIDE{3};
    static inline constexpr Flag INCREMENT{4}; // A hint to use a specialized instruction instead of ADD
    static inline constexpr Flag DECREMENT{5}; // A hint to use a specialized instruction instead of SUBTRACT

    // Logical Operations
    static inline constexpr Flag EQUAL{6};
    static inline constexpr Flag GREATER{7};
    static inline constexpr Flag LESSER{8};
    static inline constexpr Flag AND{9};
    static inline constexpr Flag OR{10};
    static inline constexpr Flag XOR{11};
    static inline constexpr Flag NOT{12};

    // Data Movement
    static inline constexpr Flag LOAD{13}; // Will read from memory
    static inline constexpr Flag STORE{14}; // Will write to memory
    static inline constexpr Flag EXCHANGE{15}; // Will swap data between two locations
    static inline constexpr Flag PREFETCH{16}; // An optimization hint

    // Control Flow
    static inline constexpr Flag JUMP{17};
    static inline constexpr Flag CALL{18};
    static inline constexpr Flag NEAR{19}; // Used in addressing to specify it only expects short distances, usually 8 bit
    static inline constexpr Flag FAR{20}; // The opposite of NEAR, used to specify it expects long distances
    static inline constexpr Flag PREDICATED{21}; // Only executes if a specified condition is true

    // Stack Operations
    static inline constexpr Flag PUSH{22};
    static inline constexpr Flag POP{23};
    static inline constexpr Flag SPILLED{24};

    // Register Roles
    static inline constexpr Flag RBP{25}; // Relative to the base pointer
    static inline constexpr Flag RIP{26}; // Relative to the instruction pointer (AKA program counter)
    static inline constexpr Flag RSP{27}; // Relative to the stack pointer
    static inline constexpr Flag RPP{28}; // Relative to a parameter pointer
    static inline constexpr Flag RAP{29}; // Relative to an array pointer
    static inline constexpr Flag ROP{30}; // Relative to an object pointer

    // Data Types
    static inline constexpr Flag CONSTANT{31};
    static inline constexpr Flag MUTABLE{32};
    static inline constexpr Flag SIGNED{33};
    static inline constexpr Flag UNSIGNED{34};
    static inline constexpr Flag FLOATING{35};

    // Atomic Operations
    static inline constexpr Flag ATOMIC{36};

    // Casting operations
    static inline constexpr Flag EXPAND{37}; // Needs to be up-casted
    static inline constexpr Flag SHRINK{38}; // Needs to be down-casted

    // Special Operations
    static inline constexpr Flag VECTOR{39}; // For SIMD/vector operations
    static inline constexpr Flag NON_TEMPORAL{40}; // For bypassing the cache hierarchy
    static inline constexpr Flag TEMPORARY{41}; // For temporary values
    static inline constexpr Flag SYSTEM{42}; // For system-related operations
    static inline constexpr Flag INTERRUPT{43}; // For interrupt handling
    static inline constexpr Flag COHERENT{44}; // For cache coherency operations
    static inline constexpr Flag VOLATILE{45}; // For volatile memory accesses

    // Security and Performance
    static inline constexpr Flag SECURE{46}; // For secure operations (e.g., ARM TrustZone)
    static inline constexpr Flag ACCELERATED{47}; // For hardware acceleration (e.g., GPU, DSP)

    // Memory Access
    static inline constexpr Flag ALIGNED{48}; // For aligned memory accesses
    static inline constexpr Flag UNALIGNED{49}; // For unaligned memory accesses
    static inline constexpr Flag IMMEDIATE{50}; // An optimization hint that a value should be embedded
    static inline constexpr Flag LOCKED{51}; // For locked memory accesses

    // Sizes
    static inline constexpr Flag SIZE8{52};
    static inline constexpr Flag SIZE16{53};
    static inline constexpr Flag SIZE32{54};
    static inline constexpr Flag SIZE64{55};
    static inline constexpr Flag SIZE128{56};
    static inline constexpr Flag SIZE256{57};
    static inline constexpr Flag SIZE512{58};

    // Misc
    static inline constexpr Flag EXTENDED{59};
    static inline constexpr Flag SIMD{60};
  };
};