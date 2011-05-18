#include "SendSettings.h"

SendSettings::SendSettings(const QString& to, const QString& cc, const QString& subject, const QString& body) :
    to_(to),
    cc_(cc),
    subject_(subject),
    body_(body)
{
}

bool SendSettings::isValid() const
{
    return !to_.isEmpty() || !cc_.isEmpty();
}

QString SendSettings::to() const
{
    return to_;
}

QString SendSettings::cc() const
{
    return cc_;
}

QString SendSettings::subject() const
{
    return subject_;
}

QString SendSettings::body() const
{
    return body_;
}
