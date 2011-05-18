#include "GraphicsGridItem.h"
#include "Printer.h"
#include "Form.h"
#include <QPainter>
#include <QPointF>

GraphicsGridItem::GraphicsGridItem(QGraphicsItem* parent) :
	QGraphicsItem(parent)
{
}

QRectF GraphicsGridItem::boundingRect() const
{
	return dynamic_cast<Form*>(scene())->pageRect();
}

void GraphicsGridItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	const Form* form = dynamic_cast<Form*>(scene());

	if (form->isPrinting())
	return;

	const QSizeF gridSize = form->gridSize();
	const QSizeF pageSize = form->pageSize();

	const qreal cx = gridSize.width();
	const qreal cy = gridSize.height();

	for (qreal x = cx; x < pageSize.width() - (cx / 2); x += cx)
	{
		for (qreal y = cy; y < pageSize.height() - (cy / 2); y += cy)
		{
			painter->drawPoint(QPointF(x, y));
		}
	}
}
