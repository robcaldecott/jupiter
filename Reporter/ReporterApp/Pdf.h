#ifndef PDF_H
#define PDF_H

#include <QObject>
#include <QString>

class DownloadFile;
class Report;
class QUrl;
class QPrinter;

class Pdf : public QObject
{
    Q_OBJECT

public:
    explicit Pdf(QObject* parent = 0);
    ~Pdf();
    void create(const QString& reportFileName, const QString& pdfFileName);

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
    QString pdfFileName;
};

#endif // PDF_H
