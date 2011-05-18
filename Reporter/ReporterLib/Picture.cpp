#include "Picture.h"

Picture::Picture()
{
}

Picture::Picture(const QUrl& url, const QRectF& rect) :
    url_(url),
    rect_(rect)
{
}

QUrl Picture::url() const
{
    return url_;
}

void Picture::setUrl(const QUrl& url)
{
    url_ = url;
}

QPixmap Picture::pixmap() const
{
    return pixmap_;
}

void Picture::setPixmap(const QPixmap& pixmap)
{
    pixmap_ = pixmap;
}

QRectF Picture::rect() const
{
    return rect_;
}

void Picture::setRect(const QRectF& rect)
{
    rect_ = rect;
}
