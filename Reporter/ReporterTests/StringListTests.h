#ifndef STRINGLISTTESTS_H
#define STRINGLISTTESTS_H

#include "AutoTest.h"

class StringListTests : public QObject
{
    Q_OBJECT

private slots:
    void trimRight();
};

DECLARE_TEST(StringListTests)

#endif // STRINGLISTTESTS_H
