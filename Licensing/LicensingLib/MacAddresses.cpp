#include "MacAddresses.h"
#include <QString>
#include <QStringList>
#include <QNetworkInterface>

// Return a list of MAC addresses
QStringList MacAddresses::toStringList()
{
	QStringList list;
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	foreach (const QNetworkInterface& interface, interfaces)
	{
		if (interface.isValid())
		{
			const QString address = interface.hardwareAddress();
			if (!address.isEmpty() && !list.contains(address))
			{
				list << address;
			}
		}
	}
	return list;
}

// Return a delimited string of MAC addresses
QString MacAddresses::toString(const QChar delimiter/*='\n'*/)
{
	QString ret;
	const QStringList list = toStringList();
	foreach (const QString& address, list)
	{
		if (!ret.isEmpty())
		{
			ret += delimiter;
		}
		ret += address;
	}
	return ret;
}
