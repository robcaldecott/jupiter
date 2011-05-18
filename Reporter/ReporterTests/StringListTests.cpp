#include "StringListTests.h"
#include "StringList.h"

void StringListTests::trimRight()
{
    QStringList list;
    list << "1";
    list << "2";
    list << "";

    list = StringList::trimRight(list);
    QCOMPARE(list.count(), 2);
    QCOMPARE(list.last(), QString("2"));
}
