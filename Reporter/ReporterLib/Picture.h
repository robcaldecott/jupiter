#ifndef PICTURE_H
#define PICTURE_H

#include <QUrl>
#include <QRectF>
#include <QPixmap>

/// Class used to store report picture details
class Picture
{
public:
    enum Alignment
    {
	Center = -1,
	Right = -2,
	Bottom = -2,
    };

    Picture();
    Picture(const QUrl& url, const QRectF& rect);

    QUrl url() const;
    void setUrl(const QUrl& url);

    QPixmap pixmap() const;
    void setPixmap(const QPixmap& pixmap);

    QRectF rect() const;
    void setRect(const QRectF& rect);

private:
    QUrl url_;
    QPixmap pixmap_;
    QRectF rect_;
};

#endif // PICTURE_H
