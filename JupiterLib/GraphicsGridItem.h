#ifndef GRAPHICSGRIDITEM_H
#define GRAPHICSGRIDITEM_H

#include "GraphicsItemTypes.h"
#include <QGraphicsItem>

class Form;

class GraphicsGridItem : public QGraphicsItem
{
public:
	enum { Type = GraphicsItemTypes::Grid };

	GraphicsGridItem(QGraphicsItem* parent = 0);
	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	int type() const { return Type; }
};

#endif // GRAPHICSGRIDITEM_H
