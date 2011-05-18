#ifndef SENDSETTINGS_H
#define SENDSETTINGS_H

#include <QString>

class SendSettings
{
public:
    SendSettings(const QString& to, const QString& cc, const QString& subject, const QString& body);
    bool isValid() const;

    QString to() const;
    QString cc() const;
    QString subject() const;
    QString body() const;

private:
    QString to_;
    QString cc_;
    QString subject_;
    QString body_;
};

#endif // SENDSETTINGS_H
