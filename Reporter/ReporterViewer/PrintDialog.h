#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QWidget>
#include <QString>

class QPrinter;
class QUrl;
class DownloadFile;
class Report;

class PrintDialog : public QWidget
{
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent = 0);
    ~PrintDialog();
    void print(const QString& fileName, const QString& printerName = "");

signals:
    void finished();

private slots:
    void downloadFinished(const QString& data);
    void downloadError(const QUrl& url, const QString& error);
    void reportLoaded();

private:
    DownloadFile* downloader;
    Report* report;
    QPrinter* printer;
    QString printerName;
};

#endif // PRINTDIALOG_H
