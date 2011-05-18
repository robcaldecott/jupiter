#ifndef PRINTDEFAULT_H
#define PRINTDEFAULT_H

#include <QObject>

class QString;
class QUrl;
class QPrinter;
class DownloadFile;
class Report;

class PrintDefault : public QObject
{
    Q_OBJECT

public:
    explicit PrintDefault(QObject* parent = 0);
    ~PrintDefault();
    void print(const QString& fileName);

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
};

#endif // PRINTDEFAULT_H
