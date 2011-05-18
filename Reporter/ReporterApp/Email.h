#ifndef EMAIL_H
#define EMAIL_H

#include <QObject>

class SendSettings;
class EmailSettings;
class Pdf;
class QString;
class QTemporaryFile;

class Email : public QObject
{
    Q_OBJECT
public:
    explicit Email(QObject *parent = 0);
    ~Email();
    void send(const QString& fileName, const SendSettings& send, const EmailSettings& email);

signals:
    void finished();

private slots:
    void pdfCreated();
    void mailSent();
    void mailError(const QString& error);

private:
    Pdf* pdf;
    QTemporaryFile* tempFile;
    SendSettings* sendSettings;
    EmailSettings* emailSettings;
};

#endif // EMAIL_H
