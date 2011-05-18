#include "PrinterTests.h"
#include "Printer.h"
#include <QPrinter>

void PrinterTests::margins()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageMargins(10, 20, 30, 45, QPrinter::Millimeter);
    QMargins margins = Printer::margins(&printer);
    QCOMPARE(margins.left(), 100);
    QCOMPARE(margins.top(), 200);
    QCOMPARE(margins.right(), 300);
    QCOMPARE(margins.bottom(), 450);
}

void PrinterTests::paperSize()
{
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    const QSizeF size = Printer::paperSize(&printer);
    QCOMPARE(size.width(), 2100.0);
    QCOMPARE(size.height(), 2970.0);
}

void PrinterTests::pageSize()
{
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    printer.setPageMargins(10, 20, 30, 40, QPrinter::Millimeter);
    const QSizeF size = Printer::pageSize(&printer);
    QCOMPARE(size.width(), 1700.0);
    QCOMPARE(size.height(), 2370.0);
}
