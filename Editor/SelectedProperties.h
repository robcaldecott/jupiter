#ifndef SELECTEDPROPERTIES_H
#define SELECTEDPROPERTIES_H

#include <QString>
#include <QColor>
#include <QUrl>

class GraphicsItem;

struct SelectedProperties
{
	QString text;
	QString fontName;
	bool bold;
	bool italic;
	quint32 fontHeight;
	QColor textColor;
	QColor backColor;
	quint32 horzAlign;
	quint32 vertAlign;
	qint32 borderWidth;
	QColor borderColor;
	bool box;
	QUrl pictureUrl;
	quint32 pictureMode;
	qreal padding;
	bool autoLines;
	bool firstLineBold;
	bool stripes;
	QColor stripeColor;

	SelectedProperties(const QList<GraphicsItem*>& items);
};

#endif // SELECTEDPROPERTIES_H
