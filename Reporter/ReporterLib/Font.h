#ifndef FONT_H
#define FONT_H

#include <QtGlobal>

class QFont;
class QSizeF;

namespace Font
{
    /// Stretch a font to fit in a specified rectangle, using
    /// a specified number of rows and columns
    QFont stretch(const QFont& baseFont, int rows, int cols, const QSizeF& size);
    /// Get the line height to use when stretching a font to fit
    qreal lineHeight(const QFont& font, int rows, qreal pageHeight);
    /// Create a font that uses a specific LPI and CPI
    QFont stretch(const QFont& baseFont, int lpi, int cpi);
    /// Get the line height to use with LPI
    qreal lineHeight(const QFont& font, int lpi);
    /// Get the cell size of a monospaced font
    QSizeF cellSize(const QFont& font);
    /// Get the cell size to use for fixed rows and cols
    QSizeF cellSize(int rows, int cols, const QSizeF& pageSize);
    /// Get the cell size to use for a specific LPI/CPI
    QSizeF cellSize(int lpi, int cpi);
}

#endif // FONT_H
