#ifndef SMTP_H
#define SMTP_H

#include <QObject>

class SendSettings;
class EmailSettings;
class QString;
class QByteArray;
class QxtSmtp;

class Smtp : public QObject
{
    Q_OBJECT

public:
    explicit Smtp(QObject *parent = 0);
    void sendFile(const SendSettings& sendSettings, const EmailSettings& emailSettings, const QString& fileName);

signals:
    void error(const QString& msg);
    void mailSent();
    void finished();

private slots:
    void authenticationFailed(const QByteArray& msg);
    void connectionFailed(const QByteArray& msg);
    void encryptionFailed(const QByteArray& msg);
    void mailFailed(int mailID, int errorCode, const QByteArray& msg);
    void mailSent(int mailID);
    void recipientRejected(int mailID, const QString& address, const QByteArray& msg);
    void senderRejected(int mailID, const QString& address, const QByteArray& msg);
    void sendFinished();

private:
    QxtSmtp* smtp;
};

#endif // SMTP_H
