#ifndef REPORTTESTS_H
#define REPORTTESTS_H

#include "AutoTest.h"

class QPrinter;

class ReportTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void constructor();
    void data();
    void rows();
    void cols();
    void split();

private:
    QPrinter* printer;
};

DECLARE_TEST(ReportTests)

#endif // REPORTTESTS_H
