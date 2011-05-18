#include "GraphicsPrintDataItem.h"
#include "Form.h"
#include "Printer.h"
#include "Font.h"
#include "Form.h"
#include <QPainter>
#include <QDebug>

GraphicsPrintDataItem::GraphicsPrintDataItem(QGraphicsItem* parent) :
	QGraphicsItem(parent)
{
	form = dynamic_cast<Form*>(scene());
	setFlag(QGraphicsItem::ItemClipsToShape);
}

QRectF GraphicsPrintDataItem::boundingRect() const
{
	return form->pageRect();
}

void GraphicsPrintDataItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	const QString data = form->pageData();
	if (data != "")
	{
		// We need the rects of all the other controls
		const QList<QRect> rects = form->itemRects();

		painter->save();
		painter->setFont(form->font());

		const qreal pageWidth = boundingRect().width();
		const qreal lineHeight = form->lineHeight();

		qreal yPos = 0;
		int row = 0;
		foreach (QString line, data.split('\n'))
		{
			for (int i = 0, count = line.count(); i < count; ++i)
			{
				foreach (const QRect& rect, rects)
				{
					if (rect.contains(i, row))
					{
					line[i] = ' ';
					break;
					}
				}
			}

			const QRectF rect(0, yPos, pageWidth, lineHeight);
			painter->drawText(rect, line);
			yPos += lineHeight;
			row++;
		}

		painter->restore();
	}
}
