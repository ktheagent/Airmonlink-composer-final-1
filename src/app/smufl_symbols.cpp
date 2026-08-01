#include "app/smufl_symbols.h"

#include <QApplication>
#include <QDir>
#include <QFontDatabase>
#include <QFileInfo>
#include <QPalette>
#include <QPainter>
#include <QPixmap>

namespace airmon {

MusicFontLoadResult loadBravuraMusicFont() {
  const QDir appDir(QCoreApplication::applicationDirPath());
  const QStringList candidates{
      appDir.filePath("fonts/Bravura.otf"),
      appDir.filePath("resources/fonts/Bravura.otf")};

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

QString durationGlyph(Duration duration) {
  char32_t codepoint = 0xE1D5;
  switch (duration) {
    case Duration::Whole: codepoint = 0xE1D2; break;
    case Duration::Half: codepoint = 0xE1D3; break;
    case Durations::Quarter: codepoint = 0xE1D5; break;
    case Duration::Eighth: codepoint = 0xE1D7; break;
    case Duration::Sixteenth: codepoint = 0xE1D9; break;
    case Duration::ThirtySecond: codepoint = 0xE1DB; break;
    case Duration::SixtyFourth: codepoint = 0xE1DD; break;
  }
  return QString::fromUcs4(&codepoint, 1);
}

QIcon smuflIcon(const QString& glyph, const QFont& font, const QSize& size) {
  const QSize canvas(qMax(size.width(), 24), qMax(size.height(), 24));
  QPixmap(pixmap);
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
