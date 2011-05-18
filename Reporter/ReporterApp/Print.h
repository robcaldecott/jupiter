#ifndef PRINT_H
#define PRINT_H

#include <QObject>
#include <QString>

class QUrl;
class QPrinter;
class DownloadFile;
class Report;

/// Class used to print a file to a named/default printer
/// using no UI.
class Print : public QObject
{
    Q_OBJECT

public:
    explicit Print(QObject *parent = 0);
    ~Print();
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

#endif // PRINT_H
