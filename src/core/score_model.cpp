#include "core/score_model.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>

namespace airmon {

ScoreModel::ScoreModel(std::size_t measures, int beatsPerMeasure)
    : measureCount_(measures), beatsPerMeasure_(beatsPerMeasure) {
  if (measures == 0 || beatsPerMeasure <= 0) {
    throw std::invalid_argument("Score dimensions must be positive.");
  }
}

Rational ScoreModel::totalBeats() const noexcept {
  return Rational(static_cast<std::int64_t>(measureCount_) * beatsPerMeasure_);
}

bool ScoreModel::canPlace(int midiPitch, Rational onset, Duration duration, Voice voice,
                          std::string* reason) const {
  if (midiPitch < 0 || midiPitch > 127) {
    if (reason) *reason = "Pitch is outside the MIDI range.";
    return false;
  }

  const Rational end = onset + beats(duration);
  if (onset < Rational(0) || end > totalBeats()) {
    if (reason) *reason = "The note lies outside the score.";
    return false;
  }

  for (const auto& note : notes_) {
    if (note.voice != voice) continue;

    const Rational noteEnd = note.onsetBeats + beats(note.duration);
    const bool sameOnset = note.onsetBeats == onset;

    if (sameOnset && note.midiPitch == midiPitch) {
      if (reason) *reason = "That pitch already exists in this voice at this onset.";
      return false;
    }

    if (sameOnset && note.duration != duration) {
      if (reason) *reason = "Notes in one chord must share one duration.";
      return false;
    }

    const bool overlaps = onset < noteEnd && end > note.onsetBeats;
    if (overlaps && !sameOnset) {
      if (reason) *reason = "The selected rhythmic position overlaps this voice.";
      return false;
    }
  }

  return true;
}

std::string ScoreModel::nextId(int midiPitch, Rational onset, Duration duration, Voice voice) const {
  std::ostringstream key;
  key << midiPitch << ':' << onset.toString() << ':' << static_cast<int>(duration) << ':'
      << static_cast<int>(voice) << ':' << notes_.size();

  const auto hash = std::hash<std::string>{}(key.str());
  std::ostringstream id;
  id << "note-" << std::hex << hash;
  return id.str();
}

const NoteEvent& ScoreModel::addNote(int midiPitch, Rational onset, Duration duration, Voice voice) {
  std::string reason;
  if (!canPlace(midiPitch, onset, duration, voice, &reason)) {
    throw std::invalid_argument(reason);
  }

  const auto createdId = nextId(midiPitch, onset, duration, voice);
  notes_.push_back({createdId, midiPitch, onset, duration, voice});
  std::stable_sort(notes_.begin(), notes_.end(), [](const auto& a, const auto& b) {
    if (a.onsetBeats != b.onsetBeats) return a.onsetBeats < b.onsetBeats;
    if (a.voice != b.voice) return a.voice < b.voice;
    return a.midiPitch < b.midiPitch;
  });

  const auto it = std::find_if(notes_.begin(), notes_.end(),
                               [&](const auto& note) { return note.id == createdId; });
  return *it;
}

bool ScoreModel::removeNote(const std::string& id) {
  const auto before = notes_.size();
  std::erase_if(notes_, [&](const auto& note) { return note.id == id; });
  return notes_.size() != before;
}

std::optional<NoteEvent> ScoreModel::findNote(const std::string& id) const {
  const auto it = std::find_if(notes_.begin(), notes_.end(),
                               [&](const auto& note) { return note.id == id; });
  return it == notes_.end() ? std::nullopt : std::optional<NoteEvent>(*it);
}

std::string ScoreModel::deterministicText() const {
  std::ostringstream out;
  out << "AIRM-NATIVE-2\nmeasures=" << measureCount_ << "\nbeats=" << beatsPerMeasure_ << '\n';
  for (const auto& note : notes_) {
    out << note.id << ',' << note.midiPitch << ',' << note.onsetBeats.toString() << ','
        << static_cast<int>(note.duration) << ',' << static_cast<int>(note.voice) << '\n';
  }
  return out.str();
}

}  // namespace airmon
