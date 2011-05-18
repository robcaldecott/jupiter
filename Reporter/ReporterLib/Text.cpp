#include "Text.h"
#include "StringList.h"
#include <QStringList>
#include <QRegExp>
#include <QDebug>

namespace
{
    const int ROWS = 66;
    const int COLS = 80;

    const int MIN_ROWS = 40;
    const int MIN_COLS = 40;
}

/// Return the size of the largest page in rows/cols
QSize Text::pageSize(const QString& str)
{
    QSize size;

    bool ff = str.contains('\f');
    const QStringList pages = str.split('\f', QString::SkipEmptyParts);
    foreach (const QString& page, pages)
    {
	const QStringList lines = StringList::trimRight(page.split(QRegExp("(\\r)|(\\n)|(\\r\\n)")));
	foreach (const QString& line, lines)
	{
	    // Remove any HTML sequences from the line
	    size.setWidth(qMax(stripHtml(line).count(), size.width()));
	}
	if (ff)
	{
	    size.setHeight(qMax(lines.count(), size.height()));
	}
    }

    size.setHeight(size.height() < MIN_ROWS ? ROWS : size.height());
    size.setWidth(size.width() < MIN_COLS ? COLS : size.width());

    return size;
}

/// Expand tabs to spaces
QString Text::expandTabs(const QString& str)
{
    return QString(str).replace("\t", "        ");
}

/// Remove HTML from a string
QString Text::stripHtml(const QString& str)
{
    return QString(str).remove(QRegExp("</?[bius]>", Qt::CaseInsensitive));
}
