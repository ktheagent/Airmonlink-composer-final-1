#include "core/coordinate_mapper.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace airmon {

CoordinateMapper::CoordinateMapper(StaffGeometry geometry) : geometry_(geometry) {
  if (geometry_.staffSpace <= 0 || geometry_.staffRight <= geometry_.staffLeft ||
      geometry_.systems == 0 || geometry_.measuresPerSystem == 0 ||
      geometry_.beatsPerMeasure <= 0) {
    throw std::invalid_argument("Invalid staff geometry");
  }
}

Point CoordinateMapper::clientToPage(Point client, const ViewTransform& t) const {
  if (t.zoom <= 0.0 || t.devicePixelRatio <= 0.0) {
    throw std::invalid_argument("Invalid view transform");
  }
  return {
      ((client.x - t.viewportOrigin.x) + t.scrollOffset.x) / t.zoom - t.pageOrigin.x,
      ((client.y - t.viewportOrigin.y) + t.scrollOffset.y) / t.zoom - t.pageOrigin.y};
}

std::optional<PointerTarget> CoordinateMapper::map(
    Point client, const ViewTransform& t) const {
  const Point page = clientToPage(client, t);
  if (page.x < geometry_.staffLeft || page.x > geometry_.staffRight) {
    return std::nullopt;
  }

  const double relativeY = page.y - geometry_.firstSystemTop;
  const auto system = static_cast<long>(
      std::llround(relativeY / geometry_.systemGap));
  if (system < 0 ||
      system >= static_cast<long>(geometry_.systems)) {
    return std::nullopt;
  }

  const double staffTop =
      geometry_.firstSystemTop + system * geometry_.systemGap;
  const double halfSpace = geometry_.staffSpace / 2.0;
  const int diatonicStepsFromTopLine =
      static_cast<int>(std::llround((page.y - staffTop) / halfSpace));

  static constexpr int scale[] = {0, 2, 4, 5, 7, 9, 11};
  const int topMidi = geometry_.middleLineMidi + 6;
  const int topDegree = 3;
  const int absoluteDegree = topDegree - diatonicStepsFromTopLine;
  const int octave = static_cast<int>(
      std::floor(static_cast<double>(absoluteDegree) / 7.0));
  const int degree = ((absoluteDegree % 7) + 7) % 7;
  const int midi =
      topMidi + octave * 12 + scale[degree] - scale[topDegree];

  const double measureWidth =
      (geometry_.staffRight - geometry_.staffLeft) /
      static_cast<double>(geometry_.measuresPerSystem);
  const auto measureInSystem = std::min<std::size_t>(
      geometry_.measuresPerSystem - 1,
      static_cast<std::size_t>(
          (page.x - geometry_.staffLeft) / measureWidth));
  const double localMeasureX =
      page.x - (geometry_.staffLeft + measureInSystem * measureWidth);
  const double rawBeat =
      localMeasureX / measureWidth * geometry_.beatsPerMeasure;

  constexpr std::int64_t kSubdivisionsPerBeat = 4;
  const std::int64_t maxSubdivision =
      static_cast<std::int64_t>(geometry_.beatsPerMeasure) *
          kSubdivisionsPerBeat -
      1;
  const std::int64_t snappedSubdivision = std::clamp<std::int64_t>(
      static_cast<std::int64_t>(
          std::llround(rawBeat * kSubdivisionsPerBeat)),
      0, maxSubdivision);
  const Rational snappedBeat(
      snappedSubdivision, kSubdivisionsPerBeat);

  const std::size_t measure =
      static_cast<std::size_t>(system) * geometry_.measuresPerSystem +
      measureInSystem;
  const std::int64_t measureStartSubdivision =
      static_cast<std::int64_t>(measure) *
      geometry_.beatsPerMeasure * kSubdivisionsPerBeat;
  const Rational onset(
      measureStartSubdivision + snappedSubdivision,
      kSubdivisionsPerBeat);

  const double snappedX =
      geometry_.staffLeft + measureInSystem * measureWidth +
      snappedBeat.toDouble() / geometry_.beatsPerMeasure * measureWidth;
  const double snappedY =
      staffTop + diatonicStepsFromTopLine * halfSpace;

  return PointerTarget{
      static_cast<std::size_t>(system),
      0,
      measure,
      onset,
      midi,
      {snappedX, snappedY}};
}

}  // namespace airmon
