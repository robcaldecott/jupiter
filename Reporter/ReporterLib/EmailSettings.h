#ifndef EMAILSETTINGS_H
#define EMAILSETTINGS_H

#include <QString>

class QSettings;

class EmailSettings
{
public:
	EmailSettings();
	EmailSettings(const QString& server, int port, bool ssl, const QString& accountName,
				  const QString& password, const QString& emailAddress);

	bool isEmpty() const;

	QString server() const;
	void setServer(const QString& server);

	int port() const;
	void setPort(int port);

	bool ssl() const;
	void setSsl(bool ssl);

	QString accountName() const;
	void setAccountName(const QString& accountName);

	QString password() const;
	void setPassword(const QString& password);

	QString emailAddress() const;
	void setEmailAddress(const QString& emailAddress);

private:
	QString server_;
	int port_;
	bool ssl_;
	QString accountName_;
	QString password_;
	QString emailAddress_;
};

#endif // EMAILSETTINGS_H
