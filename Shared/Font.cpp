#include "Font.h"
#include <QFont>
#include <QFontMetrics>
#include <QFontInfo>
#include <QSizeF>
#include <QRectF>
#include <QStringList>
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
	QFont font(baseFont.family());
	// Set the size and style
	font.setPixelSize(height);
	font.setBold(baseFont.bold());
	font.setItalic(baseFont.italic());
	font.setUnderline(baseFont.underline());

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

	if (QFontInfo(font).fixedPitch())
	{
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
	}

	return font;
}

/// Stretch a font to fit inside a rect
QFont Font::stretch(const QFont& baseFont, const QString& str, const QRectF& rect)
{
	// Count the lines
	const int lines = str.count('\n') + 1;
	// Try the obvious height
	QFont font(baseFont);
	font.setPixelSize(rect.height() / lines);

	QFontMetricsF metrics(font);
	QString longestLine;
	foreach (const QString& line, str.split('\n'))
	{
		if (metrics.width(line) > metrics.width(longestLine))
			longestLine = line;
	}

	while ((metrics.height() * lines > rect.height() || metrics.width(longestLine) > rect.width()) && font.pixelSize())
	{
		font.setPixelSize(font.pixelSize() - 1);
		metrics = QFontMetricsF(font);
	}

	return font;
}

/// Get the line height to use when stretching a font to fit
qreal Font::lineHeight(const QFont& font, int rows, qreal pageHeight)
{
	qreal height = QFontMetricsF(font).height();
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

/// Get the cell size to use for fixed rows and cols
QSizeF Font::cellSize(int rows, int cols, const QSizeF& pageSize)
{
	return QSizeF(cols > 0 ? pageSize.width() / cols : 0, rows > 0 ? pageSize.height() / rows : 0);
}

/// Get the cell size to use for a specific LPI/CPI
QSizeF Font::cellSize(int lpi, int cpi)
{
	return QSizeF(cpi > 0 ? INCH / cpi : 0, lpi > 0 ? INCH / lpi : 0);
}

/// Set the width of a font
QFont Font::setWidth(const QFont& font, int width)
{
	QFont newFont(font);

	if (QFontInfo(newFont).fixedPitch())
	{
		if (QFontMetrics(newFont).averageCharWidth() > width)
		{
			while (QFontMetrics(newFont).averageCharWidth() > width)
				newFont.setStretch(newFont.stretch() - 1);
		}
		else if (QFontMetrics(newFont).averageCharWidth() < width)
		{
			while (QFontMetrics(newFont).averageCharWidth() < width)
				newFont.setStretch(newFont.stretch() + 1);
		}
	}

	return newFont;
}
