#include "PrintDefault.h"
#include "Report.h"
#include "ReportView.h"
#include "DownloadFile.h"
#include <QtGlobal>
#include <QApplication>
#include <QPrinter>
#include <QPainter>
#include <QUrl>
#include <QMessageBox>

PrintDefault::PrintDefault(QObject* parent) :
    QObject(parent),
    report(0),
    printer(0)
{
    downloader = new DownloadFile(this);
    connect(downloader, SIGNAL(completed(QString)), this, SLOT(downloadFinished(QString)));
    connect(downloader, SIGNAL(error(QUrl,QString)), this, SLOT(downloadError(QUrl,QString)));
}

PrintDefault::~PrintDefault()
{
    delete printer;
}

void PrintDefault::print(const QString& fileName)
{
    downloader->download(fileName);
}

void PrintDefault::downloadFinished(const QString& data)
{
    printer = new QPrinter;
    printer->setPageMargins(.5, .5, .5, .5, QPrinter::Inch);
    printer->setDocName("Jupiter Reporter");

    report = new Report(printer, this);
    connect(report, SIGNAL(loaded()), this, SLOT(reportLoaded()));
    report->setData(data);
}

void PrintDefault::downloadError(const QUrl& url, const QString& error)
{
    QMessageBox::warning(0, tr("Download Error"), tr("Failed to download %1: %2").arg(url.toString()).arg(error));
    emit finished();
}

void PrintDefault::reportLoaded()
{
    QPainter painter(printer);
    ReportView view(report);
    view.paint(painter, *printer);

    emit finished();
}
