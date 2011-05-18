#include "NodeLockedLicense.h"
#include "MacAddresses.h"
#include "Rc4.h"
#include "Hex.h"
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QStringList>
#include <QDebug>
#include <QDate>

namespace
{
    // Create a hash key based on a name/email address
    QByteArray key(const QString& name, const QString& email, const QDate& expiryDate)
    {
	const QString details = name.trimmed() + '~' + email.trimmed() + '~' + expiryDate.toString(Qt::ISODate);
	return QCryptographicHash::hash(details.toUtf8(), QCryptographicHash::Md5);
    }

    // Encrypt a list of MAC addresses using a specified hash
    QString encryptMacAddresses(const QByteArray& hash)
    {
	const QByteArray macAddresses = MacAddresses::toString('~').toAscii();
	const QByteArray encrypted = Rc4::process(macAddresses, hash);
	return Hex::obfuscate(encrypted.toHex().toUpper());
    }

    // Extract a value from a string
    QString value(const QString& keyName, const QString& str)
    {
	QString value;
	const QString lookFor(QString("%1=").arg(keyName));
	int index = str.indexOf(lookFor, 0, Qt::CaseInsensitive);
	if (index != -1)
	{
	    const int start = index + lookFor.length();
	    index = str.indexOf('\n', start);
	    value = str.mid(start, index == -1 ? -1 : index - start);
	}
	return value;
    }
}

// Generate a license file that can be turned into an activation key
QString NodeLockedLicense::licenseFile(const QString& name, const QString& email, const QDate& expiryDate)
{
    return encryptMacAddresses(key(name, email, expiryDate));
}

// Generate an activation key from a license file
QString NodeLockedLicense::activationKey(const QString& license)
{
    const QString name = value("Name", license);
    const QString email = value("Email", license);
    const QDate expiryDate = QDate::fromString(value("Expires", license), Qt::ISODate);
    const QString encryptedMacAddresses = value("License", license);

    if (name.isEmpty() || email.isEmpty() || expiryDate.isNull() || encryptedMacAddresses.isEmpty())
    {
	return "";
    }

    const QByteArray hash = key(name, email, expiryDate);
    const QByteArray encryptedBytes = QByteArray::fromHex(Hex::normalize(encryptedMacAddresses).toAscii());
    const QString details = name + '~' + email + '~' + expiryDate.toString(Qt::ISODate) + '~' + Rc4::process(encryptedBytes, hash);
    const QByteArray activationKeyBytes = Rc4::process(details.toUtf8(), hash);
    return Hex::obfuscate(activationKeyBytes.toHex().toUpper());
}

// Returns true if the license is valid
bool NodeLockedLicense::isValid(const QString& name, const QString& email, const QDate& expiryDate, const QString& license)
{
    const QByteArray keyBytes = QByteArray::fromHex(Hex::normalize(license).toAscii());
    const QByteArray decryptedBytes = Rc4::process(keyBytes, key(name, email, expiryDate));
    const QString decryptedLicense = QString::fromUtf8(decryptedBytes.constData(), decryptedBytes.length());
    const QStringList list = decryptedLicense.split('~');

    if (list.count() < 4 || list.at(0) != name || list.at(1) != email || list.at(2) != expiryDate.toString(Qt::ISODate))
    {
	return false;
    }

    // Expired?
    if (QDate::currentDate() > expiryDate)
    {
	return false;
    }

    const QStringList macAddresses = MacAddresses::toStringList();
    for (int i = 3; i < list.count(); ++i)
    {
	if (macAddresses.contains(list.at(i)))
	{
	    // This MAC address is valid
	    return true;
	}
    }
    return false;
}
