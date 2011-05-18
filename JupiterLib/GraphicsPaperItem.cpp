#include "GraphicsPaperItem.h"
#include "Printer.h"
#include <QPrinter>
#include <QSizeF>
#include <QPainter>
#include <QApplication>
#include <QPalette>

namespace
{
	enum { ShadowSize = 20 };
}

GraphicsPaperItem::GraphicsPaperItem(QPrinter* printer, QGraphicsItem* parent) :
	QGraphicsItem(parent),
	printer(printer)
{
}

QRectF GraphicsPaperItem::boundingRect() const
{
	const QSizeF paperSize = Printer::paperSize(printer);
	return QRectF(0, 0, paperSize.width() + ShadowSize, paperSize.height() + ShadowSize);
}

void GraphicsPaperItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	const QSizeF paperSize = Printer::paperSize(printer);
	painter->fillRect(0, 0, paperSize.width(), paperSize.height(), QApplication::palette().base());
	painter->fillRect(paperSize.width(), ShadowSize, ShadowSize, paperSize.height(), QApplication::palette().shadow());
	painter->fillRect(ShadowSize, paperSize.height(), paperSize.width(), ShadowSize, QApplication::palette().shadow());
}
