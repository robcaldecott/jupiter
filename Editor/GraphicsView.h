#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPoint>

class Form;
class QRubberBand;
class QRectF;
class QPoint;

class GraphicsView : public QGraphicsView
{
	Q_OBJECT

public:

	enum Zoom
	{
		ActualSize,
		FitWidth,
		FitHeight,
	};

	GraphicsView(QGraphicsScene* scene, QWidget* parent = 0);
	void startRubberBanding();
	void stopRubberBanding();

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);

signals:
	void doneRubberBanding(const QRectF& rect);
	void mouseDoubleClick();

public slots:
	void setZoom(Zoom zoom);

private slots:
	void resized();

private:
	const Form* form() const;
	QPoint snap(const QPoint& point) const;

	QRubberBand* rubberBand;
	QPoint origin;
	Zoom zoom;
};

#endif // GRAPHICSVIEW_H
