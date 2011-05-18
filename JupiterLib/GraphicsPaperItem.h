#ifndef GRAPHICSPAPERITEM_H
#define GRAPHICSPAPERITEM_H

#include "GraphicsItemTypes.h"
#include <QGraphicsItem>

class QPrinter;

class GraphicsPaperItem : public QGraphicsItem
{
public:
	enum { Type = GraphicsItemTypes::Paper };

	GraphicsPaperItem(QPrinter* printer, QGraphicsItem* parent = 0);
	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	int type() const { return Type; }

private:
	QPrinter* printer;
};

#endif // GRAPHICSPAPERITEM_H
