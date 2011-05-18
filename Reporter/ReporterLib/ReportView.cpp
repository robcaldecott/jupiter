#include "ReportView.h"
#include "Report.h"
#include "Printer.h"
#include "Font.h"
#include "Text.h"
#include "Measurements.h"
#include <QPainter>
#include <QPrinter>
#include <QSizeF>
#include <QFontMetrics>
#include <QDebug>
#include <QPointF>
#include <QRegExp>

namespace
{
    void drawPageBorder(QPainter& painter, const QSizeF& pageSize)
    {
	QPen currentPen = painter.pen();
	painter.setPen(Qt::DotLine);
	painter.drawRect(0, 0, pageSize.width(), pageSize.height());
	painter.setPen(currentPen);
    }

    void drawInchGrid(QPainter& painter, const QSizeF& pageSize)
    {
	QPen currentPen = painter.pen();
	painter.setPen(Qt::DotLine);

	for (int i = Measurements::INCH; i < pageSize.width(); i += Measurements::INCH)
	{
	    painter.drawLine(i, 0, i, pageSize.height());
	}

	for (int i = Measurements::INCH; i < pageSize.height(); i += Measurements::INCH)
	{
	    painter.drawLine(0, i, pageSize.width(), i);
	}

	painter.setPen(currentPen);
    }

    void drawLine(QPainter& painter, const QRectF& rect, const QString& str)
    {
	QRegExp re("<(/?[bius])>", Qt::CaseInsensitive);
	int pos = re.indexIn(str);
	if (pos == -1)
	{
	    // No regex sequences, so draw entire line
	    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, str);
	    return;
	}

	QRectF drawRect(rect);
	int currentPos = 0;
	while (pos != -1)
	{
	    const QString chunk = str.mid(currentPos, pos - currentPos).remove(QRegExp("</?[bius]>", Qt::CaseInsensitive));
	    if (!chunk.isEmpty())
	    {
		painter.drawText(drawRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, chunk);
		drawRect.setLeft(drawRect.left() + QFontMetricsF(painter.font()).width(chunk));
	    }

	    QString cap = re.cap(1).toLower();
	    const bool flag = !(cap[0] == '/');
	    const QString tag = cap.remove('/');

	    QFont font = painter.font();
	    switch (tag[0].toAscii())
	    {
	    case 'b':
		font.setBold(flag);
		break;
	    case 'i':
		font.setItalic(flag);
		break;
	    case 'u':
		font.setUnderline(flag);
		break;
	    case 's':
		font.setStrikeOut(flag);
		break;
	    }
	    if (font != painter.font())
	    {
		painter.setFont(font);
	    }

	    currentPos = pos + re.matchedLength();
	    pos = re.indexIn(str, currentPos);
	}

	// Display final chunk
	const QString chunk = str.mid(currentPos).remove(QRegExp("</?[bius]>", Qt::CaseInsensitive));
	if (!chunk.isEmpty())
	{
	    painter.drawText(drawRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, chunk);
	}
    }
}

ReportView::ReportView(const Report* report, QObject* parent) :
    QObject(parent),
    report(report)
{
    Q_ASSERT(this->report);
}

int ReportView::paint(QPainter& painter, QPrinter& printer)
{
    const Report::Data data = report->split();
    if (data.isEmpty())
    {
	return 0;
    }

    const QSizeF pageSize = Printer::pageSize(&printer);
    const QSizeF paperSize = Printer::paperSize(&printer);
    const QMargins margins = Printer::margins(&printer);

    const int width = qRound((printer.logicalDpiX() / Measurements::INCH) * paperSize.width());
    const int height = qRound((printer.logicalDpiY() / Measurements::INCH) * paperSize.height());

    // Adjust for non-printable area
    const QPointF topLeft = Printer::topLeft(&printer);
    const qreal left = margins.left() - topLeft.x();
    const qreal top = margins.top() - topLeft.y();

    painter.setViewport(0, 0, width, height);
    painter.setWindow(-left, -top, paperSize.width(), paperSize.height());

    qreal lineHeight = 0;
    QSizeF cellSize;
    QFont font = report->font();
    font.setBold(report->bold());
    font.setKerning(false);

    if (report->stretchFont())
    {
	font = Font::stretch(font, report->rows(), report->cols(), pageSize);
	lineHeight = Font::lineHeight(font, report->rows(), pageSize.height());
	cellSize = Font::cellSize(report->rows(), report->cols(), pageSize);
    }
    else
    {
	font = Font::stretch(font, report->lpi(), report->cpi());
	lineHeight = Font::lineHeight(font, report->lpi());
	cellSize = Font::cellSize(report->lpi(), report->cpi());
    }

    painter.setFont(font);
    painter.setPen(report->fontColor());
    painter.setBackgroundMode(Qt::TransparentMode);

    int pages = 1;
    for (int page = 0, pageCount = data.count(); page < pageCount; ++page)
    {	
	drawPage(painter, pageSize, lineHeight);
	drawPictures(painter, cellSize, pageSize);

	qreal yPos = 0;
	int lineNumber = 1;
	const QStringList& lines = data.at(page);
	for (int line = 0, lineCount = lines.count(); line < lineCount; ++line)
	{
	    // Get the current line and replace tabs with spaces
	    const QString currentLine = Text::expandTabs(lines.at(line));
	    drawLine(painter, QRectF(0, yPos, pageSize.width(), lineHeight), currentLine);

	    yPos += lineHeight;
	    yPos = adjustYpos(lineNumber, yPos);
	    lineNumber++;

	    // If the next line won't fit, throw a new page
	    if (yPos + lineHeight > pageSize.height() + Measurements::MM && line < lineCount - 1)
	    {
		printer.newPage();
		yPos = 0;
		lineNumber = 1;
		pages++;

		drawPage(painter, pageSize, lineHeight);
		drawPictures(painter, cellSize, pageSize);
	    }
	}

	// Throw a new page?
	if (page < pageCount - 1)
	{
	    printer.newPage();
	    pages++;
	}
    }

    return pages;
}

void ReportView::drawPage(QPainter& painter, const QSizeF& pageSize, qreal lineHeight) const
{
    if (report->stripes())
    {
	drawStripes(painter, pageSize, lineHeight);
    }

#if 0
    if (border)
    {
	drawPageBorder(painter, pageSize);
    }

    drawInchGrid(painter, pageSize);
#endif
}

void ReportView::drawStripes(QPainter& painter, const QSizeF& pageSize, qreal lineHeight) const
{
    const QColor color = report->stripeColor();
    qreal yPos = 0;
    int line = 1;
    while (yPos < pageSize.height())
    {
	if (line % 2)
	{
	    painter.fillRect(0, yPos, pageSize.width(), lineHeight, color);
	}
	line++;
	yPos += lineHeight;
	yPos = adjustYpos(line - 1, yPos);
    }
}

void ReportView::drawPictures(QPainter& painter, const QSizeF& cellSize, const QSizeF& pageSize) const
{
    const QList<Picture> pictures = report->pictures();
    foreach (const Picture& picture, pictures)
    {
	const QRectF rect = picture.rect();

	qreal left = rect.left();
	qreal top = rect.top();
	const qreal width = (rect.width() == 0) ? pageSize.width() : (rect.width() * cellSize.width());
	const qreal height = (rect.height() == 0) ? pageSize.height() : (rect.height() * cellSize.height());

	switch (qRound(left))
	{
	case Picture::Right:
	    left = pageSize.width() - width;
	    break;
	case Picture::Center:
	    left = (pageSize.width() - width) / 2;
	    break;
	default:
	    left *= cellSize.width();
	    break;
	}

	switch (qRound(top))
	{
	case Picture::Center:
	    top = (pageSize.height() - height) / 2;
	    break;
	case Picture::Bottom:
	    top = pageSize.height() - height;
	    break;
	default:
	    top *= cellSize.height();
	    break;
	}

	painter.drawPixmap(left, top, width, height, picture.pixmap());
    }
}

qreal ReportView::adjustYpos(int lineNumber, qreal yPos) const
{
    const int lpi = report->lpi();
    if (!report->stretchFont() && lpi > 0 && yPos > 0 && (lineNumber % lpi) == 0)
    {
	yPos = (lineNumber / lpi) * Measurements::INCH;
    }
    return yPos;
}
