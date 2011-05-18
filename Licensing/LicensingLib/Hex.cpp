#include "Hex.h"
#include <QString>

namespace
{
    const QString table = "ZQXJKVBPYGFWMUCL";

    int hexToInt(const char ch)
    {
	if (ch >= '0' && ch <= '9')
	{
	    return (ch - '0');
	}

	if (ch >= 'A' && ch <= 'F')
	{
	    return (ch - 'A' + 10);
	}

	if (ch >= 'a' && ch <= 'f')
	{
	    return (ch - 'a' + 10);
	}

	return 0;
    }

    char intToHex(const int n)
    {
	if (n >= 0 && n <= 9)
	{
	    return '0' + n;
	}

	if (n >= 10 && n <= 15)
	{
	    return 'A' + (n - 10);
	}

	return 0;
    }
}

// Obfuscate a hex string
QString Hex::obfuscate(const QString& str)
{
    QString ret;
    for (int i = 0, len = str.length(); i < len; ++i)
    {
	ret += table[hexToInt(str[i].toAscii())];
    }
    return ret;
}

// Normalize an obfuscated hex string
QString Hex::normalize(const QString& str)
{
    QString ret;
    for (int i = 0, len = str.length(); i < len; ++i)
    {
	const int index = table.indexOf(str[i], 0, Qt::CaseInsensitive);
	if (index != -1)
	{
	    ret += intToHex(index);
	}
    }
    return ret;
}
