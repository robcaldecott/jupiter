#ifndef HEX_H
#define HEX_H

class QString;

namespace Hex
{
    // Obfuscate a hex string
    QString obfuscate(const QString& str);
    // Normalize an obfuscated hex string
    QString normalize(const QString& str);
}

#endif // HEX_H
