#include "DownloadFile.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

DownloadFile::DownloadFile(QObject *parent) :
	QObject(parent)
{
	manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadFile::download(const QUrl& url)
{
	// Start the download
	manager->get(QNetworkRequest(url));
}

void DownloadFile::downloadFinished(QNetworkReply* reply)
{
	if (reply->error())
		emit error(reply->url(), reply->errorString());
	else
		emit completed(reply->readAll());

	emit finished();
}
