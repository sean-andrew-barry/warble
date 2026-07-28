#include <bit>
#include <cassert>
#include <cstddef>
#include <new>
#include <vector>
#include <iostream>
#include <array>
#include <chrono>
#include <random>
#include <algorithm>

// Define the minimum and maximum block sizes, both must be powers of 2.
constexpr size_t MIN_BLOCK_SIZE = 8;  // minimum allocation size, power of 2
constexpr size_t MIN_BLOCK_WIDTH = std::bit_width(MIN_BLOCK_SIZE);
constexpr size_t MAX_BLOCK_SIZE = 1024 * 1024; // maximum allocation size, power of 2
constexpr size_t MAX_BLOCK_WIDTH = std::bit_width(MAX_BLOCK_SIZE);
constexpr size_t NUM_FREE_LISTS = MAX_BLOCK_WIDTH - MIN_BLOCK_WIDTH + 1;

// Structure representing a block in the free list.
struct FreeBlock {
  FreeBlock* next;  // Pointer to the next block in the free list.
};

class FreeListAllocator {
public:
  FreeListAllocator() {
    // Initialize all free list heads to nullptr.
    for (auto &head : freeLists) {
      head = nullptr;
    }
  }

  ~FreeListAllocator() {
    // Temporarily disabling this function, don't delete it, we'll use it later
    
    std::cout << "Destructor: Starting condensing." << std::endl;
    // Perform condensing to merge all smaller blocks into larger blocks.
    condense();
    display();

    // std::cout << "Destructor: Freeing blocks in the largest free list." << std::endl;
    // // Free all blocks in the largest list, which should contain all memory.
    // FreeBlock* current = freeLists[NUM_FREE_LISTS - 1];
    // while (current) {
    //   FreeBlock* next = current->next;
    //   ::operator delete(static_cast<void*>(current));
    //   current = next;
    // }
    // freeLists[NUM_FREE_LISTS - 1] = nullptr;

    // std::cout << "Destructor: Ensuring all other free lists are empty." << std::endl;
    // // Ensure that all other free lists are empty.
    // for (size_t i = 0; i < NUM_FREE_LISTS - 1; ++i) {
    //   if (freeLists[i] != nullptr) {
    //     std::cerr << "Error: Memory leak detected in free list at index " << i << std::endl;
    //     std::cerr << "Remaining blocks in free list " << i << ": " << std::endl;
    //     FreeBlock* remaining = freeLists[i];
    //     while (remaining) {
    //       std::cerr << "  Block at address: " << static_cast<void*>(remaining) << std::endl;
    //       remaining = remaining->next;
    //     }
    //     std::terminate();
    //   }
    // }
  }

  void display() {
    std::cout << "Current state of the free lists:" << std::endl;
    
    for (size_t i = 0; i < NUM_FREE_LISTS; ++i) {
      std::cout << "Free list index " << i << " (block size: " << getBlockSize(i) << "):" << std::endl;
      FreeBlock* current = freeLists[i];
      if (current == nullptr) {
        std::cout << "  [empty]" << std::endl;
      } else {
        while (current != nullptr) {
          std::cout << "  Block at address: " << static_cast<void*>(current) << std::endl;
          current = current->next;
        }
      }
    }
  }

  // Allocate a block of memory of at least the requested size.
  void* allocate(size_t size) {
    // Ensure the requested size is at least MIN_BLOCK_SIZE and round up to the nearest power of 2.
    size = std::max(MIN_BLOCK_SIZE, std::bit_ceil(size));
    size_t index = getFreeListIndex(size);

    std::cout << "Allocating block of " << size << " at index " << index << std::endl;

    // Check if the index is within the bounds of the free list.
    if (index >= NUM_FREE_LISTS) {
      std::cerr << "Error: Requested size index " << index << " is out of bounds." << std::endl;
      std::terminate();
    }

    // if (freeLists[index]) {
    //   // Disconnect the block and return it.
    //   FreeBlock* block = freeLists[index];
    //   freeLists[index] = block->next;
    //   return block;
    // } else if (index == NUM_FREE_LISTS - 1) {
    //   // We're at the max size and don't have a block, so we need to allocate one from the heap
    //   void* block = ::operator new(MAX_BLOCK_SIZE);
    //   return block; // No need to connect it or anything
    // } else {
    //   // Allocate one twice as big, split it in half, and return the first half.
    //   FreeBlock* block = static_cast<FreeBlock*>(allocate(size << 1));
    //   FreeBlock* half_block = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(block) + size);
    //   half_block->next = freeLists[index];
    //   freeLists[index] = half_block;

    //   return block;
    // }

    // Search for an available block, starting from the desired size and moving up if needed.
    for (size_t i = index; i < NUM_FREE_LISTS; ++i) {
      if (freeLists[i]) {
        ++allocations;
        
        // Split the block if found at a larger size.
        return splitBlock(i, index);
      }
    }

    // No suitable block is available, request a new one from the heap (maximum possible size).
    void* block = ::operator new(MAX_BLOCK_SIZE);
    std::cout << "Failed to find a block of " << size << " so allocated a new block from heap with size: " << MAX_BLOCK_SIZE << std::endl;
    // Add the new block to the appropriate free list.
    addBlockToFreeList(block, MAX_BLOCK_SIZE);
    // Split the newly allocated block to get the desired size.
    return splitBlock(NUM_FREE_LISTS - 1, index);
    // return allocate(size);
  }

  // Template version of allocate to allocate memory for a specific type.
  template<typename T>
  T* allocate() {
    return static_cast<T*>(allocate(sizeof(T)));
  }

  // Deallocate a block of memory, returning it to the free list.
  void deallocate(void* ptr, size_t size) {
    // Ensure the size is at least MIN_BLOCK_SIZE and round up to the nearest power of 2.
    size = std::max(MIN_BLOCK_SIZE, std::bit_ceil(size));
    size_t index = getFreeListIndex(size);
    std::cout << "Deallocating block of size " << size << " at index " << index << " and at address " << ptr << std::endl;

    auto* block = static_cast<FreeBlock*>(ptr);

    block->next = freeLists[index];
    freeLists[index] = block;

    // // Add the block back to the appropriate free list.
    // addBlockToFreeList(ptr, size);
  }

  // Template version of deallocate to deallocate memory for a specific type.
  template<typename T>
  void deallocate(T* ptr) {
    deallocate(static_cast<void*>(ptr), sizeof(T));
  }

  // Condense the free lists by sorting blocks in each list in place.
  void condense() {
    std::cout << "Condensing free lists." << std::endl;
    // Iterate over each free list to sort it.
    for (size_t i = 0; i < NUM_FREE_LISTS - 1; ++i) {
      if (freeLists[i] == nullptr) {
        continue;
      }

      // Perform an in-place insertion sort on the linked list.
      FreeBlock* sorted = nullptr;
      FreeBlock* current = freeLists[i];
      size_t size = getBlockSize(i);

      while (current) {
        FreeBlock* next = current->next;

        // Insert current block into the sorted list in the correct position.
        if (sorted == nullptr || reinterpret_cast<char*>(current) < reinterpret_cast<char*>(sorted)) {
          current->next = sorted;
          sorted = current;
        } else {
          FreeBlock* temp = sorted;
          while (temp->next != nullptr && reinterpret_cast<char*>(temp->next) < reinterpret_cast<char*>(current)) {
            temp = temp->next;
          }
          current->next = temp->next;
          temp->next = current;
        }
        current = next;
      }

      // // Verify that the blocks are sorted correctly by printing them.
      // std::cout << "Sorted blocks in free list index " << i << ": " << std::endl;
      // FreeBlock* verify = sorted;
      // while (verify) {
      //   std::cout << "  Block at address: " << static_cast<void*>(verify) << std::endl;
      //   verify = verify->next;
      // }

      // Update the free list with the sorted list.
      freeLists[i] = sorted;

      while (freeLists[i]) {
        FreeBlock* block = freeLists[i];
        auto* next = block->next;

        if (reinterpret_cast<char*>(block) + size == reinterpret_cast<char*>(next)) {
          std::cout << "PASSED!" << std::endl;
          freeLists[i] = next->next; // This should disconnect both `block` and `next`
          block->next = freeLists[i + 1];
          freeLists[i + 1] = block; // Link `block` in to the next list

          block = freeLists[i];
        } else {
          std::cout << "FAILED..." << std::endl;
          break;
        }
      }
    }
  }

private:
  // Array of pointers representing the heads of the free lists for each block size.
  std::array<FreeBlock*, NUM_FREE_LISTS> freeLists;
  size_t allocations = 0;

  // Get the index of the free list corresponding to a given block size.
  size_t getFreeListIndex(size_t size) const {
    // Calculate the index based on the power of 2 (distance from MIN_BLOCK_SIZE).
    return std::bit_width(size) - MIN_BLOCK_WIDTH;
  }

  // Get the block size corresponding to a given free list index.
  size_t getBlockSize(size_t index) const {
    // Block size is calculated as MIN_BLOCK_SIZE shifted left by the index value.
    return MIN_BLOCK_SIZE << index;
  }

  // Add a block to the appropriate free list.
  void addBlockToFreeList(void* block, size_t size) {
    size_t index = getFreeListIndex(size);
    auto* newBlock = static_cast<FreeBlock*>(block);
    // Insert the block at the head of the appropriate free list.
    newBlock->next = freeLists[index];
    freeLists[index] = newBlock;
    std::cout << "Added block of size " << size << " to free list at index " << index << std::endl;
  }

  // Split a block from a larger size to the desired size.
  void* splitBlock(size_t fromIndex, size_t toIndex) {
    assert(fromIndex >= toIndex);

    // Remove the block from the free list at fromIndex.
    FreeBlock* block = freeLists[fromIndex];
    if (!block) {
      std::cerr << "Error: No block available to split at index " << fromIndex << std::endl;
      std::terminate();
    }
    freeLists[fromIndex] = block->next;

    // Calculate the initial block size and maintain the current size during the loop.
    size_t currentSize = getBlockSize(fromIndex);

    // Split the block down to the desired size, adding the remaining halves back to the free list.
    while (fromIndex > toIndex) {
      --fromIndex;
      // Reduce the current size by half using a right bit shift.
      currentSize >>= 1;
      // Calculate the address of the second half of the block.
      char* blockAddress = reinterpret_cast<char*>(block);
      void* splitBlock = static_cast<void*>(blockAddress + currentSize);
      // Add the second half back to the free list.
      addBlockToFreeList(splitBlock, currentSize);
    }

    // Return the block of the desired size.
    return block;
  }
};

int main() {
  FreeListAllocator allocator;

  // Performance testing
  constexpr size_t numIterations = 16;
  constexpr size_t numLoops = 1;
  uintptr_t checksum = 0; // Use to accumulate addresses

  // Generate random allocation sizes from a predetermined set of powers of 2.
  std::vector<size_t> allocationSizes = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576 };
  std::vector<size_t> randomSizes(numIterations);
  std::vector<void*> allocatedBlocks(numIterations, nullptr);

  // Random number generation for selecting sizes.
  std::random_device rd;
  std::mt19937 gen(1); // Temporarily avoiding randomness while debugging
  std::uniform_int_distribution<> distrib(0, allocationSizes.size() - 1);

  // Fill randomSizes with values from allocationSizes.
  for (size_t i = 0; i < numIterations; ++i) {
    randomSizes[i] = allocationSizes[distrib(gen)];
  }

  auto start = std::chrono::high_resolution_clock::now();

  // Perform allocation/deallocation in phases.
  for (size_t loop = 0; loop < numLoops; ++loop) {
    // Allocation phase
    for (size_t i = 0; i < numIterations; ++i) {
      void* block = allocator.allocate(randomSizes[i]);
      allocatedBlocks[i] = block;
      checksum += reinterpret_cast<uintptr_t>(block); // Accumulate pointer value to prevent optimization out
    }

    // Deallocation phase
    for (size_t i = 0; i < numIterations; ++i) {
      allocator.deallocate(allocatedBlocks[i], randomSizes[i]);
      allocatedBlocks[i] = nullptr;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  double averageTime = static_cast<double>(duration) / (numIterations * numLoops * 2); // Divide by 2 for allocation and deallocation

  std::cout << "Performance test completed in " << duration << " ns for " << (numIterations * numLoops * 2) << " allocations and deallocations." << std::endl;
  std::cout << "Average time per allocation/deallocation: " << averageTime << " ns." << std::endl;
  std::cout << "Checksum value: " << checksum << std::endl; // Output the checksum to ensure calculations weren't optimized away

  // Perform maintenance by condensing the free lists.
  // allocator.display();

  // std::cout << "All allocations, deallocations, and maintenance performed successfully." << std::endl;
  return 0;
}
