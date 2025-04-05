export module utility.random;

import <random>;
import <chrono>;
import <cstdint>;
import <algorithm>;
import <thread>;
import <functional>;
import <limits>;

class Xorshift {
public:
  using result_type = uint32_t;
private:
  result_type state;
public:
  explicit Xorshift(uint64_t seed) : state{static_cast<result_type>(seed)} {}

  result_type operator()() {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
  }

  static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
  static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
};

class PCG {
public:
  using result_type = uint64_t;
private:
  result_type state;
  result_type inc;
public:
  PCG(result_type seed, result_type seq) : state{seed}, inc{(seq << 1u) | 1u} {
    (*this)();
  }

  result_type operator()() {
    result_type old_state = state;
    state = old_state * 6364136223846793005ULL + inc;
    result_type xor_shifted = ((old_state >> 18u) ^ old_state) >> 27u;
    result_type rot = old_state >> 59u;
    return (xor_shifted >> rot) | (xor_shifted << ((-static_cast<int64_t>(rot)) & 31));
  }

  static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
  static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
};

static thread_local uint64_t seed = [](){
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

static thread_local std::mt19937 mt19937{static_cast<uint32_t>(seed)};
static thread_local std::mt19937_64 mt19937_64{seed};
static thread_local Xorshift xorshift{seed};
static thread_local PCG pcg{seed, std::random_device{}()}; // Needs a second seed
static thread_local std::minstd_rand0 minstd_rand0{static_cast<uint32_t>(seed)};

template<typename T, typename E>
T IntDistribution(T min, T max, E&& engine) {
  return std::uniform_int_distribution<T>{min, max}(engine);
}

template<typename T, typename E>
T RealDistribution(T min, T max, E&& engine) {
  return std::uniform_real_distribution<T>{min, max}(engine);
}

export namespace utility::Random {
  uint32_t MT19937() { return mt19937(); }
  uint64_t MT19937_64() { return mt19937_64(); }
  uint32_t XORSHIFT() { return xorshift(); }
  uint64_t PCG() { return pcg(); }
  uint32_t MINSTD_RAND0() { return minstd_rand0(); }

  uint32_t MT19937(uint32_t l, uint32_t u) { return IntDistribution<uint32_t>(l, u, mt19937); }
  uint64_t MT19937_64(uint64_t l, uint64_t u) { return IntDistribution<uint64_t>(l, u, mt19937_64); }
  uint32_t XORSHIFT(uint32_t l, uint32_t u) { return IntDistribution<uint32_t>(l, u, xorshift); }
  uint64_t PCG(uint64_t l, uint64_t u) { return IntDistribution<uint64_t>(l, u, pcg); }
  uint32_t MINSTD_RAND0(uint32_t l, uint32_t u) { return IntDistribution<uint32_t>(l, u, minstd_rand0); }

  float MT19937(float l, float u) { return RealDistribution<float>(l, u, mt19937); }
  double MT19937_64(double l, double u) { return RealDistribution<double>(l, u, mt19937_64); }
  float XORSHIFT(float l, float u) { return RealDistribution<float>(l, u, xorshift); }
  double PCG(double l, double u) { return RealDistribution<double>(l, u, pcg); }
  float MINSTD_RAND0(float l, float u) { return RealDistribution<float>(l, u, minstd_rand0); }
};