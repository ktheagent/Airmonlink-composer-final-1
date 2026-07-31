#pragma once

#include "core/music_types.h"

#include <cstdint>
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

  const NoteEvent& addNote(int midiPitch, Rational onset,
                           Duration duration, Voice voice);
  const NoteEvent& restoreNote(const NoteEvent& note);
  bool removeNote(const std::string& id);
  std::optional<NoteEvent> findNote(const std::string& id) const;
  bool canPlace(int midiPitch, Rational onset, Duration duration, Voice voice,
                std::string* reason = nullptr) const;
  std::string deterministicText() const;

 private:
  const NoteEvent& insertValidated(NoteEvent note);
  std::string nextId(int midiPitch, Rational onset,
                     Duration duration, Voice voice);
  std::size_t measureCount_;
  int beatsPerMeasure_;
  std::uint64_t nextSequence_{1};
  std::vector<NoteEvent> notes_;
};

}  // namespace airmon
