#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace airmon {

enum class Voice : std::uint8_t { One = 1, Two = 2, Three = 3, Four = 4 };
enum class Duration : std::uint8_t { Whole, Half, Quarter, Eighth, Sixteenth, ThirtySecond, SixtyFourth };

inline double beats(Duration value) {
  switch (value) {
    case Duration::Whole: return 4.0;
    case Duration::Half: return 2.0;
    case Duration::Quarter: return 1.0;
    case Duration::Eighth: return 0.5;
    case Duration::Sixteenth: return 0.25;
    case Duration::ThirtySecond: return 0.125;
    case Duration::SixtyFourth: return 0.0625;
  }
  return 1.0;
}

struct NoteEvent {
  std::string id;
  int midiPitch{60};
  double onsetBeats{0.0};
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
  double onsetBeats{};
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
