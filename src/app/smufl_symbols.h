#pragma once

#include "core/music_types.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QStringList>

namespace airmon {

struct MusicFontLoadResult {
  QFont font;
  QString sourcePath;
  bool loadedFromBundle{false};
};

inline MusicFontLoadResult loadBravuraMusicFont() {
  const QDir appDir(QCoreApplication::applicationDirPath());
  const QStringList candidates{
      appDir.filePath(QStringLiteral("fonts/Bravura.otf")),
      appDir.filePath(QStringLiteral("resources/fonts/Bravura.otf"))};

  MusicFontLoadResult result;
  for (const QString& bundledPath : candidates) {
    if (!QFileInfo::exists(bundledPath)) continue;
    result.sourcePath = bundledPath;
    const int fontId = QFontDatabase::addApplicationFont(bundledPath);
    if (fontId >= 0) {
      const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
      if (!families.isEmpty()) {
        result.font = QFont(families.front());
        result.loadedFromBundle = true;
        break;
      }
    }
  }

  if (result.font.family().isEmpty()) {
    result.font = QFont(QStringLiteral("Bravura"));
  }
  result.font.setStyleStrategy(QFont::PreferAntialias);
  return result;
}

inline QString durationGlyph(Duration duration) {
  char32_t codepoint = 0xE1D5;
  switch (duration) {
    case Duration::Whole: codepoint = 0xE1D2; break;
    case Duration::Half: codepoint = 0xE1D3; break;
    case Duration::Quarter: codepoint = 0xE1D5; break;
    case Duration::Eighth: codepoint = 0xE1D7; break;
    case Duration::Sixteenth: codepoint = 0xE1D9; break;
    case Duration::ThirtySecond: codepoint = 0xE1DB; break;
    case Duration::SixtyFourth: codepoint = 0xE1DD; break;
  }
  return QString::fromUcs4(&codepoint, 1);
}

inline QIcon smuflIcon(const QString& glyph,
                       const QFont& font,
                       const QSize& size) {
  const QSize canvas(qMax(size.width(), 24), qMax(size.height(), 24));
  QPixmap pixmap(canvas);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(QApplication::palette().color(QPalette::ButtonText));
  QFont iconFont(font);
  iconFont.setPixelSize(qMax(18, canvas.height() - 4));
  painter.setFont(iconFont);
  painter.drawText(pixmap.rect(), Qt::AlignCenter, glyph);
  return QIcon(pixmap);
}

}  // namespace airmon
