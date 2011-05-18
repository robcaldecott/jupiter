#include <QtCore/QString>
#include <QtTest/QtTest>
#include "Font.h"

class tst_Font : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void cellSize_lpi_cpi();
	void cellSize_lpi_cpi_data();
	void cellSize_rows_cols();
	void cellSize_rows_cols_data();
	void cellSize_font();
	void setWidth();
	void lineHeight_rows();
	void lineHeight_lpi();
};

void tst_Font::cellSize_lpi_cpi()
{
	QFETCH(int, lpi);
	QFETCH(int, cpi);
	QFETCH(QSizeF, expected);

	QCOMPARE(Font::cellSize(lpi, cpi), expected);
}

void tst_Font::cellSize_lpi_cpi_data()
{
	QTest::addColumn<int>("lpi");
	QTest::addColumn<int>("cpi");
	QTest::addColumn<QSizeF>("expected");

	QTest::newRow("6x12") << 6 << 12 << QSizeF(21.16666666667, 42.33333333333);
	QTest::newRow("8x10") << 8 << 10 << QSizeF(25.4, 31.75);
	QTest::newRow("0x0") << 0 << 0 << QSizeF(0, 0);
}

void tst_Font::cellSize_rows_cols()
{
	QFETCH(int, rows);
	QFETCH(int, cols);
	QFETCH(QSizeF, pageSize);
	QFETCH(QSizeF, expected);

	QCOMPARE(Font::cellSize(rows, cols, pageSize), expected);
}

void tst_Font::cellSize_rows_cols_data()
{
	QTest::addColumn<int>("rows");
	QTest::addColumn<int>("cols");
	QTest::addColumn<QSizeF>("pageSize");
	QTest::addColumn<QSizeF>("expected");

	QTest::newRow("66x80 A4 Portrait") << 66 << 80 << QSizeF(1900, 2770) << QSizeF(23.75, 41.969696969697);
	QTest::newRow("66x132 A4 Portrait") << 66 << 132 << QSizeF(1900, 2770) << QSizeF(14.393939393939, 41.969696969697);
	QTest::newRow("66x132 A4 Landscape") << 66 << 132 << QSizeF(2770, 1900) << QSizeF(20.984848484848, 28.787878787878);
	QTest::newRow("0x0") << 0 << 0 << QSizeF(0, 0) << QSizeF(0, 0);
}

void tst_Font::cellSize_font()
{
	QFont font("Courier", 10);
	QFontMetricsF metrics(font);

	QCOMPARE(Font::cellSize(font), QSizeF(metrics.averageCharWidth(), metrics.height()));
}

void tst_Font::setWidth()
{
	// Fixed-width font test
	QFont newFont = Font::setWidth(QFont("Courier New"), 30);
	QVERIFY(QFontMetrics(newFont).averageCharWidth() >= 30);

	// Proportional font test
	const QFont font("Arial");
	newFont = Font::setWidth(font, 30);
	QVERIFY(QFontMetrics(newFont).averageCharWidth() == QFontMetrics(font).averageCharWidth());
}

void tst_Font::lineHeight_rows()
{
	QCOMPARE(Font::lineHeight(QFont("Courier New"), 66, 2770), 41.969696969697);
}

void tst_Font::lineHeight_lpi()
{
	const QFont font("Courier New");
	QCOMPARE(Font::lineHeight(font, 6), 42.3333333333);
	QCOMPARE(Font::lineHeight(font, 0), QFontMetricsF(font).height());
}

QTEST_MAIN(tst_Font);

#include "tst_Font.moc"
