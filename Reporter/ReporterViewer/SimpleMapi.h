#ifndef SIMPLEMAPI_H
#define SIMPLEMAPI_H

class QString;

namespace Mapi
{
    /// Returns true if MAPI support is available
    bool isAvailable();
    /// Send a file using MAPI
    bool send(const QString& fileName);
}

#endif // SIMPLEMAPI_H
