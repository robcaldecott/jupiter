#ifndef PRINTER_H
#define PRINTER_H

#include "MarginsF.h"
#include <QMargins>
#include <QSizeF>
#include <QPrinter>

class QPointF;
class QString;
class QPrinterInfo;

namespace Printer
{
    /// Return the QPrinter margins in tenths of a mm
    QMargins margins(const QPrinter* printer);
    /// Return the QPrinter margins
    MarginsF margins(const QPrinter* printer, const QPrinter::Unit unit);
    /// Return the paper size in tenths of a mm
    QSizeF paperSize(const QPrinter* printer);
    /// Return the size of the page (paper size less the current margins)
    QSizeF pageSize(const QPrinter* printer);
    /// Apply settings from one printer to another
    void applySettings(QPrinter* to, const QPrinter* from);
    /// Return the top-left corner in tenths of a mm
    QPointF topLeft(const QPrinter* printer);
    /// Returns true if the printer exists
    bool exists(const QString& printerName);
    /// Return the info for a named printer
    QPrinterInfo info(const QString& printerName);
}

#endif // PRINTER_H
