#include "Email.h"
#include "SendSettings.h"
#include "EmailSettings.h"
#include "Pdf.h"
#include "Smtp.h"
#include <QString>
#include <QTemporaryFile>
#include <iostream>

Email::Email(QObject *parent) :
    QObject(parent),
    pdf(0),
    tempFile(0),
    sendSettings(0),
    emailSettings(0)
{
}

Email::~Email()
{
    delete sendSettings;
    delete emailSettings;
}

void Email::send(const QString& fileName, const SendSettings& send, const EmailSettings& email)
{
    pdf = new Pdf(this);
    connect(pdf, SIGNAL(finished()), this, SLOT(pdfCreated()));

    tempFile = new QTemporaryFile(this);
    tempFile->setAutoRemove(false);
    tempFile->open();

    sendSettings = new SendSettings(send);
    emailSettings = new EmailSettings(email);

    // Generate a temp PDF file
    pdf->create(fileName, tempFile->fileName());
}

void Email::pdfCreated()
{
    Smtp* smtp = new Smtp(this);
    connect(smtp, SIGNAL(mailSent()), this, SLOT(mailSent()));
    connect(smtp, SIGNAL(error(QString)), this, SLOT(mailError(QString)));
    connect(smtp, SIGNAL(finished()), smtp, SLOT(deleteLater()));
    smtp->sendFile(*sendSettings, *emailSettings, tempFile->fileName());
}

void Email::mailSent()
{
    emit finished();
}

void Email::mailError(const QString& error)
{
    std::cerr << "Failed to send email: " << qPrintable(error) << std::endl;
    emit finished();
}
