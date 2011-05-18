#include "Form.h"
#include "FormPrivate.h"
#include "Printer.h"
#include "GraphicsPaperItem.h"
#include "GraphicsPageItem.h"
#include "GraphicsGridItem.h"
#include "GraphicsPrintDataItem.h"
#include "GraphicsItem.h"
#include "Font.h"
#include "Watermark.h"
#include <QPrinter>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QPen>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QApplication>
#include <QPalette>
#include <QRegExp>
#include <QStringList>
#include <QFile>
#include <QDataStream>
#include <QPainter>
#include <QClipboard>
#include <QMimeData>
#include <QKeyEvent>
#include <QDebug>

namespace
{
	const QString MimeType = "application/jupiter.reporter";

	class HideDesignItems
	{
	public:
		HideDesignItems(FormPrivate* form) :
			form(form),
			gridVisible(form->gridItem->isVisible()),
			marginsVisible(form->margins->isVisible())
		{
			form->margins->setVisible(false);
			form->gridItem->setVisible(false);
		}

		~HideDesignItems()
		{
			form->margins->setVisible(gridVisible);
			form->gridItem->setVisible(marginsVisible);
		}

	private:
		FormPrivate* form;
		bool gridVisible;
		bool marginsVisible;
	};
}

FormPrivate::FormPrivate(Form* parent) :
	q_ptr(parent),
	printer(0),
	rows(Rows),
	cols(Cols),
	currentPage(1),
	pageCount(1),
	font("Courier New"),
	version(VersionNumber),
	watermarkItem(0),
	printing(false)
{
}

FormPrivate::~FormPrivate()
{
	delete printer;
}

void FormPrivate::reset()
{
	Q_Q(Form);

	q->clear();

	rows = Rows;
	cols = Cols;
	currentPage = 1;
	pageCount = 1;
	font = QFont("Courier New");
	printData.clear();
	watermark.clear();

	delete printer;
	printer = new QPrinter;
	printer->setFullPage(true);
	printer->setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
	printer->setDocName("");

	const QSizeF paperSize = Printer::paperSize(printer);
	q->setSceneRect(0, 0, paperSize.width() + (2 * GutterSize), paperSize.height() + (2 * GutterSize));
	q->setBackgroundBrush(QApplication::palette().color(QPalette::Dark));

	paper = new GraphicsPaperItem(printer);
	paper->setPos(GutterSize, GutterSize);
	q->addItem(paper);

	margins = new QGraphicsItemGroup(paper, q);

	QPen marginPen;
	marginPen.setWidth(0);
	marginPen.setStyle(Qt::DotLine);

	for (int i = 0; i < 4; ++i)
	{
		margin[i] = new QGraphicsLineItem(paper, q);
		margin[i]->setPen(marginPen);
		margins->addToGroup(margin[i]);
	}

	page = new GraphicsPageItem(printer, paper);
	watermarkItem = new Watermark(page);
	gridItem = new GraphicsGridItem(page);
	dataItem = new GraphicsPrintDataItem(page);

	updateMargins();
	updatePage();
}

void FormPrivate::updatePrinterSettings()
{
	Q_Q(Form);

	const QSizeF paperSize = Printer::paperSize(printer);
	q->setSceneRect(0, 0, paperSize.width() + (2 * GutterSize), paperSize.height() + (2 * GutterSize));
	updateMargins();
	updatePage();
}

void FormPrivate::updateMargins()
{
	const QSizeF paperSize = Printer::paperSize(printer);
	const qreal width = paperSize.width();
	const qreal height = paperSize.height();
	const QMargins margins = Printer::margins(printer);

	margin[Left]->setLine(QLineF(margins.left(), 0, margins.left(), height));
	margin[Top]->setLine(QLineF(0, margins.top(), width, margins.top()));
	margin[Right]->setLine(QLineF(width - margins.right(), 0, width - margins.right(), height));
	margin[Bottom]->setLine(QLineF(0, height - margins.bottom(), width, height - margins.bottom()));
}

void FormPrivate::updatePage()
{
	const QMargins margins = Printer::margins(printer);
	page->setPos(margins.left(), margins.top());
}

void FormPrivate::changePrinterSettings()
{
	Q_Q(Form);

	updatePrinterSettings();

	font = Font::stretch(font, rows, cols, pageSize());
	q->update();

	emit q->changed();
	emit q->gridSizeChanged();
}

void FormPrivate::pageSetup(QWidget* parent)
{
	QPageSetupDialog pageSetupDialog(printer, parent);
	if (pageSetupDialog.exec())
		changePrinterSettings();
}

void FormPrivate::setPrintData(const QString& data)
{
	Q_Q(Form);
	// Reset page count
	currentPage = 1;
	pageCount = 1;
	// Assume default rows/cols
	rows = Rows;
	cols = Cols;
	// Split into pages
	printData = data.split('\f');
	// Trim trailing FFs
	while (!printData.isEmpty() && printData.last().isEmpty())
		printData.removeLast();
	// Normalize line endings
	for (int i = 0; i < printData.count(); ++i)
	{
		printData[i].replace(QRegExp("(\\r)|(\\r\\n)"), "\n");
		// Split into separate lines and count the rows
		const QStringList lines = printData[i].split('\n');
		rows = qMax(lines.count(), rows);
		// Count the length of each line
		foreach (const QString& line, lines)
			cols = qMax(cols, line.count());
	}
	pageCount = printData.count();
	font = Font::stretch(font, rows, cols, pageSize());

	q->update();
	emit q->changed();
}

QSizeF FormPrivate::gridSize() const
{
	const QSizeF printerPageSize = pageSize();
	const qreal cx = printerPageSize.width() / cols;
	const qreal cy = printerPageSize.height() / rows;
	return QSizeF(cx, cy);
}

QSizeF FormPrivate::pageSize() const
{
	return Printer::pageSize(printer);
}

QRectF FormPrivate::pageRect() const
{
	const QSizeF printerPageSize = pageSize();
	return QRectF(0, 0, printerPageSize.width(), printerPageSize.height());
}

void FormPrivate::showGrid(bool show)
{
	gridItem->setVisible(show);
}

void FormPrivate::showMargins(bool show)
{
	margins->setVisible(show);
}

void FormPrivate::showData(bool show)
{
	dataItem->setVisible(show);
}

qreal FormPrivate::lineHeight() const
{
	return pageSize().height() / rows;
}

bool FormPrivate::pointInPage(const QPointF& point) const
{
	const QSizeF printerPageSize = pageSize();
	const QPointF pagePoint = page->mapFromScene(point);
	return  pagePoint.x() >= 0 &&
			pagePoint.y() >= 0 &&
			pagePoint.x() <= printerPageSize.width() &&
			pagePoint.y() <= printerPageSize.height();
}

QPointF FormPrivate::snap(const QPointF& point) const
{
	const QSizeF& grid = gridSize();
	QPointF pagePoint = page->mapFromScene(point);
	pagePoint.rx() = qRound(pagePoint.x() / grid.width()) * grid.width();
	pagePoint.ry() = qRound(pagePoint.y() / grid.height()) * grid.height();
	return page->mapToScene(pagePoint);
}

GraphicsItem* FormPrivate::createItem(const QRectF& rect)
{
	Q_Q(Form);

	q->clearSelection();

	GraphicsItem* item = new GraphicsItem(page->mapRectFromScene(rect), page);
	item->setSelected(true);
	item->setFocus();
	item->setFont(font);

	emit q->changed();

	return item;
}

QRectF FormPrivate::adjustRectForPage(const QRectF& rect) const
{
	const QRectF printerRect = pageRect();

	QRectF newRect(page->mapFromScene(rect).boundingRect());

	newRect.setLeft(qMax(newRect.left(), 0.0));
	newRect.setTop(qMax(newRect.top(), 0.0));
	newRect.setRight(qMin(newRect.right(), printerRect.right()));
	newRect.setBottom(qMin(newRect.bottom(), printerRect.bottom()));

	return page->mapToScene(newRect).boundingRect();
}

void FormPrivate::selectAll()
{
	foreach (QGraphicsItem* item, items())
	item->setSelected(true);
}

void FormPrivate::deleteSelected()
{
	foreach (QGraphicsItem* item, items())
		if (item->isSelected())
			delete item;

	Q_Q(Form);
	emit q->changed();
}

QString FormPrivate::pageData() const
{
	return currentPage <= printData.count() ? printData[currentPage - 1] : "";
}

QString FormPrivate::data(const QRectF& itemRect) const
{
	const QString currentPageData = pageData();
	if (currentPageData.isEmpty())
		return "";

	const QRect r = toRect(itemRect);
	const QStringList lines = currentPageData.split('\n');

	QString str;
	for (int i = r.top(), count = qMin(lines.count(), r.top() + r.height()); i < count; ++i)
		str += lines[i].mid(r.left(), r.width()) + '\n';

	return str;
}

QRect FormPrivate::toRect(const QRectF& itemRect) const
{
	return toRect(itemRect, gridSize());
}

QRect FormPrivate::toRect(const QRectF& itemRect, const QSizeF& gridSize) const
{
	return QRect(qRound(itemRect.left() / gridSize.width()),
				 qRound(itemRect.top() / gridSize.height()),
				 qRound(itemRect.width() / gridSize.width()),
				 qRound(itemRect.height() / gridSize.height()));
}

QRect FormPrivate::toRect(const GraphicsItem* item) const
{
	Q_ASSERT(item);
	return toRect(item->rect());
}

QFont FormPrivate::stretchFont(const QFont& font) const
{
	return Font::stretch(font, rows, cols, pageSize());
}

Form::LoadResult FormPrivate::openFile(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
		return Form::OpenError;

	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_6);

	quint32 magicNumber;
	in >> magicNumber;
	if (magicNumber != MagicNumber)
		return Form::MagicError;

	in >> version;
	if (version > VersionNumber)
		return Form::VersionError;

	clearItems();

	in >> *this;

	quint32 count = 0;
	in >> count;

	for (quint32 i = 0; i < count; ++i)
	{
		GraphicsItem* item = new GraphicsItem(QRectF(), page);
		in >> *item;
	}

	updatePrinterSettings();
	watermarkItem->createFont();

	Q_Q(Form);
	q->update();

	return Form::Ok;
}

bool FormPrivate::saveFile(const QString& fileName) const
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
		return false;

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_6);
	out << (quint32)MagicNumber;
	out << (quint16)VersionNumber;

	out << *this;

	const QList<GraphicsItem*> list = items();
	out << (quint32)list.count();
	foreach (const GraphicsItem* item, list)
		out << *item;

	return true;
}

void FormPrivate::clearItems()
{
	Q_Q(Form);
	qDeleteAll(items());
	printData.clear();
	q->update();
}

QList<GraphicsItem*> FormPrivate::items() const
{
	QList<GraphicsItem*> list;

	foreach (QGraphicsItem* item, page->childItems())
		if (item->type() == GraphicsItemTypes::Item)
			list << dynamic_cast<GraphicsItem*>(item);

	return list;
}

QList<GraphicsItem*> FormPrivate::selected() const
{
	QList<GraphicsItem*> list;

	foreach (QGraphicsItem* item, page->childItems())
		if (item->isSelected())
			list << dynamic_cast<GraphicsItem*>(item);

	return list;
}

bool FormPrivate::isEmpty() const
{
	return printData.isEmpty() && items().count() == 0;
}

void FormPrivate::setRows(qint32 rows)
{
	Q_Q(Form);
	if (rows != this->rows)
	{
		this->rows = rows;
		font = Font::stretch(font, rows, cols, pageSize());
		q->update();

		emit q->changed();
		emit q->gridSizeChanged();
	}
}

void FormPrivate::setCols(qint32 cols)
{
	Q_Q(Form);
	if (cols != this->cols)
	{
		this->cols = cols;
		font = Font::stretch(font, rows, cols, pageSize());
		q->update();

		emit q->changed();
		emit q->gridSizeChanged();
	}
}

void FormPrivate::setFont(const QFont& font)
{
	Q_Q(Form);
	if (font.family() != this->font.family() && QFontInfo(font).fixedPitch() &&
			q->selectedItems().count() == 0)
	{
		this->font = stretchFont(font);
		q->update();
		emit q->changed();
	}
}

void FormPrivate::setWatermark(const QString& watermark)
{
	Q_Q(Form);
	if (watermark != this->watermark)
	{
		this->watermark = watermark;
		watermarkItem->createFont();
		q->update();
		emit q->changed();
	}
}

void FormPrivate::setBold(bool bold)
{
	Q_Q(Form);
	QFont font = this->font;
	if (font.bold() != bold && q->selectedItems().count() == 0)
	{
		this->font.setBold(bold);
		this->font = stretchFont(this->font);
		q->update();
		emit q->changed();
	}
}

void FormPrivate::setItalic(bool italic)
{
	Q_Q(Form);
	QFont font = this->font;
	if (font.italic() != italic && q->selectedItems().count() == 0)
	{
		this->font.setItalic(italic);
		this->font = stretchFont(this->font);
		q->update();
		emit q->changed();
	}
}

QList<QRect> FormPrivate::itemRects() const
{
	QList<QRect> rects;
	foreach (const GraphicsItem* item, items())
		rects << toRect(item->rect());
	return rects;
}

void FormPrivate::createPdf(const QString& fileName)
{
	HideDesignItems ui(this);

	QPrinter pdfPrinter;
	Printer::applySettings(&pdfPrinter, printer);
	pdfPrinter.setOutputFileName(fileName);
	pdfPrinter.setOutputFormat(QPrinter::PdfFormat);

	{
		QPainter painter(&pdfPrinter);
		paint(&painter, &pdfPrinter);
	}
}

void FormPrivate::paint(QPainter* painter, QPrinter* printer)
{
	Q_Q(Form);

	const QSizeF pageSize = Printer::pageSize(printer);
	const QMargins margins = Printer::margins(printer);
	const QRectF source(GutterSize + margins.left(), GutterSize + margins.top(),
						pageSize.width(), pageSize.height());

	const qreal xPixels = printer->width() / (printer->widthMM() * 10.0);
	const qreal yPixels = printer->height() / (printer->heightMM() * 10.0);

	const QPointF topLeft = Printer::topLeft(printer);
	const qreal left = margins.left() - topLeft.x();
	const qreal top = margins.top() - topLeft.y();

	qreal xPrinter = xPixels * left;
	qreal yPrinter = yPixels * top;

	const QRectF target(xPrinter, yPrinter, xPixels * pageSize.width(), yPixels * pageSize.height());

	const int savedPage = currentPage;
	printing = true;

	for (int page = 1, count = pageCount; page <= count; ++page)
	{
		currentPage = page;
		q->render(painter, target, source, Qt::IgnoreAspectRatio);

		if (page < count)
			printer->newPage();
	}

	printing = false;
	currentPage = savedPage;
}

void FormPrivate::moveForwards()
{
	Q_Q(Form);
	Q_ASSERT(q->selectedItems().count() > 0);

	QGraphicsItem* selectedItem = q->selectedItems().first();
	QList<QGraphicsItem*> collidingItems = selectedItem->collidingItems();

	qreal z = 0;
	foreach (QGraphicsItem* item, collidingItems)
	{
		if (item->zValue() >= z)
			z = item->zValue() + 0.1;
	}
	selectedItem->setZValue(z);
}

void FormPrivate::moveBackwards()
{
	Q_Q(Form);
	Q_ASSERT(q->selectedItems().count() > 0);

	QGraphicsItem* selectedItem = q->selectedItems().first();
	QList<QGraphicsItem*> collidingItems = selectedItem->collidingItems();

	qreal z = 0;
	foreach (QGraphicsItem* item, collidingItems)
	{
		if (item->zValue() <= z)
			z = item->zValue() - 0.1;
	}
	selectedItem->setZValue(z);
}

void FormPrivate::alignLeft()
{
	Q_Q(Form);
	Q_ASSERT(q->selectedItems().count() > 0);

	qreal left = q->selectedItems().first()->x();
	foreach (QGraphicsItem* item, q->selectedItems())
	{
		if (item->x() < left)
			left = item->x();
	}

	foreach (QGraphicsItem* item, q->selectedItems())
		item->setX(left);
}

void FormPrivate::alignRight()
{
	const GraphicsItem* first = selected().first();
	qreal right = first->rect().right();

	foreach (const GraphicsItem* item, selected())
	{
		if (item->rect().right() > right)
			right = item->rect().right();
	}

	foreach (GraphicsItem* item, selected())
	{
		QRectF rect = item->rect();
		rect.moveRight(right);
		item->setX(rect.left());
	}
}

void FormPrivate::alignTop()
{
	Q_Q(Form);
	Q_ASSERT(q->selectedItems().count() > 0);

	qreal top = q->selectedItems().first()->y();
	foreach (QGraphicsItem* item, q->selectedItems())
	{
		if (item->y() < top)
			top = item->y();
	}

	foreach (QGraphicsItem* item, q->selectedItems())
		item->setY(top);
}

void FormPrivate::alignBottom()
{
	const GraphicsItem* first = selected().first();
	qreal bottom = first->rect().bottom();

	foreach (const GraphicsItem* item, selected())
	{
		if (item->rect().bottom() > bottom)
			bottom = item->rect().bottom();
	}

	foreach (GraphicsItem* item, selected())
	{
		QRectF rect = item->rect();
		rect.moveBottom(bottom);
		item->setY(rect.top());
	}
}

void FormPrivate::print(QWidget* parent)
{
	QPrintDialog dlg(printer, parent);
	if (dlg.exec() == QDialog::Accepted)
	{
		changePrinterSettings();

		HideDesignItems ui(this);

		{
			QPainter painter(printer);
			paint(&painter, printer);
		}
	}
}

void FormPrivate::preview(QPrinter* printer)
{
	changePrinterSettings();
	HideDesignItems ui(this);
	QPainter painter(printer);
	paint(&painter, printer);
}

bool FormPrivate::canPaste() const
{
	const QMimeData *mimeData = QApplication::clipboard()->mimeData();
	return (mimeData && (mimeData->hasFormat(MimeType) ||
						 mimeData->hasText() ||
						 mimeData->hasImage()));
}

QList<GraphicsItem*> FormPrivate::paste()
{
	Q_Q(Form);

	QList<GraphicsItem*> items;

	QClipboard* clipboard = QApplication::clipboard();
	Q_ASSERT(clipboard);

	const QMimeData* mimeData = clipboard->mimeData();
	if (mimeData)
	{
		foreach (QGraphicsItem* item, this->items())
			item->setSelected(false);

		const int Width = 10;
		const int Height = 3;

		const QSizeF grid = gridSize();
		const QRectF rect = QRectF(0, 0, Width * grid.width(), Height * grid.height());

		if (mimeData->hasText())
		{
			GraphicsItem* item = createItem(rect);
			item->setText(mimeData->text());
			items << item;
		}

		if (mimeData->hasImage())
		{
			GraphicsItem* item = createItem(rect);
			item->setPicture(qvariant_cast<QPixmap>(mimeData->imageData()));
			items << item;
		}

		if (mimeData->hasFormat(MimeType))
		{
			QByteArray data = mimeData->data(MimeType);
			QDataStream stream(&data, QIODevice::ReadOnly);

			quint32 count;
			stream >> count;

			for (quint32 i = 0; i < count; ++i)
			{
				GraphicsItem* item = new GraphicsItem(QRectF(), page);
				stream >> *item;

				item->moveBy(grid.width(), grid.height());
				item->setSelected(true);
				item->setFocus();

				items << item;
			}

			emit q->changed();
		}
	}

	return items;
}

void FormPrivate::cut()
{
	Q_Q(Form);

	copy();
	foreach (GraphicsItem* item, selected())
		q->removeItem(item);

	emit q->changed();
}

void FormPrivate::copy() const
{
	QByteArray buffer;
	QDataStream stream(&buffer, QIODevice::WriteOnly);

	const QList<GraphicsItem*> items = selected();
	stream << (quint32)items.count();
	foreach (const GraphicsItem* item, items)
		stream << *item;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData(MimeType, buffer);
	QApplication::clipboard()->setMimeData(mimeData);
}

void FormPrivate::size(Form::Size option)
{
	const QSizeF grid = gridSize();
	const QSizeF pageSize = this->pageSize();

	qreal width = 0;
	qreal height = 0;

	switch (option)
	{
	case Form::ShrinkWidth:
		width = selected().first()->rect().width();
		foreach (GraphicsItem* item, selected())
		{
			if (item->rect().width() < width)
				width = item->rect().width();
		}
		break;
	case Form::ShrinkHeight:
		height = selected().first()->rect().height();
		foreach (GraphicsItem* item, selected())
		{
			if (item->rect().height() < height)
				height = item->rect().height();
		}
		break;
	case Form::ShrinkBoth:
		size(Form::ShrinkWidth);
		size(Form::ShrinkHeight);
		break;
	case Form::GrowWidth:
		width = selected().first()->rect().width();
		foreach (GraphicsItem* item, selected())
		{
			if (item->rect().width() > width)
				width = item->rect().width();
		}
		break;
	case Form::GrowHeight:
		height = selected().first()->rect().height();
		foreach (GraphicsItem* item, selected())
		{
			if (item->rect().height() > height)
				height = item->rect().height();
		}
		break;
	case Form::GrowBoth:
		size(Form::GrowWidth);
		size(Form::GrowHeight);
		break;
	case Form::PageWidth:
		width = grid.width() * cols;
		break;
	case Form::PageHeight:
		height = grid.height() * rows;
		break;
	case Form::PageBoth:
		size(Form::PageWidth);
		size(Form::PageHeight);
		break;
	default:
		break;
	}

	if (width > 0 || height > 0)
	{
		foreach (GraphicsItem* item, selected())
		{
			QRectF rect = item->rect();

			if (width > 0)
			{
				qreal itemWidth = width;
				if (rect.left() + itemWidth > pageSize.width())
					itemWidth -= rect.left();
				rect.setWidth(itemWidth);
			}
			if (height > 0)
			{
				qreal itemHeight = height;
				if (rect.top() + itemHeight > pageSize.height())
					itemHeight -= rect.top();
				rect.setHeight(itemHeight);
			}

			item->setRect(rect);
		}
	}
}

QDataStream& operator<<(QDataStream& stream, const FormPrivate& form)
{
	stream << form.printer->paperSize(QPrinter::Millimeter);
	stream << (qint32)form.printer->orientation();
	stream << Printer::margins(form.printer, QPrinter::Millimeter);
	stream << form.printData;
	stream << form.rows;
	stream << form.cols;
	stream << form.font;
	stream << form.watermark;
	return stream;
}

QDataStream& operator>>(QDataStream& stream, FormPrivate& form)
{
	QSizeF paperSize;
	stream >> paperSize;
	form.printer->setPaperSize(paperSize, QPrinter::Millimeter);

	qint32 orientation;
	stream >> orientation;
	form.printer->setOrientation((QPrinter::Orientation)orientation);

	MarginsF margins;
	stream >> margins;
	form.printer->setPageMargins(margins.left(), margins.top(), margins.right(), margins.bottom(), QPrinter::Millimeter);

	stream >> form.printData;
	form.pageCount = qMax(1, form.printData.count());

	stream >> form.rows;
	stream >> form.cols;
	stream >> form.font;

	if (form.version > 0)
		stream >> form.watermark;

	return stream;
}

Form::Form(QObject* parent) :
	QGraphicsScene(parent),
	d_ptr(new FormPrivate(this))
{
	Q_D(Form);
	d->reset();
}

Form::~Form()
{
}

void Form::reset()
{
	Q_D(Form);
	d->reset();
}

bool Form::saveFile(const QString& fileName) const
{
	Q_D(const Form);
	return d->saveFile(fileName);
}

Form::LoadResult Form::openFile(const QString& fileName)
{
	Q_D(Form);
	return d->openFile(fileName);
}

void Form::clearItems()
{
	Q_D(Form);
	d->clearItems();
}

void Form::pageSetup(QWidget* parent)
{
	Q_D(Form);
	d->pageSetup(parent);
}

void Form::setData(const QString& data)
{
	Q_D(Form);
	d->setPrintData(data);
}

QSizeF Form::gridSize() const
{
	Q_D(const Form);
	return d->gridSize();
}

QSizeF Form::pageSize() const
{
	Q_D(const Form);
	return d->pageSize();
}

QRectF Form::pageRect() const
{
	Q_D(const Form);
	return d->pageRect();
}

void Form::showGrid(bool show)
{
	Q_D(Form);
	d->showGrid(show);
}

void Form::showMargins(bool show)
{
	Q_D(Form);
	d->showMargins(show);
}

void Form::showData(bool show)
{
	Q_D(Form);
	d->showData(show);
}

qreal Form::lineHeight() const
{
	Q_D(const Form);
	return d->lineHeight();
}

bool Form::pointInPage(const QPointF& point) const
{
	Q_D(const Form);
	return d->pointInPage(point);
}

QPointF Form::snap(const QPointF& point) const
{
	Q_D(const Form);
	return d->snap(point);
}

GraphicsItem* Form::createItem(const QRectF& rect)
{
	Q_D(Form);
	return d->createItem(rect);
}

QRectF Form::adjustRectForPage(const QRectF& rect) const
{
	Q_D(const Form);
	return d->adjustRectForPage(rect);
}

void Form::selectAll()
{
	Q_D(Form);
	d->selectAll();
}

void Form::deleteSelected()
{
	Q_D(Form);
	d->deleteSelected();
}

QList<GraphicsItem*> Form::items() const
{
	Q_D(const Form);
	return d->items();
}

QList<GraphicsItem*> Form::selected() const
{
	Q_D(const Form);
	return d->selected();
}

QPrinter* Form::printer() const
{
	Q_D(const Form);
	return d->printer;
}

qint32 Form::rows() const
{
	Q_D(const Form);
	return d->rows;
}

void Form::setRows(qint32 rows)
{
	Q_D(Form);
	d->setRows(rows);
}

qint32 Form::cols() const
{
	Q_D(const Form);
	return d->cols;
}

void Form::setCols(qint32 cols)
{
	Q_D(Form);
	d->setCols(cols);
}

int Form::currentPage() const
{
	Q_D(const Form);
	return d->currentPage;
}

void Form::setCurrentPage(int page)
{
	Q_D(Form);
	d->currentPage = page;
	update();
}

int Form::pageCount() const
{
	Q_D(const Form);
	return d->pageCount;
}

QFont Form::font() const
{
	Q_D(const Form);
	return d->font;
}

void Form::setFont(const QFont& font)
{
	Q_D(Form);
	d->setFont(font);
}

QString Form::pageData() const
{
	Q_D(const Form);
	return d->pageData();
}

QString Form::data(const QRectF& itemRect) const
{
	Q_D(const Form);
	return d->data(itemRect);
}

QRect Form::toRect(const QRectF& itemRect) const
{
	Q_D(const Form);
	return d->toRect(itemRect);
}

QRect Form::toRect(const QRectF& itemRect, const QSizeF& gridSize) const
{
	Q_D(const Form);
	return d->toRect(itemRect, gridSize);
}

QFont Form::stretchFont(const QFont& font) const
{
	Q_D(const Form);
	return d->stretchFont(font);
}

QString Form::watermark() const
{
	Q_D(const Form);
	return d->watermark;
}

void Form::setWatermark(const QString& watermark)
{
	Q_D(Form);
	d->setWatermark(watermark);
}

void Form::setBold(bool bold)
{
	Q_D(Form);
	d->setBold(bold);
}

void Form::setItalic(bool italic)
{
	Q_D(Form);
	d->setItalic(italic);
}

bool Form::isEmpty() const
{
	Q_D(const Form);
	return d->isEmpty();
}

bool Form::bold() const
{
	Q_D(const Form);
	return d->font.bold();
}

bool Form::hasPrintData() const
{
	Q_D(const Form);
	return !d->printData.isEmpty();
}

QList<QRect> Form::itemRects() const
{
	Q_D(const Form);
	return d->itemRects();
}

void Form::createPdf(const QString& fileName)
{
	Q_D(Form);
	d->createPdf(fileName);
}

bool Form::isPrinting() const
{
	Q_D(const Form);
	return d->printing;
}

void Form::moveForwards()
{
	Q_D(Form);
	d->moveForwards();
}

void Form::moveBackwards()
{
	Q_D(Form);
	d->moveBackwards();
}

void Form::alignLeft()
{
	Q_D(Form);
	d->alignLeft();
}

void Form::alignRight()
{
	Q_D(Form);
	d->alignRight();
}

void Form::alignTop()
{
	Q_D(Form);
	d->alignTop();
}

void Form::alignBottom()
{
	Q_D(Form);
	d->alignBottom();
}

void Form::print(QWidget* parent)
{
	Q_D(Form);
	d->print(parent);
}

void Form::preview(QPrinter* printer)
{
	Q_D(Form);
	d->preview(printer);
}

QList<GraphicsItem*> Form::paste()
{
	Q_D(Form);
	return d->paste();
}

void Form::cut()
{
	Q_D(Form);
	d->cut();
}

void Form::copy() const
{
	Q_D(const Form);
	d->copy();
}

bool Form::canPaste() const
{
	Q_D(const Form);
	return d->canPaste();
}

void Form::size(Size option)
{
	Q_D(Form);
	d->size(option);
}

void Form::keyPressEvent(QKeyEvent *event)
{
	Q_D(const Form);

	switch (event->key())
	{
	// Send cursor key events to all selected items
	case Qt::Key_Left:
	case Qt::Key_Right:
	case Qt::Key_Up:
	case Qt::Key_Down:
		foreach (GraphicsItem* item, d->selected())
			sendEvent(item, event);
		break;
	default:
		QGraphicsScene::keyPressEvent(event);
		break;
	}
}
