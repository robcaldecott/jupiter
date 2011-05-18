#ifndef MACADDRESSES_H
#define MACADDRESSES_H

#include <QChar>

class QStringList;
class QString;

// Functions used to obtain the network MAC addresses for this machine.
namespace MacAddresses
{
    // Return a list of MAC addresses
    QStringList toStringList();
    // Return a delimited string of MAC addresses
    QString toString(QChar delimiter = '\n');
}

#endif // MACADDRESSES_H
