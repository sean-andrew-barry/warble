export module compiler.utility.Profiler;

import compiler.utility.Units;
import compiler.utility.Time;
import compiler.utility.Print;

import <atomic>;
import <vector>;
import <chrono>;
import <string>;
import <utility>;
import <functional>;
import <limits>;
import <algorithm>;
import <memory>;
import <type_traits>;

namespace compiler::utility {
  export class Profiler {
  private:
    struct Result {
      std::string name;
      std::function<void()> function;
      std::function<void()> setup;
      size_t batch_size = 1;
      size_t iterations = 0;
      Nano time{};
      std::vector<Nano> samples;
    };

    Nano time;
    Nano target_batch_time;
    size_t initial_batch_size;
    std::vector<Result> results;
  private:
    Nano Median(std::vector<Nano> samples) const {
      if (samples.empty()) {
        return Nano{};
      }

      std::sort(samples.begin(), samples.end());

      auto middle = samples.size() / 2;
      if ((samples.size() % 2) == 0) {
        return Nano{(samples[middle - 1].Nanoseconds() + samples[middle].Nanoseconds()) / 2.0L};
      }

      return samples[middle];
    }

    void Display() {
      for (auto& result : results) {
        utility::Print("----- Function \"" + result.name + "\" Results -----");
        utility::Print(result.iterations, " were done in ", result.time.Seconds(), " second(s).");
        utility::Print("An average of: ", (result.time.Nanoseconds() / static_cast<long double>(result.iterations)), " nanoseconds per call.");
        utility::Print("Batch size: ", result.batch_size);

        if (!result.samples.empty()) {
          auto minimum = *std::min_element(result.samples.begin(), result.samples.end());
          auto maximum = *std::max_element(result.samples.begin(), result.samples.end());
          auto median = Median(result.samples);
          auto per_call = [&](const Nano sample) {
            return sample.Nanoseconds() / static_cast<long double>(result.batch_size);
          };

          utility::Print(
            "Minimum / median / maximum: ",
            per_call(minimum), ", ",
            per_call(median), ", ",
            per_call(maximum),
            " nanoseconds per call."
          );
        }

        utility::Print(); // Newline for separation, ideally should only print if there are more results to show, but it's not a big deal if there's an extra newline at the end.
      }
    }

    Nano ExecuteBatch(Result& result) {
      if (result.setup) {
        result.setup();
      }

      auto start = Steady::Now();

      for (size_t i = 0; i < result.batch_size; ++i) {
        result.function();
      }

      auto stop = Steady::Now();
      return stop - start;
    }

    void Measure(Result& result) {
      auto elapsed = ExecuteBatch(result);

      result.time = result.time + elapsed;
      result.iterations += result.batch_size;
      result.samples.push_back(elapsed);
    }

    size_t GrowthFactor(const Nano elapsed) const {
      auto elapsed_ns = elapsed.Nanoseconds();
      auto target_ns = target_batch_time.Nanoseconds();

      if (elapsed_ns <= 0.0L) {
        return 2;
      }

      size_t factor = 1;
      auto max_factor = std::numeric_limits<size_t>::max() / 2;

      while (factor <= max_factor && (elapsed_ns * static_cast<long double>(factor)) < target_ns) {
        factor <<= 1;
      }

      return factor;
    }

    void Calibrate(Result& result) {
      result.batch_size = initial_batch_size;

      while (true) {
        auto elapsed = ExecuteBatch(result);

        if (elapsed >= target_batch_time) {
          break;
        }

        auto factor = GrowthFactor(elapsed);
        auto max_batch_size = std::numeric_limits<size_t>::max();
        // auto next_batch_size = (factor > 0 && result.batch_size <= (max_batch_size / factor))
        //   ? (result.batch_size * factor)
        //   : max_batch_size;

        // utility::Print(
        //   "Calibrating \"", result.name, "\": elapsed ", elapsed.Milliseconds(), " ms; ",
        //   "increasing batch size by a factor of ", factor, " to ", next_batch_size, "..."
        // );

        if (factor <= 1 || result.batch_size > (max_batch_size / factor)) {
          result.batch_size = max_batch_size;
          break;
        }

        result.batch_size *= factor;
      }

      auto expected_samples = static_cast<size_t>(time.Nanoseconds() / target_batch_time.Nanoseconds()) + 1;
      result.samples.reserve(expected_samples);
    }
  public:
    Profiler(Nano time = 1_s, Nano target_batch_time = 250_us, size_t initial_batch_size = 1)
      : time{time > 0_ns ? time : 1_ns}
      , target_batch_time{target_batch_time > 0_ns ? target_batch_time : 1_ns}
      , initial_batch_size{initial_batch_size > 0 ? initial_batch_size : 1} {}

    ~Profiler() {}

    template<typename T>
      requires std::is_object_v<std::remove_reference_t<T>>
    static void DoNotOptimize(T&& value) {
      auto* bytes = reinterpret_cast<const volatile unsigned char*>(std::addressof(value));
      std::atomic_signal_fence(std::memory_order_seq_cst);
      (void)*bytes;
      std::atomic_signal_fence(std::memory_order_seq_cst);
    }

    static void ClobberMemory() {
      std::atomic_signal_fence(std::memory_order_seq_cst);
    }

    void Run() {
      // utility::Print("Starting profiler with a target time of ", time.Seconds(), " second(s) and a target batch time of ", target_batch_time.Milliseconds(), " millisecond(s).");
      for (auto& result : results) {
        Calibrate(result);
      }

      // utility::Print("Calibration complete. Running benchmarks...");

      do {
        size_t done = 0;
        for (auto& result : results) {
          if (time > result.time) {
            Measure(result);
          } else {
            done += 1;
          }
        }

        if (done == results.size()) break;
      } while (true);

      Display();
    }

    void Add(const std::string& name, auto function, auto setup) {
      Result result{};
      result.name = name;
      result.function = std::move(function);
      result.setup = std::move(setup);
      result.batch_size = initial_batch_size;
      results.push_back(std::move(result));
    }

    void Add(const std::string& name, auto function) {
      Result result{};
      result.name = name;
      result.function = std::move(function);
      result.batch_size = initial_batch_size;
      results.push_back(std::move(result));
    }

    void Add(auto fn) {
      return Add(std::to_string(results.size() + 1), fn);
    }
  };
};
