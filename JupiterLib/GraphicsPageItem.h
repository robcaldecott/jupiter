#ifndef GRAPHICSPAGEITEM_H
#define GRAPHICSPAGEITEM_H

#include "GraphicsItemTypes.h"
#include <QGraphicsItem>

class QPrinter;

class GraphicsPageItem : public QGraphicsItem
{
public:
	enum { Type = GraphicsItemTypes::Page };

	GraphicsPageItem(QPrinter* printer, QGraphicsItem* parent = 0);
	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	int type() const { return Type; }

private:
	QPrinter* printer;
};

#endif // GRAPHICSPAGEITEM_H
