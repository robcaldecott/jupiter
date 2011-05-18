#ifndef TEXT_H
#define TEXT_H

#include <QString>
#include <QSize>

namespace Text
{
    /// Return the size of the largest page in rows/cols
    QSize pageSize(const QString& str);
    /// Expand tabs to spaces
    QString expandTabs(const QString& str);
    /// Remove HTML from a string
    QString stripHtml(const QString& str);
}

#endif // TEXT_H
