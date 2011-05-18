#ifndef GRAPHICSPRINTDATAITEM_H
#define GRAPHICSPRINTDATAITEM_H

#include "GraphicsItemTypes.h"
#include <QGraphicsItem>

class Form;

class GraphicsPrintDataItem : public QGraphicsItem
{
public:
	enum { Type = GraphicsItemTypes::Data };

	GraphicsPrintDataItem(QGraphicsItem* parent = 0);
	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	int type() const { return Type; }

private:
	const Form* form;
};

#endif // GRAPHICSPRINTDATAITEM_H
