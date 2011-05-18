#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QPrinterInfo>
#include "Printer.h"

class tst_Printer : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void exists();
	void doesntExist();
};

void tst_Printer::exists()
{
	const QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
	QCOMPARE(Printer::exists(printers.first().printerName()), true);
}

void tst_Printer::doesntExist()
{
	QCOMPARE(Printer::exists("xxx"), false);
}

QTEST_APPLESS_MAIN(tst_Printer);

#include "tst_Printer.moc"
