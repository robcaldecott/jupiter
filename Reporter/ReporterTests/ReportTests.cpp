#include "ReportTests.h"
#include "Report.h"
#include <QSignalSpy>

void ReportTests::initTestCase()
{
    printer = new QPrinter;
}

void ReportTests::cleanupTestCase()
{
    delete printer;
}

void ReportTests::constructor()
{
    Report report(printer);
    QCOMPARE(report.data(), QString());
    QCOMPARE(report.rows(), 0);
    QCOMPARE(report.cols(), 0);
}

void ReportTests::data()
{
    Report report(printer);
    QSignalSpy spy(&report, SIGNAL(changed()));

    report.setData("DATA");
    QCOMPARE(report.data(), QString("DATA"));
    QCOMPARE(spy.count(), 1);
}

void ReportTests::rows()
{
    Report report(printer);
    report.setData("1\n2\n\f3\n4\n");
    QCOMPARE(report.rows(), 66);
}

void ReportTests::cols()
{
    Report report(printer);
    report.setData("1\n2\n\f3\n4\n");
    QCOMPARE(report.cols(), 80);
}

void ReportTests::split()
{
    Report report(printer);
    report.setData("1\n2\n\f3\n4\n");
    const Report::Data data = report.split();
    QCOMPARE(data.size(), 2);
    QVERIFY(data.at(0) == QStringList() << "1" << "2");
    QVERIFY(data.at(1) == QStringList() << "3" << "4");
}
