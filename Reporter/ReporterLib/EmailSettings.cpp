#include "EmailSettings.h"
#include <QSettings>

EmailSettings::EmailSettings() :
    server_("smtp.gmail.com"),
    port_(465),
    ssl_(true)
{
}

EmailSettings::EmailSettings(const QString& server, int port, bool ssl, const QString& accountName,
			     const QString& password, const QString& emailAddress) :
    server_(server),
    port_(port),
    ssl_(ssl),
    accountName_(accountName),
    password_(password),
    emailAddress_(emailAddress)
{
}

bool EmailSettings::isEmpty() const
{
    return server_.isEmpty() || accountName_.isEmpty() || password_.isEmpty() || emailAddress_.isEmpty();
}

QString EmailSettings::server() const
{
    return server_;
}

void EmailSettings::setServer(const QString& server)
{
    server_ = server;
}

int EmailSettings::port() const
{
    return port_;
}

void EmailSettings::setPort(int port)
{
    port_ = port;
}

bool EmailSettings::ssl() const
{
    return ssl_;
}

void EmailSettings::setSsl(bool ssl)
{
    ssl_ = ssl;
}

QString EmailSettings::accountName() const
{
    return accountName_;
}

void EmailSettings::setAccountName(const QString& accountName)
{
    accountName_ = accountName;
}

QString EmailSettings::password() const
{
    return password_;
}

void EmailSettings::setPassword(const QString& password)
{
    password_ = password;
}

QString EmailSettings::emailAddress() const
{
    return emailAddress_;
}

void EmailSettings::setEmailAddress(const QString& emailAddress)
{
    emailAddress_ = emailAddress;
}
