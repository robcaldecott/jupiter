#include "UserLicense.h"
#include "Rc4.h"
#include "Hex.h"
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

namespace
{
    // Convert an activation key to a readable string
    QString keyToString(const QByteArray& bytes)
    {
	QString ret;
	const QString str(Hex::obfuscate(bytes.toHex().toUpper()));
	for (int i = 0, len = str.length(); i < len; ++i)
	{
	    ret += str[i];
	    if ((i + 1) % 8 == 0 && i < len - 1)
	    {
		ret += '-';
	    }
	}
	return ret;
    }

    // Compare two activation keys
    bool compareKeys(QString key1, QString key2)
    {
	key1.remove('-');
	key2.remove('-');
	return QString::compare(key1, key2, Qt::CaseInsensitive) == 0;
    }
}

// Generate an activation key
QString UserLicense::activationKey(const QString& name, const QString& email)
{
    const QByteArray details = QString(name + '~' + email).toUtf8();
    const QByteArray hash = QCryptographicHash::hash(details, QCryptographicHash::Md5);
    const QByteArray encrypted = Rc4::process(details, hash);
    const QByteArray activationKeyBytes = QCryptographicHash::hash(encrypted, QCryptographicHash::Md5);
    return keyToString(activationKeyBytes);
}

// Returns true if an activation key is valid
bool UserLicense::isValid(const QString& name, const QString& email, const QString& key)
{
    return compareKeys(activationKey(name, email), key);
}
