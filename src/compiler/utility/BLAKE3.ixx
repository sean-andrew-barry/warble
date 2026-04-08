export module compiler.utility.BLAKE3;

import <array>;
import <bit>;
import <cstddef>;
import <cstdint>;
import <cstring>;
import <ranges>;
import <span>;
import <stdexcept>;
import <type_traits>;

#if defined(_MSC_VER) && defined(_M_X64)
import <emmintrin.h>;
#endif

#if defined(_MSC_VER)
#define WARBLE_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define WARBLE_FORCEINLINE __attribute__((always_inline)) inline
#else
#define WARBLE_FORCEINLINE inline
#endif

namespace compiler::utility {
	export class BLAKE3 {
	private:
		static constexpr std::size_t key_size_ = 32;
		static constexpr std::size_t output_size_ = 32;
		static constexpr std::size_t block_size_ = 64;
		static constexpr std::size_t chunk_size_ = 1024;

		enum class VectorPath : uint8_t {
			Scalar,
			Sse2,
		};

		enum Flags : uint32_t {
			chunk_start_ = 1u << 0,
			chunk_end_ = 1u << 1,
			parent_ = 1u << 2,
			root_ = 1u << 3,
			keyed_hash_ = 1u << 4,
		};

		struct Output {
			std::array<uint32_t, 8> input_chaining_value;
			std::array<uint32_t, 16> block_words;
			uint64_t counter = 0;
			uint32_t block_length = 0;
			uint32_t flags = 0;
		};

		static constexpr std::array<uint32_t, 8> initialization_vector_ = {
			0x6A09E667u,
			0xBB67AE85u,
			0x3C6EF372u,
			0xA54FF53Au,
			0x510E527Fu,
			0x9B05688Cu,
			0x1F83D9ABu,
			0x5BE0CD19u,
		};

		static constexpr std::array<std::array<uint8_t, 16>, 7> message_schedule_ = {{
			{{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }},
			{{ 2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8 }},
			{{ 3, 4, 10, 12, 13, 2, 7, 14, 6, 5, 9, 0, 11, 15, 8, 1 }},
			{{ 10, 7, 12, 9, 14, 3, 13, 15, 4, 0, 11, 2, 5, 8, 1, 6 }},
			{{ 12, 13, 9, 11, 15, 10, 14, 8, 7, 2, 5, 3, 0, 1, 6, 4 }},
			{{ 9, 14, 11, 5, 8, 12, 15, 1, 13, 3, 0, 10, 2, 6, 4, 7 }},
			{{ 11, 15, 5, 0, 1, 9, 8, 6, 14, 10, 2, 12, 3, 4, 7, 13 }},
		}};

		std::array<uint32_t, 8> key_words_{};

		static VectorPath SelectVectorPath() {
#if defined(_MSC_VER) && defined(_M_X64)
			return VectorPath::Sse2;
#else
			return VectorPath::Scalar;
#endif
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

		static uint64_t Read64(const std::byte* data) {
			uint64_t value = 0;
			std::memcpy(&value, data, sizeof(value));
			if constexpr (std::endian::native == std::endian::little) {
				return value;
			} else {
				return std::byteswap(value);
			}
		}

		static void Write32(std::byte* data, uint32_t value) {
			if constexpr (std::endian::native == std::endian::little) {
				std::memcpy(data, &value, sizeof(value));
			} else {
				const uint32_t swapped = std::byteswap(value);
				std::memcpy(data, &swapped, sizeof(swapped));
			}
		}

		static std::array<uint32_t, 16> LoadBlockWordsPadded(std::span<const std::byte> block) {
			std::array<std::byte, block_size_> padded_block{};
			std::memcpy(padded_block.data(), block.data(), block.size());

			std::array<uint32_t, 16> words{};
			for (std::size_t index = 0; index < words.size(); ++index) {
				words[index] = Read32(padded_block.data() + (index * sizeof(uint32_t)));
			}
			return words;
		}

		static std::array<uint32_t, 16> LoadBlockWordsFull(const std::byte* block) {
			std::array<uint32_t, 16> words{};
			for (std::size_t index = 0; index < words.size(); ++index) {
				words[index] = Read32(block + (index * sizeof(uint32_t)));
			}
			return words;
		}

		static WARBLE_FORCEINLINE void Mix(
			std::array<uint32_t, 16>& state,
			std::size_t a,
			std::size_t b,
			std::size_t c,
			std::size_t d,
			uint32_t x,
			uint32_t y
		) {
			state[a] = state[a] + state[b] + x;
			state[d] = std::rotr(state[d] ^ state[a], 16);
			state[c] += state[d];
			state[b] = std::rotr(state[b] ^ state[c], 12);
			state[a] = state[a] + state[b] + y;
			state[d] = std::rotr(state[d] ^ state[a], 8);
			state[c] += state[d];
			state[b] = std::rotr(state[b] ^ state[c], 7);
		}

		static WARBLE_FORCEINLINE std::array<uint32_t, 16> CompressScalar(
			const std::array<uint32_t, 8>& chaining_value,
			const std::array<uint32_t, 16>& block_words,
			uint64_t counter,
			uint32_t block_length,
			uint32_t flags
		) {
			std::array<uint32_t, 16> state = {
				chaining_value[0],
				chaining_value[1],
				chaining_value[2],
				chaining_value[3],
				chaining_value[4],
				chaining_value[5],
				chaining_value[6],
				chaining_value[7],
				initialization_vector_[0],
				initialization_vector_[1],
				initialization_vector_[2],
				initialization_vector_[3],
				static_cast<uint32_t>(counter),
				static_cast<uint32_t>(counter >> 32),
				block_length,
				flags,
			};

			for (const auto& schedule : message_schedule_) {
				Mix(state, 0, 4, 8, 12, block_words[schedule[0]], block_words[schedule[1]]);
				Mix(state, 1, 5, 9, 13, block_words[schedule[2]], block_words[schedule[3]]);
				Mix(state, 2, 6, 10, 14, block_words[schedule[4]], block_words[schedule[5]]);
				Mix(state, 3, 7, 11, 15, block_words[schedule[6]], block_words[schedule[7]]);
				Mix(state, 0, 5, 10, 15, block_words[schedule[8]], block_words[schedule[9]]);
				Mix(state, 1, 6, 11, 12, block_words[schedule[10]], block_words[schedule[11]]);
				Mix(state, 2, 7, 8, 13, block_words[schedule[12]], block_words[schedule[13]]);
				Mix(state, 3, 4, 9, 14, block_words[schedule[14]], block_words[schedule[15]]);
			}

			for (std::size_t index = 0; index < 8; ++index) {
				state[index] ^= state[index + 8];
				state[index + 8] ^= chaining_value[index];
			}

			return state;
		}

#if defined(_MSC_VER) && defined(_M_X64)
		static WARBLE_FORCEINLINE __m128i SseLoad(const void* src) {
			return _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
		}

		static WARBLE_FORCEINLINE void SseStore(__m128i value, void* dst) {
			_mm_storeu_si128(reinterpret_cast<__m128i*>(dst), value);
		}

		static WARBLE_FORCEINLINE __m128i SseSet4(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
			return _mm_setr_epi32(
				static_cast<int32_t>(a),
				static_cast<int32_t>(b),
				static_cast<int32_t>(c),
				static_cast<int32_t>(d)
			);
		}

		static WARBLE_FORCEINLINE __m128i SseRot16(__m128i value) {
			return _mm_shufflehi_epi16(_mm_shufflelo_epi16(value, 0xB1), 0xB1);
		}

		static WARBLE_FORCEINLINE __m128i SseRot12(__m128i value) {
			return _mm_or_si128(_mm_srli_epi32(value, 12), _mm_slli_epi32(value, 20));
		}

		static WARBLE_FORCEINLINE __m128i SseRot8(__m128i value) {
			return _mm_or_si128(_mm_srli_epi32(value, 8), _mm_slli_epi32(value, 24));
		}

		static WARBLE_FORCEINLINE __m128i SseRot7(__m128i value) {
			return _mm_or_si128(_mm_srli_epi32(value, 7), _mm_slli_epi32(value, 25));
		}

		static WARBLE_FORCEINLINE void SseG1(
			__m128i& row0,
			__m128i& row1,
			__m128i& row2,
			__m128i& row3,
			__m128i message
		) {
			row0 = _mm_add_epi32(_mm_add_epi32(row0, message), row1);
			row3 = _mm_xor_si128(row3, row0);
			row3 = SseRot16(row3);
			row2 = _mm_add_epi32(row2, row3);
			row1 = _mm_xor_si128(row1, row2);
			row1 = SseRot12(row1);
		}

		static WARBLE_FORCEINLINE void SseG2(
			__m128i& row0,
			__m128i& row1,
			__m128i& row2,
			__m128i& row3,
			__m128i message
		) {
			row0 = _mm_add_epi32(_mm_add_epi32(row0, message), row1);
			row3 = _mm_xor_si128(row3, row0);
			row3 = SseRot8(row3);
			row2 = _mm_add_epi32(row2, row3);
			row1 = _mm_xor_si128(row1, row2);
			row1 = SseRot7(row1);
		}

		static WARBLE_FORCEINLINE void SseDiagonalize(__m128i& row1, __m128i& row2, __m128i& row3) {
			row1 = _mm_shuffle_epi32(row1, _MM_SHUFFLE(0, 3, 2, 1));
			row2 = _mm_shuffle_epi32(row2, _MM_SHUFFLE(1, 0, 3, 2));
			row3 = _mm_shuffle_epi32(row3, _MM_SHUFFLE(2, 1, 0, 3));
		}

		static WARBLE_FORCEINLINE void SseUndiagonalize(__m128i& row1, __m128i& row2, __m128i& row3) {
			row1 = _mm_shuffle_epi32(row1, _MM_SHUFFLE(2, 1, 0, 3));
			row2 = _mm_shuffle_epi32(row2, _MM_SHUFFLE(1, 0, 3, 2));
			row3 = _mm_shuffle_epi32(row3, _MM_SHUFFLE(0, 3, 2, 1));
		}

		static WARBLE_FORCEINLINE void SseRound(
			__m128i& row0,
			__m128i& row1,
			__m128i& row2,
			__m128i& row3,
			const std::array<uint32_t, 16>& block_words,
			const std::array<uint8_t, 16>& schedule
		) {
			const __m128i m0 = SseSet4(
				block_words[schedule[0]],
				block_words[schedule[2]],
				block_words[schedule[4]],
				block_words[schedule[6]]
			);
			const __m128i m1 = SseSet4(
				block_words[schedule[1]],
				block_words[schedule[3]],
				block_words[schedule[5]],
				block_words[schedule[7]]
			);
			SseG1(row0, row1, row2, row3, m0);
			SseG2(row0, row1, row2, row3, m1);

			SseDiagonalize(row1, row2, row3);

			const __m128i m2 = SseSet4(
				block_words[schedule[8]],
				block_words[schedule[10]],
				block_words[schedule[12]],
				block_words[schedule[14]]
			);
			const __m128i m3 = SseSet4(
				block_words[schedule[9]],
				block_words[schedule[11]],
				block_words[schedule[13]],
				block_words[schedule[15]]
			);
			SseG1(row0, row1, row2, row3, m2);
			SseG2(row0, row1, row2, row3, m3);

			SseUndiagonalize(row1, row2, row3);
		}

		static WARBLE_FORCEINLINE void CompressSse2Rows(
			__m128i& row0,
			__m128i& row1,
			__m128i& row2,
			__m128i& row3,
			const std::array<uint32_t, 8>& chaining_value,
			const std::array<uint32_t, 16>& block_words,
			uint64_t counter,
			uint32_t block_length,
			uint32_t flags
		) {
			row0 = SseLoad(chaining_value.data() + 0);
			row1 = SseLoad(chaining_value.data() + 4);
			row2 = SseLoad(initialization_vector_.data());
			row3 = SseSet4(
				static_cast<uint32_t>(counter),
				static_cast<uint32_t>(counter >> 32),
				block_length,
				flags
			);

			for (const auto& schedule : message_schedule_) {
				SseRound(row0, row1, row2, row3, block_words, schedule);
			}
		}

		static std::array<uint32_t, 8> ChainingValueSse2(const Output& output) {
			__m128i row0{};
			__m128i row1{};
			__m128i row2{};
			__m128i row3{};
			CompressSse2Rows(
				row0,
				row1,
				row2,
				row3,
				output.input_chaining_value,
				output.block_words,
				output.counter,
				output.block_length,
				output.flags
			);

			std::array<uint32_t, 8> chaining_value{};
			SseStore(_mm_xor_si128(row0, row2), chaining_value.data() + 0);
			SseStore(_mm_xor_si128(row1, row3), chaining_value.data() + 4);
			return chaining_value;
		}

		static std::array<std::byte, output_size_> RootBytesSse2(const Output& output) {
			__m128i row0{};
			__m128i row1{};
			__m128i row2{};
			__m128i row3{};
			CompressSse2Rows(
				row0,
				row1,
				row2,
				row3,
				output.input_chaining_value,
				output.block_words,
				output.counter,
				output.block_length,
				output.flags | root_
			);

			std::array<std::byte, output_size_> bytes{};
			alignas(16) std::array<uint32_t, 4> temp_words{};

			SseStore(_mm_xor_si128(row0, row2), temp_words.data());
			for (std::size_t index = 0; index < 4; ++index) {
				Write32(bytes.data() + (index * sizeof(uint32_t)), temp_words[index]);
			}

			SseStore(_mm_xor_si128(row1, row3), temp_words.data());
			for (std::size_t index = 0; index < 4; ++index) {
				Write32(bytes.data() + ((index + 4) * sizeof(uint32_t)), temp_words[index]);
			}

			return bytes;
		}
#endif

		static std::array<uint32_t, 8> ChainingValue(const Output& output) {
			switch (SelectVectorPath()) {
#if defined(_MSC_VER) && defined(_M_X64)
				case VectorPath::Sse2:
					return ChainingValueSse2(output);
#endif
				case VectorPath::Scalar: {
					const auto state = CompressScalar(
						output.input_chaining_value,
						output.block_words,
						output.counter,
						output.block_length,
						output.flags
					);

					std::array<uint32_t, 8> chaining_value{};
					for (std::size_t index = 0; index < chaining_value.size(); ++index) {
						chaining_value[index] = state[index];
					}
					return chaining_value;
				}
			}

			return {};
		}

		static std::array<std::byte, output_size_> RootBytes(const Output& output) {
			switch (SelectVectorPath()) {
#if defined(_MSC_VER) && defined(_M_X64)
				case VectorPath::Sse2:
					return RootBytesSse2(output);
#endif
				case VectorPath::Scalar: {
					const auto state = CompressScalar(
						output.input_chaining_value,
						output.block_words,
						output.counter,
						output.block_length,
						output.flags | root_
					);

					std::array<std::byte, output_size_> bytes{};
					for (std::size_t index = 0; index < output_size_ / sizeof(uint32_t); ++index) {
						Write32(bytes.data() + (index * sizeof(uint32_t)), state[index]);
					}
					return bytes;
				}
			}

			return {};
		}

		static Output ParentOutput(
			const std::array<uint32_t, 8>& left_child,
			const std::array<uint32_t, 8>& right_child,
			const std::array<uint32_t, 8>& key_words,
			uint32_t flags
		) {
			std::array<uint32_t, 16> block_words{};
			for (std::size_t index = 0; index < 8; ++index) {
				block_words[index] = left_child[index];
				block_words[index + 8] = right_child[index];
			}

			return {
				.input_chaining_value = key_words,
				.block_words = block_words,
				.counter = 0,
				.block_length = block_size_,
				.flags = flags | parent_,
			};
		}

		static Output HashChunk(
			std::span<const std::byte> chunk,
			uint64_t chunk_counter,
			const std::array<uint32_t, 8>& key_words,
			uint32_t flags
		) {
			std::array<uint32_t, 8> chaining_value = key_words;
			std::size_t compressed_blocks = 0;

			while (chunk.size() > block_size_) {
				const auto block_words = LoadBlockWordsFull(chunk.data());
				const uint32_t block_flags = flags | (compressed_blocks == 0 ? chunk_start_ : 0u);
				chaining_value = ChainingValue({
					.input_chaining_value = chaining_value,
					.block_words = block_words,
					.counter = chunk_counter,
					.block_length = block_size_,
					.flags = block_flags,
				});
				chunk = chunk.subspan(block_size_);
				++compressed_blocks;
			}

			const auto last_block_words =
				chunk.size() == block_size_
					? LoadBlockWordsFull(chunk.data())
					: LoadBlockWordsPadded(chunk);

			return {
				.input_chaining_value = chaining_value,
				.block_words = last_block_words,
				.counter = chunk_counter,
				.block_length = static_cast<uint32_t>(chunk.size()),
				.flags = flags | chunk_end_ | (compressed_blocks == 0 ? chunk_start_ : 0u),
			};
		}

		static void PushChainingValue(
			std::array<uint32_t, 8> chaining_value,
			uint64_t total_chunks,
			const std::array<uint32_t, 8>& key_words,
			uint32_t flags,
			std::array<std::array<uint32_t, 8>, 64>& cv_stack,
			std::size_t& cv_stack_size
		) {
			while ((total_chunks & 1u) == 0u) {
				chaining_value = ChainingValue(ParentOutput(
					cv_stack[cv_stack_size - 1],
					chaining_value,
					key_words,
					flags
				));
				--cv_stack_size;
				total_chunks >>= 1;
			}

			cv_stack[cv_stack_size] = chaining_value;
			++cv_stack_size;
		}

		static std::array<uint64_t, 4> ToWords(std::span<const std::byte, output_size_> bytes) {
			return {
				Read64(bytes.data() + 0),
				Read64(bytes.data() + 8),
				Read64(bytes.data() + 16),
				Read64(bytes.data() + 24),
			};
		}

		void InitializeKey(std::span<const std::byte> key) {
			if (key.size() != key_size_) {
				throw std::invalid_argument("BLAKE3 key must be exactly 32 bytes");
			}

			for (std::size_t index = 0; index < key_words_.size(); ++index) {
				key_words_[index] = Read32(key.data() + (index * sizeof(uint32_t)));
			}
		}
	public:
		explicit BLAKE3(std::span<const std::byte, key_size_> key) {
			InitializeKey(key);
		}

		explicit BLAKE3(const std::array<std::byte, key_size_>& key) {
			InitializeKey(std::span<const std::byte, key_size_>{key});
		}

		template <class T, std::size_t Extent>
			requires std::is_trivially_copyable_v<T>
		explicit BLAKE3(std::span<const T, Extent> key) {
			InitializeKey(std::as_bytes(key));
		}

		template <std::ranges::contiguous_range R>
			requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
		explicit BLAKE3(const R& key) {
			InitializeKey(std::as_bytes(std::span{key}));
		}

		[[nodiscard]] std::array<std::byte, output_size_> HashBytes(std::span<const std::byte> bytes) const {
			std::array<std::array<uint32_t, 8>, 64> cv_stack{};
			std::size_t cv_stack_size = 0;
			uint64_t chunk_counter = 0;
			constexpr uint32_t flags = keyed_hash_;

			while (bytes.size() > chunk_size_) {
				const Output chunk_output = HashChunk(bytes.first(chunk_size_), chunk_counter, key_words_, flags);
				PushChainingValue(
					ChainingValue(chunk_output),
					chunk_counter + 1,
					key_words_,
					flags,
					cv_stack,
					cv_stack_size
				);
				bytes = bytes.subspan(chunk_size_);
				++chunk_counter;
			}

			Output output = HashChunk(bytes, chunk_counter, key_words_, flags);
			while (cv_stack_size > 0) {
				output = ParentOutput(cv_stack[cv_stack_size - 1], ChainingValue(output), key_words_, flags);
				--cv_stack_size;
			}

			return RootBytes(output);
		}

		[[nodiscard]] std::array<uint64_t, 4> Hash(std::span<const std::byte> bytes) const {
			const auto digest = HashBytes(bytes);
			return ToWords(std::span<const std::byte, output_size_>{digest});
		}

		template <class T, std::size_t Extent>
			requires std::is_trivially_copyable_v<T>
		[[nodiscard]] std::array<std::byte, output_size_> HashBytes(std::span<const T, Extent> bytes) const {
			return HashBytes(std::as_bytes(bytes));
		}

		template <class T, std::size_t Extent>
			requires std::is_trivially_copyable_v<T>
		[[nodiscard]] std::array<uint64_t, 4> Hash(std::span<const T, Extent> bytes) const {
			return Hash(std::as_bytes(bytes));
		}

		template <std::ranges::contiguous_range R>
			requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
		[[nodiscard]] std::array<std::byte, output_size_> HashBytes(const R& bytes) const {
			return HashBytes(std::span{bytes});
		}

		template <std::ranges::contiguous_range R>
			requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
		[[nodiscard]] std::array<uint64_t, 4> Hash(const R& bytes) const {
			return Hash(std::span{bytes});
		}
	};
}
