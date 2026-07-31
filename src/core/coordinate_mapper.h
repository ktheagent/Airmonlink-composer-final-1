#pragma once

#include "core/music_types.h"

#include <optional>

namespace airmon {

struct StaffGeometry {
  Rect pageRect{0, 0, 794, 1123};
  double firstSystemTop{150};
  double systemGap{150};
  double staffSpace{10};
  double staffLeft{80};
  double staffRight{714};
  std::size_t systems{4};
  std::size_t measuresPerSystem{4};
  int beatsPerMeasure{4};
  int middleLineMidi{71};  // treble-staff B4
};

class CoordinateMapper {
 public:
  explicit CoordinateMapper(StaffGeometry geometry);
  std::optional<PointerTarget> map(Point clientPoint, const ViewTransform& transform) const;
  Point clientToPage(Point clientPoint, const ViewTransform& transform) const;
  const StaffGeometry& geometry() const noexcept { return geometry_; }

 private:
  StaffGeometry geometry_;
};

}  // namespace airmon
