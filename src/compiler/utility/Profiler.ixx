export module utility.profiler;

import utility.units;
import utility.time;
import utility.print;

import <vector>;
import <chrono>;
import <string>;
import <utility>;
import <functional>;
import <memory>;

namespace utility {
  export class Profiler {
  private:
    struct Result {
      std::string name;
      std::function<void()> function;
      std::function<void()> cleanup;
      size_t iterations;
      size_t cycles;
      Nano time;
    };

    std::vector<std::shared_ptr<Result>> results;
    size_t iterations;
    size_t batch_size = 1000;
    Nano time;
  private:
    void Display() {
      for (auto& result : results) {
        utility::Print("----- Function \"" + result->name + "\" Results -----");
        utility::Print(result->iterations, "were done in", result->time.Seconds(), "second(s).");
        utility::Print("An average of:", ((result->iterations / result->time.Seconds()) / 1000.0), "per millisecond.\n");
      }
    }

    void Loop(std::shared_ptr<Result> result, auto function) {
      if (result->cleanup) {
        result->cleanup();
      }

      auto total = result->time;
      auto start = Precise::Now();

      for (size_t i = 0; i < batch_size; ++i) {
        function();
      }

      auto stop = Precise::Now();
      auto elapsed = stop - start;

      result->time = result->time + elapsed;
      result->iterations += batch_size;
    }
  public:
    Profiler(Nano time = 1_s)
      : time{time} {}

    ~Profiler() {}

    void Run() {
      do {
        size_t done = 0;
        for (auto& result : results) {
          if (time > result->time) {
            result->function();
          } else {
            done += 1;
          }
        }

        if (done == results.size()) break;
      } while (true);

      Display();
    }

    void Add(const std::string& name, auto function, auto cleanup) {
      auto result = std::make_shared<Result>();
      result->name = name;
      result->function = [=](){ Loop(result, function); };
      result->cleanup = cleanup;
      results.push_back(result);
    }

    void Add(const std::string& name, auto function) {
      auto result = std::make_shared<Result>();
      result->name = name;
      result->function = [=](){ Loop(result, function); };
      results.push_back(result);
    }

    void Add(auto fn) {
      return Add(std::to_string(results.size() + 1), fn);
    }
  };
};
