#include "DownloadPictures.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QPixmap>
#include <QDebug>

DownloadPictures::DownloadPictures(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadPictures::download(const QUrl& url)
{
    QNetworkReply* reply = manager->get(QNetworkRequest(url));
    downloads[reply] = url;
}

void DownloadPictures::downloadFinished(QNetworkReply* reply)
{
    QPixmap pixmap;

    if (reply->error() == QNetworkReply::NoError)
    {
	const QByteArray bytes = reply->readAll();
	if (!bytes.isEmpty())
	{	    
	    if (!pixmap.loadFromData(bytes))
	    {
		qDebug() << "Error loading" << reply->url();
	    }
	}
    }
    else
    {
	qDebug() << reply->errorString() << reply->url();
    }

    const QUrl url = downloads[reply];
    emit downloaded(url, pixmap);

    downloads.remove(reply);
    if (downloads.isEmpty())
    {
	emit finished();
    }

    reply->deleteLater();
}
