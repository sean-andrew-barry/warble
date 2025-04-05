import utility.random;

import <random>;
import <chrono>;
import <cstdint>;
import <algorithm>;
import <thread>;
import <functional>;
import <limits>;

static inline uint64_t seed = [](){
  uint64_t seed = 0;

  // Simple hash combine function
  auto Hash = [&seed](const auto& v){
    using T = std::decay_t<decltype(v)>;
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  };

  std::random_device device{};

  // Source 1: std::random_device
  Hash(device());

  // Source 2: Current time
  Hash(std::chrono::high_resolution_clock::now().time_since_epoch().count());

  // Source 3: Thread ID
  Hash(std::this_thread::get_id());

  // Source 4: One more strong one for good measure
  Hash(device());

  return seed;
}();

thread_local std::mt19937 mt19937{static_cast<uint32_t>(seed)};
thread_local std::mt19937_64 mt19937_64{seed};
thread_local Xorshift xorshift{seed};
thread_local PCG pcg{seed, std::random_device{}()}; // Needs a second seed
thread_local std::minstd_rand0 minstd_rand0{static_cast<uint32_t>(seed)};