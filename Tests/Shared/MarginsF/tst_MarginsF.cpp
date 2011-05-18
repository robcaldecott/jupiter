#include <QtCore/QString>
#include <QtTest/QtTest>
#include "MarginsF.h"

class tst_MarginsF : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void defaultConstructor();
	void constructor();
};

void tst_MarginsF::defaultConstructor()
{
	MarginsF margins;
	QCOMPARE(margins.left(), 0.0);
	QCOMPARE(margins.top(), 0.0);
	QCOMPARE(margins.right(), 0.0);
	QCOMPARE(margins.bottom(), 0.0);
}

void tst_MarginsF::constructor()
{
	MarginsF margins(1.0, 2.0, 3.0, 4.0);
	QCOMPARE(margins.left(), 1.0);
	QCOMPARE(margins.top(), 2.0);
	QCOMPARE(margins.right(), 3.0);
	QCOMPARE(margins.bottom(), 4.0);
}

QTEST_APPLESS_MAIN(tst_MarginsF);

#include "tst_MarginsF.moc"
