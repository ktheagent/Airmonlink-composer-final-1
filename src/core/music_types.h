#pragma once

#include "core/rational.h"

#include <cstdint>
#include <string>

namespace airmon {

enum class Voice : std::uint8_t { One = 1, Two = 2, Three = 3, Four = 4 };
enum class Duration : std::uint8_t { Whole, Half, Quarter, Eighth, Sixteenth, ThirtySecond, SixtyFourth };

inline Rational beats(Duration value) {
  switch (value) {
    case Duration::Whole: return Rational(4);
    case Duration::Half: return Rational(2);
    case Duration::Quarter: return Rational(1);
    case Duration::Eighth: return Rational(1, 2);
    case Duration::Sixteenth: return Rational(1, 4);
    case Duration::ThirtySecond: return Rational(1, 8);
    case Duration::SixtyFourth: return Rational(1, 16);
  }
  throw std::invalid_argument("Unsupported duration.");
}

struct NoteEvent {
  std::string id;
  int midiPitch{60};
  Rational onsetBeats{};
  Duration duration{Duration::Quarter};
  Voice voice{Voice::One};
  bool tieFromPrevious{false};
  bool tieToNext{false};

  friend bool operator==(const NoteEvent&, const NoteEvent&) = default;
};

struct Point { double x{}; double y{}; };
struct Rect { double x{}; double y{}; double width{}; double height{}; };

struct PointerTarget {
  std::size_t systemIndex{};
  std::size_t staffIndex{};
  std::size_t measureIndex{};
  Rational onsetBeats{};
  int midiPitch{};
  Point snappedPagePoint{};
};

struct ViewTransform {
  Point viewportOrigin{};
  Point scrollOffset{};
  Point pageOrigin{};
  double zoom{1.0};
  double devicePixelRatio{1.0};
};

}  // namespace airmon
