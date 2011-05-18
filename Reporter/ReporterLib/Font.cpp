#include "Font.h"
#include <QFont>
#include <QFontMetrics>
#include <QSizeF>
#include <QDebug>

namespace
{
    const qreal INCH = 254;
}

QFont Font::stretch(const QFont& baseFont, int rows, int cols, const QSizeF& size)
{
    Q_ASSERT(rows > 0);
    Q_ASSERT(cols > 0);
    Q_ASSERT(!size.isEmpty());

    int height = size.height() / rows;

    // Create the font
    QFont font(baseFont);
    // Set the size
    font.setPixelSize(height);

    // Get the actual metrics of the font
    QFontMetrics metrics(font);

    // Is the font height greater than requested?
    if (metrics.height() > height)
    {
	// Recreate the font
	int diff = metrics.height() - height;
	font.setPixelSize(height - diff);

	metrics = QFontMetrics(font);
	while (metrics.height() * rows > size.height())
	{
	    font.setPixelSize(font.pixelSize() - 1);
	    metrics = QFontMetrics(font);
	}
    }

    metrics = QFontMetrics(font);
    Q_ASSERT(metrics.height() * rows <= size.height());

    // Is the font too wide?
    int fontWidth = metrics.averageCharWidth();

    // Ideal width
    int width = size.width() / cols;
    if (width > 0)
    {
	int newWidth = (width * 100) / fontWidth;
	font.setStretch(newWidth);

	// Anything left over?
	metrics = QFontMetrics(font);
	if (metrics.averageCharWidth() * cols < size.width())
	{
	    qreal diff = size.width() - (metrics.averageCharWidth() * cols);
	    diff /= cols;
	    font.setLetterSpacing(QFont::AbsoluteSpacing, diff);
	}
    }

    return font;
}

/// Get the line height to use when stretching a font to fit
qreal Font::lineHeight(const QFont& font, int rows, qreal pageHeight)
{
    qreal height = QFontMetrics(font).height();
    if (rows > 0 && height * rows < pageHeight)
    {
	// Get the difference
	qreal diff = pageHeight - (height * rows);
	diff /= rows;
	height += diff;
    }
    return height;
}

/// Create a font that uses a specific LPI and CPI
QFont Font::stretch(const QFont& baseFont, int lpi, int cpi)
{
    // Create the font
    QFont font(baseFont);

    if (lpi > 0)
    {
	// Set the size
	font.setPixelSize(INCH / lpi);
	// Get the actual metrics of the font
	QFontMetrics metrics(font);
	while (metrics.height() * lpi > INCH)
	{
	    font.setPixelSize(font.pixelSize() - 1);
	    metrics = QFontMetrics(font);
	}
    }

    if (cpi > 0)
    {
	QFontMetrics metrics(font);
	while (metrics.averageCharWidth() * cpi > INCH)
	{
	    font.setStretch(font.stretch() - 1);
	    metrics = QFontMetrics(font);
	}
	// Adjust letter spacing
	const qreal diff = (INCH - (metrics.averageCharWidth() * cpi)) / cpi;
	font.setLetterSpacing(QFont::AbsoluteSpacing, diff);
    }

    return font;
}


/// Get the line height to use with LPI
qreal Font::lineHeight(const QFont& font, int lpi)
{
    qreal height = QFontMetrics(font).height();
    if (lpi > 0)
    {
	if (height * lpi < INCH)
	{
	    qreal diff = INCH - (height * lpi);
	    diff /= lpi;
	    height += diff;
	}
    }
    return height;
}

/// Get the cell size of a monospaced font
QSizeF Font::cellSize(const QFont& font)
{
    QFontMetricsF metrics(font);
    return QSizeF(metrics.averageCharWidth(), metrics.height());
}

/// Get the cell size to use for fixes rows and cols
QSizeF Font::cellSize(int rows, int cols, const QSizeF& pageSize)
{
    return QSizeF(pageSize.width() / cols, pageSize.height() / rows);
}

/// Get the cell size to use for a specific LPI/CPI
QSizeF Font::cellSize(int lpi, int cpi)
{
    return QSizeF(INCH / cpi, INCH / lpi);
}
