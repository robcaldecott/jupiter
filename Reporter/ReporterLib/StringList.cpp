#include "StringList.h"
#include <QStringList>

QStringList StringList::trimRight(const QStringList& list)
{
    QStringList copy(list);

    while (!copy.isEmpty() && copy.last().isEmpty())
    {
	copy.removeLast();
    }

    return copy;
}
