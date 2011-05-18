#include "Smtp.h"
#include "SendSettings.h"
#include "EmailSettings.h"
#include "File.h"
#include <qxtsmtp.h>
#include <qxtmailmessage.h>
#include <QString>
#include <QByteArray>

namespace
{
    void addRecipients(QxtMailMessage& msg, const QString& recipients, QxtMailMessage::RecipientType type)
    {
	if (!recipients.isEmpty())
	{
	    const QStringList list = recipients.split(",", QString::SkipEmptyParts);
	    foreach (const QString& recipient, list)
	    {
		msg.addRecipient(recipient, type);
	    }
	}
    }
}

Smtp::Smtp(QObject *parent) :
    QObject(parent)
{
    smtp = new QxtSmtp(this);

    connect(smtp, SIGNAL(authenticationFailed(QByteArray)), this, SLOT(authenticationFailed(QByteArray)));
    connect(smtp, SIGNAL(connectionFailed(QByteArray)), this, SLOT(connectionFailed(QByteArray)));
    connect(smtp, SIGNAL(encryptionFailed(QByteArray)), this, SLOT(encryptionFailed(QByteArray)));
    connect(smtp, SIGNAL(mailFailed(int,int,QByteArray)), this, SLOT(mailFailed(int,int,QByteArray)));
    connect(smtp, SIGNAL(mailSent(int)), this, SLOT(mailSent(int)));
    connect(smtp, SIGNAL(recipientRejected(int,QString,QByteArray)), this, SLOT(recipientRejected(int,QString,QByteArray)));
    connect(smtp, SIGNAL(senderRejected(int,QString,QByteArray)), this, SLOT(senderRejected(int,QString,QByteArray)));
    connect(smtp, SIGNAL(finished()), this, SLOT(sendFinished()));
}

void Smtp::sendFile(const SendSettings& sendSettings, const EmailSettings& emailSettings, const QString& fileName)
{
    Q_ASSERT(sendSettings.isValid());
    Q_ASSERT(!emailSettings.isEmpty());
    Q_ASSERT(!fileName.isEmpty());

    smtp->setUsername(emailSettings.accountName().toAscii());
    smtp->setPassword(emailSettings.password().toAscii());

    if (emailSettings.ssl())
    {
	smtp->connectToSecureHost(emailSettings.server(), emailSettings.port());
    }
    else
    {
	smtp->connectToHost(emailSettings.server(), emailSettings.port());
    }

    QxtMailMessage msg;
    addRecipients(msg, sendSettings.to(), QxtMailMessage::To);
    addRecipients(msg, sendSettings.cc(), QxtMailMessage::Cc);
    msg.setSubject(sendSettings.subject());
    msg.setBody(sendSettings.body());
    msg.setSender(emailSettings.emailAddress());
    msg.setExtraHeader("X-Mailer", "Jupiter Reporter");
    msg.addAttachment("report.pdf", QxtMailAttachment(File::loadBytes(fileName), "application/pdf"));

    smtp->send(msg);
}

void Smtp::authenticationFailed(const QByteArray& msg)
{
    emit error(tr("SMTP authentication failed: %1").arg(QString(msg)));
}

void Smtp::connectionFailed(const QByteArray& msg)
{
    emit error(tr("SMTP connection failed: %1").arg(QString(msg)));
}

void Smtp::encryptionFailed(const QByteArray& msg)
{
    emit error(tr("SMTP encryption failed: %1").arg(QString(msg)));
}

void Smtp::mailFailed(int mailID, int errorCode, const QByteArray& msg)
{
    emit error(tr("SMTP error %1 sending message: %2").arg(errorCode).arg(QString(msg)));
}

void Smtp::mailSent(int mailID)
{
    emit mailSent();
}

void Smtp::recipientRejected(int mailID, const QString& address, const QByteArray& msg)
{
    emit error(tr("SMTP recipient %1 rejected: %2").arg(address).arg(QString(msg)));
}

void Smtp::senderRejected(int mailID, const QString& address, const QByteArray& msg)
{
    emit error(tr("SMTP sender %1 rejected: %2").arg(address).arg(QString(msg)));
}

void Smtp::sendFinished()
{
    emit finished();
}
