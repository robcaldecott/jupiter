#include "Print.h"
#include "DownloadFile.h"
#include "Report.h"
#include "ReportView.h"
#include "Printer.h"
#include <QPrinterInfo>
#include <QPainter>
#include <iostream>

Print::Print(QObject *parent) :
    QObject(parent),
    report(0),
    printer(0)
{
    downloader = new DownloadFile(this);
    connect(downloader, SIGNAL(completed(QString)), this, SLOT(downloadFinished(QString)));
    connect(downloader, SIGNAL(error(QUrl,QString)), this, SLOT(downloadError(QUrl,QString)));
}

Print::~Print()
{
    delete printer;
}

void Print::print(const QString& fileName, const QString& printerName/*=""*/)
{
    this->printerName = printerName;
    downloader->download(fileName);
}

void Print::downloadFinished(const QString& data)
{
    printer = new QPrinter(Printer::info(printerName));
    printer->setPageMargins(.5, .5, .5, .5, QPrinter::Inch);
    printer->setDocName("Jupiter Reporter");

    report = new Report(printer, this);
    connect(report, SIGNAL(loaded()), this, SLOT(reportLoaded()));
    report->setData(data);
}

void Print::downloadError(const QUrl& url, const QString& error)
{
    std::cerr << "Download of " << qPrintable(url.toString()) << " failed: " << qPrintable(error);
    emit finished();
}

void Print::reportLoaded()
{
    QPainter painter(printer);
    ReportView view(report);
    view.paint(painter, *printer);

    emit finished();
}
