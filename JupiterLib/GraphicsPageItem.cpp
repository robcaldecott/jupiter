#include "GraphicsPageItem.h"
#include "Printer.h"

GraphicsPageItem::GraphicsPageItem(QPrinter* printer, QGraphicsItem* parent) :
	QGraphicsItem(parent),
	printer(printer)
{
	setFlags(QGraphicsItem::ItemHasNoContents/* | QGraphicsItem::ItemClipsChildrenToShape*/);
}

QRectF GraphicsPageItem::boundingRect() const
{
	const QSizeF pageSize = Printer::pageSize(printer);
	return QRectF(0, 0, pageSize.width(), pageSize.height());
}

void GraphicsPageItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(painter);
	Q_UNUSED(option);
	Q_UNUSED(widget);
}
