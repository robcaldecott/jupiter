#ifndef REPORTVIEW_H
#define REPORTVIEW_H

#include <QObject>

class Report;
class QPainter;
class QPrinter;
class QSizeF;

class ReportView : public QObject
{
    Q_OBJECT

public:
    ReportView(const Report* report, QObject* parent = 0);
    int paint(QPainter& painter, QPrinter& printer);

signals:
    void changed();

private:
    void drawPage(QPainter& painter, const QSizeF& pageSize, qreal lineHeight) const;
    void drawStripes(QPainter& painter, const QSizeF& pageSize, qreal lineHeight) const;
    void drawPictures(QPainter& painter, const QSizeF& cellSize, const QSizeF& pageSize) const;
    qreal adjustYpos(int lineNumber, qreal yPos) const;

    const Report* report;
};

#endif // REPORTVIEW_H
