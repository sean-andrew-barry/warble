export module compiler.utility.FreeListAllocator;

import <cstdint>;
import <array>;
import <algorithm>;
import <bit>;
import <bitset>;
import <atomic>;

// Define the minimum and maximum block sizes, both must be powers of 2.
constexpr size_t MIN_BLOCK_SIZE = sizeof(void*); // minimum allocation size, power of 2
constexpr size_t MIN_BLOCK_WIDTH = std::bit_width(MIN_BLOCK_SIZE);
constexpr size_t MAX_BLOCK_SIZE = (1024 * 1024) * 64; // maximum allocation size, power of 2
constexpr size_t MAX_BLOCK_WIDTH = std::bit_width(MAX_BLOCK_SIZE);
constexpr size_t NUM_FREE_LISTS = MAX_BLOCK_WIDTH - MIN_BLOCK_WIDTH + 1;

export namespace compiler::utility {
  class FreeListAllocator {
  public:
    struct Block {
      Block* next; // Pointer to the next block in the free list.
    };
  private:
    std::array<Block*, NUM_FREE_LISTS> blocks;
    std::bitset<NUM_FREE_LISTS> map; // Bitmap to track which free lists have available blocks
    size_t total_bytes = 0; // The total number of bytes *belonging* to this allocator
  public:
  };
};