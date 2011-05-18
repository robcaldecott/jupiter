#ifndef DOWNLOADPICTURES_H
#define DOWNLOADPICTURES_H

#include <QObject>
#include <QMap>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;
class QPixmap;

class DownloadPictures : public QObject
{
    Q_OBJECT

public:
    explicit DownloadPictures(QObject *parent = 0);
    void download(const QUrl& url);

signals:
    void downloaded(const QUrl& url, const QPixmap& pixmap);
    void finished();

private slots:
    void downloadFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* manager;
    QMap<QNetworkReply*, QUrl> downloads;
};

#endif // DOWNLOADPICTURES_H
