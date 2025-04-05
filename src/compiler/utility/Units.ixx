export module utility.units;

import <string>;
import <compare>;
import <iostream>;
import <chrono>;

export namespace utility {
  constexpr uint64_t NANOSECONDS = 1;
  constexpr uint64_t MICROSECONDS = 1'000;
  constexpr uint64_t MILLISECONDS = 1'000'000;
  constexpr uint64_t SECONDS = 1'000'000'000;
  constexpr uint64_t MINUTES = 60'000'000'000;
  constexpr uint64_t HOURS = 3'600'000'000'000;
  constexpr uint64_t DAYS = 86'400'000'000'000;
  constexpr uint64_t WEEKS = 604'800'000'000'000;
  constexpr uint64_t YEARS = 31'557'600'000'000'000; // Approximating a year as 365.25 days for leap year accounting

  class Nano {
  private:
    int64_t nano;
  public:
    explicit constexpr Nano() : nano{0} {}
    explicit constexpr Nano(uint64_t ns) : nano{static_cast<int64_t>(ns)} {}
    explicit constexpr Nano(int64_t ns) : nano{ns} {}
    explicit constexpr Nano(long double ns) : nano{static_cast<int64_t>(ns)} {}

    // Constructor from std::chrono::time_point of any clock type
    template <class Clock, class Duration>
    explicit Nano(std::chrono::time_point<Clock, Duration> tp)
      : nano(std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count()) {}

    Nano(std::chrono::nanoseconds d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::microseconds d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::milliseconds d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::seconds d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::minutes d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::hours d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::days d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::weeks d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::months d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}
    Nano(std::chrono::years d) : nano{std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()} {}

    std::string ToString() const { 
      return std::to_string(Seconds()) + "s"; 
    }

    constexpr long double ToFloat() const { return static_cast<long double>(nano); }
    constexpr long double Nanoseconds() const { return ToFloat() / NANOSECONDS; }
    constexpr long double Microseconds() const { return ToFloat() / MICROSECONDS; }
    constexpr long double Milliseconds() const { return ToFloat() / MILLISECONDS; }
    constexpr long double Seconds() const { return ToFloat() / SECONDS; }
    constexpr long double Minutes() const { return ToFloat() / MINUTES; }
    constexpr long double Hours() const { return ToFloat() / HOURS; }
    constexpr long double Days() const { return ToFloat() / DAYS; }
    constexpr long double Weeks() const { return ToFloat() / WEEKS; }
    constexpr long double Years() const { return ToFloat() / YEARS; }

    constexpr Nano operator+(int64_t v) const { return Nano{nano + v}; }
    constexpr Nano operator-(int64_t v) const { return Nano{nano - v}; }
    constexpr Nano operator+(const Nano& s) const { return Nano{nano + s.nano}; }
    constexpr Nano operator-(const Nano& s) const { return Nano{nano - s.nano}; }
    auto operator<=>(const Nano& other) const = default;

    friend std::ostream& operator<<(std::ostream& os, const Nano& nano) {
      os << nano.ToString();
      return os;
    }

    static constexpr Nano Nanoseconds(int64_t v) { return Nano{v * NANOSECONDS}; }
    static constexpr Nano Microseconds(int64_t v) { return Nano{v * MICROSECONDS}; }
    static constexpr Nano Milliseconds(int64_t v) { return Nano{v * MILLISECONDS}; }
    static constexpr Nano Seconds(int64_t v) { return Nano{v * SECONDS}; }
    static constexpr Nano Minutes(int64_t v) { return Nano{v * MINUTES}; }
    static constexpr Nano Hours(int64_t v) { return Nano{v * HOURS}; }
    static constexpr Nano Days(int64_t v) { return Nano{v * DAYS}; }
    static constexpr Nano Weeks(int64_t v) { return Nano{v * WEEKS}; }
    static constexpr Nano Years(int64_t v) { return Nano{v * YEARS}; }
  };

  constexpr Nano operator"" _ns (uint64_t v) { return Nano{static_cast<int64_t>(v) * NANOSECONDS}; }
  constexpr Nano operator"" _us (uint64_t v) { return Nano{static_cast<int64_t>(v) * MICROSECONDS}; }
  constexpr Nano operator"" _ms (uint64_t v) { return Nano{static_cast<int64_t>(v) * MILLISECONDS}; }
  constexpr Nano operator"" _s  (uint64_t v) { return Nano{static_cast<int64_t>(v) * SECONDS}; }
  constexpr Nano operator"" _m  (uint64_t v) { return Nano{static_cast<int64_t>(v) * MINUTES}; }
  constexpr Nano operator"" _h  (uint64_t v) { return Nano{static_cast<int64_t>(v) * HOURS}; }
  constexpr Nano operator"" _d  (uint64_t v) { return Nano{static_cast<int64_t>(v) * DAYS}; }
  constexpr Nano operator"" _w  (uint64_t v) { return Nano{static_cast<int64_t>(v) * WEEKS}; }
  constexpr Nano operator"" _y  (uint64_t v) { return Nano{static_cast<int64_t>(v) * YEARS}; }

  constexpr Nano operator"" _ns (long double v) { return Nano{v * NANOSECONDS}; }
  constexpr Nano operator"" _us (long double v) { return Nano{v * MICROSECONDS}; }
  constexpr Nano operator"" _ms (long double v) { return Nano{v * MILLISECONDS}; }
  constexpr Nano operator"" _s  (long double v) { return Nano{v * SECONDS}; }
  constexpr Nano operator"" _m  (long double v) { return Nano{v * MINUTES}; }
  constexpr Nano operator"" _h  (long double v) { return Nano{v * HOURS}; }
  constexpr Nano operator"" _d  (long double v) { return Nano{v * DAYS}; }
  constexpr Nano operator"" _w  (long double v) { return Nano{v * WEEKS}; }
  constexpr Nano operator"" _y  (long double v) { return Nano{v * YEARS}; }

  // constexpr Nano operator"" _ns (uint64_t v) { return Nano{v}; }
  // constexpr Nano operator"" _us (uint64_t v) { return Nano{v / 1000}; }
  // constexpr Nano operator"" _ns (long double v){ return Nano{v * 0.000000001}; } // Nanoseconds
  // constexpr Nano operator"" _us (long double v){ return Nano{v * 0.000001}; } // Microseconds
  // constexpr Nano operator"" _ms (long double v){ return Nano{v * 0.001}; } // Milliseconds
  // constexpr Nano operator"" _s	(long double v){ return Nano{v}; } // Nano, the base unit
  // constexpr Nano operator"" _min(long double v){ return Nano{v * 60.0}; } // Minutes
  // constexpr Nano operator"" _h	(long double v){ return Nano{v * 3600.0}; } // Hours
  // constexpr Nano operator"" _d	(long double v){ return Nano{v * 86400.0}; } // Days
  // constexpr Nano operator"" _w	(long double v){ return Nano{v * 604800.0}; } // Weeks
  // constexpr Nano operator"" _y	(long double v){ return Nano{v * 218937600.0}; } // Years

  // constexpr Nano operator"" _ns (size_t v){ return Nano{static_cast<long double>(v) * 0.000000001}; } // Nanoseconds
  // constexpr Nano operator"" _us (size_t v){ return Nano{static_cast<long double>(v) * 0.000001}; } // Microseconds
  // constexpr Nano operator"" _ms (size_t v){ return Nano{static_cast<long double>(v) * 0.001}; } // Milliseconds
  // constexpr Nano operator"" _s	(size_t v){ return Nano{static_cast<long double>(v)}; } // Nano, the base unit
  // constexpr Nano operator"" _min(size_t v){ return Nano{static_cast<long double>(v) * 60.0}; } // Minutes
  // constexpr Nano operator"" _h	(size_t v){ return Nano{static_cast<long double>(v) * 3600.0}; } // Hours
  // constexpr Nano operator"" _d	(size_t v){ return Nano{static_cast<long double>(v) * 86400.0}; } // Days
  // constexpr Nano operator"" _w	(size_t v){ return Nano{static_cast<long double>(v) * 604800.0}; } // Weeks
  // constexpr Nano operator"" _y	(size_t v){ return Nano{static_cast<long double>(v) * 218937600.0}; } // Years
};