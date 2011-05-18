#include "PrintDialog.h"
#include "Report.h"
#include "ReportView.h"
#include "DownloadFile.h"
#include "Printer.h"
#include <QDebug>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QPainter>
#include <QTimer>
#include <QUrl>
#include <QMessageBox>

PrintDialog::PrintDialog(QWidget *parent) :
    QWidget(parent),
    report(0),
    printer(0)
{
    downloader = new DownloadFile(this);
    connect(downloader, SIGNAL(completed(QString)), this, SLOT(downloadFinished(QString)));
    connect(downloader, SIGNAL(error(QUrl,QString)), this, SLOT(downloadError(QUrl,QString)));
    // Create a hidden widget to avoid a QPrintDialog assert
    create();
}

PrintDialog::~PrintDialog()
{
    delete printer;
}

void PrintDialog::downloadFinished(const QString& data)
{
    printer = new QPrinter(Printer::info(printerName));
    if (!printer->isValid())
    {
	QPrintDialog printDialog(printer, this);
	if (printDialog.exec() != QDialog::Accepted)
	{
	    emit finished();
	    return;
	}
    }
    printer->setPageMargins(.5, .5, .5, .5, QPrinter::Inch);
    printer->setDocName("Jupiter Reporter");

    report = new Report(printer, this);
    connect(report, SIGNAL(loaded()), this, SLOT(reportLoaded()));
    report->setData(data);
}

void PrintDialog::downloadError(const QUrl& url, const QString& error)
{
    QMessageBox::warning(this, tr("Download Error"), tr("Failed to download %1: %2").arg(url.toString()).arg(error));
    emit finished();
}

void PrintDialog::print(const QString& fileName, const QString& printerName)
{
    this->printerName = printerName;
    downloader->download(fileName);
}

void PrintDialog::reportLoaded()
{
    QPainter painter(printer);
    ReportView view(report);
    view.paint(painter, *printer);

    emit finished();
}
