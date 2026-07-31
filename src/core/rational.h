#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>

namespace airmon {

class Rational {
 public:
  Rational() noexcept = default;

  Rational(std::int64_t numerator, std::int64_t denominator = 1) {
    if (denominator == 0) {
      throw std::invalid_argument("Rational denominator must not be zero.");
    }
    if (denominator < 0) {
      if (numerator == std::numeric_limits<std::int64_t>::min() ||
          denominator == std::numeric_limits<std::int64_t>::min()) {
        throw std::overflow_error("Rational sign normalization overflow.");
      }
      numerator = -numerator;
      denominator = -denominator;
    }

    const auto divisor = std::gcd(magnitude(numerator),
                                  static_cast<std::uint64_t>(denominator));
    numerator_ = numerator / static_cast<std::int64_t>(divisor);
    denominator_ = denominator / static_cast<std::int64_t>(divisor);
  }

  static Rational fromDouble(double value, std::int64_t quantum = 4096) {
    if (!std::isfinite(value)) {
      throw std::invalid_argument("Rational source must be finite.");
    }
    if (quantum <= 0) {
      throw std::invalid_argument("Rational quantum must be positive.");
    }

    const long double scaled = static_cast<long double>(value) * quantum;
    if (scaled >
            static_cast<long double>(
                std::numeric_limits<std::int64_t>::max()) ||
        scaled <
            static_cast<long double>(
                std::numeric_limits<std::int64_t>::min())) {
      throw std::overflow_error(
          "Rational source is outside the supported range.");
    }

    return Rational(
        static_cast<std::int64_t>(std::llround(scaled)), quantum);
  }

  std::int64_t numerator() const noexcept { return numerator_; }
  std::int64_t denominator() const noexcept { return denominator_; }

  double toDouble() const noexcept {
    return static_cast<double>(numerator_) /
           static_cast<double>(denominator_);
  }

  std::string toString() const {
    return std::to_string(numerator_) + "/" +
           std::to_string(denominator_);
  }

  friend Rational operator+(const Rational& lhs, const Rational& rhs) {
    const auto common =
        std::gcd(lhs.denominator_, rhs.denominator_);
    const auto lhsScale = rhs.denominator_ / common;
    const auto rhsScale = lhs.denominator_ / common;

    const auto lhsNumerator =
        checkedMultiply(lhs.numerator_, lhsScale);
    const auto rhsNumerator =
        checkedMultiply(rhs.numerator_, rhsScale);

    return Rational(
        checkedAdd(lhsNumerator, rhsNumerator),
        checkedMultiply(lhs.denominator_, lhsScale));
  }

  friend Rational operator-(const Rational& lhs, const Rational& rhs) {
    return lhs + Rational(checkedNegate(rhs.numerator_),
                          rhs.denominator_);
  }

  friend bool operator==(const Rational&, const Rational&) = default;

  friend bool operator<(const Rational& lhs,
                        const Rational& rhs) noexcept {
    const bool lhsNegative = lhs.numerator_ < 0;
    const bool rhsNegative = rhs.numerator_ < 0;

    if (lhsNegative != rhsNegative) {
      return lhsNegative;
    }

    if (!lhsNegative) {
      return lessPositive(
          magnitude(lhs.numerator_),
          static_cast<std::uint64_t>(lhs.denominator_),
          magnitude(rhs.numerator_),
          static_cast<std::uint64_t>(rhs.denominator_));
    }

    return lessPositive(
        magnitude(rhs.numerator_),
        static_cast<std::uint64_t>(rhs.denominator_),
        magnitude(lhs.numerator_),
        static_cast<std::uint64_t>(lhs.denominator_));
  }

  friend bool operator>(const Rational& lhs,
                        const Rational& rhs) noexcept {
    return rhs < lhs;
  }

  friend bool operator<=(const Rational& lhs,
                         const Rational& rhs) noexcept {
    return !(rhs < lhs);
  }

  friend bool operator>=(const Rational& lhs,
                         const Rational& rhs) noexcept {
    return !(lhs < rhs);
  }

 private:
  static std::uint64_t magnitude(std::int64_t value) noexcept {
    if (value >= 0) {
      return static_cast<std::uint64_t>(value);
    }
    return static_cast<std::uint64_t>(-(value + 1)) + 1;
  }

  static std::int64_t checkedNegate(std::int64_t value) {
    if (value == std::numeric_limits<std::int64_t>::min()) {
      throw std::overflow_error("Rational negation overflow.");
    }
    return -value;
  }

  static std::int64_t checkedAdd(std::int64_t lhs,
                                 std::int64_t rhs) {
    constexpr auto kMax =
        std::numeric_limits<std::int64_t>::max();
    constexpr auto kMin =
        std::numeric_limits<std::int64_t>::min();

    if ((rhs > 0 && lhs > kMax - rhs) ||
        (rhs < 0 && lhs < kMin - rhs)) {
      throw std::overflow_error("Rational addition overflow.");
    }
    return lhs + rhs;
  }

  static std::int64_t checkedMultiply(std::int64_t lhs,
                                      std::int64_t rhs) {
    constexpr auto kMax =
        std::numeric_limits<std::int64_t>::max();
    constexpr auto kMin =
        std::numeric_limits<std::int64_t>::min();

    if (lhs == 0 || rhs == 0) {
      return 0;
    }

    if (lhs > 0) {
      if ((rhs > 0 && lhs > kMax / rhs) ||
          (rhs < 0 && rhs < kMin / lhs)) {
        throw std::overflow_error(
            "Rational multiplication overflow.");
      }
    } else {
      if ((rhs > 0 && lhs < kMin / rhs) ||
          (rhs < 0 && rhs < kMax / lhs)) {
        throw std::overflow_error(
            "Rational multiplication overflow.");
      }
    }

    return lhs * rhs;
  }

  static bool lessPositive(std::uint64_t lhsNumerator,
                           std::uint64_t lhsDenominator,
                           std::uint64_t rhsNumerator,
                           std::uint64_t rhsDenominator) noexcept {
    bool reverse = false;

    while (true) {
      const auto lhsQuotient =
          lhsNumerator / lhsDenominator;
      const auto rhsQuotient =
          rhsNumerator / rhsDenominator;

      if (lhsQuotient != rhsQuotient) {
        return reverse ? lhsQuotient > rhsQuotient
                       : lhsQuotient < rhsQuotient;
      }

      const auto lhsRemainder =
          lhsNumerator % lhsDenominator;
      const auto rhsRemainder =
          rhsNumerator % rhsDenominator;

      if (lhsRemainder == 0 || rhsRemainder == 0) {
        if (lhsRemainder == 0 && rhsRemainder == 0) {
          return false;
        }
        const bool result = lhsRemainder == 0;
        return reverse ? !result : result;
      }

      lhsNumerator = lhsDenominator;
      lhsDenominator = lhsRemainder;
      rhsNumerator = rhsDenominator;
      rhsDenominator = rhsRemainder;
      reverse = !reverse;
    }
  }

  std::int64_t numerator_{0};
  std::int64_t denominator_{1};
};

}  // namespace airmon
