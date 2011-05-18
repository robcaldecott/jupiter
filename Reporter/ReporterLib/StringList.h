#ifndef STRINGLIST_H
#define STRINGLIST_H

class QStringList;

namespace StringList
{
    /// Strip trailing blanks from a string list
    QStringList trimRight(const QStringList& list);
}

#endif // STRINGLIST_H
