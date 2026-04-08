export module compiler.utility.Hash128;

import <cstdint>;
import <bit>;
import <chrono>;
import <cstddef>;
import <cstring>;
import <span>;
import <ranges>;
import <type_traits>;
import <array>;
import <functional>;
import <random>;
import <thread>;
import <utility>;

import compiler.utility.Macros;

#if defined(_MSC_VER) && defined(_M_X64)
import <intrin.h>;
import <emmintrin.h>;
import <immintrin.h>;
#endif

#if defined(_MSC_VER)
#define WARBLE_FORCEINLINE __forceinline
#define WARBLE_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define WARBLE_FORCEINLINE __attribute__((always_inline)) inline
#define WARBLE_NOINLINE __attribute__((noinline))
#else
#define WARBLE_FORCEINLINE inline
#define WARBLE_NOINLINE
#endif

namespace compiler::utility {
  export class Hash128 {
  private:
    static constexpr uint64_t prime_mx1_ = 0x165667919E3779F9ULL;
    static constexpr uint64_t prime_mx2_ = 0x9FB21C651E98DF25ULL;
    static constexpr uint64_t prime1_ = 11400714785074694791ULL;
    static constexpr uint64_t prime2_ = 14029467366897019727ULL;
    static constexpr uint64_t prime3_ = 1609587929392839161ULL;
    static constexpr uint64_t prime4_ = 9650029242287828579ULL;
    static constexpr uint64_t prime5_ = 2870177450012600261ULL;
    static constexpr uint64_t prime32_1_ = 0x9E3779B1ULL;
    static constexpr uint64_t prime32_2_ = 0x85EBCA77ULL;
    static constexpr uint64_t prime32_3_ = 0xC2B2AE3DULL;

    static constexpr std::size_t stripe_len_ = 64;
    static constexpr std::size_t secret_size_min_ = 136;
    static constexpr std::size_t secret_size_ = 192;
    static constexpr std::size_t acc_nb_ = 8;
    static constexpr std::size_t secret_consume_rate_ = 8;
    static constexpr std::size_t midsize_start_offset_ = 3;
    static constexpr std::size_t midsize_last_offset_ = 17;
    static constexpr std::size_t nb_stripes_per_block_ = (secret_size_ - stripe_len_) / secret_consume_rate_;
    static constexpr std::size_t block_len_ = stripe_len_ * nb_stripes_per_block_;

    static constexpr std::array<uint64_t, acc_nb_> initial_acc_ = {
      prime32_3_,
      prime1_,
      prime2_,
      prime3_,
      prime4_,
      prime32_2_,
      prime5_,
      prime32_1_,
    };

    enum class VectorPath : uint8_t {
      Scalar,
      Sse2,
      Avx2,
    };

    static constexpr std::array<std::byte, secret_size_> BuildBaseSecret() {
      return {
        std::byte{0xb8}, std::byte{0xfe}, std::byte{0x6c}, std::byte{0x39}, std::byte{0x23}, std::byte{0xa4}, std::byte{0x4b}, std::byte{0xbe}, std::byte{0x7c}, std::byte{0x01}, std::byte{0x81}, std::byte{0x2c}, std::byte{0xf7}, std::byte{0x21}, std::byte{0xad}, std::byte{0x1c},
        std::byte{0xde}, std::byte{0xd4}, std::byte{0x6d}, std::byte{0xe9}, std::byte{0x83}, std::byte{0x90}, std::byte{0x97}, std::byte{0xdb}, std::byte{0x72}, std::byte{0x40}, std::byte{0xa4}, std::byte{0xa4}, std::byte{0xb7}, std::byte{0xb3}, std::byte{0x67}, std::byte{0x1f},
        std::byte{0xcb}, std::byte{0x79}, std::byte{0xe6}, std::byte{0x4e}, std::byte{0xcc}, std::byte{0xc0}, std::byte{0xe5}, std::byte{0x78}, std::byte{0x82}, std::byte{0x5a}, std::byte{0xd0}, std::byte{0x7d}, std::byte{0xcc}, std::byte{0xff}, std::byte{0x72}, std::byte{0x21},
        std::byte{0xb8}, std::byte{0x08}, std::byte{0x46}, std::byte{0x74}, std::byte{0xf7}, std::byte{0x43}, std::byte{0x24}, std::byte{0x8e}, std::byte{0xe0}, std::byte{0x35}, std::byte{0x90}, std::byte{0xe6}, std::byte{0x81}, std::byte{0x3a}, std::byte{0x26}, std::byte{0x4c},
        std::byte{0x3c}, std::byte{0x28}, std::byte{0x52}, std::byte{0xbb}, std::byte{0x91}, std::byte{0xc3}, std::byte{0x00}, std::byte{0xcb}, std::byte{0x88}, std::byte{0xd0}, std::byte{0x65}, std::byte{0x8b}, std::byte{0x1b}, std::byte{0x53}, std::byte{0x2e}, std::byte{0xa3},
        std::byte{0x71}, std::byte{0x64}, std::byte{0x48}, std::byte{0x97}, std::byte{0xa2}, std::byte{0x0d}, std::byte{0xf9}, std::byte{0x4e}, std::byte{0x38}, std::byte{0x19}, std::byte{0xef}, std::byte{0x46}, std::byte{0xa9}, std::byte{0xde}, std::byte{0xac}, std::byte{0xd8},
        std::byte{0xa8}, std::byte{0xfa}, std::byte{0x76}, std::byte{0x3f}, std::byte{0xe3}, std::byte{0x9c}, std::byte{0x34}, std::byte{0x3f}, std::byte{0xf9}, std::byte{0xdc}, std::byte{0xbb}, std::byte{0xc7}, std::byte{0xc7}, std::byte{0x0b}, std::byte{0x4f}, std::byte{0x1d},
        std::byte{0x8a}, std::byte{0x51}, std::byte{0xe0}, std::byte{0x4b}, std::byte{0xcd}, std::byte{0xb4}, std::byte{0x59}, std::byte{0x31}, std::byte{0xc8}, std::byte{0x9f}, std::byte{0x7e}, std::byte{0xc9}, std::byte{0xd9}, std::byte{0x78}, std::byte{0x73}, std::byte{0x64},
        std::byte{0xea}, std::byte{0xc5}, std::byte{0xac}, std::byte{0x83}, std::byte{0x34}, std::byte{0xd3}, std::byte{0xeb}, std::byte{0xc3}, std::byte{0xc5}, std::byte{0x81}, std::byte{0xa0}, std::byte{0xff}, std::byte{0xfa}, std::byte{0x13}, std::byte{0x63}, std::byte{0xeb},
        std::byte{0x17}, std::byte{0x0d}, std::byte{0xdd}, std::byte{0x51}, std::byte{0xb7}, std::byte{0xf0}, std::byte{0xda}, std::byte{0x49}, std::byte{0xd3}, std::byte{0x16}, std::byte{0x55}, std::byte{0x26}, std::byte{0x29}, std::byte{0xd4}, std::byte{0x68}, std::byte{0x9e},
        std::byte{0x2b}, std::byte{0x16}, std::byte{0xbe}, std::byte{0x58}, std::byte{0x7d}, std::byte{0x47}, std::byte{0xa1}, std::byte{0xfc}, std::byte{0x8f}, std::byte{0xf8}, std::byte{0xb8}, std::byte{0xd1}, std::byte{0x7a}, std::byte{0xd0}, std::byte{0x31}, std::byte{0xce},
        std::byte{0x45}, std::byte{0xcb}, std::byte{0x3a}, std::byte{0x8f}, std::byte{0x95}, std::byte{0x16}, std::byte{0x04}, std::byte{0x28}, std::byte{0xaf}, std::byte{0xd7}, std::byte{0xfb}, std::byte{0xca}, std::byte{0xbb}, std::byte{0x4b}, std::byte{0x40}, std::byte{0x7e},
      };
    }

    static const std::array<std::byte, secret_size_>& BaseSecret() {
      static const std::array<std::byte, secret_size_> base_secret = BuildBaseSecret();
      return base_secret;
    }

    static uint64_t Read64(const std::byte* data) {
      uint64_t value = 0;
      std::memcpy(&value, data, sizeof(value));
      if constexpr (std::endian::native == std::endian::little) {
        return value;
      } else {
        return std::byteswap(value);
      }
    }

    static void Write64(std::byte* data, uint64_t value) {
      if constexpr (std::endian::native == std::endian::little) {
        std::memcpy(data, &value, sizeof(value));
      } else {
        const uint64_t swapped = std::byteswap(value);
        std::memcpy(data, &swapped, sizeof(swapped));
      }
    }

    static uint64_t Read64(std::span<const std::byte, secret_size_> secret, std::size_t offset) {
      return Read64(secret.data() + offset);
    }

    static uint32_t Read32(std::span<const std::byte, secret_size_> secret, std::size_t offset) {
      return Read32(secret.data() + offset);
    }

    static uint32_t Read32(const std::byte* data) {
      uint32_t value = 0;
      std::memcpy(&value, data, sizeof(value));
      if constexpr (std::endian::native == std::endian::little) {
        return value;
      } else {
        return std::byteswap(value);
      }
    }

    static uint32_t Rotl32(uint32_t value, int shift) {
      return std::rotl(value, shift);
    }

    static std::pair<uint64_t, uint64_t> Mult64To128(uint64_t lhs, uint64_t rhs) {
      if constexpr (compiler::utility::Macros::MSVC_X64) {
        uint64_t high = 0;
        const uint64_t low = _umul128(lhs, rhs, &high);
        return {low, high};
      } else if constexpr (compiler::utility::Macros::HAS_INT128) {
        using compiler::utility::Macros::UInt128;
        const UInt128 product = static_cast<UInt128>(lhs) * static_cast<UInt128>(rhs);
        return {static_cast<uint64_t>(product), static_cast<uint64_t>(product >> 64)};
      } else {
        const uint64_t lhs_lo = static_cast<uint32_t>(lhs);
        const uint64_t lhs_hi = lhs >> 32;
        const uint64_t rhs_lo = static_cast<uint32_t>(rhs);
        const uint64_t rhs_hi = rhs >> 32;

        const uint64_t p00 = lhs_lo * rhs_lo;
        const uint64_t p01 = lhs_lo * rhs_hi;
        const uint64_t p10 = lhs_hi * rhs_lo;
        const uint64_t p11 = lhs_hi * rhs_hi;

        const uint64_t mid = (p00 >> 32) + (p01 & 0xFFFFFFFFULL) + (p10 & 0xFFFFFFFFULL);
        const uint64_t low = (p00 & 0xFFFFFFFFULL) | (mid << 32);
        const uint64_t high = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
        return {low, high};
      }
    }

    static uint64_t Round(uint64_t acc, uint64_t lane) {
      acc += lane * prime2_;
      acc = std::rotl(acc, 31);
      acc *= prime1_;
      return acc;
    }

    static uint64_t MergeRound(uint64_t acc, uint64_t val) {
      acc ^= Round(0, val);
      acc = acc * prime1_ + prime4_;
      return acc;
    }

    static uint64_t XXH64Avalanche(uint64_t h) {
      h ^= h >> 33;
      h *= prime2_;
      h ^= h >> 29;
      h *= prime3_;
      h ^= h >> 32;
      return h;
    }

    static uint64_t XXH3Avalanche(uint64_t h) {
      h ^= h >> 37;
      h *= prime_mx1_;
      h ^= h >> 32;
      return h;
    }

    static uint64_t Mul128Fold64(uint64_t lhs, uint64_t rhs) {
      if constexpr (compiler::utility::Macros::MSVC_X64) {
        uint64_t high = 0;
        const uint64_t low = _umul128(lhs, rhs, &high);
        return low ^ high;
      } else if constexpr (compiler::utility::Macros::HAS_INT128) {
        using compiler::utility::Macros::UInt128;
        const UInt128 product = static_cast<UInt128>(lhs) * static_cast<UInt128>(rhs);
        return static_cast<uint64_t>(product) ^ static_cast<uint64_t>(product >> 64);
      } else {
        const uint64_t lhs_lo = static_cast<uint32_t>(lhs);
        const uint64_t lhs_hi = lhs >> 32;
        const uint64_t rhs_lo = static_cast<uint32_t>(rhs);
        const uint64_t rhs_hi = rhs >> 32;

        const uint64_t p00 = lhs_lo * rhs_lo;
        const uint64_t p01 = lhs_lo * rhs_hi;
        const uint64_t p10 = lhs_hi * rhs_lo;
        const uint64_t p11 = lhs_hi * rhs_hi;

        const uint64_t mid = (p00 >> 32) + (p01 & 0xFFFFFFFFULL) + (p10 & 0xFFFFFFFFULL);
        const uint64_t low = (p00 & 0xFFFFFFFFULL) | ((mid & 0xFFFFFFFFULL) << 32);
        const uint64_t high = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
        return low ^ high;
      }
    }

    static uint64_t Mix16(
      const std::byte* data,
      std::span<const std::byte, secret_size_> secret,
      std::size_t secret_offset,
      uint64_t seed
    ) {
      const uint64_t input_lo = Read64(data + 0) ^ (Read64(secret, secret_offset + 0) + seed);
      const uint64_t input_hi = Read64(data + 8) ^ (Read64(secret, secret_offset + 8) - seed);
      return Mul128Fold64(input_lo, input_hi);
    }

    static WARBLE_FORCEINLINE std::pair<uint64_t, uint64_t> Mix32(
      uint64_t low,
      uint64_t high,
      const std::byte* input_1,
      const std::byte* input_2,
      std::span<const std::byte, secret_size_> secret,
      std::size_t secret_offset,
      uint64_t seed
    ) {
      low += Mix16(input_1, secret, secret_offset + 0, seed);
      low ^= Read64(input_2 + 0) + Read64(input_2 + 8);
      high += Mix16(input_2, secret, secret_offset + 16, seed);
      high ^= Read64(input_1 + 0) + Read64(input_1 + 8);
      return {low, high};
    }

    static std::array<std::byte, secret_size_> DeriveSecret(uint64_t seed) {
      std::array<std::byte, secret_size_> secret = BaseSecret();
      for (std::size_t offset = 0; offset < secret_size_; offset += 16) {
        const uint64_t lo = Read64(secret.data() + offset + 0) + seed;
        const uint64_t hi = Read64(secret.data() + offset + 8) - seed;
        Write64(secret.data() + offset + 0, lo);
        Write64(secret.data() + offset + 8, hi);
      }
      return secret;
    }

    static bool HasAvx2Runtime() {
#if defined(_MSC_VER) && defined(_M_X64) && defined(__AVX2__)
      static const bool has_avx2 = []() {
        int cpu_info[4] = {0, 0, 0, 0};
        __cpuidex(cpu_info, 1, 0);

        const bool osxsave = (cpu_info[2] & (1 << 27)) != 0;
        const bool avx = (cpu_info[2] & (1 << 28)) != 0;
        if (!osxsave || !avx) {
          return false;
        }

        const unsigned long long xcr0 = _xgetbv(0);
        const bool xmm_state = (xcr0 & 0x2ULL) != 0;
        const bool ymm_state = (xcr0 & 0x4ULL) != 0;
        if (!xmm_state || !ymm_state) {
          return false;
        }

        __cpuidex(cpu_info, 7, 0);
        const bool avx2 = (cpu_info[1] & (1 << 5)) != 0;
        return avx2;
      }();
      return has_avx2;
#else
      return false;
#endif
    }

    struct SecretSelection {
      std::span<const std::byte, secret_size_> secret;
      uint64_t seed;
    };

    static SecretSelection SelectSecret(uint64_t seed) {
      const auto& base_secret = BaseSecret();
      if (seed == 0) {
        return {
          std::span<const std::byte, secret_size_>{base_secret.data(), secret_size_},
          0,
        };
      }

      struct SeedCache {
        uint64_t seed = 0;
        std::array<std::byte, secret_size_> secret{};
        bool initialized = false;
      };

      thread_local SeedCache cache{};
      if (!cache.initialized || cache.seed != seed) {
        cache.secret = DeriveSecret(seed);
        cache.seed = seed;
        cache.initialized = true;
      }

      return {
        std::span<const std::byte, secret_size_>{cache.secret.data(), secret_size_},
        seed,
      };
    }

    static void Prefetch(const std::byte* data) {
#if defined(_MSC_VER) && defined(_M_X64)
      _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_T0);
#else
      (void)data;
#endif
    }

    static VectorPath SelectVectorPath() {
#if defined(_MSC_VER) && defined(_M_X64)
#if defined(__AVX2__)
      if (HasAvx2Runtime()) {
        return VectorPath::Avx2;
      }
#endif
      return VectorPath::Sse2;
#else
      return VectorPath::Scalar;
#endif
    }

    static WARBLE_FORCEINLINE void AccumulateStripeScalar(
      uint64_t* acc,
      const std::byte* data,
      const std::byte* secret
    ) {
      for (std::size_t lane = 0; lane < acc_nb_; ++lane) {
        const uint64_t data_val = Read64(data + (lane * 8));
        const uint64_t data_key = data_val ^ Read64(secret + (lane * 8));
        acc[lane ^ 1] += data_val;
        acc[lane] +=
          static_cast<uint64_t>(static_cast<uint32_t>(data_key)) *
          static_cast<uint64_t>(static_cast<uint32_t>(data_key >> 32));
      }
    }

    static WARBLE_FORCEINLINE void AccumulateScalar(
      uint64_t* acc,
      const std::byte* input,
      const std::byte* secret,
      std::size_t stripe_count
    ) {
      for (std::size_t stripe = 0; stripe < stripe_count; ++stripe) {
        const std::byte* const stripe_input = input + (stripe * stripe_len_);
        Prefetch(stripe_input + 384);
        AccumulateStripeScalar(acc, stripe_input, secret + (stripe * secret_consume_rate_));
      }
    }

    static WARBLE_FORCEINLINE void ScrambleScalar(uint64_t* acc, const std::byte* secret) {
      for (std::size_t lane = 0; lane < acc_nb_; ++lane) {
        uint64_t value = acc[lane];
        value ^= value >> 47;
        value ^= Read64(secret + (lane * 8));
        value *= prime32_1_;
        acc[lane] = value;
      }
    }

    using AccumulateFn = void(*)(uint64_t*, const std::byte*, const std::byte*, std::size_t);
    using ScrambleFn = void(*)(uint64_t*, const std::byte*);

#if defined(_MSC_VER) && defined(_M_X64)
    static WARBLE_FORCEINLINE void AccumulateSse2(
      uint64_t* acc,
      const std::byte* input,
      const std::byte* secret,
      std::size_t stripe_count
    ) {
      auto* const xacc = reinterpret_cast<__m128i*>(acc);

      for (std::size_t stripe = 0; stripe < stripe_count; ++stripe) {
        const auto* const xinput = reinterpret_cast<const __m128i*>(input + (stripe * stripe_len_));
        const auto* const xsecret = reinterpret_cast<const __m128i*>(secret + (stripe * secret_consume_rate_));
        Prefetch(reinterpret_cast<const std::byte*>(xinput) + 384);

        for (std::size_t i = 0; i < stripe_len_ / sizeof(__m128i); ++i) {
          const __m128i data_vec = _mm_loadu_si128(xinput + i);
          const __m128i key_vec = _mm_loadu_si128(xsecret + i);
          const __m128i data_key = _mm_xor_si128(data_vec, key_vec);
          const __m128i data_key_hi = _mm_shuffle_epi32(data_key, _MM_SHUFFLE(0, 3, 0, 1));
          const __m128i product = _mm_mul_epu32(data_key, data_key_hi);
          const __m128i data_swap = _mm_shuffle_epi32(data_vec, _MM_SHUFFLE(1, 0, 3, 2));
          xacc[i] = _mm_add_epi64(_mm_add_epi64(xacc[i], data_swap), product);
        }
      }
    }

    static WARBLE_FORCEINLINE void ScrambleSse2(uint64_t* acc, const std::byte* secret) {
      auto* const xacc = reinterpret_cast<__m128i*>(acc);
      const auto* const xsecret = reinterpret_cast<const __m128i*>(secret);
      const __m128i prime32 = _mm_set1_epi32(static_cast<int>(prime32_1_));

      for (std::size_t i = 0; i < stripe_len_ / sizeof(__m128i); ++i) {
        const __m128i acc_vec = xacc[i];
        const __m128i shifted = _mm_srli_epi64(acc_vec, 47);
        const __m128i data_vec = _mm_xor_si128(acc_vec, shifted);
        const __m128i key_vec = _mm_loadu_si128(xsecret + i);
        const __m128i data_key = _mm_xor_si128(data_vec, key_vec);
        const __m128i data_key_hi = _mm_shuffle_epi32(data_key, _MM_SHUFFLE(0, 3, 0, 1));
        const __m128i prod_lo = _mm_mul_epu32(data_key, prime32);
        const __m128i prod_hi = _mm_mul_epu32(data_key_hi, prime32);
        xacc[i] = _mm_add_epi64(prod_lo, _mm_slli_epi64(prod_hi, 32));
      }
    }

#if defined(__AVX2__)
    static WARBLE_FORCEINLINE void AccumulateAvx2(
      uint64_t* acc,
      const std::byte* input,
      const std::byte* secret,
      std::size_t stripe_count
    ) {
      auto* const xacc = reinterpret_cast<__m256i*>(acc);

      for (std::size_t stripe = 0; stripe < stripe_count; ++stripe) {
        const auto* const xinput = reinterpret_cast<const __m256i*>(input + (stripe * stripe_len_));
        const auto* const xsecret = reinterpret_cast<const __m256i*>(secret + (stripe * secret_consume_rate_));
        Prefetch(reinterpret_cast<const std::byte*>(xinput) + 384);

        for (std::size_t i = 0; i < stripe_len_ / sizeof(__m256i); ++i) {
          const __m256i data_vec = _mm256_loadu_si256(xinput + i);
          const __m256i key_vec = _mm256_loadu_si256(xsecret + i);
          const __m256i data_key = _mm256_xor_si256(data_vec, key_vec);
          const __m256i data_key_hi = _mm256_srli_epi64(data_key, 32);
          const __m256i product = _mm256_mul_epu32(data_key, data_key_hi);
          const __m256i data_swap = _mm256_shuffle_epi32(data_vec, _MM_SHUFFLE(1, 0, 3, 2));
          xacc[i] = _mm256_add_epi64(_mm256_add_epi64(xacc[i], data_swap), product);
        }
      }
    }

    static WARBLE_FORCEINLINE void ScrambleAvx2(uint64_t* acc, const std::byte* secret) {
      auto* const xacc = reinterpret_cast<__m256i*>(acc);
      const auto* const xsecret = reinterpret_cast<const __m256i*>(secret);
      const __m256i prime32 = _mm256_set1_epi32(static_cast<int>(prime32_1_));

      for (std::size_t i = 0; i < stripe_len_ / sizeof(__m256i); ++i) {
        const __m256i acc_vec = xacc[i];
        const __m256i shifted = _mm256_srli_epi64(acc_vec, 47);
        const __m256i data_vec = _mm256_xor_si256(acc_vec, shifted);
        const __m256i key_vec = _mm256_loadu_si256(xsecret + i);
        const __m256i data_key = _mm256_xor_si256(data_vec, key_vec);
        const __m256i data_key_hi = _mm256_srli_epi64(data_key, 32);
        const __m256i prod_lo = _mm256_mul_epu32(data_key, prime32);
        const __m256i prod_hi = _mm256_mul_epu32(data_key_hi, prime32);
        xacc[i] = _mm256_add_epi64(prod_lo, _mm256_slli_epi64(prod_hi, 32));
      }
    }
#endif
#endif

    static WARBLE_FORCEINLINE uint64_t Mix2Accs(const uint64_t* acc, const std::byte* secret) {
      return Mul128Fold64(acc[0] ^ Read64(secret + 0), acc[1] ^ Read64(secret + 8));
    }

    static WARBLE_FORCEINLINE uint64_t MergeAccs(const uint64_t* acc, const std::byte* secret, uint64_t start) {
      uint64_t result = start;

      for (std::size_t i = 0; i < (acc_nb_ / 2); ++i) {
        result += Mix2Accs(acc + (2 * i), secret + (16 * i));
      }

      return XXH3Avalanche(result);
    }

    template <AccumulateFn Accumulate, ScrambleFn Scramble>
    static WARBLE_FORCEINLINE std::pair<uint64_t, uint64_t> HashLongImpl(
      std::span<const std::byte> bytes,
      std::span<const std::byte, secret_size_> secret
    ) {
      const std::size_t size = bytes.size();
      const std::byte* const input = bytes.data();
      const std::byte* const secret_ptr = secret.data();
      alignas(32) std::array<uint64_t, acc_nb_> acc = initial_acc_;

      const std::size_t block_count = (size - 1) / block_len_;
      for (std::size_t block = 0; block < block_count; ++block) {
        Accumulate(acc.data(), input + (block * block_len_), secret_ptr, nb_stripes_per_block_);
        Scramble(acc.data(), secret_ptr + secret_size_ - stripe_len_);
      }

      const std::size_t stripe_count = ((size - 1) - (block_len_ * block_count)) / stripe_len_;
      Accumulate(acc.data(), input + (block_count * block_len_), secret_ptr, stripe_count);
      Accumulate(acc.data(), input + size - stripe_len_, secret_ptr + secret_size_ - stripe_len_ - 7, 1);

      uint64_t low = MergeAccs(acc.data(), secret_ptr + 11, static_cast<uint64_t>(size) * prime1_);
      uint64_t high = MergeAccs(
        acc.data(),
        secret_ptr + secret_size_ - stripe_len_ - 11,
        ~(static_cast<uint64_t>(size) * prime2_)
      );
      return {low, high};
    }

    static std::pair<uint64_t, uint64_t> Hash0To16(
      std::span<const std::byte> bytes,
      std::span<const std::byte, secret_size_> secret,
      uint64_t seed
    ) {
      const std::size_t size = bytes.size();
      const std::byte* const ptr = bytes.data();

      if (size == 0) {
        const uint64_t bitflipl = Read64(secret, 64) ^ Read64(secret, 72);
        const uint64_t bitfliph = Read64(secret, 80) ^ Read64(secret, 88);
        return {XXH64Avalanche(seed ^ bitflipl), XXH64Avalanche(seed ^ bitfliph)};
      }

      if (size <= 3) {
        const uint32_t c1 = static_cast<uint32_t>(std::to_integer<uint8_t>(ptr[0]));
        const uint32_t c2 = static_cast<uint32_t>(std::to_integer<uint8_t>(ptr[size >> 1]));
        const uint32_t c3 = static_cast<uint32_t>(std::to_integer<uint8_t>(ptr[size - 1]));
        const uint32_t combined_low = (c1 << 16) | (c2 << 24) | c3 | (static_cast<uint32_t>(size) << 8);
        const uint32_t combined_high = Rotl32(std::byteswap(combined_low), 13);
        const uint64_t bitflipl = static_cast<uint64_t>(Read32(secret, 0) ^ Read32(secret, 4)) + seed;
        const uint64_t bitfliph = static_cast<uint64_t>(Read32(secret, 8) ^ Read32(secret, 12)) - seed;
        return {
          XXH64Avalanche(static_cast<uint64_t>(combined_low) ^ bitflipl),
          XXH64Avalanche(static_cast<uint64_t>(combined_high) ^ bitfliph),
        };
      }

      if (size <= 8) {
        const uint64_t adjusted_seed = seed ^ (static_cast<uint64_t>(std::byteswap(static_cast<uint32_t>(seed))) << 32);
        const uint64_t input = static_cast<uint64_t>(Read32(ptr)) | (static_cast<uint64_t>(Read32(ptr + size - 4)) << 32);
        const uint64_t bitflip = (Read64(secret, 16) ^ Read64(secret, 24)) + adjusted_seed;
        const uint64_t keyed = input ^ bitflip;
        auto [low, high] = Mult64To128(keyed, prime1_ + (static_cast<uint64_t>(size) << 2));
        high += (low << 1);
        low ^= (high >> 3);
        low ^= low >> 35;
        low *= prime_mx2_;
        low ^= low >> 28;
        high = XXH3Avalanche(high);
        return {low, high};
      }

      const uint64_t bitflipl = (Read64(secret, 32) ^ Read64(secret, 40)) - seed;
      const uint64_t bitfliph = (Read64(secret, 48) ^ Read64(secret, 56)) + seed;
      const uint64_t input_lo = Read64(ptr);
      uint64_t input_hi = Read64(ptr + size - 8);
      auto [low, high] = Mult64To128(input_lo ^ input_hi ^ bitflipl, prime1_);
      low += static_cast<uint64_t>(size - 1) << 54;
      input_hi ^= bitfliph;
      high += input_hi + (static_cast<uint64_t>(static_cast<uint32_t>(input_hi)) * (prime32_2_ - 1));
      low ^= std::byteswap(high);
      auto [result_low, result_high_base] = Mult64To128(low, prime2_);
      const uint64_t result_high = result_high_base + (high * prime2_);
      return {XXH3Avalanche(result_low), XXH3Avalanche(result_high)};
    }

    static std::pair<uint64_t, uint64_t> Hash17To128(
      std::span<const std::byte> bytes,
      std::span<const std::byte, secret_size_> secret,
      uint64_t seed
    ) {
      const std::size_t size = bytes.size();
      const std::byte* const ptr = bytes.data();

      uint64_t low = static_cast<uint64_t>(size) * prime1_;
      uint64_t high = 0;

      if (size > 32) {
        if (size > 64) {
          if (size > 96) {
            std::tie(low, high) = Mix32(low, high, ptr + 48, ptr + size - 64, secret, 96, seed);
          }
          std::tie(low, high) = Mix32(low, high, ptr + 32, ptr + size - 48, secret, 64, seed);
        }
        std::tie(low, high) = Mix32(low, high, ptr + 16, ptr + size - 32, secret, 32, seed);
      }
      std::tie(low, high) = Mix32(low, high, ptr, ptr + size - 16, secret, 0, seed);

      const uint64_t acc_low = low;
      const uint64_t acc_high = high;
      low = XXH3Avalanche(acc_low + acc_high);
      high = 0 - XXH3Avalanche(
        (acc_low * prime1_) +
        (acc_high * prime4_) +
        ((static_cast<uint64_t>(size) - seed) * prime2_)
      );
      return {low, high};
    }

    static std::pair<uint64_t, uint64_t> Hash129To240(
      std::span<const std::byte> bytes,
      std::span<const std::byte, secret_size_> secret,
      uint64_t seed
    ) {
      const std::size_t size = bytes.size();
      const std::byte* const ptr = bytes.data();

      uint64_t low = static_cast<uint64_t>(size) * prime1_;
      uint64_t high = 0;

      for (std::size_t offset = 32; offset < 160; offset += 32) {
        std::tie(low, high) = Mix32(low, high, ptr + offset - 32, ptr + offset - 16, secret, offset - 32, seed);
      }

      low = XXH3Avalanche(low);
      high = XXH3Avalanche(high);

      for (std::size_t offset = 160; offset <= size; offset += 32) {
        std::tie(low, high) = Mix32(
          low,
          high,
          ptr + offset - 32,
          ptr + offset - 16,
          secret,
          midsize_start_offset_ + offset - 160,
          seed
        );
      }

      std::tie(low, high) = Mix32(
        low,
        high,
        ptr + size - 16,
        ptr + size - 32,
        secret,
        secret_size_min_ - midsize_last_offset_ - 16,
        0 - seed
      );

      const uint64_t acc_low = low;
      const uint64_t acc_high = high;
      low = XXH3Avalanche(acc_low + acc_high);
      high = 0 - XXH3Avalanche(
        (acc_low * prime1_) +
        (acc_high * prime4_) +
        ((static_cast<uint64_t>(size) - seed) * prime2_)
      );
      return {low, high};
    }

    static WARBLE_NOINLINE std::pair<uint64_t, uint64_t> HashLong(
      std::span<const std::byte> bytes,
      std::span<const std::byte, secret_size_> secret
    ) {
      switch (SelectVectorPath()) {
#if defined(_MSC_VER) && defined(_M_X64)
        case VectorPath::Sse2:
          return HashLongImpl<AccumulateSse2, ScrambleSse2>(bytes, secret);
#if defined(__AVX2__)
        case VectorPath::Avx2:
          return HashLongImpl<AccumulateAvx2, ScrambleAvx2>(bytes, secret);
#endif
#endif
        case VectorPath::Scalar:
          return HashLongImpl<AccumulateScalar, ScrambleScalar>(bytes, secret);
      }
      return HashLongImpl<AccumulateScalar, ScrambleScalar>(bytes, secret);
    }

    static uint64_t MakeProcessSeed() {
      uint64_t seed = 0;

      auto Mix = [&seed](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        seed ^= static_cast<uint64_t>(std::hash<T>{}(value)) + prime1_ + (seed << 6) + (seed >> 2);
      };

      std::random_device device{};
      Mix(device());
      Mix(device());
      Mix(std::chrono::high_resolution_clock::now().time_since_epoch().count());
      Mix(std::this_thread::get_id());
      return XXH64Avalanche(seed);
    }

    static std::pair<uint64_t, uint64_t> Compute(std::span<const std::byte> bytes, uint64_t seed) {
      const std::size_t size = bytes.size();
      const auto base_secret = std::span<const std::byte, secret_size_>{BaseSecret().data(), secret_size_};

      if (size <= 16) {
        return Hash0To16(bytes, base_secret, seed);
      }

      if (size <= 128) {
        return Hash17To128(bytes, base_secret, seed);
      }

      if (size <= 240) {
        return Hash129To240(bytes, base_secret, seed);
      }

      const auto selected = SelectSecret(seed);
      return HashLong(bytes, selected.secret);
    }

    uint64_t low_ = 0;
    uint64_t high_ = 0;
  public:
    Hash128() = default;

    explicit Hash128(std::span<const std::byte> bytes) {
      auto [low, high] = Compute(bytes, ProcessSeed());
      low_ = low;
      high_ = high;
    }

    Hash128(std::span<const std::byte> bytes, uint64_t seed) {
      auto [low, high] = Compute(bytes, seed);
      low_ = low;
      high_ = high;
    }

    template <class T, std::size_t Extent>
      requires std::is_trivially_copyable_v<T>
    explicit Hash128(std::span<const T, Extent> s)
      : Hash128(std::as_bytes(s)) {}

    template <class T, std::size_t Extent>
      requires std::is_trivially_copyable_v<T>
    Hash128(std::span<const T, Extent> s, uint64_t seed)
      : Hash128(std::as_bytes(s), seed) {}

    template <std::ranges::contiguous_range R>
      requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
    explicit Hash128(const R& r)
      : Hash128(std::span{r}) {}

    template <std::ranges::contiguous_range R>
      requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
    Hash128(const R& r, uint64_t seed)
      : Hash128(std::span{r}, seed) {}

    [[nodiscard]] static uint64_t ProcessSeed() {
      static const uint64_t process_seed = MakeProcessSeed();
      return process_seed;
    }

    [[nodiscard]] static Hash128 FromBytes(std::span<const std::byte> bytes) {
      return Hash128{bytes, ProcessSeed()};
    }

    [[nodiscard]] static Hash128 FromBytes(std::span<const std::byte> bytes, uint64_t seed) {
      return Hash128{bytes, seed};
    }

    template <class T, std::size_t Extent>
      requires std::is_trivially_copyable_v<T>
    [[nodiscard]] static Hash128 FromSpan(std::span<const T, Extent> s) {
      return Hash128{s};
    }

    template <class T, std::size_t Extent>
      requires std::is_trivially_copyable_v<T>
    [[nodiscard]] static Hash128 FromSpan(std::span<const T, Extent> s, uint64_t seed) {
      return Hash128{s, seed};
    }

    template <std::ranges::contiguous_range R>
      requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
    [[nodiscard]] static Hash128 FromRange(const R& r) {
      return Hash128{r};
    }

    template <std::ranges::contiguous_range R>
      requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
    [[nodiscard]] static Hash128 FromRange(const R& r, uint64_t seed) {
      return Hash128{r, seed};
    }

    [[nodiscard]] uint64_t Low() const noexcept { return low_; }
    [[nodiscard]] uint64_t High() const noexcept { return high_; }
    [[nodiscard]] std::array<uint64_t, 2> Words() const noexcept { return {low_, high_}; }
    [[nodiscard]] std::pair<uint64_t, uint64_t> Pair() const noexcept { return {low_, high_}; }

    auto operator<=>(const Hash128&) const = default;
  };
}