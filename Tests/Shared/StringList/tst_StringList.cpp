#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QStringList>
#include "StringList.h"

class tst_StringList : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void trimRight();
	void trimRight_data();
};

void tst_StringList::trimRight()
{
	QFETCH(QStringList, data);
	QFETCH(QStringList, expected);

	QCOMPARE(StringList::trimRight(data), expected);
}

void tst_StringList::trimRight_data()
{
	QTest::addColumn<QStringList>("data");
	QTest::addColumn<QStringList>("expected");

	QTest::newRow("right") << (QStringList() << "1" << "2" << "") << (QStringList() << "1" << "2");
	QTest::newRow("middle") << (QStringList() << "1" << "" << "3") << (QStringList() << "1" << "" << "3");
	QTest::newRow("left") << (QStringList() << "" << "2" << "3") << (QStringList() << "" << "2" << "3");
}

QTEST_APPLESS_MAIN(tst_StringList);

#include "tst_StringList.moc"
