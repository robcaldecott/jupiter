#include "ColorComboBox.h"
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QPixmapCache>

namespace
{
	const QSize DefaultSize(24, 24);
}

ColorComboBox::ColorComboBox(QWidget *parent) :
	QComboBox(parent)
{
	foreach (const QString& name, QColor::colorNames())
		addColor(QColor(name), name);
}

void ColorComboBox::setColor(const QColor& color)
{
	if (color.isValid())
	{
		int index = findData(color);
		if (index == -1)
		{
			addColor(color, color.name());
			index = count() - 1;
		}
		setCurrentIndex(index);
	}
	else
		setCurrentIndex(-1);
}

void ColorComboBox::addColor(const QColor& color, const QString& name)
{
	QPixmap pixmap(DefaultSize);
	if (!QPixmapCache::find(name, &pixmap))
	{
		pixmap.fill(Qt::transparent);

		QPainter painter(&pixmap);
		painter.setPen(Qt::NoPen);
		painter.setBrush(color);
		painter.drawEllipse(0, 0, DefaultSize.width(), DefaultSize.height());
		painter.end();

		QPixmapCache::insert(name, pixmap);
	}
	addItem(pixmap, name, color);
}
