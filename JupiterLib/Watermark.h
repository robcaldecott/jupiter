#ifndef WATERMARK_H
#define WATERMARK_H

#include "GraphicsItemTypes.h"
#include <QGraphicsObject>
#include <QFont>

class Form;

class Watermark : public QGraphicsObject
{
	Q_OBJECT

public:
	enum { Type = GraphicsItemTypes::Watermark };

	explicit Watermark(QGraphicsItem *parent = 0);
	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	int type() const { return Type; }
	void createFont();

public slots:
	void formSizeChanged();

private:
	const Form* const form;
	QFont font;
};

#endif // WATERMARK_H
