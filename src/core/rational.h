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
    const auto divisor = std::gcd(numerator, denominator);
    numerator_ = numerator / divisor;
    denominator_ = denominator / divisor;
  }

  static Rational fromDouble(double value, std::int64_t quantum = 4096) {
    if (!std::isfinite(value)) {
      throw std::invalid_argument("Rational source must be finite.");
    }
    if (quantum <= 0) {
      throw std::invalid_argument("Rational quantum must be positive.");
    }
    const long double scaled = static_cast<long double>(value) * quantum;
    if (scaled > static_cast<long double>(std::numeric_limits<std::int64_t>::max()) ||
        scaled < static_cast<long double>(std::numeric_limits<std::int64_t>::min())) {
      throw std::overflow_error("Rational source is outside the supported range.");
    }
    return Rational(static_cast<std::int64_t>(std::llround(scaled)), quantum);
  }

  std::int64_t numerator() const noexcept { return numerator_; }
  std::int64_t denominator() const noexcept { return denominator_; }
  double toDouble() const noexcept {
    return static_cast<double>(numerator_) / static_cast<double>(denominator_);
  }
  std::string toString() const {
    return std::to_string(numerator_) + "/" + std::to_string(denominator_);
  }

  friend Rational operator+(const Rational& lhs, const Rational& rhs) {
    const auto common = std::gcd(lhs.denominator_, rhs.denominator_);
    const auto lhsScale = rhs.denominator_ / common;
    const auto rhsScale = lhs.denominator_ / common;
    return Rational(lhs.numerator_ * lhsScale + rhs.numerator_ * rhsScale,
                    lhs.denominator_ * lhsScale);
  }

  friend Rational operator-(const Rational& lhs, const Rational& rhs) {
    return lhs + Rational(-rhs.numerator_, rhs.denominator_);
  }

  friend bool operator==(const Rational&, const Rational&) = default;

  friend bool operator<(const Rational& lhs, const Rational& rhs) noexcept {
    return lhs.numerator_ * rhs.denominator_ < rhs.numerator_ * lhs.denominator_;
  }
  friend bool operator>(const Rational& lhs, const Rational& rhs) noexcept { return rhs < lhs; }
  friend bool operator<=(const Rational& lhs, const Rational& rhs) noexcept { return !(rhs < lhs); }
  friend bool operator>=(const Rational& lhs, const Rational& rhs) noexcept { return !(lhs < rhs); }

 private:
  std::int64_t numerator_{0};
  std::int64_t denominator_{1};
};

}  // namespace airmon
