#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include "GraphicsItemTypes.h"
#include <QGraphicsObject>
#include <QScopedPointer>
#include <QFont>

class QDataStream;
class QPixmap;
class QUrl;
class QByteArray;

class GraphicsItem : public QGraphicsObject
{
	Q_OBJECT

public:
	friend QDataStream& operator<<(QDataStream&, const GraphicsItem&);
	friend QDataStream& operator>>(QDataStream&, GraphicsItem&);

	enum { Type = GraphicsItemTypes::Item };
	enum PictureMode { KeepAspectRatio, Stretch };
	enum FontHeight { Normal, Stretched };

	GraphicsItem(const QRectF& rect, QGraphicsItem* parent = 0);
	~GraphicsItem();
	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	QPainterPath shape() const;
	int type() const { return Type; }
	QRectF rect() const;

	QFont font() const;
	QColor textColor() const;
	QColor backColor() const;
	QColor borderColor() const;
	qint32 borderWidth() const;
	quint32 horzAlign() const;
	quint32 vertAlign() const;
	QString text() const;
	QUrl pictureUrl() const;
	bool box() const;
	quint32 pictureMode() const;
	quint32 fontHeight() const;
	qreal padding() const;
	bool autoLines() const;
	bool firstLineBold() const;
	bool stripes() const;
	QColor stripeColor() const;

signals:
	void changed();

public slots:
	void setFont(const QFont& font);
	void setBackColor(const QColor& color);
	void setTextColor(const QColor& color);
	void setBorderColor(const QColor& color);
	void setBorderWidth(qint32 width);
	void setBold(bool bold);
	void setItalic(bool italic);
	void setHorzAlign(quint32 align);
	void setVertAlign(quint32 align);
	void setText(const QString& text);
	void setPictureUrl(const QUrl& url);
	void setPicture(const QPixmap& pixmap);
	void setBox(bool box);
	void setPictureMode(quint32 mode);
	void gridSizeChanged();
	void setFontHeight(quint32 height);
	void setRect(const QRectF& rect);
	void setPadding(qreal padding);
	void setAutoLines(bool autoLines);
	void setFirstLineBold(bool firstLineBold);
	void setStripes(bool stripes);
	void setStripeColor(const QColor& color);

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant& value);
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
	void keyPressEvent(QKeyEvent* event);

private slots:
	void pictureDownloaded(const QByteArray& bytes);

private:
	struct Impl;
	QScopedPointer<Impl> d;
};

#endif // GRAPHICSITEM_H
