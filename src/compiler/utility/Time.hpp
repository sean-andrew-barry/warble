#pragma once

#include "compiler/utility/Units.hpp"
#include <chrono>
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

namespace compiler::utility {
  template<typename Source>
  class Clock {
  private:
    const typename Source::time_point start_time;
  public:
    Clock() : start_time{Source::now()} {}

    Nano Now() const { return Nano{Source::now()}; }
    Nano Lifetime() const { return Nano{Source::now() - start_time}; }
  };

  namespace System {
    Clock<std::chrono::system_clock> clock{};

    auto Lifetime() { return clock.Lifetime(); }
    auto Now() { return clock.Now(); }
  };

  namespace Steady {
    Clock<std::chrono::steady_clock> clock{};

    auto Lifetime() { return clock.Lifetime(); }
    auto Now() { return clock.Now(); }
  };

  namespace Precise {
    Clock<std::chrono::high_resolution_clock> clock{};

    auto Lifetime() { return clock.Lifetime(); }
    auto Now() { return clock.Now(); }
  };

  uint64_t FencedTSC() {
    _mm_lfence();
    return __rdtsc();
  }

  uint64_t TSC() {
    return __rdtsc();
  }

  namespace Cycles {
    const uint64_t start_time = TSC();

    uint64_t Lifetime() { return TSC() - start_time; }
    uint64_t Now() { return TSC(); }
  };
};
