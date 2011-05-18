#include "Pdf.h"
#include "Report.h"
#include "ReportView.h"
#include "DownloadFile.h"
#include <QPrinter>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QUrl>
#include <iostream>

Pdf::Pdf(QObject* parent) :
    QObject(parent),
    report(0),
    printer(0)
{
    downloader = new DownloadFile(this);
    connect(downloader, SIGNAL(completed(QString)), this, SLOT(downloadFinished(QString)));
    connect(downloader, SIGNAL(error(QUrl,QString)), this, SLOT(downloadError(QUrl,QString)));
}

Pdf::~Pdf()
{
    delete printer;
}

void Pdf::create(const QString& reportFileName, const QString& pdfFileName)
{
    this->pdfFileName = pdfFileName;
    downloader->download(reportFileName);
}

void Pdf::downloadFinished(const QString& data)
{
    printer = new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(pdfFileName);
    printer->setColorMode(QPrinter::Color);
    printer->setPageMargins(.5, .5, .5, .5, QPrinter::Inch);

    report = new Report(printer, this);
    connect(report, SIGNAL(loaded()), this, SLOT(reportLoaded()));
    report->setData(data);
}

void Pdf::downloadError(const QUrl& url, const QString& error)
{
    std::cerr << "Download of " << qPrintable(url.toString()) << " failed: " << qPrintable(error) << std::endl;
    emit finished();
}

void Pdf::reportLoaded()
{
    QPainter painter;

    if (painter.begin(printer))
    {
	ReportView view(report);
	view.paint(painter, *printer);
	painter.end();
    }
    else
    {
	std::cerr << "Error creating PDF " << qPrintable(pdfFileName) << std::endl;
    }

    emit finished();
}
