#include "GraphicsItem.h"
#include "Form.h"
#include "Font.h"
#include "DownloadFile.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
#include <QStyleOptionGraphicsItem>
#include <QFont>
#include <QKeyEvent>
#include <QUrl>
#include <QPixmap>
#include <QFile>
#include <QFontMetricsF>

namespace
{
	const QColor boxColor = QColor(0xE5, 0xE5, 0xE5);
}

struct GraphicsItem::Impl
{
	enum { VersionNumber = 8 };

	const GraphicsItem* q;
	const Form* const form;
	QSizeF size;
	bool sizing;
	QFont font;
	QColor textColor;
	QColor backColor;
	QColor borderColor;
	qint32 borderWidth;
	quint32 horzAlign;
	quint32 vertAlign;
	quint16 version;
	QString text;
	QUrl pictureUrl;
	QPixmap pixmap;
	quint32 pictureMode;
	quint32 fontHeight;
	QSizeF gridSize;
	qreal padding;
	bool autoLines;
	bool firstLineBold;
	bool stripes;
	QColor stripeColor;

	Impl(const QRectF& rect, GraphicsItem* parent) :
		q(parent),
		form(dynamic_cast<Form*>(parent->scene())),
		size(rect.size()),
		sizing(false),
		textColor(Qt::black),
		backColor(Qt::transparent),
		borderColor(Qt::black),
		borderWidth(0),
		horzAlign(0),
		vertAlign(0),
		version(VersionNumber),
		pictureMode(KeepAspectRatio),
		fontHeight(Normal),
		gridSize(form->gridSize()),
		padding(0),
		autoLines(true),
		firstLineBold(false),
		stripes(false),
		stripeColor(boxColor)
	{
	}

	bool sizeHitTest(const QPointF& pos) const
	{
		const QSizeF gridSize = form->gridSize() / 2;
		const QRectF rect(0, 0, size.width(), size.height());
		const QPointF bottomRight = rect.bottomRight();
		const QPointF topLeft(bottomRight.x() - gridSize.width(), bottomRight.y() - gridSize.height());
		return QRectF(topLeft, bottomRight).contains(pos);
	}

	QRectF rect() const
	{
		return QRectF(q->pos(), size);
	}

	QRectF textRect() const
	{
		qreal cx = xPadding();
		qreal cy = yPadding();
		QRectF r = rect();

		if (cx * 2 > r.width())
			cx = 0;

		if (cy * 2 > r.height())
			cy = 0;

		r.adjust(cx, cy, -cx, -cy);

		return r.isValid() ? r : rect();
	}

	QRect blockRect() const
	{
		const QRectF r = rect();
		return QRect(qRound(r.left() / gridSize.width()),
					 qRound(r.top() / gridSize.height()),
					 qRound(r.width() / gridSize.width()),
					 qRound(r.height() / gridSize.height()));
	}

	qreal xPadding() const
	{
		return padding > 0 ? gridSize.width() * padding : 0;
	}

	qreal yPadding() const
	{
		return padding > 0 ? gridSize.height() * padding : 0;
	}

	int rows() const
	{
		return qRound(size.height() / gridSize.height());
	}

	int cols() const
	{
		return qRound(size.width() / gridSize.width());
	}

	void downloadPicture()
	{
		DownloadFile* download = new DownloadFile;
		connect(download, SIGNAL(finished()), download, SLOT(deleteLater()));
		connect(download, SIGNAL(completed(QByteArray)), q, SLOT(pictureDownloaded(QByteArray)));
		download->download(pictureUrl);
	}
};

GraphicsItem::GraphicsItem(const QRectF& rect, QGraphicsItem* parent) :
	QGraphicsObject(parent),
	d(new Impl(rect, this))
{
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable |
			 QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);
	setPos(rect.topLeft());

	connect(d->form, SIGNAL(gridSizeChanged()), this, SLOT(gridSizeChanged()));
}

GraphicsItem::~GraphicsItem()
{
}

QRectF GraphicsItem::boundingRect() const
{
	return QRectF(-d->borderWidth / 2,
				  -d->borderWidth / 2,
				   d->size.width() + d->borderWidth,
				   d->size.height() + d->borderWidth);
}

void GraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(widget);

	QString str = d->text.isEmpty() ? d->form->data(d->rect()) : d->text;

	painter->setRenderHint(QPainter::Antialiasing, !d->form->isPrinting());
	painter->setPen(d->borderWidth > 0 ? QPen(QBrush(d->borderColor), d->borderWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin) : QPen(Qt::NoPen));
	painter->setBrush(d->backColor == Qt::transparent ? QBrush(Qt::NoBrush) : QBrush(d->backColor));
	painter->drawRect(0, 0, d->size.width(), d->size.height());
	painter->setRenderHint(QPainter::Antialiasing, false);

	if (!d->pixmap.isNull())
	{
		qreal x = 0;
		qreal y = 0;
		qreal width = d->size.width();
		qreal height = d->size.height();

		if (d->pictureMode == KeepAspectRatio)
		{
			const qreal pictureAspectRatio = (qreal)d->pixmap.width() / (qreal)d->pixmap.height();
			const qreal aspectRatio = d->size.width() / d->size.height();

			const qreal pictureWidth = d->pixmap.widthMM() * 10;
			const qreal pictureHeight = d->pixmap.heightMM() * 10;

			if (pictureAspectRatio > aspectRatio)
			{
				height = d->size.width() / pictureWidth * pictureHeight;
				y = (d->size.height() - height) / 2;
			}
			else if (pictureAspectRatio < aspectRatio)
			{
				width = d->size.height() / pictureHeight * pictureWidth;
				x = (d->size.width() - width) / 2;
			}
		}

		painter->setBackgroundMode(Qt::TransparentMode);
		painter->drawPixmap(x, y, width, height, d->pixmap);
	}
	else
	{
		painter->setBackgroundMode(Qt::TransparentMode);
		painter->setPen(d->textColor);

		const qreal xPadding = d->xPadding();
		const qreal yPadding = d->yPadding();

		qreal yPos = yPadding;
		qreal lineHeight = d->form->lineHeight();

		if (d->fontHeight == Normal)
		{
			if (d->padding > 0)
			{
				const int rows = d->rows();
				const int cols = d->cols();
				const QFont font = Font::stretch(d->font, rows, cols, d->textRect().size());
				painter->setFont(font);
				lineHeight = d->textRect().height() / rows;
			}
			else
			{
				const QFont font = d->form->stretchFont(d->font);
				painter->setFont(font);
			}
		}
		else
		{
			str = str.trimmed();
			const QFont font = Font::stretch(d->font, str, d->textRect());
			painter->setFont(font);
			lineHeight = QFontMetricsF(font).height();
		}

		if (d->vertAlign)
		{
			str = str.trimmed();
			int lines = str.count('\n') + 1;
			qreal height = lines * lineHeight;

			switch (d->vertAlign)
			{
			case Qt::AlignTop:
				break;
			case Qt::AlignVCenter:
				yPos = (d->size.height() - height) / 2;
				break;
			case Qt::AlignBottom:
				yPos = d->size.height() - height;
				break;
			}
		}

		const qreal width = d->size.width() - (2 * xPadding);

		bool firstLine = true;
		QFont font = painter->font();
		int lineNumber = 1;

		foreach (QString line, str.split('\n'))
		{
			if ((yPos + lineHeight) > d->size.height())
				break;

			if (firstLine && d->firstLineBold && !painter->font().bold())
			{
				QFont boldFont(font);
				boldFont.setBold(true);
				boldFont = Font::setWidth(boldFont, QFontMetrics(font).averageCharWidth());
				painter->setFont(boldFont);
			}

			const QRectF rect(xPadding, yPos, width, lineHeight);

			if (d->stripes && lineNumber % 2)
				painter->fillRect(rect, d->stripeColor);

			int flags = Qt::AlignLeft;
			if (d->horzAlign)
			{
				line = line.trimmed();
				flags |= d->horzAlign;
				painter->drawText(rect, flags, line);
			}
			else if (d->autoLines && painter->fontInfo().fixedPitch())
			{
				QString stripped(line);

				stripped.replace('=', ' ');
				stripped.replace('-', ' ');

				painter->drawText(rect, flags, stripped);

				painter->setPen(QPen(QBrush(d->textColor), d->borderWidth > 0 ? d->borderWidth : 1,
									 Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));

				qreal x = rect.left();
				const qreal y = rect.top();
				const qreal width = d->textRect().width() / d->cols();
				const qreal height = d->textRect().height() / d->rows();

				painter->setRenderHint(QPainter::Antialiasing, !d->form->isPrinting());

				foreach (const QChar ch, line)
				{
					switch (ch.toAscii())
					{
					case '=':
						painter->drawLine(x, y + height / 3, x + width, y + height / 3);
						painter->drawLine(x, y + (height / 3) * 2, x + width, y + (height / 3) * 2);
						break;
					case '-':
						painter->drawLine(x, y + height / 2, x + width, y + height / 2);
						break;
					default:
						break;
					}

					x += width;
				}

				painter->setRenderHint(QPainter::Antialiasing, false);
			}
			else
			{
				painter->drawText(rect, flags, line);
			}

			yPos += lineHeight;
			firstLine = false;
			lineNumber++;

			if (font != painter->font())
				painter->setFont(font);
		}
	}

	if (!d->form->isPrinting())
	{
		if (isSelected())
		{
			painter->setPen(QPen(option->palette.window(), 0, Qt::SolidLine));
			painter->setBrush(Qt::NoBrush);
			painter->drawRect(0, 0, d->size.width(), d->size.height());

			painter->setPen(QPen(option->palette.highlight(), 0, Qt::DashLine));
			painter->setBrush(Qt::NoBrush);
			painter->drawRect(0, 0, d->size.width(), d->size.height());
		}
		else if (d->borderWidth == 0 && d->backColor == Qt::transparent)
		{
			painter->setPen(QPen(option->palette.highlight(), 0, Qt::DotLine));
			painter->setBrush(Qt::NoBrush);
			painter->drawRect(0, 0, d->size.width(), d->size.height());
		}
	}
}

QPainterPath GraphicsItem::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

QRectF GraphicsItem::rect() const
{
	return d->rect();
}

QVariant GraphicsItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionChange)
	{
		// Clip to the items parent
		QPointF pt = value.toPointF();

		const QRectF parentRect = parentItem()->boundingRect();

		pt.setX(qMax(pt.x(), parentRect.left()));
		pt.setX(qMin(pt.x(), parentRect.right() - d->size.width()));

		pt.setY(qMax(pt.y(), parentRect.top()));
		pt.setY(qMin(pt.y(), parentRect.bottom() - d->size.height()));

		// Snap to grid
		const QSizeF gridSize = d->form->gridSize();
		pt.setX(qRound(pt.x() / gridSize.width()) * gridSize.width());
		pt.setY(qRound(pt.y() / gridSize.height()) * gridSize.height());

		if (pt != pos())
			emit changed();

		return pt;
	}

	if (change == ItemPositionHasChanged || change == ItemZValueChange ||
			change == ItemZValueHasChanged)
	{
		emit changed();
	}

	return QGraphicsItem::itemChange(change, value);
}

void GraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (d->sizeHitTest(event->pos()))
		d->sizing = true;
	else if (flags() & QGraphicsItem::ItemIsMovable)
		setCursor(Qt::OpenHandCursor);
	QGraphicsItem::mousePressEvent(event);
}

void GraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (d->sizing)
	{
		QRectF rect = QRectF(QPointF(0, 0), event->pos());
		const QSizeF gridSize = d->form->gridSize();

		// Snap
		rect.setRight(qRound(rect.right() / gridSize.width()) * gridSize.width());
		rect.setBottom(qRound(rect.bottom() / gridSize.height()) * gridSize.height());

		// Minimum size
		rect.setWidth(qMax(rect.width(), gridSize.width()));
		rect.setHeight(qMax(rect.height(), gridSize.height()));

		// Clip to parent
		QPointF pt(mapFromParent(parentItem()->boundingRect().bottomRight()));
		rect.setRight(qMin(rect.right(), pt.x()));
		rect.setBottom(qMin(rect.bottom(), pt.y()));

		// Update if the size changed
		if (d->size != rect.size())
		{
			d->size = rect.size();
			prepareGeometryChange();
			emit changed();
		}
	}
	else
		QGraphicsItem::mouseMoveEvent(event);
}

void GraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	d->sizing = false;
	setCursor(Qt::ArrowCursor);
	QGraphicsItem::mouseReleaseEvent(event);
}

void GraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
	if (d->sizeHitTest(event->pos()))
		setCursor(Qt::SizeFDiagCursor);
	else if (isSelected() && (flags() & QGraphicsItem::ItemIsMovable))
		setCursor(Qt::OpenHandCursor);
	else
		setCursor(Qt::ArrowCursor);
	QGraphicsItem::hoverMoveEvent(event);
}

void GraphicsItem::keyPressEvent(QKeyEvent* event)
{
	const QRect rect = d->blockRect();
	const int rows = d->form->rows();
	const int cols = d->form->cols();

	QPointF newPos(pos());
	bool posChanged = true;

	switch (event->key())
	{
	case Qt::Key_Left:
		newPos.setX(qMax(0.0, newPos.x() - d->gridSize.width()));
		break;
	case Qt::Key_Right:
		if (rect.right() < cols)
			newPos.setX(newPos.x() + d->gridSize.width());
		break;
	case Qt::Key_Up:
		newPos.setY(qMax(0.0, newPos.y() - d->gridSize.height()));
		break;
	case Qt::Key_Down:
		if (rect.bottom() < rows)
			newPos.setY(newPos.y() + d->gridSize.height());
		break;
	default:
		posChanged = false;
		break;
	}

	if (posChanged)
	{
		setPos(newPos);
		update();
		emit changed();
	}
}

QFont GraphicsItem::font() const
{
	return d->font;
}

void GraphicsItem::setFont(const QFont& font)
{
	if (isSelected() && font.family() != "" && !font.isCopyOf(d->font))
	{
		d->font = font;
		update();
		emit changed();
	}
}

QColor GraphicsItem::textColor() const
{
	return d->textColor;
}

void GraphicsItem::setTextColor(const QColor& color)
{
	if (isSelected() && d->textColor != color)
	{
		d->textColor = color;
		update();
		emit changed();
	}
}

QColor GraphicsItem::backColor() const
{
	return d->backColor;
}

void GraphicsItem::setBackColor(const QColor& color)
{
	if (isSelected() && d->backColor != color)
	{
		d->backColor = color;
		update();
		emit changed();
	}
}

QColor GraphicsItem::borderColor() const
{
	return d->borderColor;
}

void GraphicsItem::setBorderColor(const QColor& color)
{
	if (isSelected() && d->borderColor != color)
	{
		d->borderColor = color;
		update();
		emit changed();
	}
}

qint32 GraphicsItem::borderWidth() const
{
	return d->borderWidth;
}

void GraphicsItem::setBorderWidth(qint32 width)
{
	if (isSelected() && width != d->borderWidth)
	{
		d->borderWidth = width;
		prepareGeometryChange();
		emit changed();
	}
}

void GraphicsItem::setBold(bool bold)
{
	if (isSelected() && d->font.bold() != bold)
	{
		d->font.setBold(bold);
		update();
		emit changed();
	}
}

void GraphicsItem::setItalic(bool italic)
{
	if (isSelected() && d->font.italic() != italic)
	{
		d->font.setItalic(italic);
		update();
		emit changed();
	}
}

quint32 GraphicsItem::horzAlign() const
{
	return d->horzAlign;
}

void GraphicsItem::setHorzAlign(quint32 align)
{
	if (isSelected() && d->horzAlign != align)
	{
		d->horzAlign = align;
		update();
		emit changed();
	}
}

quint32 GraphicsItem::vertAlign() const
{
	return d->vertAlign;
}

void GraphicsItem::setVertAlign(quint32 align)
{
	if (isSelected() && d->vertAlign != align)
	{
		d->vertAlign = align;
		update();
		emit changed();
	}
}

QString GraphicsItem::text() const
{
	return d->text;
}

void GraphicsItem::setText(const QString& text)
{
	if (isSelected() && d->text != text)
	{
		d->text = text;
		update();
		emit changed();
	}
}

QUrl GraphicsItem::pictureUrl() const
{
	return d->pictureUrl;
}

void GraphicsItem::setPictureUrl(const QUrl& url)
{
	if (isSelected() && d->pictureUrl != url)
	{
		d->pictureUrl = url;
		d->pixmap = QPixmap();

		if (!d->pictureUrl.isEmpty())
		{
			if (QFile::exists(d->pictureUrl.toString()))
				d->pixmap.load(d->pictureUrl.toString());
			else
				d->downloadPicture();
		}

		update();
		emit changed();
	}
}

void GraphicsItem::setPicture(const QPixmap& pixmap)
{
	d->pixmap = pixmap;
	d->pictureUrl = "picture";
	update();
	emit changed();
}

void GraphicsItem::pictureDownloaded(const QByteArray& bytes)
{
	QPixmap pixmap;
	if (pixmap.loadFromData(bytes))
	{
		d->pixmap = pixmap;
		update();
	}
}

bool GraphicsItem::box() const
{
	return d->borderWidth > 0;
}

void GraphicsItem::setBox(bool box)
{
	if (isSelected())
	{
		if (box)
		{
			d->borderWidth = 1;
			if (d->backColor == Qt::white || d->backColor == Qt::transparent)
				d->backColor = boxColor;
		}
		else
		{
			d->borderWidth = 0;
			if (d->backColor == boxColor)
				d->backColor = Qt::transparent;
		}
		update();
		emit changed();
	}
}

quint32 GraphicsItem::pictureMode() const
{
	return d->pictureMode;
}

void GraphicsItem::setPictureMode(quint32 mode)
{
	if (isSelected() && d->pictureMode != mode)
	{
		d->pictureMode = mode;
		update();
		emit changed();
	}
}

quint32 GraphicsItem::fontHeight() const
{
	return d->fontHeight;
}

void GraphicsItem::setFontHeight(quint32 height)
{
	if (isSelected() && d->fontHeight != height)
	{
		d->fontHeight = height;
		update();
		emit changed();
	}
}

void GraphicsItem::gridSizeChanged()
{
	const QRect rect = d->form->toRect(d->rect(), d->gridSize);
	d->gridSize = d->form->gridSize();

	const QPointF newPos(rect.left() * d->gridSize.width(), rect.top() * d->gridSize.height());
	const QSizeF newSize(rect.width() * d->gridSize.width(), rect.height() * d->gridSize.height());

	setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
	d->size = newSize;
	setPos(newPos);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges);

	prepareGeometryChange();
}

void GraphicsItem::setRect(const QRectF& rect)
{
	setPos(rect.topLeft());
	d->size = rect.size();
	prepareGeometryChange();
	emit changed();
}

qreal GraphicsItem::padding() const
{
	return d->padding;
}

void GraphicsItem::setPadding(qreal padding)
{
	if (isSelected() && !qFuzzyCompare(padding, d->padding))
	{
		d->padding = padding;
		update();
		emit changed();
	}
}


bool GraphicsItem::autoLines() const
{
	return d->autoLines;
}

void GraphicsItem::setAutoLines(bool autoLines)
{
	if (isSelected() && autoLines != d->autoLines)
	{
		d->autoLines = autoLines;
		update();
		emit changed();
	}
}

bool GraphicsItem::firstLineBold() const
{
	return d->firstLineBold;
}

void GraphicsItem::setFirstLineBold(bool firstLineBold)
{
	if (isSelected() && firstLineBold != d->firstLineBold)
	{
		d->firstLineBold = firstLineBold;
		update();
		emit changed();
	}
}

bool GraphicsItem::stripes() const
{
	return d->stripes;
}

void GraphicsItem::setStripes(bool stripes)
{
	if (isSelected() && stripes != d->stripes)
	{
		d->stripes = stripes;
		update();
		emit changed();
	}
}

QColor GraphicsItem::stripeColor() const
{
	return d->stripeColor;
}

void GraphicsItem::setStripeColor(const QColor& color)
{
	if (isSelected() && color != d->stripeColor)
	{
		d->stripeColor = color;
		update();
		emit changed();
	}
}

QDataStream& operator<<(QDataStream& stream, const GraphicsItem& item)
{
	stream << (quint16)GraphicsItem::Impl::VersionNumber;
	stream << item.pos();
	stream << item.d->size;
	stream << item.d->font;
	stream << item.d->textColor;
	stream << item.d->backColor;
	stream << item.d->borderColor;
	stream << item.d->borderWidth;
	stream << item.d->horzAlign;
	stream << item.d->vertAlign;
	stream << item.d->text;
	stream << item.d->pixmap;
	stream << item.d->pictureUrl;
	stream << item.d->pictureMode;
	stream << item.d->fontHeight;
	stream << item.zValue();
	stream << item.d->padding;
	stream << item.d->autoLines;
	stream << item.d->firstLineBold;
	stream << item.d->stripes;
	stream << item.d->stripeColor;
	return stream;
}

QDataStream& operator>>(QDataStream& stream, GraphicsItem& item)
{
	stream >> item.d->version;

	QPointF pos;
	stream >> pos;
	item.setPos(pos);

	stream >> item.d->size;
	stream >> item.d->font;
	stream >> item.d->textColor;
	stream >> item.d->backColor;
	stream >> item.d->borderColor;
	stream >> item.d->borderWidth;

	if (item.d->version > 0)
	{
		stream >> item.d->horzAlign;
		stream >> item.d->vertAlign;
	}

	if (item.d->version > 1)
	{
		stream >> item.d->text;
		stream >> item.d->pixmap;
		stream >> item.d->pictureUrl;
	}

	if (item.d->version > 2)
	{
		stream >> item.d->pictureMode;
		stream >> item.d->fontHeight;
	}

	if (item.d->version > 3)
	{
		qreal z = 0;
		stream >> z;
		item.setZValue(z);
	}

	if (item.d->version > 4)
	{
		stream >> item.d->padding;
	}

	if (item.d->version > 5)
	{
		stream >> item.d->autoLines;
	}

	if (item.d->version > 6)
	{
		stream >> item.d->firstLineBold;
	}

	if (item.d->version > 7)
	{
		stream >> item.d->stripes;
		stream >> item.d->stripeColor;
	}

	if (!QFile::exists(item.d->pictureUrl.toString()))
		item.d->downloadPicture();

	return stream;
}
