#include "Print.h"
#include "Pdf.h"
#include "Email.h"
#include "SendSettings.h"
#include "EmailSettings.h"
#include "NodeLockedLicense.h"
#include <QApplication>
#include <QxtCommandOptions>
#include <QSettings>
#include <QDesktopServices>
#include <QDate>
#include <iostream>

namespace
{
    SendSettings parseSendSettings(const QxtCommandOptions& options)
    {
	return SendSettings(options.value("to").toString(),
			    options.value("cc").toString(),
			    options.value("subject").toString(),
			    options.value("body").toString());
    }

    EmailSettings parseEmailSettings(const QxtCommandOptions& options)
    {
	return EmailSettings(options.value("server").toString(),
			     options.value("port").toInt(),
			     options.count("ssl") == 1,
			     options.value("account").toString(),
			     options.value("password").toString(),
			     options.value("from").toString());
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Jupiter Reporter");
    a.setOrganizationName("XE-QT Solutions");
    a.setOrganizationDomain("xe-qt-solutions.com");

    QxtCommandOptions options;
    options.add("version", "show version number and build date");
    options.alias("version", "ver");
    options.add("help", "show this help text");

    options.addSection("printing and PDF");
    options.add("print", "print a report to a specific printer (blank for default)", QxtCommandOptions::ValueOptional);
    options.add("pdf", "save a report as a PDF file", QxtCommandOptions::ValueRequired);

    options.addSection("email");
    options.add("to", "list of email recipients", QxtCommandOptions::ValueRequired);
    options.add("cc", "list of email cc recipients", QxtCommandOptions::ValueRequired);
    options.add("subject", "the email subject", QxtCommandOptions::ValueRequired);
    options.add("body", "the body of the email message", QxtCommandOptions::ValueRequired);
    options.add("server", "the email smtp server", QxtCommandOptions::ValueRequired);
    options.add("port", "the smtp server port", QxtCommandOptions::ValueRequired);
    options.add("ssl", "use a secure socket when sending email");
    options.add("account", "the name of the smtp account", QxtCommandOptions::ValueRequired);
    options.add("password", "the smtp account password", QxtCommandOptions::ValueRequired);
    options.add("from", "the email address of the email sender", QxtCommandOptions::ValueRequired);

    options.addSection("licensing");
    options.add("lic", "display license details");
    options.add("name", "the licensee name", QxtCommandOptions::ValueRequired);
    options.add("email", "the licensee email address", QxtCommandOptions::ValueRequired);
    options.add("expires", "the license expiry date (YYYY-MM-DD format)", QxtCommandOptions::ValueRequired);
    options.add("key", "the activation key to unlock advanced features", QxtCommandOptions::ValueRequired);
    options.parse(a.arguments());

    if (options.count("version"))
    {
	std::cout << "Jupiter Reporter v" << VER_MAJOR << "." << VER_MINOR << ", built on " << __DATE__ << " at " << __TIME__ << std::endl;
	std::cout << "Copyright (c) 2010 XE-QT Solutions Ltd., All rights reserved." << std::endl;
	return 0;
    }

    QSettings settings(QString("%1/license.ini").arg(QDesktopServices::storageLocation(QDesktopServices::DataLocation)), QSettings::IniFormat);

    // Generate license?
    if (options.count("name") || options.count("email") || options.count("expires"))
    {
	const QString name = options.value("name").toString();
	const QString email = options.value("email").toString();
	const QString expires = options.value("expires").toString();

	if (name.isEmpty() || email.isEmpty() || expires.isEmpty())
	{
	    options.showUsage();
	    return 0;
	}

	settings.setValue("License/name", name);
	settings.setValue("License/email", email);
	settings.setValue("License/expires", expires);
	settings.setValue("License/license", NodeLockedLicense::licenseFile(name, email, QDate::fromString(expires, Qt::ISODate)));

	std::cout << "Generated " << qPrintable(settings.fileName()) << std::endl;

	return 0;
    }

    if (options.count("key"))
    {
	settings.setValue("License/key", options.value("key").toString());
	settings.remove("License/license");
	std::cout << "Updated " << qPrintable(settings.fileName()) << std::endl;
	return 0;
    }

    // Read license
    const QString name = settings.value("License/name").toString();
    const QString email = settings.value("License/email").toString();
    const QDate expires = QDate::fromString(settings.value("License/expires").toString(), Qt::ISODate);
    const QString key = settings.value("License/key").toString();

    bool isLicensed = NodeLockedLicense::isValid(name, email, expires, key);

    if (options.count("lic"))
    {
	std::cout << "License: " << qPrintable(settings.fileName()) << std::endl;

	if (!isLicensed)
	{
	    std::cout << "No valid license exists.  Advanced functionality will be disabled." << std::endl;
	}
	else
	{
	    std::cout << "Licensed to " << qPrintable(name) << " (" << qPrintable(email) << "), expires on " << qPrintable(expires.toString()) << std::endl;
	}
	return 0;
    }

    if (options.positional().isEmpty() || options.count("help") || options.showUnrecognizedWarning() ||
	(options.count("print") == 0 && options.count("pdf") == 0 && options.count("to") == 0))
    {
	options.showUsage();
	return 0;
    }

    const QString fileName = options.positional().first();
    Q_ASSERT(fileName != "");

    if (options.count("print"))
    {
	Print print;
	QObject::connect(&print, SIGNAL(finished()), &a, SLOT(quit()));
	print.print(fileName, options.value("print").toString());
	return a.exec();
    }

    if (options.count("pdf"))
    {
	if (isLicensed)
	{
	    Pdf pdf;
	    QObject::connect(&pdf, SIGNAL(finished()), &a, SLOT(quit()));
	    pdf.create(fileName, options.value("pdf").toString());
	    return a.exec();
	}
	else
	{
	    std::cout << "PDF support requires a license!" << std::endl;
	    return 0;
	}
    }

    if (options.count("to") || options.count("cc"))
    {
	if (isLicensed)
	{
	    Email email;
	    email.send(fileName, parseSendSettings(options), parseEmailSettings(options));
	    QObject::connect(&email, SIGNAL(finished()), &a, SLOT(quit()));
	    return a.exec();
	}
	else
	{
	    std::cout << "Email support requires a license!" << std::endl;
	    return 0;
	}
    }

    return 0;
}
