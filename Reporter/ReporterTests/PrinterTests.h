#ifndef PRINTERTESTS_H
#define PRINTERTESTS_H

#include "AutoTest.h"

class PrinterTests : public QObject
{
    Q_OBJECT

private slots:
    void margins();
    void paperSize();
    void pageSize();
};

DECLARE_TEST(PrinterTests)

#endif // PRINTERTESTS_H
