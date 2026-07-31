#pragma once

#include "core/music_types.h"

#include <optional>
#include <string>
#include <vector>

namespace airmon {

class ScoreModel {
 public:
  explicit ScoreModel(std::size_t measures = 8, int beatsPerMeasure = 4);

  const std::vector<NoteEvent>& notes() const noexcept { return notes_; }
  std::size_t measureCount() const noexcept { return measureCount_; }
  int beatsPerMeasure() const noexcept { return beatsPerMeasure_; }
  Rational totalBeats() const noexcept;

  const NoteEvent& addNote(int midiPitch, Rational onset, Duration duration, Voice voice);
  bool removeNote(const std::string& id);
  std::optional<NoteEvent> findNote(const std::string& id) const;
  bool canPlace(int midiPitch, Rational onset, Duration duration, Voice voice,
                std::string* reason = nullptr) const;
  std::string deterministicText() const;

 private:
  std::string nextId(int midiPitch, Rational onset, Duration duration, Voice voice) const;
  std::size_t measureCount_;
  int beatsPerMeasure_;
  std::vector<NoteEvent> notes_;
};

}  // namespace airmon
