#ifndef FORM_H
#define FORM_H

#include <QGraphicsScene>
#include <QScopedPointer>

class GraphicsItem;
class QSizeF;
class QRectF;
class QRect;
class QPointF;
class QFont;
class QString;
class QFile;
class QDataStream;
class QPrinter;
class QPainter;

class FormPrivate;
class Form : public QGraphicsScene
{
	Q_OBJECT

public:
	enum LoadResult
	{
		Ok,
		OpenError,
		MagicError,
		VersionError
	};

	enum Size
	{
		ShrinkWidth,
		ShrinkHeight,
		ShrinkBoth,
		GrowWidth,
		GrowHeight,
		GrowBoth,
		PageWidth,
		PageHeight,
		PageBoth,
	};

	explicit Form(QObject* parent);
	~Form();

	void reset();
	bool saveFile(const QString& fileName) const;
	LoadResult openFile(const QString& fileName);
	void clearItems();
	void setData(const QString& data);
	QSizeF gridSize() const;
	QSizeF pageSize() const;
	QRectF pageRect() const;
	qreal lineHeight() const;
	bool pointInPage(const QPointF& point) const;
	QPointF snap(const QPointF& point) const;
	GraphicsItem* createItem(const QRectF& rect);
	QRectF adjustRectForPage(const QRectF& rect) const;
	QString pageData() const;
	QString data(const QRectF& itemRect) const;
	QRect toRect(const QRectF& itemRect) const;
	QRect toRect(const QRectF& itemRect, const QSizeF& gridSize) const;
	QFont stretchFont(const QFont& font) const;
	QList<GraphicsItem*> items() const;
	QList<GraphicsItem*> selected() const;
	bool isEmpty() const;
	bool hasPrintData() const;
	QList<QRect> itemRects() const;
	void createPdf(const QString& fileName);
	bool isPrinting() const;
	QList<GraphicsItem*> paste();
	bool canPaste() const;

	QPrinter* printer() const;

	qint32 rows() const;
	qint32 cols() const;

	int currentPage() const;
	int pageCount() const;

	QFont font() const;
	bool bold() const;

	QString watermark() const;

signals:
	void changed();
	void gridSizeChanged();

public slots:
	void setCurrentPage(int page);
	void setRows(qint32 rows);
	void setCols(qint32 cols);
	void pageSetup(QWidget* parent = 0);
	void showGrid(bool show = true);
	void showMargins(bool show = true);
	void showData(bool show = true);
	void selectAll();
	void deleteSelected();
	void setFont(const QFont& font);
	void setWatermark(const QString& watermark);
	void setBold(bool bold);
	void setItalic(bool italic);
	void moveForwards();
	void moveBackwards();
	void alignLeft();
	void alignRight();
	void alignTop();
	void alignBottom();
	void print(QWidget* parent = 0);
	void preview(QPrinter* printer);
	void cut();
	void copy() const;
	void size(Size option);

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	QScopedPointer<FormPrivate> d_ptr;
	Q_DECLARE_PRIVATE(Form)
};

#endif // FORM_H
