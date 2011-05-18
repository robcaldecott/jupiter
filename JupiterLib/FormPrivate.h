#ifndef FORMPRIVATE_H
#define FORMPRIVATE_H

#include "Form.h"
#include <QtGlobal>
#include <QList>
#include <QStringList>

class GraphicsItem;
class GraphicsPaperItem;
class GraphicsPageItem;
class GraphicsGridItem;
class GraphicsPrintDataItem;
class Watermark;

class QDataStream;
class QWidget;
class QString;
class QSizeF;
class QRectF;
class QRect;
class QPointF;
class QFont;
class QPrinter;
class QGraphicsItemGroup;
class QGraphicsLineItem;

class FormPrivate
{
public:
	friend QDataStream& operator<<(QDataStream&, const FormPrivate&);
	friend QDataStream& operator>>(QDataStream&, FormPrivate&);

	enum
	{
		GutterSize = 50,
		Rows = 66,
		Cols = 80,
		MagicNumber = 0x20042008,
		VersionNumber = 1,
	};

	enum Margin
	{
		Left,
		Top,
		Right,
		Bottom,
	};

	FormPrivate(Form* parent);
	~FormPrivate();

	void reset();
	void updatePrinterSettings();
	void updateMargins();
	void updatePage();
	void changePrinterSettings();
	void pageSetup(QWidget* parent);
	void setPrintData(const QString& data);
	QSizeF gridSize() const;
	QSizeF pageSize() const;
	QRectF pageRect() const;
	void showGrid(bool show);
	void showMargins(bool show);
	void showData(bool show);
	qreal lineHeight() const;
	bool pointInPage(const QPointF& point) const;
	QPointF snap(const QPointF& point) const;
	GraphicsItem* createItem(const QRectF& rect);
	QRectF adjustRectForPage(const QRectF& rect) const;
	void selectAll();
	void deleteSelected();
	QString pageData() const;
	QString data(const QRectF& itemRect) const;
	QRect toRect(const QRectF& itemRect) const;
	QRect toRect(const QRectF& itemRect, const QSizeF& gridSize) const;
	QRect toRect(const GraphicsItem* item) const;
	QFont stretchFont(const QFont& font) const;
	Form::LoadResult openFile(const QString& fileName);
	bool saveFile(const QString& fileName) const;
	void clearItems();
	QList<GraphicsItem*> items() const;
	QList<GraphicsItem*> selected() const;
	bool isEmpty() const;
	void setRows(qint32 rows);
	void setCols(qint32 cols);
	void setFont(const QFont& font);
	void setWatermark(const QString& watermark);
	void setBold(bool bold);
	void setItalic(bool italic);
	QList<QRect> itemRects() const;
	void createPdf(const QString& fileName);
	void paint(QPainter* painter, QPrinter* printer);
	bool isPrinting() const;
	void moveForwards();
	void moveBackwards();
	void alignLeft();
	void alignRight();
	void alignTop();
	void alignBottom();
	void print(QWidget* parent);
	void preview(QPrinter* printer);
	bool canPaste() const;
	QList<GraphicsItem*> paste();
	void cut();
	void copy() const;
	void size(Form::Size option);

	Form* const q_ptr;
	Q_DECLARE_PUBLIC(Form)

	QPrinter* printer;
	QStringList printData;
	GraphicsPaperItem* paper;
	QGraphicsItemGroup* margins;
	QGraphicsLineItem* margin[4];
	GraphicsPageItem* page;
	GraphicsGridItem* gridItem;
	GraphicsPrintDataItem* dataItem;
	qint32 rows;
	qint32 cols;
	int currentPage;
	int pageCount;
	QFont font;
	QString watermark;
	quint16 version;
	Watermark* watermarkItem;
	bool printing;
};

#endif // FORMPRIVATE_H
