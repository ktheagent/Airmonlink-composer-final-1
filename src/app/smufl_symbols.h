#pragma once

#include "core/music_types.h"

#include <QFont>
#include <QIcon>
#include <QSize>
#include <QString>

namespace airmon {

struct MusicFontLoadResult {
  QFont font;
  QString sourcePath;
  bool loadedFromBundle{false};
};

MusicFontLoadResult loadBravuraMusicFont();
QString durationGlyph(Duration duration);
QIcon smuflIcon(const QString& glyph, const QFont& font, const QSize& size);

}  // namespace airmon
