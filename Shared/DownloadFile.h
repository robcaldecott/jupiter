#ifndef DOWNLOADFILE_H
#define DOWNLOADFILE_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;
class QString;
class QByteArray;

class DownloadFile : public QObject
{
	Q_OBJECT
public:
	explicit DownloadFile(QObject *parent = 0);
	void download(const QUrl& url);

signals:
	void completed(const QByteArray& data);
	void error(const QUrl& url, const QString& error);
	void finished();

public slots:
	void downloadFinished(QNetworkReply* reply);

private:
	QNetworkAccessManager* manager;
};

#endif // DOWNLOADFILE_H
