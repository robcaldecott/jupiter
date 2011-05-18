#include "Printer.h"
#include <QPrinter>
#include <QDebug>
#include <QPointF>
#include <QPrinterInfo>

/// Return the QPrinter margins in tenths of a mm
QMargins Printer::margins(const QPrinter* printer)
{
	Q_ASSERT(printer);

	qreal left = 0;
	qreal top = 0;
	qreal right = 0;
	qreal bottom = 0;

	printer->getPageMargins(&left, &top, &right, &bottom, QPrinter::Millimeter);

	return QMargins(qRound(left * 10), qRound(top * 10), qRound(right * 10), qRound(bottom * 10));
}

/// Return the QPrinter margins
MarginsF Printer::margins(const QPrinter* printer, const QPrinter::Unit unit)
{
	Q_ASSERT(printer);

	qreal left = 0;
	qreal top = 0;
	qreal right = 0;
	qreal bottom = 0;

	printer->getPageMargins(&left, &top, &right, &bottom, unit);

	return MarginsF(left, top, right, bottom);
}

/// Return the paper size in tenths of a mm
QSizeF Printer::paperSize(const QPrinter* printer)
{
	Q_ASSERT(printer);

	return printer->paperSize(QPrinter::Millimeter) * 10;
}

/// Return the size of the page (paper size less the current margins)
QSizeF Printer::pageSize(const QPrinter* printer)
{
	Q_ASSERT(printer);

	const QSizeF size = Printer::paperSize(printer);
	const QMargins margins = Printer::margins(printer);

	return QSizeF(size.width() - (margins.left() + margins.right()),
				  size.height() - (margins.top() + margins.bottom()));
}

/// Apply settings from one printer to another
void Printer::applySettings(QPrinter* to, const QPrinter* from)
{
	Q_ASSERT(to);
	Q_ASSERT(from);

	to->setPaperSize(from->paperSize(QPrinter::Millimeter), QPrinter::Millimeter);
	to->setFullPage(from->fullPage());
	to->setOrientation(from->orientation());

	qreal left = 0;
	qreal top = 0;
	qreal right = 0;
	qreal bottom = 0;
	from->getPageMargins(&left, &top, &right, &bottom, QPrinter::Millimeter);
	to->setPageMargins(left, top, right, bottom, QPrinter::Millimeter);
}

/// Return the top-left corner in tenths of a mm
QPointF Printer::topLeft(const QPrinter* printer)
{
	Q_ASSERT(printer);

	const QRectF rect = printer->pageRect(QPrinter::Millimeter);
	QPointF pt = rect.topLeft();
	pt.rx() *= 10;
	pt.ry() *= 10;
	return pt;
}

/// Returns true if the printer exists
bool Printer::exists(const QString& printerName)
{
	return !info(printerName).isNull();
}

/// Return the info for a named printer
QPrinterInfo Printer::info(const QString& printerName)
{
	QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
	foreach (const QPrinterInfo& printer, printers)
	{
		if (printer.printerName().compare(printerName, Qt::CaseInsensitive) == 0)
		{
			return printer;
		}
	}
	return QPrinterInfo();
}
