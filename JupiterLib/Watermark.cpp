#include "Watermark.h"
#include "Form.h"
#include <QFontMetricsF>
#include <QPainter>

namespace
{
	const QColor Color = QColor(0xE5, 0xE5, 0xE5);
}

Watermark::Watermark(QGraphicsItem *parent) :
	QGraphicsObject(parent),
	form(dynamic_cast<Form*>(parent->scene()))
{
	connect(form, SIGNAL(gridSizeChanged()), this, SLOT(formSizeChanged()));
	createFont();
}

QRectF Watermark::boundingRect() const
{
	return form->pageRect();
}

void Watermark::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	if (form->watermark() != "")
	{
		painter->setFont(font);
		painter->setPen(Color);
		painter->drawText(form->pageRect(), Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextSingleLine,
						  form->watermark());
	}
}

void Watermark::formSizeChanged()
{
	createFont();
	update();
}

void Watermark::createFont()
{
	setTransformOriginPoint(form->pageRect().width() / 2, form->pageRect().height() / 2);
	setRotation(-45);

	font.setFamily("Arial");
	font.setPixelSize(254); // 1 inch
	font.setBold(true);

	const QString text = form->watermark();
	const QSizeF pageSize = form->pageSize();

	QFontMetricsF metrics(font);
	while (metrics.height() > pageSize.height() || metrics.width(text) > pageSize.width())
	{
		font.setPixelSize(font.pixelSize() - 2);
		metrics = QFontMetricsF(font);
	}
}
