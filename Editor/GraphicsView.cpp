#include "GraphicsView.h"
#include "Form.h"
#include <QRubberBand>
#include <QMouseEvent>
#include <QRectF>
#include <QTimer>

GraphicsView::GraphicsView(QGraphicsScene* scene, QWidget* parent) :
	QGraphicsView(scene, parent),
	rubberBand(0),
	zoom(ActualSize)
{
	setDragMode(QGraphicsView::RubberBandDrag);
	centerOn(0, 0);
}

void GraphicsView::startRubberBanding()
{
	setDragMode(QGraphicsView::NoDrag);
	setCursor(Qt::CrossCursor);
}

void GraphicsView::stopRubberBanding()
{
	if (rubberBand)
	{
		rubberBand->hide();
		delete rubberBand;
		rubberBand = 0;
	}
	setDragMode(QGraphicsView::RubberBandDrag);
	setCursor(Qt::ArrowCursor);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
	if (dragMode() == QGraphicsView::NoDrag && form()->pointInPage(mapToScene(event->pos())))
	{
		origin = snap(event->pos());
		rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
		rubberBand->setGeometry(QRect(origin, QSize()));
		rubberBand->show();
	}
	else
	{
		QGraphicsView::mousePressEvent(event);
	}
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	if (rubberBand)
	{
		// Make sure the rect stays within the page
		const QRect rubberBandRect(QRect(origin, snap(event->pos())).normalized());
		const QRectF sceneRect(form()->adjustRectForPage(mapToScene(rubberBandRect).boundingRect()));
		rubberBand->setGeometry(mapFromScene(sceneRect).boundingRect());
	}
	else
	{
		QGraphicsView::mouseMoveEvent(event);
	}
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	if (rubberBand)
	{
		const QRect rect = rubberBand->geometry();
		const QPointF topLeft = mapToScene(rect.topLeft());
		const QPointF bottomRight = mapToScene(rect.bottomRight());
		emit doneRubberBanding(QRectF(topLeft, bottomRight));
		stopRubberBanding();
	}
	else
	{
		QGraphicsView::mouseReleaseEvent(event);
	}
}

const Form* GraphicsView::form() const
{
	return dynamic_cast<Form*>(scene());
}

QPoint GraphicsView::snap(const QPoint& point) const
{
	QPointF scenePoint = mapToScene(point);
	scenePoint = form()->snap(scenePoint);
	return mapFromScene(scenePoint);
}

void GraphicsView::resizeEvent(QResizeEvent* event)
{
	QGraphicsView::resizeEvent(event);
	resized();
}

void GraphicsView::resized()
{
	QTransform t;

	QRectF rectScene = scene()->sceneRect();
	QRectF rectView = viewport()->rect();

	switch (zoom)
	{
	case ActualSize:
		t.scale(physicalDpiX() / 254.0, physicalDpiY() / 254.0);
		break;
	case FitWidth:
		t.scale(rectView.width() / rectScene.width(), rectView.width() / rectScene.width());
		break;
	case FitHeight:
		t.scale(rectView.height() / rectScene.height(), rectView.height() / rectScene.height());
		break;
	}

	setTransform(t);
	centerOn(0, 0);
}

void GraphicsView::setZoom(Zoom zoom)
{
	this->zoom = zoom;
	resized();
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
	Q_UNUSED(event);

	if (form()->selected().count() > 0)
		emit mouseDoubleClick();
}
